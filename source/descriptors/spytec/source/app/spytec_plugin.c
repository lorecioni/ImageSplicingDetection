#include "libdescriptors.h"

void Extraction(char *img_path, char *fv_path) {
    CImage *cimg=NULL;
    Image *img=NULL;
    ImageHSV **imgHSV; //imagem no espaco HSV - mesmo tamanho da imagem original
    ImageEdges **imgEdges; //imagem de bordas (detectadas por Sobel)
    int i, j;
    double *coefs=NULL;

    //leitura da imagem de entrada
    cimg = ReadCImage(img_path);

    //cria imagem HSV
    imgHSV = CreateImageHSV(cimg->C[0]->ncols, cimg->C[0]->nrows);

    //cria imagem de bordas
    imgEdges = CreateImageEdges(cimg->C[0]->ncols, cimg->C[0]->nrows);

    //converter para HSV
    RGB2HSV_spytec(cimg, imgHSV);

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
    //copia G da imgEdges para imagem cinza
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            img->val[j+img->tbrow[i]] = (int) imgEdges[i][j].G;
        }
    }
    //WriteImage(img, "sobel.ppm");

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

    //arquivo com as caracteristicas (fv)
    WriteFileDouble(fv_path, coefs, QTD_COEFS);

    for (i=0; i<cimg->C[0]->nrows; i++) {
        free(imgHSV[i]);
    }
    free(imgHSV);

    for (i=0; i<cimg->C[0]->nrows; i++) {
        free(imgEdges[i]);
    }
    free(imgEdges);

    free(coefs);

    DestroyDImage(&imgHH);
    DestroyDImage(&imgHL);
    DestroyDImage(&imgLH);
    DestroyDImage(&imgLL);
    DestroyCImage(&cimg);
    DestroyImage(&img);

}


double Distance(char *fv1_path, char *fv2_path)
{
    double *fv1=NULL;
    double *fv2=NULL;
    double distance=0;

    fv1 = ReadFileDouble(fv1_path, QTD_COEFS);
    fv2 = ReadFileDouble(fv2_path, QTD_COEFS);

    distance=L2DoubleDistance(fv1, fv2, QTD_COEFS);

    free(fv1);
    free(fv2);

    return distance;
}


