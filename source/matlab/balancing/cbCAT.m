function outMat = cbCAT(xyz_est,xyz_target,type)
%outMat = cbCAT(xyz_est,xyz_target,type)
% Chromatic adaptation transform via von Kries's method.
%  type     chooses the LMS-like space to apply scaling in, valid options:
%           'vonKries', 'bradford', 'sharp', 'cmccat2000', 'cat02', 'xyz'
% See http://www.brucelindbloom.com/index.html?Eqn_ChromAdapt.html

xyz_est = makecol(xyz_est);
xyz_target = makecol(xyz_target);

% the following are mostly taken from S. Bianco. "Two New von Kries Based
% Chromatic Adapatation Transforms Found by Numerical Optimization."
if strcmpi(type,'vonKries') %Hunt-Pointer-Estevez normalized to D65
    xfm = [.40024 .7076 -.08081; -.2263 1.16532 0.0457; 0 0 .91822];
elseif strcmpi(type,'bradford')
    xfm = [.8951 .2664 -.1614; -.7502 1.7135 0.0367; 0.0389 -0.0685 1.0296];
elseif strcmpi(type,'sharp')
    xfm = [1.2694 -.0988 -.1706; -.8364 1.8006 0.0357; 0.0297 -0.0315 1.0018];
elseif strcmpi(type,'cmccat2000')
    xfm = [.7982 .3389 -.1371; -.5918 1.5512 0.0406; 0.0008 0.239 0.9753];
elseif strcmpi(type,'cat02')
    xfm = [0.7328 0.4296 -.1624; -.7036 1.6975 0.0061; 0.0030 0.0136 0.9834];
else
    xfm = eye(3);
end
    
gain = (xfm*xyz_target)./(xfm*xyz_est);
outMat = xfm\(diag(gain)*xfm);
sRGBtoXYZ =   [0.4124564  0.3575761  0.1804375; ...
               0.2126729  0.7151522  0.0721750; ...
               0.0193339  0.1191920  0.9503041];
outMat = inv(sRGBtoXYZ)*outMat*sRGBtoXYZ;
end