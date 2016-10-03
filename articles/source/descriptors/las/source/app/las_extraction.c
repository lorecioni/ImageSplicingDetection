#include "libdescriptors.h"

int main(int argc, char** argv)
{
    CImage *cimg=NULL;
    Image *img=NULL;
    //ImageHSV **imgHSV; //imagem no espaco HSV - mesmo tamanho da imagem original
    Image *g1=NULL, *g2=NULL, *g3=NULL, *g4=NULL; //imgs dos gradientes LAS
    int i, j;
    float *hist=NULL;

    if (argc != 4) {
        fprintf(stderr,"usage: las_extraction <image_path> <fv_path> <channel_number>\n");
        exit(-1);
    }
	
    /*//leitura da imagem de entrada
    cimg = ReadCImage(argv[1]);

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
    }*/


    // adicionei essa parte para ja entrar com a imagem no espaço desejado
    cimg = ReadCImage(argv[1]);
    int c = atoi(argv[3]);
    img = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);

    //copia o canal desejado para a imagem cinza
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            img->val[j+img->tbrow[i]] = cimg->C[c]->val[j+cimg->C[c]->tbrow[i]];
        }
    }
   //WriteImage(img,"teste.ppm");
   //////////////////////////////////////////////////////////////////////////

    //calcula g1, g2, g3, g4
    g1 = CreateImage(img->ncols, img->nrows);  //cria as imgs
    g2 = CreateImage(img->ncols, img->nrows);
    g3 = CreateImage(img->ncols, img->nrows);
    g4 = CreateImage(img->ncols, img->nrows);
    CalculaGradientes(img, g1, g2, g3, g4);

    //hist = (float*) calloc(QTD_BINS*QTD_BINS*QTD_BINS*QTD_BINS, sizeof(float));
    hist = HistogramaGradiente(g1, g2, g3, g4);

    NormalizaHistograma(hist, QTD_BINS*QTD_BINS*QTD_BINS*QTD_BINS, img->nrows*img->ncols);
/*
    float soma=0;
    for (i=0; i<QTD_BINS*QTD_BINS*QTD_BINS*QTD_BINS; i++) {
        if (hist[i] != 0) {
            printf("[%d]=%f\n", i, hist[i]);
            soma += hist[i];
        }
    }
    printf("\nsoma=%f\n", soma);
*/
    WriteFileFloat(argv[2], hist, QTD_BINS*QTD_BINS*QTD_BINS*QTD_BINS);

    //liberando memoria
    /*for (i=0; i<cimg->C[0]->nrows; i++) {
        free(imgHSV[i]);
    }
    free(imgHSV);*/
    free(hist);

    DestroyCImage(&cimg);
    DestroyImage(&img);
    DestroyImage(&g1);
    DestroyImage(&g2);
    DestroyImage(&g3);
    DestroyImage(&g4);

    return(0);
}
