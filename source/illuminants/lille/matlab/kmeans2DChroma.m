%LAST UPDATED Nov 24th, 2011
%SHIDA@cvc.uab.es 

function [IDX, CC]=kmeans2DChroma(rgb,k)

iR=rgb(:,1)./ sum(rgb,2);
iG=rgb(:,2)./ sum(rgb,2);
data=[iR iG];

[IDX, centers] = kmeans(data, k);

CC=[centers 1-sum(centers,2)]*255;