%LAST UPDATED Nov 24th, 2011
%SHIDA@cvc.uab.es 

function DC=makeDC_40labels_minIntens(im,superpix,supixCL,labels,cL,iL)
% INPUTS:
%     im= rgb image n X m X 3
%     superpix= superpixel segmentation n X m
%     supixCL= object color estimated by robust estimator per superpixel
%     it's #superpixels X 3  (This input is not really necessary but it is
%     used to speed the process by avoiding the superpixels with low
%     intensity, small and ... This can be removed if not needed
%     labels= a 40 X 2 , that is for each label (1:40) the first colomn is
%     the object color index and the second is the illuminant color index
%     cL= 20 X 3   Object colors
%     iL= 2 X 3    Illuminant colors
% OUTPUT:
%     DC= #superpixels X #labels : Datacost is per superpixel and per label

DC=zeros(max(superpix(:)),size(labels,1));
im=reshape(im,size(im,1)*size(im,2),3);

for i=1:max(superpix(:)) % for each supix
    ind=find(superpix==i);
    %Remove the "unwanted" superpixels (for which no object color could be
    %estimated) this is to speed up only and avoid noisy segments
    %REMOVE THIS PART IF YOU DON'T WANT IT
    if sum(supixCL(i,:))==0 %size(ind,1)<1
        %DC(i,:)=repmat([0],1,size(labels,1));
        continue
    end
    %pixels in the superpixel
    f=im(ind,:);
    for j=1:size(labels,1) % for each label
        %Given each patch, and for each choice of DRM model (C and l )
        %model the patch by DRM compute the reconstruction error between
        %original and reconstructed
        cost=computeDRMcost(f,cL(labels(j,1),:),iL(labels(j,2),:));
        DC(i,j)=cost;
    end
end
