%LAST UPDATED Nov 24th, 2011
%SHIDA@cvc.uab.es 

function adist=angDistPixwise(im1,im2)
%Pixelwise Angular distance between images
%im1=GT
%im2=estimate

if size(im1,1)*size(im1,2)~=size(im2,1)*size(im2,2)
    fprintf('Error: Sizes do not match.\n');
    return
end

%vectorize
s1=size(im1,1); s2=size(im1,2);
im1=reshape(im1,size(im1,1)*size(im1,2),3);
im2=reshape(im2,size(im2,1)*size(im2,2),3);

%normalize
im1n=im1./repmat(sqrt(sum(im1.^2,2))+eps,[1 3]);
im2n=im2./repmat(sqrt(sum(im2.^2,2))+eps,[1 3]);

%DOT product and angular error
dotp=sum(im1n.*im2n,2);
adist=real(acos(dotp));

% figure; imshow(uint8(reshape(adist,s1,s2)))

%Removing the zero pixels (zero in GT image)
indx=find(sum(im1,2)==0);
adist(indx)=[];

% % %removing low intensity pixels
% % thr=5; %intensities lower than thr for im1 are removed
% % %easy fix
% % adist(sum(im1,2)<thr)=0;



