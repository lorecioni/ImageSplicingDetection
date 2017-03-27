
images_path = 'data';
pattern = 'splicing-';
folders = dir(images_path);
i = 1;

positive = zeros(0);
negative = zeros(0);
count = 0;
while i <= size(folders, 1)
    f = folders(i);
    if(~strcmp(f.name, '.') && ~strcmp(f.name, '..') ...
            && ~f.isdir && strncmpi(f.name, pattern, length(pattern)) ...
            && length(strfind(f.name,'svm')) == 0)
        
        image = [images_path, '/', f.name]; 
        disp(image);
        data = load(image);
        positive = horzcat(positive, data.positive);
        negative = horzcat(negative, data.negative);
        
        
        
        count = count + 1;
    end
    i=i+1;
    
    %if count == 1
        %break;
    %end
end



Labels = [zeros(1, length(negative)) ones(1, length(positive))];
Scores = [negative positive];

%Curva ROC
plotroc(Labels,Scores)

        %Recupero valore di soglia ottimale
        [X,Y,T,AUC,OPTROCPT] = perfcurve(Labels,Scores,1);
        ACC = ((1-X)+Y)/2;
        %Calcolo accuracy (M)
        [M idx] = max(ACC);
        fprintf('\tAccuracy: %f\n', M);
        %Valore di soglia
        Th = T(idx);
        fprintf('\tThreshold: %f\n', Th);

%Labels ottenute (0 e 1)
%Labels=[zeros(100,1)' ones(100,1)'];
%Scores relativi (valori ottenuti)
%Score =[rand(100,1)' 10*rand(100,1)'];

