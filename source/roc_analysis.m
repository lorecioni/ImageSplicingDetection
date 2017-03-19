

%Labels ottenute (0 e 1)
%Labels=[zeros(100,1)' ones(100,1)'];
%Scores relativi (valori ottenuti)
%Score =[rand(100,1)' 10*rand(100,1)'];

Labels = [zeros(1, length(negative)) ones(1, length(positive))];
Scores = [negative positive];

%Curva ROC
plotroc(Labels,Scores)

%Recupero valore di soglia ottimale
[X,Y,T,AUC,OPTROCPT] = perfcurve(Labels,Scores,1);
ACC = ((1-X)+Y)/2;
%Calcolo accuracy (M)
[M idx] = max(ACC);
disp(M)
%Valore di soglia
T(idx)