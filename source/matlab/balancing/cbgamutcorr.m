function corr = cbgamutcorr(x1,y1,x2,y2)
%corr = cbgamutcorr(x1,y1,x2,y2)
% Given a two sets of points, this computes the convex hulls of each set
% and their intersection.  The gamut correlation is then defined as
% A_intersection/sqrt(A_hull1*A_hull2), where A is an area.

% clear
% x1 = randn(1,100);
% y1 = randn(1,100);
% 
% x2 = randn(1,100)+3;
% y2 = randn(1,100);

[k1,a1] = convhull(x1,y1);
[k2,a2] = convhull(x2,y2);

[xp1,yp1] = poly2cw(x1(k1),y1(k1));
[xp2,yp2] = poly2cw(x2(k2),y2(k2));
% xp1 = x1(k1); yp1 = y1(k1);
% xp2 = x2(k2); yp2 = y2(k2);


[x,y] = polybool('&',xp1,yp1,xp2,yp2);

% figure(10),clf
% hold on
% patch(x, y, 1, 'FaceColor', 'g')
% plot(x1,y1,'.',x2,y2,'r.')
% plot(x1(k1),y1(k1),'-',x2(k2),y2(k2),'r-')
% hold off

corr = polyarea(x,y)/sqrt(a1*a2)
end