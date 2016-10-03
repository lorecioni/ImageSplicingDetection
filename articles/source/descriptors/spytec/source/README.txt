[ESTRUTURA DE DIRETÓRIOS]
A biblioteca possui a seguinte estrutura de diretórios:

./app                --> irá conter as funções principais do descritor
./bin                --> executáveis do descritor
./include            --> cabeçalhos das funções (.h)
./lib                --> arquivo lib gerado após a compilação
./obj                --> objetos gerados após a compilação
./src                --> arquivos com funções de manipulação de imagens, funções internas do descritor, etc

----------------------------------------------
Para implementar o seu descritor, use como exemplo o descritor GCH (Global Color Histogram) que vem junto com a biblioteca.
----------------------------------------------
[IMPLEMENTAÇÃO]
- Criar um programa responsável pela extração de características (ex: gch_extraction.c) [dentro de 'app']
    - Entrada: imagem a ser processada (caminho no sistema de arquivos)
    - Saída: arquivo com as características extraídas (caminho no sistema de arquivos)

- Criar um programa responsável pelo cálculo de distância (ex: gch_distance.c) [dentro de 'app']
    - Entradas: arquivo de características da imagem 1 e arquivo de características da imagem 2 (caminhos no sistema de arquivos)
    - Saída: distância entre as duas imagens (pode ser impressa na tela / use tipo 'double' para a distância)

