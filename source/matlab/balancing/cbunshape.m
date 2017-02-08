function out = cbunshape(mat,s)
%out = cbunshape(im,[width,height])
% Takes a 3xn matrix of RGB pixels and returns a height x width x 3 RGB
% image

width = s(1); height = s(2);
out = reshape(mat,[3,width,height]);
out = permute(out,[2 3 1]);
end
