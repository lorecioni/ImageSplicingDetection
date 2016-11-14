function [o3] = retinex_jobson(img_file);
img = double(imread(img_file));

h1 = fspecial('gaussian', [400 400], 250);
h2 = fspecial('gaussian', [200 200], 80);
h3 = fspecial('gaussian', [30 30], 10);

w = [1.0/3.0 1.0/3.0 1.0/3.0]; % weights for combination

for c = 1:3 % 3
	i = img(:,:,c);
	i(i == 0) = 1;

	f1 = log(conv2(i, h1, 'same'));
	f2 = log(conv2(i, h2, 'same'));
	f3 = log(conv2(i, h3, 'same'));
	logi = log(i);
	r = (w(1, 1)*(logi-f1)) + (w(1, 2)*(logi-f2)) + (w(1, 3)*(logi-f3));
	o2(:,:,c) = r;
end

%figure(1);
%minO2 = min(min(min(o2)))
%maxO2 = max(max(max(o2)))
o3 = (o2-minO2) ./ (maxO2-minO2);
%imshow(o3);

%colormap(jet);
%colorbar
%o3 = o2 - min(min(min(o2)));
%o3 = o3 ./max(max(max(o3)));
%imshow(o3);
