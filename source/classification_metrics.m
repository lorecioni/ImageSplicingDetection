%Curva ROC
plotroc(labels, scores)

%Recupero valore di soglia ottimale
[X,Y,T,AUC,OPTROCPT] = perfcurve(labels, scores,1);
ACC = ((1-X)+Y)/2;
fprintf('\tAUC: %f\n', AUC);

%Calcolo accuracy (M)
[M, idx] = max(ACC);
fprintf('\tAccuracy: %f\n', M);
%Valore di soglia
Th = T(idx);
fprintf('\tThreshold: %f\n', Th);

%Precision recall curve
figure;
[Xpr,Ypr,Tpr,AUCpr] = perfcurve(labels, scores, 1, 'xCrit', 'reca', 'yCrit', 'prec');
plot(Xpr,Ypr);
axis([0 1 0 1]);
xlabel('Recall'); ylabel('Precision')
title(['Precision-recall curve (AUC: ' num2str(AUCpr) ')'])
ACC = ((1-X)+Y)/2;
[~, idx] = max(ACC);
F_score = 2* (Xpr(idx) * Ypr(idx)/(Xpr(idx) + Ypr(idx)));
fprintf('\tF-Score: %f\n', F_score);