dsPath='/net/cv/illum/multi_shida/img/'; % 'D:\Research\MatlabCode\Erlangen\CVPR2012_ReEstimated\img\';
% gtPath='D:\Research\MatlabCode\Erlangen\CVPR2012_ReEstimated\gt\';
gtPath='/net/cv/illum/multi_shida/gt_50/' % 'D:\Research\MatlabCode\Erlangen\CVPR2012_ReEstimated\gt_50\';
illDiffPath='/disks/data1/riess/mimorem/table_shida/diffuse/random_forest/gt_50/'; % 'D:\Research\MatlabCode\Erlangen\CVPR2012_ReEstimated\diffuse\';
illSpecPath='/disks/data1/riess/mimorem/table_shida/specular/vote/voting_lenient_6000/'; %'D:\Research\MatlabCode\Erlangen\CVPR2012_ReEstimated\voting_lenient_6000\';
% illSpecPath='D:\Research\MatlabCode\Erlangen\calibrated_output\iebv_2000_v70_30\';
supPath='/net/cv/illum/multi_shida/seg_100_30/'; %'D:\Research\MatlabCode\Erlangen\CVPR2012_ReEstimated\seg_100_30\';


%LAST UPDATED Nov 24th, 2011
%SHIDA@cvc.uab.es 

%%% dsPath='D:\Research\MatlabCode\Erlangen\CVPR2012_ReEstimated\img\';
%%% % gtPath='D:\Research\MatlabCode\Erlangen\CVPR2012_ReEstimated\gt\';
%%% gtPath='D:\Research\MatlabCode\Erlangen\CVPR2012_ReEstimated\gt_50\';
%%% illDiffPath='D:\Research\MatlabCode\Erlangen\CVPR2012_ReEstimated\diffuse\';
%%% illSpecPath='D:\Research\MatlabCode\Erlangen\CVPR2012_ReEstimated\voting_lenient_6000\';
%%% % illSpecPath='D:\Research\MatlabCode\Erlangen\calibrated_output\iebv_2000_v70_30\';
%%% supPath='D:\Research\MatlabCode\Erlangen\CVPR2012_ReEstimated\seg_100_30\';

dsFiles= dir(fullfile(dsPath,'*.png'));
dslength = length(dsFiles);
imFullNames=cell(dslength,1);
for i=1:dslength
    imFullNames{i}=dsFiles(i,1).name(1:end-4);% fullfile(dsPath,dsFiles(i,1).name);
end

% subSet=[10 12 14 26 28 31 42 44 46 50 52 54 64 66 68 72 74 76];
% subSet=[26:31 42:47 64:69 72:77]; %DIFFUSE
% subSet=[1:32 41:86];
subSet=[2:7 10:15 18:23 26:31 42:47 50:55 58:61 64:69 72:77 80:85];
imFullNames=imFullNames(subSet);

% errsMIMO=zeros(size(imFullNames,1),1);
% errsSpec=zeros(size(imFullNames,1),1);
% errsDiff=zeros(size(imFullNames,1),1);

% load errsSpec
% load errsDiff
% errsSpec=errsSpec(subSet);
% errsDiff=errsDiff(subSet);
for itr=49%1:size(imFullNames,1) % FOr each image in the dataset
    itr
   imName=imFullNames{itr};
   supName=[imName(1:end-4) '_seg.png'];
%    im=imread([dsPath imName '.png']);
   gt=double(imread([gtPath imName '_gt.png']));
   gt=imresize(gt,.2,'nearest');
%    figure; imshow(uint8(gt))
   illSpecName=[imName '_6000.png'];
   illDiffName=[imName '_10000.png'];
   specIm=double(imread([illSpecPath illSpecName]));
   diffIm=double(imread([illDiffPath illDiffName]));
   specIm=imresize(specIm,.2,'nearest');
   diffIm=imresize(diffIm,.2,'nearest');
   adistDiff=angDistPixwise(gt,diffIm);
   adistSpec=angDistPixwise(gt,specIm);
%    figure; imshow(uint8(specIm))
%    figure; imshow(uint8(diffIm))
   errsSpec(itr)=mean(adistSpec);
   errsDiff(itr)=mean(adistDiff);
   tic
   run_mimorem
%    run_mimorem_illPlane
%     run_mimorem_onlyLightDiff
%     run_mimorem_onlyLight_DiffSpec
%     run_mimorem_Spec_OnlyConf
%    run_mimorem_onlyLightSpec
   toc
% 
   adist=angDistPixwise(double(gt),lightMask);
   errsMIMO(itr)=mean(adist);
   mean(adist)
   %adist=angDistPixwise(double(im),repmat(reshape([1 1 1],[1 1 3]),[size(im,1) size(im,2) 1]));
   
end
median(errsMIMO)*180/pi
median(errsDiff)*180/pi
median(errsSpec)*180/pi

% [errsMIMO(itr) errsSpec(itr) errsDiff(itr)]./pi*180

% figure; plot(1:size(errsMIMO(:),1),errsMIMO,'b-o');
% hold
% plot(1:size(errsMIMO(:),1),errsDiff,'m-+')
% plot(1:size(errsMIMO(:),1),errsSpec,'r-*')
% %plot(errsSpec+errsDiff,'c.')
% grid on
% % legend('MIMO','Diff','Spec','Spec Diff');
% legend('MIMO','Diff','Spec');
% hold


%PLOT THE RESULTS PER IMAGE
lineWid=2;
figure; plot(1:size(errsMIMO(:),1),errsMIMO*180/pi,'b-o','LineWidth',lineWid);
hold
plot(1:size(errsMIMO(:),1),errsDiff*180/pi,'m-+','LineWidth',lineWid)
plot(1:size(errsMIMO(:),1),errsSpec*180/pi,'r-*','LineWidth',lineWid)
%plot(errsSpec+errsDiff,'c.')
grid on
% legend('MIMO','Diff','Spec','Spec Diff');
legend('MIMO','Diff','Spec');
line([1,size(errsMIMO(:),1)],[median(errsMIMO)*180/pi,median(errsMIMO)*180/pi])
line([1,size(errsMIMO(:),1)],[median(errsDiff)*180/pi,median(errsDiff)*180/pi])
line([1,size(errsMIMO(:),1)],[median(errsSpec)*180/pi,median(errsSpec)*180/pi])
xlabel('Image number','Fontsize',14);
ylabel('Estimation Error (degrees)','Fontsize',14);
hold


