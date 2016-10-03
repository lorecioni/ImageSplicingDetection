#include "libdescriptors.h"

void Extraction(char *img_path, char *fv_path)
{
    CImage *cimg=NULL;
    Image *img=NULL;
    ImageHSV **imgHSV; //imagem no espaco HSV - mesmo tamanho da imagem original
    Image *g1=NULL, *g2=NULL, *g3=NULL, *g4=NULL; //imgs dos gradientes LAS
    int i, j;
    float *hist=NULL;

    //leitura da imagem de entrada
    cimg = ReadCImage(img_path);

    //cria imagem HSV
    imgHSV = (ImageHSV**) calloc(cimg->C[0]->nrows, sizeof(ImageHSV*));
    for (i=0; i<cimg->C[0]->nrows; i++) {
        imgHSV[i] = (ImageHSV*) calloc(cimg->C[0]->ncols, sizeof(ImageHSV));
    }

    //converter para HSV
    RGB2HSV_las(cimg, imgHSV);
    //cria imagem cinza
    img = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);
    //copia canal V do HSV para a imagem cinza
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            img->val[j+img->tbrow[i]] = imgHSV[i][j].V;
        }
    }

    //calcula g1, g2, g3, g4
    g1 = CreateImage(img->ncols, img->nrows);  //cria as imgs
    g2 = CreateImage(img->ncols, img->nrows);
    g3 = CreateImage(img->ncols, img->nrows);
    g4 = CreateImage(img->ncols, img->nrows);
    CalculaGradientes(img, g1, g2, g3, g4);

    //hist = (float*) calloc(QTD_BINS*QTD_BINS*QTD_BINS*QTD_BINS, sizeof(float));
    hist = HistogramaGradiente(g1, g2, g3, g4);
    NormalizaHistograma(hist, QTD_BINS*QTD_BINS*QTD_BINS*QTD_BINS, img->nrows*img->ncols);

    WriteFileFloat(fv_path, hist, QTD_BINS*QTD_BINS*QTD_BINS*QTD_BINS);

    //liberando memoria
    for (i=0; i<cimg->C[0]->nrows; i++) {
        free(imgHSV[i]);
    }
    free(imgHSV);
    free(hist);

    DestroyCImage(&cimg);
    DestroyImage(&img);
    DestroyImage(&g1);
    DestroyImage(&g2);
    DestroyImage(&g3);
    DestroyImage(&g4);
}


double Distance(char *fv1_path, char *fv2_path)
{
    float *las1=NULL;
    float *las2=NULL;
    double distance;

    las1 = ReadFileFloat(fv1_path, QTD_BINS*QTD_BINS*QTD_BINS*QTD_BINS);
    las2 = ReadFileFloat(fv2_path, QTD_BINS*QTD_BINS*QTD_BINS*QTD_BINS);

    distance=L1FloatDistance(las1, las2, QTD_BINS*QTD_BINS*QTD_BINS*QTD_BINS);
    //printf("%lf\n",distance);

    free(las1);
    free(las2);

    return(distance);
}
