function x = makecol(x)
%x = makecol(x)
% returns x as a column vector

s = size(x);
if (length(s) == 2) && (s(1) < s(2))
    x = x.';
end

end