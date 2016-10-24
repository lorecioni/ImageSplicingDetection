close all;


files = dir('features/*.txt');
out = [];
for file = files'
    fileID = fopen(['features/' file.name]);    
    C = cell2mat(textscan(fileID, '%f32'));
    out = cat(2, out, C);
end

values = out(3, :);

POSITIVE = 46;
NEGATIVE = length(values) - POSITIVE;

ordered = sort(values);

tps = zeros(1, length(values));
fps = zeros(1, length(values));
for i = 1:length(ordered)
    current = ordered(i);
    tp = 0; fp = 0;
    for j = 1:POSITIVE
        if values(j) < current
            tp = tp + 1;
        end
    end
    for k = POSITIVE + 1:length(values)
       if values(k) < current
           fp = fp + 1;
       end
    end
    tps(i) = tp / POSITIVE;
    fps(i) = fp / NEGATIVE;
end

[fps_o, index] = sort(fps);
tps_o = tps(index);

plot(fps_o, tps_o);

valTrue = out(1, 1:46);
valFalse = out(1, 47:end);

%figure;hist(valTrue);
%figure;hist(valFalse);
