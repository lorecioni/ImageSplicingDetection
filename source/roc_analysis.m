function [pd,fa,th,acc,acc2,auc] = roc_analysis(data1,data0)
%% ROC building varying threshold
%% INPUT:
%   data1: data from negative class;
%   data0: data from positive class;

th = sort([data1;data0]);
l = length(th);
ss = length(data1);
sf = length(data0);

TP = zeros(l,1);
TN = zeros(l,1);
pd = zeros(l,1);
fa = zeros(l,1);
acc = zeros(l,1);
acc2 = zeros(l,1);

for i = 1:l
    TP(i) = sum(data0<th(i));
    pd(i) = TP(i)/sf;
    TN(i) = sum(data1 >= th(i));
    fa(i) = 1 - (TN(i))/ss;
    acc(i) = (TP(i) + TN(i))/(ss+sf);
    acc2(i) = (pd(i) + (1-fa(i)))/2;
end
    
% pd(1)=0;
cc = rand(3,1);
%figure
plot(fa,pd,'LineWidth',1.2,'Color',cc,'Marker','*','MarkerSize',3)
axis square
title('ROC')
xlabel('PFA')
ylabel('PD')

auc = trapz(fa,pd);
end