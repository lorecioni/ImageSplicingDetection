
%%%%%% function changed %%%%%%%
% According to Javier, the matrix below is intended as a post-multiplication
% matrix

function [pre_img, imin, imax] = pre_mult_and_normalize(img)

%pre = [...
%    -.00000779733186107023  -0.00101367902088967    0.0141474767010052;
%	 0.00000427190873786287  0.00674077064132028   -0.00312999210138898;
%	 0.00732611407791239    -0.000690377156493227   0.00177821818671831];

% EXCHANGE ROWS
pre = [  0.00732611407791239    -0.000690377156493227  0.00177821818671831;
	     0.00000427190873786287  0.00674077064132028  -0.00312999210138898;
        -0.00000779733186107023 -0.00101367902088967   0.0141474767010052];

% EXCHANGE COLUMNS (SHOULD BE WRONG: EVERYTHING TURNS RED)
%pre = [  0.0141474767010052  -0.00101367902088967  -0.00000779733186107023 
%	    -0.00312999210138898  0.00674077064132028   0.00000427190873786287 
%	     0.00177821818671831 -0.000690377156493227  0.00732611407791239     ];


preInv = inv(pre);
     
img_lin = reshape(img, size(img, 1)*size(img, 2), 3);
%pre_img_lin = ((pre * img_lin')' * pre);
pre_img_lin = (img_lin * pre);
%pre_img_lin = (pre * img_lin')';

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
%	blubb = reshape(pre_img_lin, size(pre_img_lin, 1)*size(pre_img_lin, 2), 1);
%	figure(2);
%	hist(blubb);
	pre_img_lin(pre_img_lin < 0) = 0;
end

imin = min(min(pre_img_lin));
imax = max(max(pre_img_lin));

pre_img = reshape(pre_img_lin, size(img, 1), size(img, 2), 3);

pre_img = (pre_img - imin) ./ (imax - imin);
%figure(1);
%imshow(pre_img);
%waitforbuttonpress();
