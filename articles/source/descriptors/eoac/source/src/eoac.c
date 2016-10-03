#include "eoac.h"

bool ValidPixelEdges(int nrows, int ncols, int x, int y)
{
  if ((x >= 0)&&(x < ncols)&&
      (y >= 0)&&(y < nrows))
    return(true);
  else
    return(false);
}

void CalculaAutocorrelograma(int nrows, int ncols, ImageEdges **img, int *distances, int **eoac) {
    ulong x, y;
    uchar d;
    AdjRel *A;
    Pixel viz;  //pixel vizinho
    int i;

    A = Circular(1.0);

    //percorre a imagem
    for (y=0; y<nrows; y++) {
        for (x=0; x<ncols; x++) {

            for (d=0; d<QTD_DIST; d++) {

                //verifica os adjacentes considerando a distancia
                for (i=1; i<A->n; i++) {
                    viz.x = x + distances[d]*A->dx[i];
                    viz.y = y + distances[d]*A->dy[i];

                    if (ValidPixelEdges(nrows, ncols, viz.x, viz.y)) { //se o ponto vizinho estiver dentro da imagem

                        //verifica se o valor de magnitude e orientacao deles eh semelhante
                        if ( (fabs(img[y][x].G-img[viz.y][viz.x].G) < TS_AMP) && 
                             (fabs(    ((int) img[y][x].Ang/ANG_INTERVAL) - ((int) img[viz.y][viz.x].Ang/ANG_INTERVAL)     ) <= TS_ANG) ) {

                            eoac[  ((int) img[y][x].Ang / ANG_INTERVAL)  ][d]++; //incrementa posicao do autocorrelograma
                        }
                    }
                }
            }
        }
    }
    DestroyAdjRel(&A);
}

//Encontra o minimo valor entre 3
float min(float x, float y, float z) {

    if ( (x<=y) && (x<=z) ) {
        return x;
    } else if ( (y<=x) && (y<=z) ) {
        return y;
    } else if ( (z<=x) && (z<=y) ) {
        return z;
    }
    return -1;
}

//Encontra o maximo valor entre 3
float max(float x, float y, float z) {

    if ( (x>=y) && (x>=z) ) {
        return x;
    } else if ( (y>=x) && (y>=z) ) {
        return y;
    } else if ( (z>=x) && (z>=y) ) {
        return z;
    }
    return -1;
}

void RGB2HSV_eoac(CImage *RGB, ImageHSV **HSV) {

    float r, g, b;
    float minVal, maxVal, delta;
    int i, j;

    for (i = 0; i < RGB->C[0]->nrows; i++) {
        for (j = 0; j < RGB->C[0]->ncols; j++) {

            //normaliza de 0 a 1
            r = (float) RGB->C[0]->val[j+RGB->C[0]->tbrow[i]]/255;
            g = (float) RGB->C[1]->val[j+RGB->C[1]->tbrow[i]]/255;
            b = (float) RGB->C[2]->val[j+RGB->C[2]->tbrow[i]]/255;

            minVal = min(r, g, b);
            maxVal = max(r, g, b);
            delta = maxVal - minVal;

            //H
            if (delta == 0) {
                HSV[i][j].H = 0;
            } else if (maxVal==r && g>=b) {
                HSV[i][j].H = 60*((g-b)/delta);
            } else if (maxVal==r && g<b) {
                HSV[i][j].H = 60*((g-b)/delta) + 360;
            } else if (maxVal==g) {
                HSV[i][j].H = 60*((b-r)/delta) + 120;
            } else if (maxVal==b) {
                HSV[i][j].H = 60* ((r-g)/delta) + 240;
            }

            //S
            if (maxVal==0) {
                HSV[i][j].S = 0;
            } else {
                HSV[i][j].S = delta/maxVal;
            }

            //V
            HSV[i][j].V = maxVal;

            //normalizando S e V entre 0 e 255
            HSV[i][j].S *= 255;
            HSV[i][j].V *= 255;

            //colocando H de 0 a 1
            //HSV[i][j].H /= 360;

        }
    }
}

void Sobel(Image *img, ImageEdges **imgEdges) {
    int i,j,k,m; //contadores
    int Gx, Gy;
    float rad; //guarda o angulo em radianos

    int mConvY[3][3] = {   //mascara de convolucao = SOBEL
        {-1,  0,  1},
        {-2,  0,  2},
        {-1,  0,  1}
    };

    int mConvX[3][3] = {   //mascara de convolucao = SOBEL
        { 1,  2,  1},
        { 0,  0,  0},
        {-1, -2, -1}
    };

    //percorre a imagem
    for (i = 0; i < img->nrows; i++) {
        for (j = 0; j < img->ncols; j++) {

            Gx = 0; //inicializa acumuladores
            Gy = 0;

            //posiciona a janela com o centro no pixel atual da imagem (x,y)
            for (k=-1; k<=1; k++) {
                for (m=-1; m<=1; m++) {
                    //verifica se o ponto eh valido
                    if ( ValidPixel(img,(j+k),(i+m)) ) {
                        Gx += mConvX[m+1][k+1] * img->val[(j+k)+img->tbrow[(i+m)]];
                        Gy += mConvY[m+1][k+1] * img->val[(j+k)+img->tbrow[(i+m)]];
                    }
                }
            } //fim mascara = janela deslizante

            imgEdges[i][j].G = sqrt( pow(Gx,2) + pow(Gy,2) );

            //calculando o angulo
            //verifica se Gx eh diferente de zero para evitar erros
            if (Gx == 0) {
                if (Gy == 0) {
                    imgEdges[i][j].Ang = 0;  //Gx e Gy = 0
                } else {
                    imgEdges[i][j].Ang = 90;  //Gx=0 e Gy!=0
                }
            } else {
                //usa formula padrao de obtencao do angulo
                rad = atan2f(Gy, Gx);         //atanf retorna entre -PI e +PI
                imgEdges[i][j].Ang = RAD2DEG(rad);  //converte para graus
            }

            //angulos devem ser positivos
            if (imgEdges[i][j].Ang < 0) {
                imgEdges[i][j].Ang = 360 - fabsf(imgEdges[i][j].Ang);
            }

            //limita valores do gradiente ao intervalo [0,255]
            if (imgEdges[i][j].G>255) {
                imgEdges[i][j].G = 255;
            }
            if (imgEdges[i][j].G<0) {
                imgEdges[i][j].G = 0;
            }

            //Aplica threshold = find prominent edges
            if (imgEdges[i][j].G <= TS_G) {
                imgEdges[i][j].G = 0;
            }

        }
    } //fim - percorre imagem
}


float *ReadEOAC(char *filename) {
    FILE *f;
    float *eoac=NULL;

    eoac = (float*) calloc((QTD_ANG*QTD_DIST), sizeof(float));

    if ((f = fopen(filename, "rb")) == NULL) {
        printf("erro abrindo arquivo: %s\n", filename);
        exit(0);
    }
    fread(eoac, sizeof(float), (QTD_ANG*QTD_DIST), f);
    fclose(f);
    return eoac;
}

