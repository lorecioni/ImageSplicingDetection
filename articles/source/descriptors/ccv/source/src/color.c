#include "color.h"

int triplet(int a,int b,int c) {
  return(((a&0xff)<<16)|((b&0xff)<<8)|(c&0xff));
}

int RGB2YCbCr(int v) {
  float y,cb,cr;

  y=0.257*(float)t0(v)+
    0.504*(float)t1(v)+
    0.098*(float)t2(v)+16.0;

  cb=-0.148*(float)t0(v)-
      0.291*(float)t1(v)+
      0.439*(float)t2(v)+
      128.0;

  cr=0.439*(float)t0(v)-
     0.368*(float)t1(v)-
     0.071*(float)t2(v)+
     128.0;

  return(triplet(ROUND(y),ROUND(cb),ROUND(cr)));
}

int YCbCr2RGB(int v) {
  float r,g,b;

  r=1.164*((float)t0(v)-16.0)+
    1.596*((float)t2(v)-128.0);

  g=1.164*((float)t0(v)-16.0)-
    0.813*((float)t2(v)-128.0)-
    0.392*((float)t1(v)-128.0);
  
  b=1.164*((float)t0(v)-16.0)+
    2.017*((float)t1(v)-128.0);

  if (r<0.0) r=0.0;
  if (g<0.0) g=0.0;
  if (b<0.0) b=0.0;
  if (r>255.0) r=255.0;
  if (g>255.0) g=255.0;
  if (b>255.0) b=255.0;

  return(triplet(ROUND(r),ROUND(g),ROUND(b)));
}

#define GMAX(a,b,c) (((a>=b)&&(a>=c))?a:((b>c)?b:c))
#define GMIN(a,b,c) (((a<=b)&&(a<=c))?a:((b<c)?b:c))

int RGB2HSV(int vi) {
  float r = (float) t0(vi), 
        g = (float) t1(vi), 
        b = (float) t2(vi), v, x, f; 
  float a[3];

  int i; 

  r/=255.0;
  g/=255.0;
  b/=255.0;

  // RGB are each on [0, 1]. S and V are returned on [0, 1] and H is 
  // returned on [0, 6]. 

  x = GMIN(r, g, b); 
  v = GMAX(r, g, b); 
  if (v == x) {
    a[0]=0.0;
    a[1]=0.0;
    a[2]=v;
  } else {
    f = (r == x) ? g - b : ((g == x) ? b - r : r - g); 
    i = (r == x) ? 3 : ((g == x) ? 5 : 1);
    a[0]=((float)i)-f/(v-x);
    a[1]=(v-x)/v;
    a[2]=v;
  }

  // (un)normalize

  a[0]*=255.0/6.0;
  a[1]*=255.0;
  a[2]*=255.0;
  
  return(triplet(ROUND(a[0]),ROUND(a[1]),ROUND(a[2])));
}

int HSV2RGB(int vi) {
  // H is given on [0, 6]. S and V are given on [0, 1]. 
  // RGB are each returned on [0, 1]. 
  float h = (float)t0(vi), 
        s = (float)t1(vi), 
        v = (float)t2(vi), m, n, f; 
  float a[3];
  int i; 

  h/=255.0/6.0;
  s/=255.0;
  v/=255.0;

  if (s==0.0) {
    a[0]=a[1]=a[2]=v;
  } else {
    i = (int) floor(h); 
    f = h - (float)i; 
    if(!(i & 1)) f = 1 - f; // if i is even 
    m = v * (1 - s); 
    n = v * (1 - s * f); 
    switch (i) { 
    case 6: 
    case 0: a[0]=v; a[1]=n; a[2]=m; break;
    case 1: a[0]=n; a[1]=v; a[2]=m; break;
    case 2: a[0]=m; a[1]=v; a[2]=n; break;
    case 3: a[0]=m; a[1]=n; a[2]=v; break;
    case 4: a[0]=n; a[1]=m; a[2]=v; break;
    case 5: a[0]=v; a[1]=m; a[2]=n; break;
    } 
  }

  // (un)normalize
  for(i=0;i<3;i++)
    a[i]*=255;

  return(triplet(ROUND(a[0]),ROUND(a[1]),ROUND(a[2])));
}

int RGB2XYZ(int vi) {
  float r = (float) t0(vi), 
        g = (float) t1(vi), 
        b = (float) t2(vi); 
  float x, y, z; 

  r/=255.0;
  g/=255.0;
  b/=255.0;

  // RGB are each on [0, 1]. 
  // XYZ are returned on [0, 1]. 

  if (r > 0.04045)
    r = pow((r + 0.055) / 1.055, 2.4);
  else
    r = r / 12.92;
  if (g > 0.04045)
    g = pow((g + 0.055) / 1.055, 2.4);
  else
    g = g / 12.92;
  if (b > 0.04045)
    b = pow((b + 0.055) / 1.055, 2.4);
  else
    b = b / 12.92;

  x = 0.4124 * r + 0.3576 * g + 0.1805 * b;
  y = 0.2126 * r + 0.7152 * g + 0.0722 * b;
  z = 0.0193 * r + 0.1192 * g + 0.9505 * b;

  // (un)normalize

  x*=255.0;
  y*=255.0;
  z*=255.0;
  
  return(triplet(ROUND(x),ROUND(y),ROUND(z)));
}

