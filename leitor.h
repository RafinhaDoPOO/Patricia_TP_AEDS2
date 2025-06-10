#ifndef LEITOR_H
#define LEITOR_H

typedef struct {
    char *nome;
    char *conteudo;
} Arquivo;

typedef struct {
    int quantidade;
    Arquivo *arquivos;
} ListaArquivos;

ListaArquivos *ler_arq_entrada(const char *nome_entrada);
void ler_conteudo_arquivos(ListaArquivos *lista);
void printar_conteudo_arquivos(const ListaArquivos *lista);
void liberar_lista(ListaArquivos *lista);

#endif