#include "libdescriptors.h"

int main(int argc, char** argv)
{
    CImage *cimg=NULL;
    Image *img=NULL;
    //ImageHSV **imgHSV; //imagem no espaco HSV - mesmo tamanho da imagem original
    ImageEdges **imgEdges; //imagem de bordas (detectadas por Sobel)
    int i, j,c;
    double *coefs=NULL;
    char * channel = argv[3];
    c = atoi(channel);



    if (argc != 4) {
        fprintf(stderr,"usage: spytec_extraction <image_path> <fv_path> <channel>\n");
        exit(-1);
    }

    //leitura da imagem de entrada
    cimg = ReadCImage(argv[1]);

    //cria imagem HSV
    //imgHSV = CreateImageHSV(cimg->C[0]->ncols, cimg->C[0]->nrows);

    //cria imagem de bordas
    imgEdges = CreateImageEdges(cimg->C[0]->ncols, cimg->C[0]->nrows);

    //converter para HSV
    //RGB2HSV_spytec(cimg, imgHSV);

    //cria imagem cinza
    img = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);

    //copia canal V do HSV para a imagem cinza
    /*for (i = 0; i < cimg->C[0]->nrows; i++) {
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
    //copia G da imgEdges para imagem cinza
    //printf("Imagem de bordas\n");
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            img->val[j+img->tbrow[i]] = (int) imgEdges[i][j].G;
            //printf("%d\t", img->val[j+img->tbrow[i]]);
        }
        //printf("\n");
    }
    WriteImage(img, "sobel.ppm");

    //detecta bordas na imagem de entrada
    //imgBordas = Canny(cimg, 80, 30);

    //cria imagens de coeficientes wavelet (metade do tamanho da imagem original em ambas dimensoes)
    DImage *imgHH = CreateDImage((int)((img->ncols+1)/2.0), (int)((img->nrows+1)/2.0));
    DImage *imgHL = CreateDImage((int)((img->ncols+1)/2.0), (int)((img->nrows+1)/2.0));
    DImage *imgLH = CreateDImage((int)((img->ncols+1)/2.0), (int)((img->nrows+1)/2.0));
    DImage *imgLL = CreateDImage((int)((img->ncols+1)/2.0), (int)((img->nrows+1)/2.0));

    //aplica Haar Wavelets
    HaarWaveletDecomposition(img, imgHH, imgHL, imgLH, imgLL);

    //normaliza coeficientes wavelet para intervalo [0,1]
/*    Normalize(imgHH);
    Normalize(imgHL);
    Normalize(imgLH);
    Normalize(imgLL);*/
    //NormalizaMax normaliza pelo max valor encontrado na respectiva imagem
/*    NormalizeMax(imgHH);
    NormalizeMax(imgHL);
    NormalizeMax(imgLH);
    NormalizeMax(imgLL);
*/
    //NormalizaMaxHH normaliza pelo max valor encontrado na imagem HH
    double max = 0.0;
    for (i = 0; i < imgHH->nrows; i++) {
        for (j = 0; j < imgHH->ncols; j++) {
            if (imgHH->val[j+imgHH->tbrow[i]] > max) 
                max = imgHH->val[j+imgHH->tbrow[i]];
        }
    }
    NormalizeMaxHH(imgHH, max);
    NormalizeMaxHH(imgHL, max);
    NormalizeMaxHH(imgLH, max);
    NormalizeMaxHH(imgLL, max);
/**/


    //ordena coeficientes e retorna os QTD_COEFS maiores
    coefs = OrdenaCoeficientes(imgHH, imgHL, imgLH, imgLL);
    //feature vector sao os 'QTD_COEFS' maiores coeficientes
    //printf("foi ateh a ordenacao\n");

    //arquivo com as caracteristicas (fv)
    WriteFileDouble(argv[2], coefs, QTD_COEFS);

    //DestroyImageHSV(imgHSV, cimg->C[0]->nrows);
    /*for (i=0; i<cimg->C[0]->nrows; i++) {
        free(imgHSV[i]);
    }
    free(imgHSV);
    //printf("liberou HSV\n");*/

    //DestroyImageEdges(imgEdges, cimg->C[0]->nrows);
    for (i=0; i<cimg->C[0]->nrows; i++) {
        free(imgEdges[i]);
    }
    free(imgEdges);
    //printf("liberou edges\n");

    free(coefs);

    DestroyDImage(&imgHH);
    DestroyDImage(&imgHL);
    DestroyDImage(&imgLH);
    DestroyDImage(&imgLL);
    //printf("liberou dimgs\n");
    DestroyCImage(&cimg);
    //printf("liberou cimg\n");
    DestroyImage(&img);
    //printf("liberou img\n");

    return(0);
}
