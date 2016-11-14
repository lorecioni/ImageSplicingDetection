%LAST UPDATED Nov 24th, 2011
%SHIDA@cvc.uab.es 

function cost=computeDRMcost(f,C,l)
%given each patch, and for each choice of DRM model (C and l )
%model the patch by DRM
cl=(C.*l)/norm(C.*l)*255;
%This function decomposes the pixels to mb and ms
[mb ms]=solveMbMsPositive2(double(f),cl,l);
%only positive
% mb(mb<0)=0;
% ms(ms<0)=0;

%Recontruct the image
reconst=mb'*cl+ms'*l;
%compute the reconstruction error between original and reconstructed
diff=(double(f) - real(reconst)) .^ 2;
cost=sqrt(sum(diff(:)));
%cost=sqrt(sum(diff(:)))/size(f,1); %Normalizing by the size of the patch
%??But maybe we should have more penalty when the a bigger patch is estimated
%wrongfully??