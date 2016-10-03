%LAST UPDATED Nov 24th, 2011
%SHIDA@cvc.uab.es 

function [illList]=loadIllum(fullname,scale)
%--Read the illuminant estimation
%--Label pixels by their illuminants

ilmg=double(imread([fullname]));
if nargin>1
    ilmg=imresize(ilmg,scale,'nearest');
end
 figure; imshow(uint8(ilmg))
ilF=reshape(ilmg,[size(ilmg,1)*size(ilmg,2) 3]); % n by 3 RGB
pixIllCode=ilF(:,1)*10^6+ilF(:,2)*10^3+ilF(:,3); %-Hashing the pixel rgb values!!
illListCode=unique(pixIllCode); %-Extract illuminants list
% Removing the NoIlluminant on Christian images
% illListCode(find(illListCode==85085085))=[]; 
% illListCode(find(illListCode==71081051))=[]; %No estimate: In Diffuse
% illListCode(find(illListCode==1001001))=[];  %No estimate: In Spec
illList=[floor(illListCode/10^6), floor(rem(illListCode,10^6)/10^3), rem(illListCode,10^3)];

%-Label pixels by their illuminants
% lightInds=zeros(size(ilF,1),1); % the light indx assigned to each pix
% for i=1:size(illList,1)
%     ind=find(pixIllCode==illListCode(i));
%     lightInds(ind)=i;
% end