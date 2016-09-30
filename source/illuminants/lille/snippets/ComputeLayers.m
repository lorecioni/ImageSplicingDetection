function Layers = ComputeLayers(nrows, ncols)
power = 2^fix(log2(gcd(nrows, ncols)));              % start from the Greatest Common Divisor
while(power > 1 & ((rem(nrows, power) ~= 0) | (rem(ncols, power) ~= 0)))
   power = power/2;                                  % and find the greatest common divisor
end                                                  % that is a power of 2
Layers = log2(power);


