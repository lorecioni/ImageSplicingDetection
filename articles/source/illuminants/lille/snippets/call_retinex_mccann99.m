clear;
img_file = '/net/cv/forensics/battiato/images/ucid00072.tif';
img_file = '/disks/data1/riess/code/reflectance/lille/snippets/small_spdif2_w_b_shpst.png';
img_file = '/disks/data1/riess/code/reflectance/lille/snippets/small_spdif2_w_b.png';

img = double(imread(img_file));
if (size(img, 1) == 510) % hack to make shidas images working: dimension is
                         % 768x510, it would factor nicely if dimension were 768x512
	img = [img; ones(2, size(img, 2), 3)];
end
img(img == 0) = 1;
img = log(img);
for c = 1:3
	r(:,:,c) = retinex_mccann99(img(:,:,c), 10);
end

minR = min(min(min(r)));
maxR = max(max(max(r)));
s = (r - minR) ./ (maxR - minR);
figure(1);
imshow(s);
