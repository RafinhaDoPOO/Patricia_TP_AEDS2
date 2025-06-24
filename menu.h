// menu.h
#ifndef MENU_H
#define MENU_H

#include "patricia.h"
#include "hash.h"
#include "leitor.h"
#include "processador.h"

#include <ctype.h>
#include <math.h>



void calcular_relevancia_patricia (ArvorePat no, ListaArquivos* lista) ;
void calcular_relevancia_hash (HashTable* ht, ListaArquivos* lista) ;

void inicializar_sistema(const char *arquivo_entrada); 
// Exibe o menu interativo com base nas estruturas já construídas
void exibir_menu(ListaArquivos* lista, ArvorePat indice_patricia, HashTable* ht);

#endif
