#include "libdescriptors.h"

typedef struct _imageHSV {
  float H,S,V;
} ImageHSV;

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

void RGB2HSV_unser(CImage* RGB, ImageHSV** HSV) {

    float r, g, b;
    float minVal, maxVal, delta;
    //float del_R, del_G, del_B;
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


int main(int argc, char** argv)
{
    CImage *cimg=NULL; //imagem de entrada
    ImageHSV **imgHSV; //imagem no espaco HSV - mesmo tamanho da imagem original
    Image *img=NULL;
    //Histogram *unser=NULL;
    float *unser=NULL;/*ALTERACOES */
    int i,j;

    if (argc != 3) {
        fprintf(stderr,"usage: unser_extraction <img_path> <fv_path>\n");
        exit(-1);
    }

    //le img colorida
    cimg = ReadCImage(argv[1]);

    //Alocacao da imagem no espaco HSV
    imgHSV = (ImageHSV**) calloc(cimg->C[0]->nrows, sizeof(ImageHSV*));
    for (i=0; i<cimg->C[0]->nrows; i++) {
        imgHSV[i] = (ImageHSV*) calloc(cimg->C[0]->ncols, sizeof(ImageHSV));
    }

    //converte para HSV
    RGB2HSV_unser(cimg, imgHSV);

    /*Imprime imagem HSV na tela
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            printf("%f\t%f\t%f\t|\t", imgHSV[i][j].H, imgHSV[i][j].S, imgHSV[i][j].V);
        }
        printf("\n");
    }
    printf("\n");
    /**/

    //cria imagem cinza
    img = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);

    //copia canal V do HSV para a imagem cinza
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            img->val[j+img->tbrow[i]] = imgHSV[i][j].V;
        }
    }

    /*Imprime imagem CINZA na tela
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            printf("%d\t|\t", img->val[j+img->tbrow[i]]);
        }
        printf("\n");
    }
    printf("\n");
    /**/

    ////////////////////////////////////////////////////////////////////////
    /* ALTERACOES PARA USAR VETOR COM floats (como sugerido pelo Jurandy) */
    //unser = Unser(img);  //antes a funcao retornava um Histogram
    unser = Unser(img);    //agora retorna um float*

    //WriteFileHistogram(unser,argv[2]);
    //grava vetor de floats (ao inves de uchar da versao original)
    /*printf("%d\n",SIZE);
    for (i=0; i < SIZE; i++) {
        printf("%f\t",unser[i]);
    }
    printf("\n");*/
    FILE *fp;
    if ((fp = fopen(argv[2], "wb")) == NULL) {
        printf("ERRO CRIANDO ARQUIVO DE FV\n");
        exit(0);
    }
    fwrite(unser, sizeof(float), SIZE, fp);
    fclose(fp);
    //WriteStreamHistogram(h, fp);
    /* FIM = ALTERACOES */
    ////////////////////////////////////////////////////////////////////////

    //liberando memoria
    for (i=0; i<cimg->C[0]->nrows; i++) {
        free(imgHSV[i]);
    }
    free(imgHSV);

    //DestroyHistogram(&unser);
    free(unser); /* ALTERACOES */

    DestroyCImage(&cimg);
    DestroyImage(&img);

    return(0);
}
