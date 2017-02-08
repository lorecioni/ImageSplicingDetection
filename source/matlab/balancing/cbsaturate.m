function y = cbsaturate(x,bounds)
%y = cbsaturate(x,bounds)
% Saturates the vector x given the bounds pair [low high]

low = bounds(1); high = bounds(2);

y=zeros(size(x));
for i = 1:length(x)
    if x(i) < low
        y(i) = low;
    elseif x(i) > high
        y(i) = high;
    else
        y(i) = x(i);
    end
end
end