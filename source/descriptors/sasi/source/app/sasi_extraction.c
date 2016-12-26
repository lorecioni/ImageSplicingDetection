#include "libtexturedescriptors.h"

void Extraction(char *img_path, char *fv_path, char * channel)
{

    CImage *cimg=NULL;
    Image *img=NULL;
    //ImageHSV **imgHSV; //imagem no espaco HSV - mesmo tamanho da imagem original
    FeatureVector1D *sasi=NULL;
    int i,j,c;
    int sizes[3];
    sizes[0]=3;
    sizes[1]=5;
    sizes[2]=7;
    c = atoi(channel);
    FILE *fout;

    /*cimg = ReadCImage(img_path);
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
    }*/

    // adicionei essa parte para ja entrar com a imagem no espaço desejado
    cimg = ReadCImage(img_path);
    img = CreateImage(cimg->C[0]->ncols, cimg->C[0]->nrows);

    //copia o canal desejado para a imagem cinza
    for (i = 0; i < cimg->C[0]->nrows; i++) {
        for (j = 0; j < cimg->C[0]->ncols; j++) {
            img->val[j+img->tbrow[i]] = cimg->C[c]->val[j+cimg->C[c]->tbrow[i]];
        }
    }
   //WriteImage(img,"teste.ppm");
   //////////////////////////////////////////////////////////////////////////

    sasi = SASI(img, 3, sizes);

    //WriteFileFeatureVector1D(sasi,fv_path);
    if ((fout = fopen(fv_path, "wb")) == NULL) {
        printf("ERRO CRIANDO ARQUIVO DE FV\n");
        exit(0);
    }
    /////IMPRIMINDO O FV PRA VERIFICAR OS VALORES
    //for (i=0; i<sasi->n; i++) {
    //   printf("%lf ", sasi->X[i]);
    //}
    //printf("\n");
	
    //fwrite(&sasi->n,sizeof(int),1,fout);
    fprintf(fout,"%d\n",sasi->n);
    //fprintf(fout,"%d\n",sasi->n);
    //fwrite(sasi->X,sizeof(double),sasi->n,fout);
    for(i=0;i<sasi->n;i++) {/*escreve elementos do vetor de caracteristicas*/
	fprintf(fout, "%f ", (float)sasi->X[i]);
    }
    fclose(fout);

    //liberando memoria imgHSV, foi comentado para servir para qq imagem. para voltar ao codigo original tem q descomentar
    /*for (i=0; i<cimg->C[0]->nrows; i++) {
        free(imgHSV[i]);
    }
    free(imgHSV);*/

    DestroyFeatureVector1D(&sasi);
    DestroyCImage(&cimg);
    DestroyImage(&img);

}


int main(int argc, char** argv)
{

  if (argc != 4) {
    fprintf(stderr,"usage: gch_extraction <image_path> <fv_path>\n");
    exit(-1);
  }

  Extraction(argv[1], argv[2], argv[3]);

  return(0);
}
