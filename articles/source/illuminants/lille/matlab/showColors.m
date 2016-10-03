%LAST UPDATED Nov 24th, 2011
%SHIDA@cvc.uab.es 

function showColors(colors,length,gamma)

if nargin < 2
    length=100;
end

width=400;

if size(colors,1)>1  % matrix of colors
    pic=zeros(length*size(colors,1),width,3);
    for i=1:size(colors,1)
        pic((i-1)*length+1:i*length,:,:)=repmat(reshape(colors(i,:),[1 1 3]),[length width 1]);
    end
    
else
    pic=repmat(reshape(colors,[1 1 3]),[length width 1]);
end

if nargin > 2
    pic=real(gammacorrection(pic,1/gamma));
end

figure; imshow(uint8(pic))