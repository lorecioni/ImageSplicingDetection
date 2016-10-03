#include "libtexturedescriptors.h"

void Extraction(char *img_path, char *fv_path)
{
    CImage *cimg=NULL;
    Image *img=NULL;
    ImageHSV **imgHSV; //imagem no espaco HSV - mesmo tamanho da imagem original
    FeatureVector1D *sasi=NULL;
    int i,j;
    int sizes[3];
    sizes[0]=3;
    sizes[1]=5;
    sizes[2]=7;
    FILE *fout;

    cimg = ReadCImage(img_path);
    img = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);

    //Alocacao da imagem no espaco HSV
    imgHSV = (ImageHSV**) calloc(cimg->C[0]->nrows, sizeof(ImageHSV*));
    for (i=0; i<cimg->C[0]->nrows; i++) {
        imgHSV[i] = (ImageHSV*) calloc(cimg->C[0]->ncols, sizeof(ImageHSV));
    }

    //converte para HSV
    RGB2HSV_sasi(cimg, imgHSV);

    //copia canal V do HSV para a imagem cinza
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            img->val[j+img->tbrow[i]] = imgHSV[i][j].V;
        }
    }

    sasi = SASI(img, 3, sizes);

    if ((fout = fopen(fv_path, "wb")) == NULL) {
        printf("ERRO CRIANDO ARQUIVO DE FV\n");
        exit(0);
    }
    //fprintf(fout,"%d\n",sasi->n);
    fwrite(&sasi->n,sizeof(int),1,fout);
    fwrite(sasi->X,sizeof(double),sasi->n,fout);
    fclose(fout);

    //liberando memoria imgHSV
    for (i=0; i<cimg->C[0]->nrows; i++) {
        free(imgHSV[i]);
    }
    free(imgHSV);

    DestroyFeatureVector1D(&sasi);
    DestroyCImage(&cimg);
    DestroyImage(&img);

}

double Distance(char *fv1_path, char *fv2_path)
{
    FeatureVector1D *sasi1=NULL;
    FeatureVector1D *sasi2=NULL;
    double distance;

    sasi1 = ReadFileFeatureVector1DBin(fv1_path);
    sasi2 = ReadFileFeatureVector1DBin(fv2_path);

    distance = DotDistance(sasi1, sasi2); // 1/semelhanca -1 

    DestroyFeatureVector1D(&sasi1);
    DestroyFeatureVector1D(&sasi2);

    return (distance);
}
