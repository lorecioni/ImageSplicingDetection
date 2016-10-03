%LAST UPDATED Nov 24th, 2011
%SHIDA@cvc.uab.es 

function [Mb Ms]=solveMbMsPositive2(f,cb,cs)

%-----------------------------------------------------------------------
% solving Ax=b using x=inv(A)*b where x is [Mb Ms]' and A is [cb 0;0 cs]
% and b is Fi+D*P
% rgbSize is the n*m size of the image
%-----------------------------------------------------------------------

%fiDP=getFiDP( f,cb,cs);  %we realize that this part which was for managing
%the errors wasn�t necessary after all since the pinv function already
%takes care of it.

A=[cb(1) cs(1);cb(2) cs(2);cb(3) cs(3)]; 
A=real(A);

ainv=pinv(A);
mbms=ainv * f';

%%%Here we want to only fix those pixels whos mb or ms (or both)
%%%are negative (so we build a mask) : [I know the following code could 
%%%have been written in a much more efficient way but that's the way it is!]

% First those whose ms is negative (by putting them to zero and figuring
% out the corresponding mb)
sMask=sign(mbms(2,:)) <0;
indx=find(sMask); b=f(indx,:)';
mx=pinv(cb')*b;
Mb=mbms(1,:);
Ms=mbms(2,:);
Mb(indx)=mx;
Ms(indx)=zeros(size(indx,1));

% Second those whose mb is negative:
bMask=sign(mbms(1,:)) <0;
indx=find(bMask);

% putting them to zero and figuring out the corresponding ms
% b=f(indx,:)';
% mx=pinv(cs')*b;
% Mb=mbms(1,:);
% Ms=mbms(2,:);

%just remove them when the body color doesn't match
Ms(indx)=zeros(size(indx,1));%mx;  
Mb(indx)=zeros(size(indx,1));


%Mb=reshape(Mb,rgbSize(1),rgbSize(2));
%Ms=reshape(Ms,rgbSize(1),rgbSize(2));