int XYZ2RGB(int vi) {
  float x = (float) t0(vi), 
        y = (float) t1(vi), 
        z = (float) t2(vi); 
  float r, g, b;

  x/=255.0;
  y/=255.0;
  z/=255.0;

  // XYZ are each on [0, 1]. 
  // RGB are returned on [0, 1]. 

  r =  3.2410 * x - 1.5374 * y - 0.4986 * z;
  g = -0.9692 * x + 1.8760 * y + 0.0416 * z;
  b =  0.0556 * x - 0.2040 * y + 1.0570 * z;

  if (r <= 0.00304)
    r *= 12.92;
  else
    r = 1.055 * pow(r, 1.0 / 2.4) - 0.055;
  if (g <= 0.00304)
    g *= 12.92;
  else
    g = 1.055 * pow(g, 1.0 / 2.4) - 0.055;
  if (b <= 0.00304)
    b *= 12.92;
  else
    b = 1.055 * pow(b, 1.0 / 2.4) - 0.055;

  // (un)normalize

  r*=255.0;
  g*=255.0;
  b*=255.0;

  if (r<0.0) r=0.0;
  if (g<0.0) g=0.0;
  if (b<0.0) b=0.0;
  if (r>255.0) r=255.0;
  if (g>255.0) g=255.0;
  if (b>255.0) b=255.0;
  
  return(triplet(ROUND(r),ROUND(g),ROUND(b)));
}

int XYZ2LAB(int vi) {
  float x = (float) t0(vi), 
        y = (float) t1(vi), 
        z = (float) t2(vi); 
  float l, a, b;

  x/=255.0;
  y/=255.0;
  z/=255.0;

  // XYZ are each on [0, 1]. 
  // L is returned on [0, 100] and A is returned on [-500, 500] and
  // B is returned on [-200, 200].
  
  if (x > 0.008856)
    x = pow(x, 1.0 / 3.0);
  else
    x = 7.787 * x + 16.0 / 116.0;
  if (y > 0.008856)
    y = pow(y, 1.0 / 3.0);
  else
    y = 7.787 * y + 16.0 / 116.0;
  if (z > 0.008856)
    z = pow(z, 1.0 / 3.0);
  else
    z = 7.787 * z + 16.0 / 116.0;

  l = 116.0 * y - 16.0;
  a = 500.0 * (x - y);
  b = 200.0 * (y - z);

  // (un)normalize

  l*=255.0/100.0;
  a=255.0 * (a + 500.0) / 1000.0;
  b=255.0 * (b + 200.0) / 400.0;

  return(triplet(ROUND(l),ROUND(a),ROUND(b)));
}

int LAB2XYZ(int vi) {
  float l = (float) t0(vi), 
        a = (float) t1(vi), 
        b = (float) t2(vi); 
  float s, x, y, z;

  l*=100.0/255.0;
  a=1000.0 * a / 255.0 - 500.0;
  b=400.0 * b / 255.0 - 200.0;

  // L is on [0, 100] and A is on [-500, 500] and B is on [-200, 200].
  // XYZ are returned on [0, 1]. 

  y = (l + 16.0) / 116.0;
  x = y + a / 500.0;
  z = y - b / 200.0;
 
  s = 6.0 / 29.0;
  if (y > s)
    y = y * y * y;
  else
    y = (y - 16.0 / 116.0) * 3.0 * s * s;
  if (x > s)
    x = x * x * x;
  else
    x = (x - 16.0 / 116.0) * 3.0 * s * s;
  if (z > s)
    z = z * z * z;
  else
    z = (z - 16.0 / 116.0) * 3.0 * s * s;

  // (un)normalize

  x*=255.0;
  y*=255.0;
  z*=255.0;

  return(triplet(ROUND(x),ROUND(y),ROUND(z)));
}

int lighter(int c, int n) {
  int r,g,b,r0,g0,b0,i;
  r=c>>16;
  g=(c>>8)&0xff;
  b=c&0xff;

  for(i=0;i<n;i++) {
    r0=r+r/10; if (r0==r) r0=r+1;
    g0=g+g/10; if (g0==g) g0=g+1;
    b0=b+b/10; if (b0==b) b0=b+1;

    if (r0>255) r0=255;
    if (b0>255) b0=255;
    if (g0>255) g0=255;

    r=r0; g=g0; b=b0;
  }

  return( (r<<16)|(g<<8)|b );  
}

int darker(int c, int n) {
  int r,g,b,r0,g0,b0,i;
  r=c>>16;
  g=(c>>8)&0xff;
  b=c&0xff;

  for(i=0;i<n;i++) {
    r0=r-r/10; if (r0==r) r0=r-1;
    g0=g-g/10; if (g0==g) g0=g-1;
    b0=b-b/10; if (b0==b) b0=b-1;

    if (r0<0) r0=0;
    if (b0<0) b0=0;
    if (g0<0) g0=0;

    r=r0; g=g0; b=b0;
  }

  return( (r<<16)|(g<<8)|b );
}

