function im = cbimread(filename)
%im = cbimread(filename)
% Reads an image and returns it in 0-1 float format.

im = double(imread(filename))/255;
end