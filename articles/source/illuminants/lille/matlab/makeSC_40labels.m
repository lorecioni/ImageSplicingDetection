%LAST UPDATED Nov 24th, 2011
%SHIDA@cvc.uab.es 

function SC=makeSC_40labels(labels,cL,iL)

SC=zeros(size(labels,1));

for i=1:size(labels,1)
    for j=i+1:size(labels,1)
        ccost=angDist(cL(labels(i,1),:),cL(labels(j,1),:));
        icost=angDist(iL(labels(i,2),:),iL(labels(j,2),:));
        SC(i,j)=ccost+icost;
        SC(j,i)=SC(i,j);
    end
end