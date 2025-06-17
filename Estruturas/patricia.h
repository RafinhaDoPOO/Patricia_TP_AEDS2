// patricia.h
#ifndef PATRICIATREE_H
#define PATRICIATREE_H
#define D 8 // aparentemente para ler a palavra de 8 bits
#include "processador.h"


typedef Palavra* Dado;//palavra
typedef unsigned int Index;
typedef unsigned char Dib;//death in bits?????
//define o tipo do no da arvore
typedef enum {
    interno,
    externo
}NoType;
typedef struct NoPatricia* ArvorePat;
typedef struct NoPatricia{
    NoType nt;
    union {
        struct{
            Index index;
            ArvorePat esq, dir;
        }noInterno;
        struct{
            Dado palavra;
        }NoExterno;
    }No;
}NoPatricia;



// Funcoes principais
ArvorePat construir_indice_patricia(ListaArquivos* lista);
Dib bit(Index i, Dado k);

//Nova função
Palavra* buscar_na_patricia(ArvorePat arvore, const char* texto_palavra);
void liberar_indice_patricia(ArvorePat arvore);
ArvorePat construir_indice_patricia(ListaArquivos* lista);

short isExternal(ArvorePat p);

ArvorePat createnoInterno(int i, ArvorePat left, ArvorePat right);
ArvorePat criarnoExterno(Dado k);
ArvorePat search(Dado k, ArvorePat t);
ArvorePat insertBetween(Dado k, ArvorePat* t, Index i, int idDocNew, int qtdeNew);
ArvorePat insert(Dado k, ArvorePat* t, int idDoc);

#endif
