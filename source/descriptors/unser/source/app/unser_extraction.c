#include "libdescriptors.h"

int main(int argc, char** argv)
{
    CImage *cimg=NULL; //imagem de entrada
    //ImageHSV **imgHSV; //imagem no espaco HSV - mesmo tamanho da imagem original
    Image *img=NULL;
    //FeatureVector1D *unser=NULL;
    float *unser=NULL;
    int i,j;

    if (argc != 4) {
        fprintf(stderr,"usage: unser_extraction <image_path> <fv_path> <channel_number>\n");
        exit(-1);
    }

    /*//le img colorida
    cimg = ReadCImage(argv[1]);

    //Alocacao da imagem no espaco HSV
    imgHSV = (ImageHSV**) calloc(cimg->C[0]->nrows, sizeof(ImageHSV*));
    for (i=0; i<cimg->C[0]->nrows; i++) {
        imgHSV[i] = (ImageHSV*) calloc(cimg->C[0]->ncols, sizeof(ImageHSV));
    }

    //converte para HSV
    RGB2HSV_unser(cimg, imgHSV);*/

    /*Imprime imagem HSV na tela
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            printf("%f\t%f\t%f\t|\t", imgHSV[i][j].H, imgHSV[i][j].S, imgHSV[i][j].V);
        }
        printf("\n");
    }
    printf("\n");
    */

    //cria imagem cinza
    cimg = ReadCImage(argv[1]);
    int c = atoi(argv[3]);
    img = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);

    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            img->val[j+img->tbrow[i]] = cimg->C[c]->val[j+cimg->C[c]->tbrow[i]];
        }
    }

    /*//copia canal V do HSV para a imagem cinza
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            img->val[j+img->tbrow[i]] = imgHSV[i][j].V;
        }
    }*/

    /*Imprime imagem CINZA na tela
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            printf("%d\t|\t", img->val[j+img->tbrow[i]]);
        }
        printf("\n");
    }
    printf("\n");
     */

    unser = Unser_float(img);    //agora retorna um float*

//     printf("%d\n", unser->n);
//     for (i=0; i<unser->n; i++) {
//         printf("%f\t", unser->X[i]);
//     }
//     printf("\n");

    //VERSAO Q O JURANDY SUGERIU
    //WriteFileFeatureVector1D(unser,argv[2]);
    FILE *fp;
    if ((fp = fopen(argv[2], "wb")) == NULL) {
        printf("ERRO CRIANDO ARQUIVO DE FV\n");
        exit(0);
    }
    fprintf(fp,"%d\n",SIZE);
    for (i=0; i < SIZE; i++) {
        fprintf(fp,"%f ",(float)unser[i]);
    }

    //fwrite(unser, sizeof(float), SIZE, fp);
    fclose(fp);

    /*//liberando memoria
    for (i=0; i<cimg->C[0]->nrows; i++) {
        free(imgHSV[i]);
    }
    free(imgHSV);*/

    free(unser);
    DestroyCImage(&cimg);
    DestroyImage(&img);
    //DestroyFeatureVector1D(&unser);

    return(0);
}
