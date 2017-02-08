function xyz = xy2XYZ(xy,Y)
%xyz = xy2XYZ(xy,Y)
% Converts xyY chromaticity to CIE XYZ.
x = xy(1); y = xy(2);
xyz = [Y/y*x; Y; Y/y*(1-x-y)];
end