/* 
   inverseColor
   Returns the inverse in RGB-space.

   input:  RGB triplet
   output: RGB triplet
   author: bergo
*/
int          inverseColor(int c) {
  int r,g,b;
  r=c>>16;
  g=(c>>8)&0xff;
  b=c&0xff;
  r=255-r; g=255-g; b=255-b;
  return( (r<<16)|(g<<8)|b );  
}

/*
  mergeColorsRGB
  Merges 2 colors, (1-ratio) of a with (ratio) of b.
  Works in RGB space.

  input:  2 RGB triplets, merge ratio
  output: RGB triplet
  author: bergo
*/
int mergeColorsRGB(int a,int b,float ratio) {
  float c[6];
  int r[3];

  c[0] = (float) (a >> 16);
  c[1] = (float) (0xff & (a >> 8));
  c[2] = (float) (0xff & a);

  c[3] = (float) (b >> 16);
  c[4] = (float) (0xff & (b >> 8));
  c[5] = (float) (0xff & b);

  c[0] = (1.0-ratio) * c[0] + ratio * c[3];
  c[1] = (1.0-ratio) * c[1] + ratio * c[4];
  c[2] = (1.0-ratio) * c[2] + ratio * c[5];

  r[0] = (int) c[0];
  r[1] = (int) c[1];
  r[2] = (int) c[2];

  return( (r[0]<<16) | (r[1]<<8) | r[2] );
}

/*
  mergeColorsYCbCr
  Merges 2 colors, (1-ratio) of a with (ratio) of b.
  Works in YCbCr space.

  input:  2 RGB triplets, merge ratio
  output: RGB triplet
  author: bergo
*/
int          mergeColorsYCbCr(int a,int b,float ratio) {
  float c[6];
  int ya,yb;
  int r[3];

  ya=RGB2YCbCr(a);
  yb=RGB2YCbCr(b);

  c[0] = (float) (a >> 16);
  c[1] = (float) (0xff & (a >> 8));
  c[2] = (float) (0xff & a);

  c[3] = (float) (b >> 16);
  c[4] = (float) (0xff & (b >> 8));
  c[5] = (float) (0xff & b);

  c[0] = (1.0-ratio) * c[0] + ratio * c[3];
  c[1] = (1.0-ratio) * c[1] + ratio * c[4];
  c[2] = (1.0-ratio) * c[2] + ratio * c[5];

  r[0] = (int) c[0];
  r[1] = (int) c[1];
  r[2] = (int) c[2];

  ya = (r[0]<<16) | (r[1]<<8) | r[2];
  return(YCbCr2RGB(ya));
}

/*
  gray
  builds the gray triplet with R=c, G=c, B=c

  input:  gray level (0-255)
  output: RGB triplet
  author: bergo
*/
int gray(int c) {
  return( (c<<16) | (c<<8) | c );
}

int randomColor() {
  static int seeded = 0;
  int a,b,c;

  if (!seeded) {
    srand(time(0));
    seeded = 1;
  }

  a = 1+(int) (255.0*rand()/(RAND_MAX+1.0));
  b = 1+(int) (255.0*rand()/(RAND_MAX+1.0));
  c = 1+(int) (255.0*rand()/(RAND_MAX+1.0));
  return(triplet(a,b,c));
}


double HSVColorDistance(int color1, int color2){
  float teta, dx, dy;
  float fh1,fs1,fv1;
  float fh2,fs2,fv2;
  float dv,D;
  double distance;
  int   color;

  color = RGB2HSV(color1);
  fh1 = t0(color)/255.0;
  fs1 = t1(color)/255.0;
  fv1 = t2(color)/255.0;

  color = RGB2HSV(color2);
  fh2 = t0(color)/255.0;
  fs2 = t1(color)/255.0;
  fv2 = t2(color)/255.0;

  teta = (fh2-fh1)*2.0*PI;
  if(teta<0.0) teta = -teta;
  if(teta>PI) teta = 2*PI-teta;

  teta *= 2.0;
  if(teta>PI) teta = PI;

  fs1 *= fv1;
  fs2 *= fv2;

  dx = fs1 - cos(teta)*fs2;
  dy = 0.0 - sin(teta)*fs2;

  D = dx*dx+dy*dy;
  dv = fv2-fv1;

  distance = sqrt(D + dv*dv)/2.0;

  return distance;
}


double RGBColorDistance(int color1, int color2){
  float fr1,fg1,fb1;
  float fr2,fg2,fb2;
  float dr,dg,db;
  double distance;

  fr1 = t0(color1)/255.0;
  fg1 = t1(color1)/255.0;
  fb1 = t2(color1)/255.0;
  
  fr2 = t0(color2)/255.0;
  fg2 = t1(color2)/255.0;
  fb2 = t2(color2)/255.0;

  dr = fr1-fr2;
  dg = fg1-fg2;
  db = fb1-fb2;
  dr *= dr;
  dg *= dg;
  db *= db;

  distance = sqrt((dr + dg + db)/3.0);

  return distance;
}

