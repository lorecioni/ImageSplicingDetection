#include "libdescriptors.h"

void Extraction(char *img_path, char *fv_path)
{
    CImage *cimg=NULL; //imagem de entrada
    ImageHSV **imgHSV; //imagem no espaco HSV - mesmo tamanho da imagem original
    Image *img=NULL;
    float *unser=NULL;
    int i,j;

    //le img colorida
    cimg = ReadCImage(img_path);

    //Alocacao da imagem no espaco HSV
    imgHSV = (ImageHSV**) calloc(cimg->C[0]->nrows, sizeof(ImageHSV*));
    for (i=0; i<cimg->C[0]->nrows; i++) {
        imgHSV[i] = (ImageHSV*) calloc(cimg->C[0]->ncols, sizeof(ImageHSV));
    }

    //converte para HSV
    RGB2HSV_unser(cimg, imgHSV);

    //cria imagem cinza
    img = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);

    //copia canal V do HSV para a imagem cinza
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            img->val[j+img->tbrow[i]] = imgHSV[i][j].V;
        }
    }

    unser = Unser_float(img);    //agora retorna um float*

    FILE *fp;
    if ((fp = fopen(fv_path, "wb")) == NULL) {
        printf("ERRO CRIANDO ARQUIVO DE FV\n");
        exit(0);
    }
    fwrite(unser, sizeof(float), SIZE, fp);
    fclose(fp);

    //liberando memoria
    for (i=0; i<cimg->C[0]->nrows; i++) {
        free(imgHSV[i]);
    }
    free(imgHSV);

    free(unser);
    DestroyCImage(&cimg);
    DestroyImage(&img);

}


double Distance(char *fv1_path, char *fv2_path)
{
    float *unser1=NULL;
    float *unser2=NULL;
    double distance;
    int i;

    unser1 = ReadUnser(fv1_path);
    unser2 = ReadUnser(fv2_path);

    distance=0.0;
    for (i=0; i<SIZE; i++) {
        distance += (double) fabsf(unser1[i] - unser2[i]);
    }
    //printf("%lf\n",distance);
    free(unser1);
    free(unser2);

    return distance;
}
