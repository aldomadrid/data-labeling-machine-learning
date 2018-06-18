%turn videos into frames

source = 'outputvideo6.avi'; %name of video added as source
video = VideoReader(source); %video as videoreader object
frames = video.Numberofframes; 

mkdir images6    %create a new folder
cd images6

for i=1:frames
    single = read(video, i);    
    x = sprintf('img%d.jpg', i);
    imwrite(single, x);
end