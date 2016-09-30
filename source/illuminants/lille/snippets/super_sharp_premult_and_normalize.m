
%%%%%% function changed %%%%%%%
% According to Javier, the matrix below is intended as a pre-multiplication
% matrix

function [pre_img, imin, imax] = super_sharp_premult_and_normalize(img)

% EXCHANGE ROWS
pre = [ 3.0779   -2.1620    1.3128;
       -2.7466    3.8185   -2.5281;
        0.2921    -1.1498   2.0365];

preInv = inv(pre);
     
img_lin = reshape(img, size(img, 1)*size(img, 2), 3);
pre_img_lin = (pre * img_lin')';

imin = min(min(pre_img_lin))
imax = max(max(pre_img_lin))

%pre_img = (pre_img + imin) ./ (imax - imin);
if (imin < 0)
	imin = 0;
%    % set all pixels where one channel is < 0 to [0, 0, 0]
%    for c1 = 1:3
%        p(c1,:) = pre_img_lin(:,c1);
%    end
%    for c1 = 1:3
%        for c2 = 1:3
%            q = pre_img_lin(:,c2);
%            q(p(c1, :) < 0) = 0;
%            pre_img_lin(:,c2) = q;
%        end
%    end
	blubb = reshape(pre_img_lin, size(pre_img_lin, 1)*size(pre_img_lin, 2), 1);
	figure(2);
	hist(blubb);

%	pre_img_lin(pre_img_lin < 0) = 0;
end

imin = min(min(pre_img_lin));
imax = max(max(pre_img_lin));

pre_img = reshape(pre_img_lin, size(img, 1), size(img, 2), 3);

pre_img = (pre_img - imin) ./ (imax - imin);
figure(1);
imshow(pre_img);
waitforbuttonpress();
