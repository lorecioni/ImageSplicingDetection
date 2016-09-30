%LAST UPDATED Nov 24th, 2011
%SHIDA@cvc.uab.es 

function con=findConnections2(value)
%-Find supix connections
con=spalloc(max(value(:)),max(value(:)),4585); %with room to eventually hold NZMAX nonzeros
temp=zeros(size(value,1),size(value,2),4);
temp(:,:,1)=[value(2:end,:); zeros(1,size(value,2))];
temp(:,:,2)=[value(:,2:end), zeros(size(value,1),1)];
temp(:,:,3)=[zeros(1,size(value,2)); value(1:end-1,:)];
temp(:,:,4)=[zeros(size(value,1),1), value(:,1:end-1)];
% temp(:,:,3)=[value(2:end,2:end), zeros(size(value,1)-1,1); zeros(1,size(value,2))];

temp=reshape(temp,size(value,1)*size(value,2),4);
value=reshape(value,size(value,1)*size(value,2),1);
% indices=cell(max(value(:)),1);
for i=1:max(value) % from 1 : # of supixs
    ind=find(value==i); % inds of the pixs who's supix is i
    ind=temp(ind,:); % inds of the neighbors of pixs whose supix is i
    ind=unique(ind); % remove duplicate
    ind=ind(ind>0); % don't consider the 0 pading
    ind=ind(:);    
    ind(ind==i)=[]; %removing self
    con(i,ind)=1;
end