- Se forem criadas funções auxiliares, você deve colocá-las no diretório 'src' dentro de um arquivo com o nome do descritor (ex: gch.c). Neste caso você pode precisar de um arquivo de cabeçalhos (ex: gch.h). Coloque-o no diretório 'include' e adicione uma linha no arquivo 'libcolordescriptors.h' correspondendo ao seu arquivo de cabeçalhos (ex: #include "gch.h").

- Alterar o arquivo Makefile da raiz da biblioteca da seguinte maneira:
    - Abaixo da linha 26, cujo conteúdo é "$(OBJ)/gch.o", adicione uma linha correspondente ao seu descritor. [não esquecer das '\' como separadores de linhas]
    - Abaixo da linha 44, cujo conteúdo é "$(OBJ)/gch.o", adicione uma linha correspondente ao seu descritor. [não esquecer das '\' como separadores de linhas]
    - Copie as linhas 106-108, cujo conteúdo é
        $(OBJ)/gch.o: $(SRC)/gch.c
            gcc $(FLAGS) -c $(SRC)/gch.c -I$(INCLUDE) \
            -o $(OBJ)/gch.o
    e cole-as logo abaixo trocando os valores "gch" para o nome do seu descritor (são 4 locais para trocar).

- Compilar o programa:
    - Entre no diretório 'app'
    - Digite 'make prog', sendo 'prog' o nome do arquivo '.c' que você deseja compilar (sem a extensão '.c')
    - O executável é gerado no diretório 'bin'


IMPORTANTE:
- Lembre-se sempre de liberar toda a memória usada pelo seu descritor.
- Tente deixar sua implementação bastante rápida, pois o tempo de execução do descritor será medido.


[Obs.: Uma outra alternativa para você implementar seu descritor, é usar apenas 1 arquivo '.c'. Neste caso, você deve colocar no '.c' todas as funções e estruturas que o descritor usa. Desta forma, você não vai precisar do Makefile da raiz nem da estrutura de diretórios da biblioteca. Basta colocar tudo no arquivo '.c' e compilar diretamente com o gcc. As instruções de compilação do link abaixo também explicam como compilar o arquivo '.c' na hora de gerar o plugin]
[FIM IMPLEMENTACAO]
----------------------------------------------

Antes de gerar o plugin teste bem o seu descritor.
Lembre-se que você só deve cadastrar o seu descritor na ferramenta quando tudo estiver bem testado. O cadastro do descritor na ferramenta apenas finaliza o seu processo de implementação, portanto, ele não é nenhum processo que visa ajudar você a melhorar a sua implementação. Quando cadastrado, supõe-se que nada mais precise ser alterado na implementação.

----------------------------------------------
[GERAÇÃO DO PLUGIN]
- Dentro de 'app', criar um arquivo que irá conter ambas as funções de extração e distância do seu descritor (ex: gch_plugin.c). O cabeçalho e o funcionamento das funções deve respeitar as instruções encontradas em:
    http://www.lis.ic.unicamp.br/~otavio/tests/ferramenta/cadastra_descritor.php

- Não use 'make' para compilar. Siga as instruções de compilação do link acima.

- Assim que você der o upload do plugin, a ferramenta irá verificar se a sua implementação possui as funções necessárias e se elas estão de acordo com as instruções especificadas. A ferramenta também irá testar a função de extração de características e a função de distância usando duas imagens de exemplo. Se o teste for bem sucedido, você deverá preencher alguns campos com informações sobre o descritor na página seguinte. A sigla do descritor será atribuída automaticamente a partir do nome do plugin enviado, portanto, use um nome sem acentos ou espaços. Os outros campos são: Nome do descritor, Autor (coloque os nomes dos autores do artigo no qual você se baseou) e Tipo.

== OBS: Se a ferramenta apresentar a mensagem de erro "Possível erro de compilação" é provável que você tenha compilado em 32 bits. A ferramenta só irá aceitar compilação em 64 bits.
[FIM PLUGIN]

----------------------------------------------
[IMAGENS PPM]
Na hora de testar seu descritor você precisará de imagens PPM, que são arquivos de imagens sem compactação. Caso você não tenha imagens deste tipo, use o programa 'convert' do Linux para conveter seus arquivos JPEG em PPM. Exemplo:
convert imagem.jpg imagem.ppm

----------------------------------------------
[ESTRUTURAS E FUNÇÕES DA BIBLIOTECA]

Abaixo estão detalhes de algumas estruturas de dados fornecidas pela ferramenta. Use-as na sua implementação:

typedef struct _image {
  int *val;
  int ncols,nrows;
  int *tbrow;
} Image;

Esta estrutura representa uma imagem em níveis de cinza, ou seja, cada pixel tem apenas um valor referente ao brilho.


typedef struct cimage {
  Image *C[3];
} CImage;

Esta estrutura representa uma imagem colorida (RGB). Como se pode ver, ela é composta de 3 imagens em níveis de cinza, cada uma correspondendo a um dos canais R, G e B.

Observando estas estruturas, é possível perceber que, apesar de uma imagem ser bidimensional (largura 'ncols' e altura 'nrows') não existe uma matriz na qual você pode acessar as posições x e y independentemente. Nestas estruturas as imagens são consideradas como matrizes linearizadas. Ou seja, é como se cada linha da imagem fosse concatenada ao final de outra linha, resultando num vetor. Se uma imagem tem, por exemplo, ncols=20 e nrows=50, a representação dela nesta estrutura será um vetor de nrows*ncols posições. A primeira linha da imagem (linha superior) ficará nas posições 0 a 19, a segunda linha ficará nas posições 20 a 39, e assim por diante.
No entanto, a estrutura possui um facilitador na hora de acessar os valores dos pixels da imagem. O vetor tbrow é usado para acessar uma linha da imagem de maneira eficiente. Ele possui nrows posições e cada posição i dele tem um valor referente a quantidade de pixels que existem na estrutura antes do início da linha i. Por exemplo, se a imagem tem ncols=20 e nrows=50, o vetor tbrow na posição 3 terá o seguinte valor: tbrow[3]=20+20+20=60. Quando você usa as funções de criação e leitura de imagens da biblioteca CreateImage, CreateCImage, ReadImage ou ReadCImage, os valores de tbrow já vêem preenchidos, portanto você não precisa mexer neles.
Caso você precise acessar uma posição específica (x,y) da imagem, basta fazer como o exemplo abaixo:

    valor_red = img->C[0]->val[y+img->C[0]->tbrow[x]];

onde img é a imagem, C[0] indica os valores do canal R (red), y é a coluna e x é a linha

Se você precisar de um laço que percorre a imagem toda você pode usar uma das duas opções abaixo:
1)
    for (i = 0; i < img->C[0]->nrows; i++) {
        for (j = 0; j < img->C[0]->ncols; j++) {
            r = img->C[0]->val[j+img->C[0]->tbrow[i]];
            g = img->C[1]->val[j+img->C[1]->tbrow[i]];
            b = img->C[2]->val[j+img->C[2]->tbrow[i]];
        }
    }

2)
    for (i = 0; i < n; i++) {   //n = nrows*ncols
        r = img->C[0]->val[i];
        g = img->C[1]->val[i];
        b = img->C[2]->val[i];
    }



Funções úteis:

CImage *CreateCImage(int ncols, int nrows)          //cria uma imagem colorida
CImage *ReadCImage(char *filename)                  //le uma imagem colorida
void    WriteCImage(CImage *cimg, char *filename)   //grava uma imagem colorida no disco
CImage *CImageRGBtoHSV(CImage *cimg)                //converte imagem RGB para HSV
void    DestroyCImage(CImage **cimg)                //libera a memória de uma imagem colorida








