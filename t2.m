function t2(Directory,r,out_filename)
a1='*.mp4';
a2=strcat(Directory,a1);%to retrieve '.mp4' files
Direc = dir(a2);%to get the directory
file3=fopen(out_filename,'w');%opening a file
for i = 1 : length(Direc)%to read all the files in directory
    frame_count=0;%to monitor the frame number
    a = strcat('/Users/Priya/Documents/MATLAB/Task2/',Direc(i).name);%getting the video file
    v(i)=VideoReader(a);%reading each video file
    while hasFrame(v(i))%to read the whole video
        frame_count=frame_count+1;
        video=readFrame(v(i));%reading each frame in each video
        z=rgb2gray(video);%rgb values of each frame converted into gray scale values
        [frames,descriptors] = sift(z);%calling the SIFT function
        s=size(z);%getting size of frame
        ro=s(1)/r;%getting x values of each cell after it is split into (r*r) cells
        co=s(2)/r;%getting y values of each cell after it is split into (r*r) cells
        for j=1:length(frames)%to read each frame of the video
            x=frames(1:2,j);%to extract centre (x,y) of the SIFT vector
            y=frames(3,j);%to extract scale value of the SIFT vector
            z1=frames(4,j);%to extract orientation of the SIFT vector
            z2=descriptors(1:128,j);%to extract descriptors values of the SIFT vector
            cell_x=floor(x(1)/co);%to find x value of cell number to which SIFT vector belongs to
            cell_y=floor(x(2)/ro);%to find y value of cell number to which SIFT vector belongs to 
            cell = floor((cell_x * r) + cell_y);%to find the cell number in which SIFT vector lies
            fprintf(file3,'video');
            fprintf(file3,'::');
            fprintf(file3,'%d',i);%printing video number
            fprintf(file3,'frame');
            fprintf(file3,'%d',frame_count);%printing frame number
            fprintf(file3,'::');
            fprintf(file3,'cell_num :  %d ',cell);%printing the cell number of SIFT vector
            fprintf(file3,'\n');
            fprintf(file3,'X : %f',x(1));%printing x value of SIFT vector
            fprintf(file3,'\v');
            fprintf(file3,'Y : %f \v',x(2));%printing y value of SIFT vector
            fprintf(file3,'scale');
            fprintf(file3,'==');
            fprintf(file3,'%f',y);%printing scale value of SIFT vector
            fprintf(file3,'\v');
            fprintf(file3,'orientation');
            fprintf(file3,'==');
            fprintf(file3,'%f',z1);%printing orientation of SIFT vector
            fprintf(file3,'\n');
            fprintf(file3,'descriptor');
            fprintf(file3,'=');
            fprintf(file3,' %f',z2);%printing descriptor values of SIFT vector
            fprintf(file3,'\n');
         
    end
   
    end
end
    
fclose(file3);%closing the file