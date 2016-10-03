%LAST UPDATED Nov 24th, 2011
%SHIDA@cvc.uab.es 

function [lightMask colorMask]=visGraphCut_CandL(L,superpix,labels,cL,iL)

%gamma=2.4;

lightMask=zeros(size(superpix,1)*size(superpix,2),3);
colorMask=zeros(size(superpix,1)*size(superpix,2),3);
for i=1:max(superpix(:)) %for each superpixel
    indx=find(superpix==i);
    lightMask(indx,:)=repmat(iL(labels(L(i),2),:),size(indx,1),1);
    colorMask(indx,:)=repmat(cL(labels(L(i),1),:),size(indx,1),1);
end
lightMask=reshape(lightMask,[size(superpix,1) size(superpix,2) 3]);
colorMask=reshape(colorMask,[size(superpix,1) size(superpix,2) 3]);
%figure; imshow(uint8(lightMask))
%figure; imshow(uint8(colorMask))



% figure; imshow(uint8(gammacorrection(lightMask,1/gamma)))
% figure; imshow(uint8(gammacorrection(colorMask,1/gamma)))