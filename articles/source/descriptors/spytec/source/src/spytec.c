#include "spytec.h"

ImageHSV **CreateImageHSV(int ncols, int nrows) {
    ImageHSV **imgHSV = NULL;
    int i;

    imgHSV = (ImageHSV**) calloc(nrows, sizeof(ImageHSV*));
    for (i=0; i<nrows; i++) {
        imgHSV[i] = (ImageHSV*) calloc(ncols, sizeof(ImageHSV));
    }
    return imgHSV;
}

void DestroyImageHSV(ImageHSV **img, int nrows) {
    int i;

    //printf("entrou na libera HSV\n");

    if (img != NULL) {
        for (i=0; i<nrows; i++) {
            free(img[i]);
        }
        free(img);
        img = NULL;
    }
}

ImageEdges **CreateImageEdges(int ncols, int nrows) {
    ImageEdges **img = NULL;
    int i;

    img = (ImageEdges**) calloc(nrows, sizeof(ImageEdges*));
    for (i=0; i<nrows; i++) {
        img[i] = (ImageEdges*) calloc(ncols, sizeof(ImageEdges));
    }
    return img;
}

void DestroyImageEdges(ImageEdges **img, int nrows) {
    int i;

    if (img != NULL) {
        for (i=0; i<nrows; i++) {
            free(img[i]);
        }
        free(img);
        img = NULL;
    }
}


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

