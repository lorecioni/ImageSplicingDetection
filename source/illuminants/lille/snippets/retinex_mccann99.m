function Retinex = retinex_mccann99(L, nIterations)

% RETINEX_McCANN99: 
%         Computes the raw Retinex output from an intensity image, based on the
%         more recent model described in:
%         McCann, J., "Lessons Learned from Mondrians Applied to Real Images and 
%         Color Gamuts", Proc. IS&T/SID Seventh Color Imaging Conference, pp. 1-8, 1999
%
% INPUT:  L           - logarithmic single-channel intensity image to be processed
%         nIterations - number of Retinex iterations
%
% OUTPUT: Retinex     - raw Retinex output
%
% NOTES:  - The input image is assumed to be logarithmic and in the range [0..1]
%         - To obtain the retinex "sensation" prediction, a look-up-table needs to
%         be applied to the raw retinex output
%         - For colour images, apply the algorithm individually for each channel
%
% AUTHORS: Florian Ciurea, Brian Funt and John McCann. 
%          Code developed at Simon Fraser University.
%
% For information about the code see: Brian Funt, Florian Ciurea, and John McCann
% "Retinex in Matlab," by Proceedings of the IS&T/SID Eighth Color Imaging 
% Conference: Color Science, Systems and Applications, 2000, pp 112-121.
%
% paper available online at http://www.cs.sfu.ca/~colour/publications/IST-2000/
%
% Copyright 2000. Permission granted to use and copy the code for research and 
% educational purposes only.  Sale of the code is not permitted. The code may be 
% redistributed so long as the original source and authors are cited.

global OPE RRE Maximum
[nrows ncols] = size(L);                            % get size of the input image
nLayers = ComputeLayers(nrows, ncols);               % compute the number of pyramid layers
nrows = nrows/(2^nLayers);                           % size of image to process for layer 0
ncols = ncols/(2^nLayers);
if (nrows*ncols > 25)                                % not processing images of area > 25
  error('invalid image size.')                       % at first layer
end
Maximum = max(L(:));                                 % maximum color value in the image
OP = Maximum*ones([nrows ncols]);                    % initialize Old Product
for layer = 0:nLayers
   RR = ImageDownResolution(L, 2^(nLayers-layer));   % reduce input to required layer size
   
   OPE = [zeros(nrows,1) OP zeros(nrows,1)];         % pad OP with additional columns
   OPE = [zeros(1,ncols+2); OPE; zeros(1,ncols+2)];  % and rows
   RRE = [RR(:,1) RR RR(:,end)];                     % pad RR with additional columns
   RRE = [RRE(1,:); RRE; RRE(end,:)];                % and rows
   
   for iter = 1:nIterations
     CompareWithNeighbor(-1, 0);                     % North
     CompareWithNeighbor(-1, 1);                     % North-East
     CompareWithNeighbor(0, 1);                      % East
     CompareWithNeighbor(1, 1);                      % South-East
     CompareWithNeighbor(1, 0);                      % South
     CompareWithNeighbor(1, -1);                     % South-West
     CompareWithNeighbor(0, -1);                     % West
     CompareWithNeighbor(-1, -1);                    % North-West
   end
   NP = OPE(2:(end-1), 2:(end-1));
   OP = NP(:, [fix(1:0.5:ncols) ncols]);             %%% these two lines are equivalent with 
   OP = OP([fix(1:0.5:nrows) nrows], :);             %%% OP = imresize(NP, 2) if using Image
   nrows = 2*nrows; ncols = 2*ncols;                 % Processing Toolbox in MATLAB
end
Retinex = NP;


