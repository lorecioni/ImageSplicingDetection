img_file = '/net/cv/illum/multi_shida/img_shpst/spdif2_w_b.png';
img_file = '/net/cv/illum/multi_shida/img/spdif2_w_b.png';
img_file = '/net/cv/forensics/battiato/images/ucid00072.tif';
o3 = retinex_jobson(img_file);

figure(1);
imshow(o3);
%colormap(jet);
%colorbar
%o3 = o2 - min(min(min(o2)));
%o3 = o3 ./max(max(max(o3)));
%imshow(o3);
