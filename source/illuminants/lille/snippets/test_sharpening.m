image_file = '/net/cv/illum/multi_shida/img/mspec_r_b.png';

img = double(imread(image_file));
img = img ./ max(max(max(img)));

post = [ 2.57802961667760  -1.32188044638028  0.649329484660944;
        -2.52336650503095   3.07256263980345 -1.52191312391749;
         0.625417860539208 -1.32260040379914  1.77407732591195];

pre = [-0.00000779733186107023 -0.00101367902088967   0.0141474767010052;
        0.00000427190873786287  0.00674077064132028  -0.00312999210138898;
        0.00732611407791239  -0.000690377156493227  0.00177821818671831];

img_lin = reshape(img, size(img, 1)*size(img, 2), 3);

pre_img_lin = pre * img_lin';
post_img_lin = img_lin * post;

post_img = reshape(post_img_lin, size(img, 1), size(img, 2), 3);
pre_img = reshape(pre_img_lin', size(img, 1), size(img, 2), 3);

min_post_img = min(min(min(post_img)))
min_pre_img = min(min(min(pre_img)))

post_img = post_img - min_post_img;
post_img = post_img / max(max(max(post_img)));

pre_img(pre_img < 0) = 0;
pre_img = pre_img / max(max(max(pre_img)));


%Matrices:
%Positivity:
%premultiply:
%-7,79733186107023e-06    -0,00101367902088967    0,0141474767010052
%4,27190873786287e-06    0,00674077064132028    -0,00312999210138898
%0,00732611407791239    -0,000690377156493227    0,00177821818671831
%Zickler method's
%postmultiply:
%2,57802961667760    -1,32188044638028    0,649329484660944
%-2,52336650503095    3,07256263980345    -1,52191312391749
%0,625417860539208    -1,32260040379914    1,77407732591195

