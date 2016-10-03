#include "canny.h"

bool ValidPixelEdges(int nrows, int ncols, int x, int y)
{
  if ((x >= 0)&&(x < ncols)&&
      (y >= 0)&&(y < nrows))
    return(true);
  else
    return(false);
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

void RGB2HSV_canny(CImage *RGB, ImageHSV **HSV) {

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

void Smooth(Image *img, Image *imgSuave) {
    int i,j,k,m; //contadores
    int aux;

    int mConv[5][5] = {   //mascara de convolucao = para sigma=1.4, ou 0.4 segundo WikiPedia
        {2, 4, 5, 4, 2},
        {4, 9,12, 9, 4},
        {5,12,15,12, 5},
        {4, 9,12, 9, 4},
        {2, 4, 5, 4, 2}
    };
    int soma=159; //soma dos valores da mascara

    //percorre a imagem
    for (i = 0; i < img->nrows; i++) {
        for (j = 0; j < img->ncols; j++) {

            aux = 0; //inicializa acumuladores

            //posiciona a janela com o centro no pixel atual da imagem (i,j)
            for (k=-2; k<=2; k++) {
                for (m=-2; m<=2; m++) {
                    //verifica se o ponto eh valido
                    if ( ValidPixel(img,(j+k),(i+m)) ) {
                        aux += mConv[m+2][k+2] * img->val[(j+k)+img->tbrow[(i+m)]];
                        //printf("atual(%d,%d)=%d*%d=%d\n", i, j, mConv[m+2][k+2], img->val[(j+k)+img->tbrow[(i+m)]], aux);
                    }
                }
            } //fim mascara = janela deslizante

            imgSuave->val[j+imgSuave->tbrow[i]] = (int) aux/soma;
            if (imgSuave->val[j+imgSuave->tbrow[i]] > 255) {
                printf("maior que 255 -> aux=%d\tval=%d\n", aux, imgSuave->val[j+imgSuave->tbrow[i]]);
            }

        }
    } //fim - percorre imagem
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

//ajusta as direcoes do gradiente para 0, 45, 90 ou 135 graus
void NormDirection(ImageEdges **imgEdges, int nrows, int ncols) {

    int i, j;

    //percorre a imagem
    for (i = 0; i < nrows; i++) {
        for (j = 0; j < ncols; j++) {

            //estes ifs abaixo aceitam angulos acima de 180 graus
            if ( (imgEdges[i][j].Ang >= 0 && imgEdges[i][j].Ang < 22.5) || (imgEdges[i][j].Ang >= 157.5 && imgEdges[i][j].Ang <= 180) || 
                 (imgEdges[i][j].Ang > 180 && imgEdges[i][j].Ang < 202.5) || (imgEdges[i][j].Ang >= 337.5 && imgEdges[i][j].Ang <= 360)) {
                imgEdges[i][j].Ang = 0;
            } else if ( (imgEdges[i][j].Ang >= 22.5 && imgEdges[i][j].Ang < 67.5) || (imgEdges[i][j].Ang >= 202.5 && imgEdges[i][j].Ang < 247.5)){
                imgEdges[i][j].Ang = 45;
            } else if ( (imgEdges[i][j].Ang >= 67.5 && imgEdges[i][j].Ang < 112.5) || (imgEdges[i][j].Ang >= 247.5 && imgEdges[i][j].Ang < 292.5)) {
                imgEdges[i][j].Ang = 90;
            } else if ( (imgEdges[i][j].Ang >= 112.5 && imgEdges[i][j].Ang < 157.5) || (imgEdges[i][j].Ang >= 292.5 && imgEdges[i][j].Ang < 337.5)) {
                imgEdges[i][j].Ang = 135;
            } else {
                printf("angulo acima de 180 graus! Erro!!!\t ang->%f\n", imgEdges[i][j].Ang);
            }
        }
    }
}

//aplica non-maximum suppression = deixa borda fina
void NonMaxSuppression(ImageEdges **imgEdges, Image *img) {

    int i, j;

    //percorre a imagem
    for (i = 0; i < img->nrows; i++) {
        for (j = 0; j < img->ncols; j++) {

            //caso 1: Ang=0, verifica vizinho: acima e abaixo
            if (imgEdges[i][j].Ang == 0) {

                //pixel acima e abaixo sao validos
                if (ValidPixel(img,j,i-1) && ValidPixel(img,j,i+1) && (imgEdges[i][j].G > imgEdges[i-1][j].G && imgEdges[i][j].G > imgEdges[i+1][j].G)) {
                    img->val[j+img->tbrow[i]] = imgEdges[i][j].G;
                } else img->val[j+img->tbrow[i]] = 0;

            //caso 2: Ang=90, verifica vizinho: esquerda e direita
            } else if (imgEdges[i][j].Ang == 90) {

                //pixel esquerda e direita sao validos
                if (ValidPixel(img,j-1,i) && ValidPixel(img,j+1,i) && (imgEdges[i][j].G > imgEdges[i][j-1].G && imgEdges[i][j].G > imgEdges[i][j+1].G)) {
                    img->val[j+img->tbrow[i]] = imgEdges[i][j].G;
                } else img->val[j+img->tbrow[i]] = 0;

            //caso 3: Ang=135, verifica vizinho: diagonal superior direita e diagona inferior esquerda
            } else if (imgEdges[i][j].Ang == 45) {

                //pixel diagonal superior direita e diagona inferior esquerda sao validos
                if (ValidPixel(img,j+1,i-1) && ValidPixel(img,j-1,i+1) && (imgEdges[i][j].G > imgEdges[i-1][j+1].G && imgEdges[i][j].G > imgEdges[i+1][j-1].G)) {
                    img->val[j+img->tbrow[i]] = imgEdges[i][j].G;
                } else img->val[j+img->tbrow[i]] = 0;

            //caso 4: Ang=45, verifica vizinho: diagonal superior esquerda e diagona inferior direita
            } else if (imgEdges[i][j].Ang == 135) {

                //pixel diagonal superior esquerda e diagona inferior direita sao validos
                if (ValidPixel(img,j-1,i-1) && ValidPixel(img,j+1,i+1) && (imgEdges[i][j].G > imgEdges[i-1][j-1].G && imgEdges[i][j].G > imgEdges[i+1][j+1].G)) {
                    img->val[j+img->tbrow[i]] = imgEdges[i][j].G;
                } else img->val[j+img->tbrow[i]] = 0;

            } else {
                printf("NAO ERA PRA TER ENTRADO AQUI!!! Angulo classificado errado!!\n");
            }

        } //fim - percorre imagem
    }

}

//procura vizinhos maiores que T2 -- i=linha, j=coluna
void CaminhaPelaBorda(Image *imgT2, int i, int j, Image *imgF) {

    int k, m;

    for (k=-1; k<=1; k++) {
        for (m=-1; m<=1; m++) {
            //se encontrar um vizinho q esta na imgT2 e ainda nao esta na imgF, marca ele como borda e caminha a partir dele
            if (ValidPixel(imgT2,(j+k),(i+m)) && imgT2->val[(j+k)+imgT2->tbrow[(i+m)]] != 0 && imgF->val[(j+k)+imgF->tbrow[(i+m)]] == 0) {
                imgF->val[(j+k)+imgF->tbrow[(i+m)]] = 255;
                CaminhaPelaBorda(imgT2, i+m, j+k, imgF); //caminha a partir dele
            }
        }
    }

}

Image *Hysteresis(Image *img, int T1, int T2) {

    int i, j, k, m;
    Image *imgT1=NULL, *imgT2=NULL, *imgF=NULL;

    imgT1 = CreateImage(img->ncols, img->nrows);
    imgT2 = CreateImage(img->ncols, img->nrows);
    imgF  = CreateImage(img->ncols, img->nrows);

    //percorre a imagem
    for (i = 0; i < img->nrows; i++) {
        for (j = 0; j < img->ncols; j++) {

            if (img->val[j+img->tbrow[i]] > T1) {
                imgT1->val[j+imgT1->tbrow[i]] = img->val[j+img->tbrow[i]];
            } else if (img->val[j+img->tbrow[i]] > T2) {
                imgT2->val[j+imgT2->tbrow[i]] = img->val[j+img->tbrow[i]];
            }

        }
    }

    //WriteImage(imgT1, "hysteresis_T1.ppm");
    //WriteImage(imgT2, "hysteresis_T2.ppm");

    //percorre a imagem
    for (i = 0; i < img->nrows; i++) {
        for (j = 0; j < img->ncols; j++) {

            //verifica se o ponto esta acima de T1
            if (imgT1->val[j+imgT1->tbrow[i]] != 0) {
                imgF->val[j+imgF->tbrow[i]] = 255;  //eh borda com certeza

                //se existir um vizinho de T1 que seja maior que T2, tambem eh borda
                for (k=-1; k<=1; k++) {
                    for (m=-1; m<=1; m++) {
                        if ( ValidPixel(imgT2,(j+k),(i+m)) && ( (imgT2->val[(j+k)+imgT2->tbrow[(i+m)]] != 0) ) ) {
                            imgF->val[(j+k)+imgF->tbrow[(i+m)]] = 255;  //eh borda caso o ponto esteja ligado a T1
                            CaminhaPelaBorda(imgT2, (i+m), (j+k), imgF);
                        }
                    }
                }
            }
        }
    }
    //WriteImage(imgF, "hysteresis_Final.ppm");

    DestroyImage(&imgT1);
    DestroyImage(&imgT2);
    return(imgF);
}


Image *Canny(CImage *cimg, int T1, int T2) {
    //CImage *cimg=NULL; //, *cimgTemp=NULL;
    Image *img=NULL, *imgSuave=NULL, *imgFinal=NULL, *imgBordas=NULL; //, *imgTemp=NULL;
    ImageHSV **imgHSV; //imagem no espaco HSV - mesmo tamanho da imagem original
    ImageEdges **imgEdges; //imagem de bordas

    int i,j;

    //leitura da imagem de entrada
    //cimg = ReadCImage(argv[1]); //JA VIRA POR PARAMETRO

    //Alocacao da imagem no espaco HSV
    imgHSV = (ImageHSV**) calloc(cimg->C[0]->nrows, sizeof(ImageHSV*));
    for (i=0; i<cimg->C[0]->nrows; i++) {
        imgHSV[i] = (ImageHSV*) calloc(cimg->C[0]->ncols, sizeof(ImageHSV));
    }

    //Alocacao da imagem de bordas
    imgEdges = (ImageEdges**) calloc(cimg->C[0]->nrows, sizeof(ImageEdges*));
    for (i=0; i<cimg->C[0]->nrows; i++) {
        imgEdges[i] = (ImageEdges*) calloc(cimg->C[0]->ncols, sizeof(ImageEdges));
    }
    ////////////////////////////////
    //FIM ALOCACOES DE MEMORIA
    ////////////////////////////////

    //converter para HSV
    RGB2HSV_canny(cimg, imgHSV);

    //cria imagem cinza
    img = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);
    imgSuave = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);
    imgFinal = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);

    //copia canal V do HSV para a imagem cinza
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            img->val[j+img->tbrow[i]] = imgHSV[i][j].V;
        }
    }

    //Suaviza - Gaussian smooth
    Smooth(img, imgSuave);

    //Aplica sobel na img cinza
    Sobel(imgSuave, imgEdges);
    //!SOBEL ESTA OK (PELO MENOS O GRADIENTE; NAO GARANTO OS ANGULOS)!!!!!!!!!!!

    //Normaliza direcoes do gradiente (0, 45, 90 ou 135)
    NormDirection(imgEdges, cimg->C[0]->nrows, cimg->C[0]->ncols);

    //Supressao de nao-maximos
    NonMaxSuppression(imgEdges, imgFinal);

    //Hysteresis
    imgBordas = Hysteresis(imgFinal, T1, T2);

    //////////////////////////////////////////////////
    //liberando memoria imgHSV
    for (i=0; i<cimg->C[0]->nrows; i++) {
        free(imgHSV[i]);
    }
    free(imgHSV);

    //liberando memoria imgEdges
    for (i=0; i<cimg->C[0]->nrows; i++) {
        free(imgEdges[i]);
    }
    free(imgEdges);

    DestroyImage(&img);
    DestroyImage(&imgSuave);
    DestroyImage(&imgFinal);

    return(imgBordas);
}

