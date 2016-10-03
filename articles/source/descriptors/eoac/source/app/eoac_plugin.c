#include "libcolordescriptors.h"

void Extraction(char *img_path, char *fv_path) {
    CImage *cimg=NULL;
    Image *img=NULL;//, *imgTemp=NULL;
    ImageHSV **imgHSV; //imagem no espaco HSV - mesmo tamanho da imagem original
    ImageEdges **imgEdges; //imagem de bordas (detectadas por Sobel)

    int distances[QTD_DIST] = {1, 3, 5, 7};  //distancias consideradas no autocorrelograma
    int **eoac=NULL;  //autocorrelograma
    float **eoac_norm=NULL; //autocorrelograma normalizado

    int i,j;
    int sum;

    FILE *fout;

    //leitura da imagem de entrada
    cimg = ReadCImage(img_path);

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

    //Alocacao do autocorrelograma e do autocorrelograma normalizado
    eoac = (int**) calloc(QTD_ANG, sizeof(int*));  //QTD_ANG linhas = depende da quantizacao dos angulos
    eoac_norm = (float**) calloc(QTD_ANG, sizeof(float*));
    for (i=0; i<QTD_ANG; i++) {
        eoac[i] = (int*) calloc(QTD_DIST, sizeof(int));  //QTD_DIST colunas
        eoac_norm[i] = (float*) calloc(QTD_DIST, sizeof(float));
    }
    ////////////////////////////////
    //FIM ALOCACOES DE MEMORIA
    ////////////////////////////////


    //converter para HSV
    RGB2HSV_eoac(cimg, imgHSV);

    //cria imagem cinza
    img = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);

    //copia canal V do HSV para a imagem cinza
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            img->val[j+img->tbrow[i]] = imgHSV[i][j].V;
        }
    }

    //aplica sobel na img cinza
    Sobel(img, imgEdges);

    //Calcula autocorrelograma da imagem de bordas
    CalculaAutocorrelograma(cimg->C[0]->nrows, cimg->C[0]->ncols, imgEdges, distances, eoac);

    sum=0;
    for (i=0; i<QTD_ANG; i++) {
        for (j=0; j<QTD_DIST; j++) {
            sum += eoac[i][j];  //conta o total de valores nos bins do eoac
        }
    }

    //normalizacao para invariancia a escala (por consequencia, ja normaliza para variacoes de iluminacao, segundo o paper)
    //dividir cada bin pelo total de valores em todos os bins (=sum)
    for (i=0; i<QTD_ANG; i++) {
        for (j=0; j<QTD_DIST; j++) {
            eoac_norm[i][j] = (float) eoac[i][j]/sum;
        }
    }
    //nao normalizei para rotacao, pois resultados com normalizacao apenas de escala foi melhor, segundo o paper

    //arquivo com as caracteristicas (fv)
    if ((fout = fopen(fv_path, "wb")) == NULL) {
        printf("ERRO CRIANDO ARQUIVO DE FV\n");
        exit(0);
    }
    for (i=0; i<QTD_ANG; i++) {
        fwrite(eoac_norm[i], sizeof(float), QTD_DIST, fout);
    }
    fclose(fout);

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

    //liberando memoria eoac e eoac_norm
    for (i=0; i<QTD_ANG; i++) {
        free(eoac[i]);
        free(eoac_norm[i]);
    }
    free(eoac);
    free(eoac_norm);

    DestroyCImage(&cimg);
    DestroyImage(&img);

}

double Distance(char *fv1_path, char *fv2_path)
{
    float *eoac1=NULL, *eoac2=NULL;
    double distance;
    int i;

    eoac1 = ReadEOAC(fv1_path);
    eoac2 = ReadEOAC(fv2_path);

    distance=0.0;
    for (i=0; i<(QTD_ANG*QTD_DIST); i++) {
        distance += (double) fabsf(eoac1[i] - eoac2[i]);
    }

    //printf("%lf\n",distance);

    free(eoac1);
    free(eoac2);

    return distance;
}
