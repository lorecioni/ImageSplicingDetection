%LAST UPDATED Nov 24th, 2011
%SHIDA@cvc.uab.es 

function superpix=loadSupix(fullName,scale)

%--Load supixs
seg=imread(fullName);
if nargin>1
    seg=imresize(seg,scale,'nearest');
end

size1=size(seg,1);
size2=size(seg,2);
%-Make it double and grayscale superpixel index
green=dec2bin(reshape(seg(:,:,2),size1*size2,1));
red=double(reshape(seg(:,:,1),size1*size2,1));
g=(green=='1'); 
if size(g,2)<3
    g=[zeros(size(g,1),3-size(g,2)) g];
end
superpix=red + (g(:,1)* 2^10) + (g(:,2)* 2^9) + (g(:,3)* 2^8) ;
% clear g
% clear red
superpix=reshape(superpix,size1, size2)+1; %starting from 1 instead of zero
% figure; imshow(superpix,[])
% save('superpix.mat', 'superpix');
% load superpix.mat