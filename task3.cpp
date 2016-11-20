#include <string.h>
#include "stdafx.h"
#include "stdio.h"
#include "conio.h"
#include <iostream>

using namespace std;

#include "filesystem"

namespace fs = std::tr2::sys;



extern "C"
{
#include <libavutil/motion_vector.h>
#include <libavformat/avformat.h>
# pragma comment (lib, "avformat.lib")
}
static AVFormatContext *fmt_ctx = NULL;
static AVCodecContext *video_dec_ctx = NULL;
static AVStream *video_stream = NULL;
static  char src_filename[100];//to get the file name
static int video_stream_idx = -1;
static AVFrame *frame = NULL;
static AVPacket pkt;
static int frame_count;//to monitor number of frames
static int r;//resolution value
FILE *file3;//to access output file
static int decode_packet(int *got_frame, int cached)
{
	int decoded = pkt.size;

	*got_frame = 0;

	if (pkt.stream_index == video_stream_idx) {
		int ret = avcodec_decode_video2(video_dec_ctx, frame, got_frame, &pkt);
		if (ret < 0) {
			//fprintf(stderr, "Error decoding video frame (%s)\n", av_err2str(ret));
			return ret;
		}

		if (*got_frame) {
			int i;
			AVFrameSideData *sd;
			frame_count++;//incrementing frame number
			sd = av_frame_get_side_data(frame, AV_FRAME_DATA_MOTION_VECTORS);
			int height = video_dec_ctx->height;//getting height of the video
			int width = video_dec_ctx->width;//getting width of the video
			int x = height / r;//getting row value of each cell
			int y = width / r;//getting column value of each cell
			while (height % r != 0)
			{
				height = x*r;
			}
			while (width % r != 0)
			{
				width = y*r;
			}
			if (sd) {
				const AVMotionVector *mvs = (const AVMotionVector *)sd->data;
				for (i = 0; i < sd->size / sizeof(*mvs); i++) {
					const AVMotionVector *mv = &mvs[i];//motion vector
					int row_x = floor((mv->dst_x - 1) / y);//calculating the row in which motion vector is present
					int column_y = floor((mv->dst_y - 1) / x);//calculating the column in which motion vector is present
					if (mv->dst_x == 0 && mv->dst_y == 0)
					{
						row_x = 0;
						column_y = 0;
					}
					else if (mv->dst_x == 0 && mv->dst_y != 0)
					{
						row_x = 0;
					}
					else if (mv->dst_x != 0 && mv->dst_y == 0)
					{
						 column_y = 0;
					}
					if (mv->dst_y > height)
					{
						column_y = floor((height - 1) / x);
					}
					if (mv->dst_x > width)
					{
						row_x = floor((width - 1) / y);
					}
					int cell_num = (row_x*r) + column_y;//calculating the cell number in which motion vector is present
					fprintf(file3, "frame number : %d  \n", frame_count);//printing frame number
					fprintf(file3, "cellx:%d  celly:%d  ", row_x, column_y);//printing row and column value of the cell in which motion vector lies
					fprintf(file3, "cellnumber : %d  \n", cell_num);//printing the cell number in which the motion vector lies
					fprintf(file3, "source: %d width:  %d height: %d source_x: %d source_y: %d dest_x: %d dest_y: %d\n",
						mv->source,
						mv->w, mv->h, mv->src_x, mv->src_y,
						mv->dst_x, mv->dst_y);//printing source and destination values of motion vector along with height and width of it
				}
			}
		}
	}

	return decoded;
}

static int open_codec_context(int *stream_idx,
	AVFormatContext *fmt_ctx, enum AVMediaType type)
{
	int ret;
	AVStream *st;
	AVCodecContext *dec_ctx = NULL;
	AVCodec *dec = NULL;
	AVDictionary *opts = NULL;

	ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
	if (ret < 0) {
		//fprintf(stderr, "Could not find %s stream in input file '%s'\n",av_get_media_type_string(type), src_filename);
		return ret;
	}
	else {
		*stream_idx = ret;
		st = fmt_ctx->streams[*stream_idx];

		/* find decoder for the stream */
		dec_ctx = st->codec;
		dec = avcodec_find_decoder(dec_ctx->codec_id);
		if (!dec) {
			//fprintf(stderr, "Failed to find %s codec\n",av_get_media_type_string(type));
			return AVERROR(EINVAL);
		}

		/* Init the video decoder */
		av_dict_set(&opts, "flags2", "+export_mvs", 0);
		if ((ret = avcodec_open2(dec_ctx, dec, &opts)) < 0) {
			//fprintf(stderr, "Failed to open %s codec\n",av_get_media_type_string(type));
			return ret;
		}
	}

	return 0;
}

int main()
{
	int ret = 0, got_frame;
	int video_number = 0;//to get the video number
						 //char full_path[] = "C:\\Users\\pgunaraj\\Downloads\\DataR\\";
	char full_path[200];//to get the directory
	char out_file[100];//to get output file
	cout << "enter full path: ";
	cin >> full_path;//getting directory
	cout << "enter r: ";
	cin >> r;//getting resolution value
	cout << "enter name of output file: ";
	cin >> out_file;//getting output file to write into
	file3 = fopen(out_file, "w+");//opening the output file
		if (fs::is_directory(full_path))
		{
			cout << "dg";
			fs::directory_iterator end_iter;
			for (fs::directory_iterator dir_itr(full_path);
				dir_itr != end_iter;
				++dir_itr)
			{
				video_number = video_number + 1;//incrementing video number
				frame_count = 0;
				strcpy(src_filename, dir_itr->path().string().c_str());
				fprintf(file3, "Video Number:%d", video_number);//printing video number
				fprintf(file3, "\n");
				av_register_all();

				if (avformat_open_input(&fmt_ctx, src_filename, NULL, NULL) < 0) {
					fprintf(stderr, "Could not open source file %s\n", src_filename);
					exit(1);
				}

				if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
					fprintf(stderr, "Could not find stream information\n");
					exit(1);
				}

				if (open_codec_context(&video_stream_idx, fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0) {
					video_stream = fmt_ctx->streams[video_stream_idx];
					video_dec_ctx = video_stream->codec;
				}

				av_dump_format(fmt_ctx, 0, src_filename, 0);

				if (!video_stream) {
					fprintf(stderr, "Could not find video stream in the input, aborting\n");
					ret = 1;
					goto end;
				}

				frame = av_frame_alloc();
				if (!frame) {
					fprintf(stderr, "Could not allocate frame\n");
					ret = AVERROR(ENOMEM);
					goto end;
				}

				printf("framenum,source,blockw,blockh,srcx,srcy,dstx,dsty,flags\n");

				/* initialize packet, set data to NULL, let the demuxer fill it */
				av_init_packet(&pkt);
				pkt.data = NULL;
				pkt.size = 0;

				/* read frames from the file */
				while (av_read_frame(fmt_ctx, &pkt) >= 0) {
					AVPacket orig_pkt = pkt;
					do {
						ret = decode_packet(&got_frame, 0);
						if (ret < 0)
							break;
						pkt.data += ret;
						pkt.size -= ret;
					} while (pkt.size > 0);
					av_packet_unref(&orig_pkt);
				}

				/* flush cached frames */
				pkt.data = NULL;
				pkt.size = 0;
				do {
					decode_packet(&got_frame, 1);
				} while (got_frame);
			end:
				avcodec_close(video_dec_ctx);
				avformat_close_input(&fmt_ctx);
				av_frame_free(&frame);
				//kreturn ret < 0;

				//fclose(file3);//closing the file
			}
		}


}