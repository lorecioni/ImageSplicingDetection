%LAST UPDATED Nov 24th, 2011
%SHIDA@cvc.uab.es 

%Estimates the C.*L using Robust estimator [Beigpour and van de Weijer
%2011] for pixels in a superpixel with minimum intensity (per channel) of
%minIntThr

function model=supix2CL_minIntens(im,superpix,minIntThr)
%The RGB value of the C*l estimation given by the robust estimator per
%superpixel

if (nargin<3)
    % Min Intensity Threshold for which we consider estimating an object color for the patch 
    minIntThr=50; 
end

model=zeros(max(superpix(:)),3);
imf=reshape(im,size(im,1)*size(im,2),3);
for i=1:max(superpix(:)) %for each label
    ind=find(superpix==i);
    f=imf(ind,:);
    %Only consider superpixels that have enough non-zero or low intensity
    %pixels.
    if sum(sum(f))/(3*size(f,1))>minIntThr
        %Uncomment to see the scaled intensity patch for visualization only
        %figure; imshow(uint8(double(im*3).*repmat(superpix==i,[1 1 3])))
        cbSVD=robust(double(f),5,10);
        model(i,:)=cbSVD';
        %Uncomment to see the estimated object color
        %showcolor(cbSVD,50)
    end
end