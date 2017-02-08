function sensorCorrelation(filename,outFile,catType,plots)
%sensorCorrelation(filename,outFile,catType,plot)
% Performs the sensor correlation method for illuminant estimation and then
% a chromatic adaptation transform for correction.
% Set plot = 0 or 1 to turn diagnostic plots on or off.

% clear
tutorialinit
load refGamut.mat %a sample set of reference gamuts simulated by ISET for mired = 118:23.5:400

xyz_D65 = [95.04; 100; 108.88]; %http://en.wikipedia.org/wiki/D65, normalized Y = 100
%% import image
im_orig = cbimread(filename);
% figure
% imshow(im_orig)
% title('Original Image')


%% sensor correlation
clipVal = 225;

imRGB_orig = cbreshape(im_orig)*255;
imRGB = imRGB_orig;

%% pre-process
imRGB = imRGB(:,max(imRGB,[],1)<clipVal+1); %simplify by ignoring pixels with any RGB value >= 225+1 since we will saturate later

%populate color space
imRGBSpace = zeros(clipVal+1,clipVal+1,clipVal+1,'uint8');
for i=1:size(imRGB,2)
    c = imRGB(:,i);
    imRGBSpace(c(1)+1,c(2)+1,c(3)+1) = imRGBSpace(c(1)+1,c(2)+1,c(3)+1) || 1;
end

%compute connectivity lookup table
% imRGBSpaceConn = imfilter(imRGBSpace,conndef(size(imRGB,1),'maximal')).*imRGBSpace>2; %only keep colors that are connected to more than 2 neighbors
%the above is the truly correct way but since we'll never access the color
%entries not in the image, we don't need to zero them out, this is good enough and faster
imRGBSpaceConn = imfilter(imRGBSpace,conndef(size(imRGB,1),'maximal'))>2; %must have more than 1 neighbor

imRGBConn = false(1,size(imRGB,2));
for i=1:length(imRGBConn)
    c = imRGB(:,i);
    imRGBConn(i) = imRGBSpaceConn(c(1)+1,c(2)+1,c(3)+1);
end
imRGB = imRGB(:,imRGBConn); %keep only pixels in the image with connected color values
 clear imRGBSpaceConn imRGBSpace imRGBConn

imRGB = imRGB(:,max(imRGB,[],1)<clipVal); %ignore pixels with any RGB value >= 225, saturated

% normalize
I = zeros(1,size(imRGB_orig,2));
for i = 1:length(I)
    I(i) = norm(imRGB_orig(:,i)); %sqrt(R^2+G^2+B^2)
end
imRGB = 255*imRGB/max(I);

%%
[gamut_im,gamutArea_im] = convhull(imRGB(1,:),imRGB(3,:)); %image gamut is the convex hull of (R,B) points
[x,y] = poly2cw(imRGB(1,gamut_im), imRGB(3,gamut_im));
gamut_im = [x; y]; %the image gamut vertices

% gamut_illum =[]; %gamuts of typical illuminants %2xnx# illum
% gamutArea_illum = []; % # illum x 1

kScale = .1:.1:1;
corr = zeros(length(kScale),length(gamutArea_illum)); %rows are scaling, cols are different illuminants
for j=1:length(kScale)
    k = kScale(j);
    for i = 1:length(gamut_temps)
        gi = gamut_illum{i};
        [x,y] = polybool('&',k*gamut_im(1,:),k*gamut_im(2,:),gi(1,:),gi(2,:));
        corr(j,i) = polyarea(x,y)/sqrt(gamutArea_im*gamutArea_illum(i));
    end
end
% corr
[corr,k] = max(corr,[],1);
[maxCorrVal,targetIllum] = max(corr);
k = k(targetIllum);
T = gamut_temps(targetIllum)

%% correct
xyzEst = xy2XYZ(XYZ2xy(gamut_XYZ(:,targetIllum)),100); %xyz color cast estimate normalized Y to 100

% catType = 'vonKries';

imRGB = imRGB_orig;
imRGB = cbCAT(xyzEst,xyz_D65,catType)*imRGB;

% figure
% imshow(cbunshape(imRGB,size(im_orig))/255)
% title(['Sensor Correlation Corrected, T=' num2str(T)])

imwrite(cbunshape(imRGB,size(im_orig))/255,outFile,'png');
%% plot
if plots
    figure,clf
    title('Gamut Plots')
    for i=1:length(gamut_temps)
        gi = gamut_illum{i};
        x = gi(1,:);
        y = gi(2,:);
        hold on
        if i == targetIllum
            patch(x, y, 1, 'FaceColor', 'b')
        else
            patch(x, y, 1, 'FaceColor', 'g')
        end
        hold off
    end
    hold on
    % plot(imRGB(1,:),imRGB(3,:),'.')
    patch(gamut_im(1,:), gamut_im(2,:), 1, 'FaceColor', 'r')
    hold off
    print(gcf,'-dpng',[outFile '-fig1'])
end
% [k2,a2] = convhull(x2,y2);
% 
% [xp1,yp1] = poly2cw(x1(k1),y1(k1));
% [xp2,yp2] = poly2cw(x2(k2),y2(k2));
% % xp1 = x1(k1); yp1 = y1(k1);
% % xp2 = x2(k2); yp2 = y2(k2);
% 
% 
% [x,y] = polybool('&',xp1,yp1,xp2,yp2);
% 
% % figure(10),clf
% % hold on
% % patch(x, y, 1, 'FaceColor', 'g')
% % plot(x1,y1,'.',x2,y2,'r.')
% % plot(x1(k1),y1(k1),'-',x2(k2),y2(k2),'r-')
% % hold off
% 
% corr = polyarea(x,y)/sqrt(a1*a2)
end