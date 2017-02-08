function xy = XYZ2xy(xyz)
%xy = XYZ2xy(xyz)
% Converts CIE XYZ to xy chromaticity.

X = xyz(1);
Y = xyz(2);
s = sum(xyz);
xy = [X/s; Y/s];

end