void RGB2HSV_spytec(CImage *RGB, ImageHSV **HSV) {

    float r, g, b;
    float minVal, maxVal, delta;
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

void Sobel(Image *img, ImageEdges **imgEdges) {
    int i,j,k,m; //contadores
    int Gx, Gy;
    float rad; //guarda o angulo em radianos

    int mConvY[3][3] = {   //mascara de convolucao = SOBEL
        {-1,  0,  1},
        {-2,  0,  2},
        {-1,  0,  1}
    };

    int mConvX[3][3] = {   //mascara de convolucao = SOBEL
        { 1,  2,  1},
        { 0,  0,  0},
        {-1, -2, -1}
    };

    //percorre a imagem
    for (i = 0; i < img->nrows; i++) {
        for (j = 0; j < img->ncols; j++) {

            Gx = 0; //inicializa acumuladores
            Gy = 0;

            //posiciona a janela com o centro no pixel atual da imagem (x,y)
            for (k=-1; k<=1; k++) {
                for (m=-1; m<=1; m++) {
                    //verifica se o ponto eh valido
                    if ( ValidPixel(img,(j+k),(i+m)) ) {
                        Gx += mConvX[m+1][k+1] * img->val[(j+k)+img->tbrow[(i+m)]];
                        Gy += mConvY[m+1][k+1] * img->val[(j+k)+img->tbrow[(i+m)]];
                    }
                }
            } //fim mascara = janela deslizante

            //printf("Gx=%d\tGy=%d\tG=%f\n", Gx, Gy, sqrt(pow(Gx,2)+pow(Gy,2)));
            imgEdges[i][j].G = sqrt( pow(Gx,2) + pow(Gy,2) );

            //calculando o angulo
            //verifica se Gx eh diferente de zero para evitar erros
            if (Gx == 0) {
                if (Gy == 0) {
                    imgEdges[i][j].Ang = 0;  //Gx e Gy = 0
                } else {
                    imgEdges[i][j].Ang = 90;  //Gx=0 e Gy!=0
                }
            } else {
                //usa formula padrao de obtencao do angulo
                rad = atan2f(Gy, Gx);         //atanf retorna entre -PI e +PI
                imgEdges[i][j].Ang = RAD2DEG(rad);  //converte para graus
            }

            //angulos devem ser positivos
            if (imgEdges[i][j].Ang < 0) {
                imgEdges[i][j].Ang = 360 - fabsf(imgEdges[i][j].Ang);
            }

            //limita valores do gradiente ao intervalo [0,255]
            if (imgEdges[i][j].G>255) {
                //!!!!!!!!!!!!!VERIFICAR AQUI!!!!!!!!
                //imgEdges[i][j].G = 255; 
            }
            if (imgEdges[i][j].G<0) {
                imgEdges[i][j].G = 0;
            }

            //Aplica threshold = find prominent edges
            if (imgEdges[i][j].G <= TS_G) {
                imgEdges[i][j].G = 0;
            }

        }
    } //fim - percorre imagem
}


void HaarWaveletDecomposition(Image *img, DImage *imgResHH, DImage *imgResHL, DImage *imgResLH, DImage *imgResLL) {

    DImage *imgResRowsH=NULL, *imgResRowsL=NULL;
    int i, j;
    double HaarDenominador = 2.0;//sqrt(2.0); //usando haar como (1/raiz(2)   -1/raiz(2))

    imgResRowsH = CreateDImage((int)((img->ncols+1)/2.0), img->nrows);
    imgResRowsL = CreateDImage((int)((img->ncols+1)/2.0), img->nrows);

    /*FUNCAO DO JAVIER!!!!!!!!
    ii=0;
    for(i=0; i<(img->nrows*img->ncols)/2; i++) {
        ii   = i << 1;
        printf("ii=%d\t", ii);
        imgResRowsH->val[i] = (double)(img->val[ii] + img->val[ii+1]) / 2.0;  //alta freq H
        imgResRowsL->val[i] = (double)(img->val[ii] - img->val[ii+1]) / 2.0;  //baixa freq L
    }*/

    //!printf("tamanho imgOriginal = %dx%d\n", img->ncols, img->nrows);
    /*
    printf("imagem original:\n");
    for (i=0; i<img->nrows; i++) {
        for (j=0; j<img->ncols; j++) {
            printf("%d\t", img->val[j+img->tbrow[i]]);
        }
        printf("\n");
    }*/

    //Aplica Haar nas linhas
    for (i = 0; i < img->nrows; i++) {
        for (j = 0; j < img->ncols; j+=2) {
            if ( ValidPixel(img,(j+1),i) ) {
                imgResRowsH->val[(j/2)+imgResRowsH->tbrow[i]] = (img->val[j+img->tbrow[i]] + img->val[(j+1)+img->tbrow[i]])/HaarDenominador;
                imgResRowsL->val[(j/2)+imgResRowsL->tbrow[i]] = abs((img->val[j+img->tbrow[i]] - img->val[(j+1)+img->tbrow[i]])/HaarDenominador);
            } else {
                imgResRowsH->val[(j/2)+imgResRowsH->tbrow[i]] = img->val[j+img->tbrow[i]]/HaarDenominador; //se imagem tem largura impar, copia o pixel original
                imgResRowsL->val[(j/2)+imgResRowsL->tbrow[i]] = img->val[j+img->tbrow[i]]/HaarDenominador;
            }
        }
    }


    //!printf("tamanho imgResRows  = %dx%d\n", imgResRowsH->ncols, imgResRowsH->nrows);
    /*
    printf("imagem imgResRows:\n");
    for (i=0; i<imgResRowsH->nrows; i++) {
        for (j=0; j<imgResRowsH->ncols; j++) {
            printf("%.2lf\t",imgResRowsH->val[j+imgResRowsH->tbrow[i]]);
        }
        printf("\n");
    }*/

    //depois que aplicou nas linhas, aplica nas colunas

    //cria imagens de resultado
    //imgResHH = CreateDImage((int)((img->ncols+1)/2.0), (int)((img->nrows+1)/2.0));
    //imgResHL = CreateDImage((int)((img->ncols+1)/2.0), (int)((img->nrows+1)/2.0));
    //imgResLH = CreateDImage((int)((img->ncols+1)/2.0), (int)((img->nrows+1)/2.0));
    //imgResLL = CreateDImage((int)((img->ncols+1)/2.0), (int)((img->nrows+1)/2.0));

    //aplica nas colunas
    //Aplica Haar nas linhas
    for (i = 0; i < imgResRowsH->nrows; i+=2) {
        for (j = 0; j < imgResRowsH->ncols; j++) {
            //if ( ValidPixel(imgResRowsH,j,(i+1)) ) {
            if ( (i+1) < imgResRowsH->nrows ) {
                imgResHH->val[j+imgResHH->tbrow[(i/2)]] = (imgResRowsH->val[j+imgResRowsH->tbrow[i]] + imgResRowsH->val[j+imgResRowsH->tbrow[(i+1)]])/HaarDenominador;
                imgResHL->val[j+imgResHL->tbrow[(i/2)]] = fabs((imgResRowsH->val[j+imgResRowsH->tbrow[i]] - imgResRowsH->val[j+imgResRowsH->tbrow[(i+1)]])/HaarDenominador);
                imgResLH->val[j+imgResLH->tbrow[(i/2)]] = (imgResRowsL->val[j+imgResRowsL->tbrow[i]] + imgResRowsL->val[j+imgResRowsL->tbrow[(i+1)]])/HaarDenominador;
                imgResLL->val[j+imgResLL->tbrow[(i/2)]] = fabs((imgResRowsL->val[j+imgResRowsL->tbrow[i]] - imgResRowsL->val[j+imgResRowsL->tbrow[(i+1)]])/HaarDenominador);
            } else {
                imgResHH->val[j+imgResHH->tbrow[(i/2)]] = imgResRowsH->val[j+imgResRowsH->tbrow[i]]/HaarDenominador;
                imgResHL->val[j+imgResHL->tbrow[(i/2)]] = imgResRowsL->val[j+imgResRowsL->tbrow[i]]/HaarDenominador;
                imgResLH->val[j+imgResLH->tbrow[(i/2)]] = imgResRowsH->val[j+imgResRowsH->tbrow[i]]/HaarDenominador;
                imgResLL->val[j+imgResLL->tbrow[(i/2)]] = imgResRowsL->val[j+imgResRowsL->tbrow[i]]/HaarDenominador;
            }
        }
    }

    //!printf("tamanho imgResHH  = %dx%d\n", imgResHH->ncols, imgResHH->nrows);
    /**/
    Image *temp = CreateImage(imgResHH->ncols, imgResHH->nrows);
    //!printf("imagem imgResHH:\n");
    for (i=0; i<imgResHH->nrows; i++) {
        for (j=0; j<imgResHH->ncols; j++) {
            //printf("%.2lf\t",imgResHH->val[j+imgResHH->tbrow[i]]);
            temp->val[j+temp->tbrow[i]] = (int) imgResHH->val[j+imgResHH->tbrow[i]]; //copia pra img temp
        }
        //printf("\n");
    }
    //!WriteImage(temp, "imgHH.ppm");

    for (i=0; i<imgResHL->nrows; i++) {
        for (j=0; j<imgResHL->ncols; j++) {
            temp->val[j+temp->tbrow[i]] = (int) imgResHL->val[j+imgResHL->tbrow[i]]; //copia pra img temp
        }
    }
    //!WriteImage(temp, "imgHL.ppm");

    for (i=0; i<imgResLH->nrows; i++) {
        for (j=0; j<imgResLH->ncols; j++) {
            temp->val[j+temp->tbrow[i]] = (int) imgResLH->val[j+imgResLH->tbrow[i]]; //copia pra img temp
        }
    }
    //!WriteImage(temp, "imgLH.ppm");

    for (i=0; i<imgResLL->nrows; i++) {
        for (j=0; j<imgResLL->ncols; j++) {
            temp->val[j+temp->tbrow[i]] = (int) imgResLL->val[j+imgResLL->tbrow[i]]; //copia pra img temp
        }
    }
    //!WriteImage(temp, "imgLL.ppm");
    DestroyImage(&temp);
   /* */

    //libera as imagens temporarias
    DestroyDImage(&imgResRowsH);
    DestroyDImage(&imgResRowsL);

    //foram geradas as 4 imagens!
}

void Normalize(DImage *img) {
    int i,j;

    //double max = 1020.0; //valor maximo possivel: 4*255, pois sobel pode ser [1*255 + 2*255 + 1*255]
    double max = 255.0;

    //normaliza valores da imagem para o intervalo [0,1]
    for (i = 0; i < img->nrows; i++) {
        for (j = 0; j < img->ncols; j++) {
            img->val[j+img->tbrow[i]] = img->val[j+img->tbrow[i]] / max; //divide o valor por 255 (255 eh o maior possivel)
            //if (img->val[j+img->tbrow[i]] > 1) printf("VALOR MAIOR QUE 1: %lf\n", img->val[j+img->tbrow[i]]);
        }
    }
}

void NormalizeMax(DImage *img) {
    int i,j;

    //normaliza valores da imagem para o intervalo [0,1]
    double max = 0.0;
    for (i = 0; i < img->nrows; i++) {
        for (j = 0; j < img->ncols; j++) {
            if (img->val[j+img->tbrow[i]] > max) 
                max = img->val[j+img->tbrow[i]];
        }
    }
    //!printf("max=%lf\n", max);
    for (i = 0; i < img->nrows; i++) {
        for (j = 0; j < img->ncols; j++) {
            img->val[j+img->tbrow[i]] = img->val[j+img->tbrow[i]] / max; //divide o valor max
        }
    }
}

void NormalizeMaxHH(DImage *img, double max) {
    int i,j;

    //usa o maximo da imagem HH como o denominador

    //normaliza valores da imagem para o intervalo [0,1]
    for (i = 0; i < img->nrows; i++) {
        for (j = 0; j < img->ncols; j++) {
            img->val[j+img->tbrow[i]] = img->val[j+img->tbrow[i]] / max; //divide o valor max
        }
    }
}

//Usada pelo qsort para ordenar os coeficientes
int ComparaDouble(void * x, void * y) {
    double a = (*(double*)x);
    double b = (*(double*)y);
    //printf("x=%lf\ty=%lf\n", a, b);
    if ((a-b)<0.0)      return 1;
    else if ((a-b)>0.0) return -1;
    else                return  0;
}

//Combina os coeficientes das 4 imagens e retorna os F maiores num vetor
double *OrdenaCoeficientes(DImage *imgHH, DImage *imgHL, DImage *imgLH, DImage *imgLL) {

    int i;
    long int n;
    double *coefs=NULL;
    double *max_coefs=NULL;

    max_coefs = AllocDoubleArray(QTD_COEFS);
    coefs = AllocDoubleArray((imgHH->ncols*2)*(imgHH->nrows*2)); //cada img dos parametros tem 1/4 do tamanho original.
                                                                //para juntar as 4, basta duplicar cada dimensao dela

    //!printf("espaco alocado=%d\n",(imgHH->ncols*2)*(imgHH->nrows*2));

    for (i=0; i<(imgHH->ncols*imgHH->nrows); i++) {
        coefs[i] = imgHH->val[i]; //copia img pro vetor
    }
    //printf("coefs[%d]=%lf\n", i-1, coefs[i-1]);

    n = imgHH->nrows*imgHH->ncols;
    //!printf("fim img HH -> proximo indice do vetor de coef=%ld\n", n);
    for (i=0; i<(imgHL->ncols*imgHL->nrows); i++) {
        coefs[n+i] = imgHL->val[i]; //copia img pro vetor
    }
    //printf("coefs[%ld]=%lf\n", n+i-1, coefs[n+i-1]);

    n += imgHL->nrows*imgHL->ncols;
    //!printf("fim img HL -> proximo indice do vetor de coef=%ld\n", n);
    for (i=0; i<(imgLH->ncols*imgLH->nrows); i++) {
        coefs[n+i] = imgLH->val[i]; //copia img pro vetor
    }
    //printf("coefs[%ld]=%lf\n", n+i-1, coefs[n+i-1]);

    n += imgLL->nrows*imgLL->ncols;
    //!printf("fim img LH -> proximo indice do vetor de coef=%ld\n", n);
    for (i=0; i<(imgLL->ncols*imgLL->nrows); i++) {
        coefs[n+i] = imgLL->val[i]; //copia img pro vetor
    }
    //printf("coefs[%ld]=%lf\n", n+i-1, coefs[n+i-1]);

    //!printf("fim img LL -> ultimo indice do vetor de coef=%ld\n\n", n+i);
/*
    printf("Antes de ordenar:\n");
    for (i=0; i<(imgLL->ncols*imgLL->nrows)*4; i++) {
        printf("%lf ",coefs[i]);
    }
/**/
    //!printf("\n");

    qsort(coefs, ((imgHH->ncols*2)*(imgHH->nrows*2)), sizeof(double), (void*)ComparaDouble);

/*
    printf("DEpois de ordenar:\n");
    for (i=0; i<(imgLL->ncols*imgLL->nrows)*4; i++) {
        printf("%lf ",coefs[i]);
    }
/**/
/**/
    //!printf("Maiores coefs:\n");
    //Nao copia o maior coeficiente - assim como em Fourier (segundo o Jurandy)
    for (i=1; i<QTD_COEFS+1; i++) {
        //!printf("%lf ",coefs[i]);
        max_coefs[i-1] = coefs[i];  //copia para o vetor os maiores coeficientes
    }
/**/
    //!printf("\n");
    free(coefs);
    return max_coefs;
}


double *ReadFileDouble(char *filename, int size) {
    FILE *fp;
    double *fv=NULL;

    fv = AllocDoubleArray(size);

    if ((fp = fopen(filename, "rb")) == NULL) {
        printf("ERRO LENDO ARQUIVO DE FV\n");
        exit(0);
    }
    fread(fv, sizeof(double), size, fp);
    fclose(fp);

    return fv;
}

void WriteFileDouble(char *filename, double *vet, int size) {
    FILE *fout;
    int i = 0;

    //if ((fout = fopen(filename, "wb")) == NULL) {
    if ((fout = fopen(filename, "w")) == NULL) {
        printf("ERRO CRIANDO ARQUIVO DE FV\n");
        exit(0);
    }
   
    fprintf(fout,"%d\n",size);
    while (i < size){
	fprintf(fout,"%f ",vet[i]);
	i++;
    }
    //fwrite(vet, sizeof(double), size, fout);
    fclose(fout);
}

double L2DoubleDistance(double *v1, double *v2, int size) {
    int i;
    double d=0.0;

    for (i=0; i<size; i++) {
        d += pow((v1[i]-v2[i]), 2);
    }
    return sqrt(d);
}
