function [post_img, imin, imax] = post_mult_and_normalize(img)

%%%%%% function changed %%%%%%%
% According to Javier, the matrix below is intended as a pre-multiplication
% matrix

post = [ 2.57802961667760  -1.32188044638028  0.649329484660944;
        -2.52336650503095   3.07256263980345 -1.52191312391749;
         0.625417860539208 -1.32260040379914  1.77407732591195];

postInv = inv(post);

img_lin = reshape(img, size(img, 1)*size(img, 2), 3);
%post_img_lin = (postInv * (img_lin * post)')';
%post_img_lin = (img_lin * post);
post_img_lin = (post * img_lin')';

imin = min(min(post_img_lin))
imax = max(max(post_img_lin))

x1 = reshape(img(1, 1, :), 1, 3)
x2 = reshape(img(1, 2, :), 1, 3)

x1*post
x2*post

%post_img = (post_img + imin) ./ (imax - imin);
if (imin < 0)
	imin = 0;

	blubb = reshape(post_img_lin, size(post_img_lin, 1)*size(post_img_lin, 2), 1);
	figure(2);
	hist(blubb);

%    % set all pixels to 0 where one channel is negative
%    for c1 = 1:3
%        p(c1,:) = post_img_lin(:,c1);
%    end
%    for c1 = 1:3
%        for c2 = 1:3
%            q = post_img_lin(:,c2);
%            q(p(c1, :) < 0) = 0;
%            post_img_lin(:,c2) = q;
%        end
%    end

	post_img_lin(post_img_lin < 0) = 0;
end

post_img = reshape(post_img_lin, size(img, 1), size(img, 2), 3);

post_img = (post_img - imin) ./ (imax - imin);
figure(1);
imshow(post_img);
waitforbuttonpress();
