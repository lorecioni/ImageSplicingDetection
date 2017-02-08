function t = cbmired2temp(x)
%t = cbmired2temp(m)
% Converts the given value in mirads to temperature (Kelvin).
% t = 10^6/m

t = 10^6./x;