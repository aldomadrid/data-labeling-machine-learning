%resize images in folder
files = dir('*.png'); 
number = size(files, 1); 

%mkdir resized   %making a directory to store resized images

for i = 1:number
    frame = imread(files(i).name); %read 
    cd resized
    imgR = imresize(frame, [64 64]);
    imwrite(imgR, files(i).name); 
    cd 'C:\Users\aldo9\Documents\unsized'
end
    