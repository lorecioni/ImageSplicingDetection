function grayWorld(filename,outFile,catType,maxIter,plots)
%grayWorld(filename,outfile,catType,maxIter,plot)
% Performs color balancing via the gray world assumption and a chromatic
% adpatation transform.  Can be run interatively to possibly improve the
% result.  Set plot = 0 or 1 to turn diagnostic plots on or off.


%% import image
im_orig = cbimread(filename);
% figure
% imshow(im_orig)
% title('Original Image')

%% various constants
xyz_D65 = [95.04; 100; 108.88]; %http://en.wikipedia.org/wiki/D65, normalized Y = 100

sRGBtoXYZ =   [0.4124564  0.3575761  0.1804375; ...
               0.2126729  0.7151522  0.0721750; ...
               0.0193339  0.1191920  0.9503041];

%% grayworld
% catType = 'vonKries';
% maxIter = 1;
b = 0.001; %convergence threshold

imRGB_orig = cbreshape(im_orig)*255;
imRGB = imRGB_orig;

grayDiff =[];
for iter = 1:maxIter
    rgbEst = mean(imRGB,2); %grayworld, average everything
    grayDiff = [grayDiff norm([rgbEst(1)-rgbEst(2),rgbEst(1)-rgbEst(3),rgbEst(2)-rgbEst(3)])];
    
    if grayDiff(end) < b
        disp(['Converged. RGB difference vector < ' num2str(b) ' in magnitude.'])
        break
    elseif iter >= 2 && abs(grayDiff(end-1)-grayDiff(end)) < 10^-6
        disp(['RGB difference vector no longer improving.'])
        break
    end
    
    xyEst = XYZ2xy(sRGBtoXYZ*rgbEst); %calculate xy chromaticity
    xyzEst = xy2XYZ(xyEst,100); %normalize Y to 100 so D65 luminance comparable
    imRGB = cbCAT(xyzEst,xyz_D65,catType)*imRGB;
end

imwrite(cbunshape(imRGB,size(im_orig))/255,outFile,'png');

if plots
    length(grayDiff) %number of iterations done
%     figure
%     imshow(cbunshape(imRGB,size(im_orig))/255)
%     title('Gray World Corrected')
    
    figure
    plot(grayDiff)
    title('GW: Norm of RGB Difference Vector vs Iterations')
    print(gcf,'-dpng',[outFile '-fig1'])
end
