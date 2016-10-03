%LAST UPDATED Nov 24th, 2011
%SHIDA@cvc.uab.es 

function SC=makeSC_labelAngErr(labels)

SC=zeros(size(labels,1));

for i=1:size(labels,1)
        cost=angDist(labels,labels(i,:));
        SC(i,:)=cost';
end