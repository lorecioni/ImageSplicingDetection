%%
%reads in the saved MacBeth checker images under ISET simulated illuminants and computes the image gamut
clear
tutorialinit
load XYZ
XYZ = data;

mired = [118:23.5:400]';
T = cbmired2temp(mired);
gamut_illum ={}; %gamuts of typical illuminants %2xnx# illum
gamut_temps = T;
gamutArea_illum = []; % # illum x 1
gamut_XYZ = [];
for i = 1:length(T)
    gamut_XYZ = [gamut_XYZ XYZ'*blackbody(wavelength,T(i))];
end
%%

refs = {'118','141','165','189','212','236','259','283','306','330','353','376','400'};
figure(10),clf
title('Reference Gamuts from ISET Macbeth Simulation, 118-400 mired')


%%
for i = 1:length(refs)
    imRGB = cbimread(['Images/Macbeth-BBR/' refs{i} '.tif'])*255;
    imRGB_orig = cbreshape(imRGB);
    
    [gamut_im,gamutArea_im] = convhull(imRGB_orig(1,:),imRGB_orig(3,:)); %image gamut is the convex hull of (R,B) points
    [x,y] = poly2cw(imRGB_orig(1,gamut_im), imRGB_orig(3,gamut_im));
    gamut_im = [x; y]; %the image gamut vertices
    
    gamut_illum =[gamut_illum; gamut_im]; %gamuts of typical illuminants %2xnx# illum
    gamutArea_illum = [gamutArea_illum; gamutArea_im];
    
    figure(10),hold on
    patch(x, y, 1)
    hold off
    xlabel('R'),ylabel('B')
end

save refGamut.mat gamut_illum gamut_temps gamutArea_illum gamut_XYZ