function out = cbreshape(im)
%out = cbreshape(im)
% Takes a width x height x 3 RGB image and returns a matrix where each column is an RGB
% pixel.

s = size(im);
m = s(1); n = s(2);
out = reshape(permute(im,[3 1 2]),[3 m*n 1]);

% x=[]
% for i=1:3
%     x=[x;reshape(im(:,:,i),m*n,1)'];
% end
end
