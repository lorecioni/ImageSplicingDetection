%LAST UPDATED Nov 24th, 2011
%SHIDA@cvc.uab.es 

%Oct 31th, 2011


% 1. Import the image, superpixels, illuminant estimations (2 files, Spec. and Diffuse)
% 2. Decide on the "specularity level" of the superpixel and generate a map
% 3. Cluster the illuminants with meanshift, choose top 10 most populated
% centers: 10 of spec and diff
% 4. Get the CL per supix: [divide by Ld and Ls and cluster all and take 10
% most populated]
% 5. For each pair of illuminants (withing the 10) perform Graphcut with
% labels being: on both ill and obj color

% % % % % %   - only illuminants
% % % % % %   - only object color
% % % % % %   - both => in this case take set of object colors using kmeans 


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%Initialization
% gamma=2.4;
% writeFlag=0;
% numAllill=10; % # all illuminants
% numIll=2;     % # chosen illuminants
% numObj=10;    % # object colors


% imName='IMG03287';
% %resPath='D:\Research\MatlabCode\Graph Cut\Images\MIMOREM\mim1_';
% %estimations path
% supPath='D:\Research\MatlabCode\Erlangen\calibrated_output\iebv_2000_v70_30\';
% illSpecPath='D:\Research\MatlabCode\Erlangen\calibrated_output\iebv_2000_v70_30\';
% illDiffPath='D:\Research\MatlabCode\Erlangen\CVPR2012 Project\gw\img\';
% illSpecName=['iic_' imName '_16bit_2000_est.png'];
% illDiffName='spdif_r_b_0_1_1.png';%['iic_' imName '_16bit_2000_est.png'];
% supName=['seg_idx_' imName '_16bit.png'];
% im16Path='D:\Research\MatlabCode\Graph Cut\Dataset_16bit\';
% 
% % %--Read the calibrated 12bit image
% im16=imread([im16Path imName '_16bit.png']);
% im=im16/2^4;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 1. Import the image, superpixels, illuminant estimations (2 files, Spec. and
% Diffuse)

%-Load the image
im16=imread([dsPath imName '.png']);
im=im16/2^4;
im=imresize(im,.2,'nearest');
% figure; imshow(uint8(im))


%-Load superpixels
superpix=loadSupix([supPath supName],.2);

%--Read the illuminant estimations
%[illListSpec, lightIndsSpec]=loadIllum([illSpecPath illSpecName]); %Specular
[illListDiff]=loadIllum([illDiffPath illDiffName],.2); %Diffuse
[illListSpec]=loadIllum([illSpecPath illSpecName],.2); %Specular

% supixIllDiff=supix2ill(superpix,[illDiffPath illDiffName]);
% supixIllSpec=supix2ill(superpix,[illSpecPath illSpecName]);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 2. Decide on the "specularity level" of the superpixel and generate a map

%THIS LINE WORKS BUT WE DON'T NEED IT YET !!!!!!!
% imOut=Lehmann_SpecVsDiff(im*10); 

% % % % % costSpec=ill2reconstErr(im,superpix,supixCL,supixIllSpec);
% % % % % model=supix2DRM3(im,superpix,supixCL,illListSpec);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 3. Cluster the illuminants with meanshift, choose top 10 most populated
% centers: 10 of spec and diff
illList=[illListSpec; illListDiff];
%--Cluster illuminants
% [IDXk, lightsk]=kmeans2DChroma(illList,numIll);
% % [IDX, lights]=meanshift2DChroma(illList,.06); %%%THRESHOLD ????
% % % lights=pickIlls(im,3);  
% % % [IDX, lights] = kmeans(illList, numIll); %one is gray
% % %l2 norm
% % lights=lights./repmat(sqrt(sum(lights.^2,2)),[1 3]) *255;
% % % pick the 10 most populated clusters
% % temp=hist(IDX,size(lights,1));
% % [a,b]=sort(temp,'descend');
% % numIll=10; 
% % mylights=lights(b(1:numIll),:);
% % showColors(mylights,50,1)

% [IDX, mylights]=kmeans(illList,10);
[IDX, mylights]=kmeans2DChroma(illList,10);
mylights=mylights./repmat(sqrt(sum(mylights.^2,2)),[1 3]) *255;
% showColors(mylights,50,1)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 4. Get the CL per supix: [divide by Ld and Ls and cluster all and take 10
% most populated]

%-% Super pixel C*L values
supixCL=supix2CL_minIntens(im,superpix,20);
% supixCL=supix2CL_minIntens(im16,superpix);
% supixC=[supixCL./(supixIllSpec+eps); supixCL./(supixIllDiff+eps)];
% supixC=supixC./repmat(sqrt(sum(supixC.^2,2)),[1 3]) *255;


