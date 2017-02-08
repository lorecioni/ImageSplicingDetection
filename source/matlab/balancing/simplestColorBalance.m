function simplestColorBalance(filename,outFile,satLevel,plots)
%simplestColorBalance(filename,outFile,satLevel,plot)
% Performs color balancing via histogram normalization.
% satLevel controls the percentage of pixels to clip to white and black.
% Set plot = 0 or 1 to turn diagnostic plots on or off.

tutorialinit

%% import image
im_orig = cbimread(filename);
% figure
% imshow(im_orig)
% title('Original Image')


%% full width histogram method
% satLevel = .01; %percentage of the image to saturate to black or white, tweakable param
q = [satLevel/2 1-satLevel/2];

imRGB_orig = cbreshape(im_orig)*255;
imRGB = zeros(size(imRGB_orig));
N = size(imRGB_orig,2);
color = {'r','g','b'};
for ch = 1:3
    if plots
        figure
        subplot(211)
        hist(imRGB_orig(ch,:),256)
        set(findobj(gca,'Type','patch'),'FaceColor',color{ch},'EdgeColor',color{ch})
        xlim([0 255])
        title('Original Histogram')
    end
    tiles = quantile(imRGB_orig(ch,:),q);
%     [sum(imRGB_orig(ch,:)<tiles(1))/N,sum(imRGB_orig(ch,:)>tiles(2))/N] %check percentages are correct
    imRGB(ch,:) = cbsaturate(imRGB_orig(ch,:),tiles); %saturate at the appropriate pts. in distribution
    bottom = min(imRGB(ch,:)); top = max(imRGB(ch,:));
    imRGB(ch,:) = (imRGB(ch,:)-bottom)*255/(top-bottom);
    
    if plots
        subplot(212)
        hist(imRGB(ch,:),256)
        set(findobj(gca,'Type','patch'),'FaceColor',color{ch},'EdgeColor',color{ch})
        xlim([0 255])
        title('Corrected Histogram')
    end
    print(gcf,'-dpng',[outFile '-fig' num2str(ch)])
end

imwrite(cbunshape(imRGB,size(im_orig))/255,outFile,'png');
% figure
% imshow(cbunshape(imRGB,size(im_orig))/255)
% title('Simplest Color Balance Corrected')

