%LAST UPDATED Nov 24th, 2011
%SHIDA@cvc.uab.es 

function [cb perp]=robust(f,scale,maxItr)

%One example for calling this function:  [cbSVD PerpCb]=robust(f,5,10);

%Maximum number of iteration in the case in doesn't converge
if(nargin<3)
    maxItr=5;
end

%Scale for the gausian
if(nargin<2)
    scale=5;
end

% % Look for the darker pixels as more likely to be diffuse
% m=sum(f,2)<3*100; 
% f=f.*repmat(m,[1 3]) ;
%Get the initial eigen vector estimate
[U,S,V]=svd(f'*f);

%
for itr=1:maxItr
    %the current estimated direction of object color
    est=V(:,1);
    %The distance of a pixel color value to the current
    %estimation vector
    dist=sum((f-(f*est)*est').^2,2);
    %The Gaussian error weighting based on the computed distance of the
    %pixels
    weight=exp(-dist./(2*scale^2));
    %weighting the data
    wf=repmat(weight,1,3).*f;
    %Re-estimating the direction using the weighted pixels
    [U,S,V]=svd(wf'*wf);
    %CONVERGENCE: If the estimation vector doesn't change
    v=V(:,1)./norm(V(:,1));
    est=est./norm(est);    
    a=acos(dot(v,est))/pi*180;
    if a<1
        break;
    end
end
%Final estimate: the direction (unit vector) of the object color
cb=V(:,1)./norm(V(:,1));


% f_cs=f-(f*cb)*cb';
% [U,S,V]=svd(f_cs'*f_cs);

%IF NEEDED: The perpendicular direction can also be reported
perp=V(:,1)./norm(V(:,1));

%Flip the direction to make it positive
cb=cb * -255;
perp= perp * -255;

%If the estimation fails, the final value is reported as zero
if min(cb)<0 %if one of the directions are negative then it shows the algorithm failed
    cb=[0 0 0];
end