ind=find(sum(supixCL,2)==0); % Removing zeros !!! VERY IMPORTANT !!!
supixCLdata=supixCL;
supixCLdata(ind,:)=[];

% meanshift2DChroma
% % [IDX, CLs]=meanshift2DChroma(supixCLdata,.06);  %%%THRESHOLD ????
% % %l2 norm
% % CLs=CLs./repmat(sqrt(sum(CLs.^2,2)),[1 3]) *255;
% % % pick the 10 most populated clusters
% % temp=hist(IDX,size(CLs,1));
% % [a,b]=sort(temp,'descend');
% % numObj=10; 
% % myCLs=CLs(b(1:numObj),:);
% % showColors(myCLs,50,1)

% %KMEANS ON ALL PIXELS !!
% pixrgb=reshape(im,size(im,1)*size(im,2),3); % all pixels
% intens=sum(pixrgb,2);
% myInd=find(intens<10);
% pixrgb(myInd,:)=[]; %removing dark pixs
% [IDX, CLs]=kmeans2DChroma(double(pixrgb),10);

% [IDX, CLs]=kmeans(supixCLdata,10);
[IDX, CLs]=kmeans2DChroma(supixCLdata,10);
myCLs=CLs./repmat(sqrt(sum(CLs.^2,2)),[1 3]) *255;
showColors(myCLs,50)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 5. For each pair of illuminants (withing the 10) perform Graphcut with
% labels being: on both ill and obj color
% % % % % %- only illuminants
% % % % % %- only object color
% % % % % %- both => in this case take set of object colors using kmeans 

% % We need to calculate:
% - DATA COST
% - SMOOTHNESS COST
% - SPARSE SMOOTHNESS

% - SPARSE SMOOTHNESS : As it's const for any choice of labeling
SS=findConnections2(superpix); %Connections between pixels
% SS=findConnections3(superpix,supixCL); %Connections between pixels *

% - Define Labels: 
% for each distinct pair of ill we have 2*10 C=CL/L
% so we have 2 * 2 * 10 labels (L and C)

% we make a loop on the choice of pairs

% - THE LOOP on the pairs of illuminant:
% we have the lists : mylights and myCLs
% For each pair of illuminants in mylights

% Init error
graphE=10^4;

%tic
for i=1:size(mylights,1)-1
    for j=i+1:size(mylights,1)
%         [i,j]
        % - DEFINE LABELS        
        %choose a pair of lights
        iL=[mylights(i,:); mylights(j,:)]; %illuminant labels
        % C labels (obj colors)
        cL= repmat(myCLs,[2 1])./[repmat(mylights(i,:),[size(myCLs,1) 1]); repmat(mylights(j,:),[size(myCLs,1) 1])];
        cL=cL./repmat(sqrt(sum(cL.^2,2)),[1 3]) *255; %l2 norm
        %cL=[myCLs./mylights(i,:); myCLs./mylights(j,:)]; % C labels (obj colors)
        
        labels=[repmat(1:size(cL,1),1,size(iL,1)); 
                reshape(meshgrid(1:size(iL,1),1:size(cL,1)),1,size(iL,1)*size(cL,1))]';
                
        % - DATA COST
        %this function takes 18sec !!
%         model=makeDC_40labels(im,superpix,labels,cL,iL); % Reconstruction Err 
        model=makeDC_40labels_minIntens(im,superpix,supixCL,labels,cL,iL);
        DC=model';
        
        % - SMOOTHNESS COST
        %SC=real(makeSC_40labels(labels,cL,iL));
        %SC=real(makeSC_40labels(labels,cL,iL)).*(ones(size(DC,1))-
        %eye(size(DC,1))).*.001;
%         SC=(ones(size(DC,1))- eye(size(DC,1)))*.001;  
%         SC=real(makeSC_40labels(labels,cL,iL))*10^-6;
%         SC=real(makeSC_labelAngErr(mylights))*10^-4;
        SC=zeros(size(DC,1));
        % - RUN GRAPHCUT
        gch = GraphCut('open',DC,SC,SS);
        [gch L] = GraphCut('expand',gch);
        [gch se de] = GraphCut('energy', gch);
%         [gch e] = GraphCut('energy', gch);
        gch = GraphCut('close', gch);
        L=L+1; %Because labels start from 1 not 0 !!
        if (se+de)<graphE
            [i j se de se+de]
            graphE=se+de;
            finallabels=labels;
            finalcL=cL;
            %[m1 m2]=min(model,[],2);
            finalL=L; %m2;%
            finaliL=iL;
%             [lightMask colorMask]=visGraphCut_CandL(L,superpix,labels,cL,iL);
        end        
    end
end
[lightMask colorMask]=visGraphCut_CandL(finalL,superpix,finallabels,finalcL,finaliL);
figure; imshow(uint8(lightMask))
figure; imshow(uint8(colorMask))
%toc

% if itr==20
%     itr
% end

