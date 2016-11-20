function task1(Directory,r,n,out_filename)
a1='*.mp4';
a2=strcat(Directory,a1);%to retrieve '.mp4' files
Direc = dir(a2);%to get the directory
file1=fopen(out_filename,'w');%opening a file
for i = 1 : length(Direc)%to read all the files in directory
    frame_count=0;%to monitor the frame number
    a = strcat(Directory,Direc(i).name);%getting the video file
    v(i)=VideoReader(a);%reading each video file
    while hasFrame(v(i))%to read the whole video
        frame_count=frame_count+1;
        video=readFrame(v(i));%reading each frame in each video
        z=rgb2gray(video);%rgb values of each frame converted into gray scale values
        s=size(z);%getting size of frame
        ro=floor(s(1)/r);%getting row values of each cell after it is split into (r*r) cells
        co=floor(s(2)/r);%getting column values of each cell after it is split into (r*r) cells
        rv=1:r;
        cv=1:r;
        r1=rem(s(1),r);%remainder of 120/r to get the extra_row
        r2=rem(s(2),r);%remainder of 160/r to get the extra_col
        if (r1 == 0)%for factors of 120 and 160
        for x=1:r
            rv(x)=ro;%creating 'r' rows
        end
        end
        if (r2 == 0)%for factors of 120 and 160
        for x=1:r
            cv(x)=co;%creating 'r' columns
        end
        end
        if (r1 ~= 0)%for non-factors of 120 and 160
            for x=1:r-1
                rv(x)=ro;%creating 'r' rows
            end
            for x1=r:r
                rv(x1)=ro+r1;%last value in matrix will be ro+extra_row
            end
        end
        if (r2 ~= 0)%for non-factors of 120 and 160
            for x2=1:r-1
                cv(x2)=co;%creating 'r' columns
            end
            for x3=r:r
                cv(x3)=co+r2;%last value in matrix will be ro+extra_row
            end
        end
        z1=mat2cell(z,rv,cv);%converting the frame in matrix form to (r*r) cells
        cell_count=0;%to monitor cell number
        for j=1:r
            for l=1:r
                z2=cell2mat(z1(j,l));%each cell converted to a matrix
                [count,bin]=imhist(z2,n);%histogram for each cell is got with number
                                      %of bins of histogram and count with respect to the bin
                fprintf(file1,'video-');
                fprintf(file1,'%d',i);%printing video number
                fprintf(file1,'\t');
                fprintf(file1,'frame-');
                fprintf(file1,'%d',frame_count);%printing frame number
                fprintf(file1,'\t');
                fprintf(file1,'cell-');
                fprintf(file1,'%d',cell_count+1);%printing the cell number
                fprintf(file1,'\t');
                fprintf(file1,'bin-');
                fprintf(file1,'%d ',floor(bin));%printing the bins of histogram
                fprintf(file1,'\t');
                fprintf(file1,'count-');
                fprintf(file1,'%d ',count);%printing the count of gray scale 
                                            %value of each bin in the histogram
                fprintf(file1,'\n');
            end
        end
    end
end
fclose(file1);%closing the file