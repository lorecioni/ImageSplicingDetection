#include "libcolordescriptors.h"

int main(int argc, char** argv) {
    CImage *cimg=NULL;
    Image *img=NULL;//, *imgTemp=NULL;
    //ImageHSV **imgHSV; //imagem no espaco HSV - mesmo tamanho da imagem original
    ImageEdges **imgEdges; //imagem de bordas (detectadas por Sobel)

    int distances[QTD_DIST] = {1, 3, 5, 7};  //distancias consideradas no autocorrelograma
    int **eoac=NULL;  //autocorrelograma
    float **eoac_norm=NULL; //autocorrelograma normalizado

    int i,j,c;
    int sum;

    char * channel = argv[3];
    c = atoi(channel);

    FILE *fout;

    if (argc != 4) {
        fprintf(stderr,"usage: eoac_extraction <image_path> <fv_path> <channel>\n");
        exit(-1);
    }

    //leitura da imagem de entrada
    cimg = ReadCImage(argv[1]);

    //Alocacao da imagem no espaco HSV
    /*imgHSV = (ImageHSV**) calloc(cimg->C[0]->nrows, sizeof(ImageHSV*));
    for (i=0; i<cimg->C[0]->nrows; i++) {
        imgHSV[i] = (ImageHSV*) calloc(cimg->C[0]->ncols, sizeof(ImageHSV));
    }*/

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
    //RGB2HSV_eoac(cimg, imgHSV);

    //cria imagem cinza
    img = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);

    /*//copia canal V do HSV para a imagem cinza
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            img->val[j+img->tbrow[i]] = imgHSV[i][j].V;
        }
    }*/

    //copia o canal desejado para a imagem cinza
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            img->val[j+img->tbrow[i]] = cimg->C[c]->val[j+cimg->C[c]->tbrow[i]];
        }
    }

    //aplica sobel na img cinza
    Sobel(img, imgEdges);

    /*USADO APENAS PARA VERIFICAR SE SOBEL ESTAVA FUNCIONANDO
    //cria imagem cinza temporaria pra visualizar bordas do Sobel
    imgTemp = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);
    //copia bordas para imgTemp
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            imgTemp->val[j+img->tbrow[i]] = (int) (imgEdges[i][j].G);
        }
    }
    WriteImage(imgTemp, "sobel.ppm");

    //copia orientacao das bordas para imgTemp
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            imgTemp->val[j+img->tbrow[i]] = (int) (imgEdges[i][j].Ang);
        }
    }
    WriteImage(imgTemp, "sobel_Ang.ppm");
    //exit(1);
    /*/

    //calcular autocorrelograma da imagem de bordas

    CalculaAutocorrelograma(cimg->C[0]->nrows, cimg->C[0]->ncols, imgEdges, distances, eoac);

    sum=0;
    //printf("Autocorrelograma:\n");
    for (i=0; i<QTD_ANG; i++) {
        //printf("[%d]\t", i);
        for (j=0; j<QTD_DIST; j++) {
            //printf("%d\t", eoac[i][j]);
            sum += eoac[i][j];  //conta o total de valores nos bins do eoac
        }
        //printf("\n");
    }
    //printf("\n");

    //normalizacao para invariancia a escala (por consequencia, ja normaliza para variacoes de iluminacao, segundo o paper)
    //dividir cada bin pelo total de valores em todos os bins (=sum)
    //printf("Autocorrelograma normalizado:\n");
    for (i=0; i<QTD_ANG; i++) {
        //printf("[%d]\t", i);
        for (j=0; j<QTD_DIST; j++) {
            eoac_norm[i][j] = (float) eoac[i][j]/sum;
            //printf("%f\t", eoac_norm[i][j]);
        }
        //printf("\n");
    }
    //printf("\n");
    //nao normalizei para rotacao, pois resultados com normalizacao apenas de escala foi melhor, segundo o paper


    //arquivo com as caracteristicas (fv)
    if ((fout = fopen(argv[2], "wb")) == NULL) {
        printf("ERRO CRIANDO ARQUIVO DE FV\n");
        exit(0);
    }

    fprintf(fout, "%d \n", (QTD_ANG*QTD_DIST));
    for (i=0; i<QTD_ANG; i++) {
        for (j=0; j<QTD_DIST; j++) {
            fprintf(fout, "%f ", eoac_norm[i][j]);
        }
    }
    //fprintf(fout, "\n");

    /*for (i=0; i<QTD_ANG; i++) {
        fwrite(eoac_norm[i], sizeof(float), QTD_DIST, fout);
    }*/
    fclose(fout);

    //eh pra ser igual os numeros abaixo????????????? nao... eh pra ser diferente mesmo
    //printf("Total de valores em eoac: %d\n", sum);
    //printf("Total de pixels na img: %d\n", cimg->C[0]->nrows*cimg->C[0]->ncols);

    //printf("SUCESSO!!!\n");

    //////////////////////////////////////////////////
    //liberando memoria imgHSV
    /*for (i=0; i<cimg->C[0]->nrows; i++) {
        free(imgHSV[i]);
    }
    free(imgHSV);*/

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
    //DestroyImage(&imgTemp);
    return(0);
}
