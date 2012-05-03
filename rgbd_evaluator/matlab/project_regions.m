function [feat,featp,scales]=project_regions(feat,H,make_circles)

s=size(feat);
s1=s(1);

featp=feat;
scales=zeros(1,s1);

for c1=1:s1,%%%%%%%%%%%%%%%%%
%feat(c1,3:5)=(1/25)*feat(c1,3:5);
Mi1=[feat(c1,3) feat(c1,4);feat(c1,4) feat(c1,5)];

%compute affine transformation
[v1 e1]=eig(Mi1);
d1=(1/sqrt(e1(1))); 
d2=(1/sqrt(e1(4))); 
sc1=sqrt(d1*d2);

% make circles from ellipses
if make_circles
    d1=sc1;
    d2=sc1;
end

%major/minor length
feat(c1,6)=d1;
feat(c1,7)=d2; 

%scales(c1)=sqrt(feat(c1,6)*feat(c1,7));

%bounding box
feat(c1,8) = sqrt(feat(c1,5)/(feat(c1,3)*feat(c1,5) - feat(c1,4)^2));
feat(c1,9) = sqrt(feat(c1,3)/(feat(c1,3)*feat(c1,5) - feat(c1,4)^2));

Aff=getAff(feat(c1,1),feat(c1,2),sc1, H);

%project to image 2
l1=[feat(c1,1),feat(c1,2),1];
l1_2=H*l1';
l1_2=l1_2/l1_2(3);
featp(c1,1)=l1_2(1);
featp(c1,2)=l1_2(2);
BMB=inv(Aff*inv(Mi1)*Aff');
[v1 e1]=eig(BMB);
featp(c1,6)=(1/sqrt(e1(1)));
featp(c1,7)=(1/sqrt(e1(4)));
featp(c1,3:5)=[BMB(1) BMB(2) BMB(4)];

% make circles from ellipses
if make_circles
    radius = sqrt( featp(c1,6)*featp(c1,7) );
    featp(c1,3) = 1 / (radius^2);
    featp(c1,4) = 0;
    featp(c1,5) = 1 / (radius^2);
    featp(c1,6) = radius;
    featp(c1,7) = radius;
end

%bounding box in image 2
featp(c1,8) = sqrt(featp(c1,5)/(featp(c1,3)*featp(c1,5) - featp(c1,4)^2));
featp(c1,9) = sqrt(featp(c1,3)/(featp(c1,3)*featp(c1,5) - featp(c1,4)^2));

% surface area
scales(c1)=sqrt(featp(c1,6)*featp(c1,7));

end
%end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function Aff=getAff(x,y,sc,H)
h11=H(1);
h12=H(4);
h13=H(7);
h21=H(2);
h22=H(5);
h23=H(8);
h31=H(3);
h32=H(6);
h33=H(9);
fxdx=h11/(h31*x + h32*y +h33) - (h11*x + h12*y +h13)*h31/(h31*x + h32*y +h33)^2;
fxdy=h12/(h31*x + h32*y +h33) - (h11*x + h12*y +h13)*h32/(h31*x + h32*y +h33)^2;

fydx=h21/(h31*x + h32*y +h33) - (h21*x + h22*y +h23)*h31/(h31*x + h32*y +h33)^2;
fydy=h22/(h31*x + h32*y +h33) - (h21*x + h22*y +h23)*h32/(h31*x + h32*y +h33)^2;

          Aff=[fxdx fxdy;fydx fydy];
%end
