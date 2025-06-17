#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "patricia.h"
#define D 8 // aparentemente para ler a palavra de 8 bits
/*
Implementação da estrutura do ziviani por partes para concretizar as ideias
ainda nao e a versao do trabalho, vou seguir adaptando conforme o vasco vence
*/

//retorna o iesimo  da chave k a partir da esquerda
Dib bit(Index i, Key k){
    int c;
    if(i == 0){
        return 0;
    }
     // Determine which character the bit 'i' falls into
    // char_index is 0-based
    int char_index = (i - 1) / D; // D is bits per char (e.g., 8)

    // Determine which bit within that character (0-based from MSB)
    int bit_in_char_index = (i - 1) % D;

    // Check if we've gone past the end of the string
    // If the char_index is beyond the string length, consider the bit to be 0
    if (char_index >= strlen(k)) {
        return 0; // Effectively padding with zeros for shorter keys
    }

    unsigned char c = k[char_index];

    // Shift to get the desired bit at the least significant position
    // (D - 1 - bit_in_char_index) gives the number of shifts needed from the MSB side
    return (c >> (D - 1 - bit_in_char_index)) & 1;
}

short isExternal (patTree p){
    return (p->nt == external);
}

patTree createInternalNode( int i, patTree left, patTree right){
     patTree p;
    p = (patTree)malloc(sizeof(patNode));
    if (p == NULL) {
        perror("Failed to allocate memory for internal node");
        exit(EXIT_FAILURE);
    }
    p->nt = internal;
    p->Node.internalNode.left = left; // arrmazena subarvore a esq
    p->Node.internalNode.right = right; // mema coisa com a direita
    p->Node.internalNode.index = i; //armazena o index de diferenca
    return p;
}
patTree createExternalNode(Key k) {
    patTree p;
    p = (patTree)malloc(sizeof(patNode));
    if (p == NULL) {
        perror("Failed to allocate memory for external node");
        exit(EXIT_FAILURE);
    }
    p->nt = external;
    p->Node.externalNodeData.key = strdup(k); // Duplicate the string to manage memory
    if (p->Node.externalNodeData.key == NULL) {
        perror("Failed to duplicate key string");
        exit(EXIT_FAILURE);
    }
    p->Node.externalNodeData.occurences = NULL; // Initialize to empty list
    return p;
}
//aguenta agora comparacao entre strings e pode returnar o no externo
patTree search(Key k, patTree t){

    if(t == NULL){
        return NULL;//nao encontrado
    }
    
    if (isExternal(t)){
        if(strcmp(k, t->Node.externalNodeData.key) == 0){
            printf("Found\n");
            //printando as ocorrencias pra verificar
            occurenceList* current = t->Node.externalNodeData.occurences;
            printf("Occurrences");
            while (current != NULL){
                printf("<%d, %d>", current->data.qtde,current->data.idDoc);
                current = current->next;
            }
            printf("\n");
            return t;//retorna o no externo encontrado
        }
        else{
        printf("Not found\n");
        return NULL;
        }
    }
    if (bit(t->Node.internalNode.index, k) == 0){
        //extrai o bit da posicao indicada pelo indice do no interno
        //0 esquerda e 1 direita
        return search(k, t->Node.internalNode.left);
    }else{
        return search(k, t->Node.internalNode.right);
    }
    return NULL;
}

/*
insere uma nova chave k na árvore, criando nós internos e externos conforme necessário,
 usando o bit na posição i como ponto de divisão.
*/
patTree insertBetween( Key k, patTree *t, Index i, int idDocNew, int qtdeNew){
    patTree p;
    p = createExternalNode(k);//cria um novo no externo 
    addOrUpdateOccurence(&(p->Node.externalNodeData.occurences), qtdeNew, idDocNew);
    if(isExternal(*t) || i < (*t)->Node.internalNode.index){
        
        if (bit(i,k) == 1){
            return (createInternalNode(i,*t, p));
        }else{
            return (createInternalNode(i, p, *t));
        }
    }else{
        if(bit((*t)->Node.internalNode.index, k) == 1){
            (*t)->Node.internalNode.right = insertBetween(k,&(*t)->Node.internalNode.right, i, idDocNew, qtdeNew);
        }else{
            (*t)->Node.internalNode.left = insertBetween(k,&(*t)->Node.internalNode.left, i, idDocNew, qtdeNew);
        }
        return (*t);
    }
}

patTree insert(Key k, patTree *t, int idDoc){
    patTree p;
    Index i;
    if (*t == NULL){
        p = createExternalNode(k);
        addOrUpdateOccurence(&(p->Node.externalNodeData.occurences), 1, idDoc);
        return p;
    }else{
        p = *t;
        //percorrer a arvore ate achar o potencial no externo
        while(!isExternal(p)){
            if(bit(p->Node.internalNode.index, k) == 1){
                p = p->Node.internalNode.right;
            }else{
                p = p->Node.internalNode.left;
            }
        }
    }
    //apos achar o primeiro bit que difere
    //comparar 'k' com a chave em 'p'
    if (strcmp(k, p->Node.externalNodeData.key) == 0){
        //a chave ja existe na arvore, so atualiza sua lista de ocorrencias
        //adiciona ou atualiza a ocorrencia do current idDoc em 1
        addOrUpdateOccurence(&(p->Node.externalNodeData.occurences), 1, idDoc);
        return *t;
    }

    //se as chaves forem diferentes, encontra o primeiro bit que difere
    i = 1;
    //iterar sobre o numero maximo de bits das duas palavras para nao ficar num loop
    //infinito de comparacoes de prefixos iguais, colocar aqui o limite sendo
    // D * (max_len_k > max_len_p_key ? max_len_k : max_len_p_key)
    int max_len_k = strlen(k);
    int max_len_p_key = strlen(p->Node.externalNodeData.key);
    int max_possible_bits = D * (max_len_k > max_len_p_key ? max_len_k : max_len_p_key);

     while ((i <= max_possible_bits) && (bit(i, k) == bit(i, p->Node.externalNodeData.key))) {
        i++;
    }


    // If i exceeds max_possible_bits, it means one string is a prefix of another or they are identical (already handled by strcmp)
    // The `strcmp` check above handles identical strings. If `i > max_possible_bits`, it indicates an issue or a prefix case
    // where one string ends before a differing bit is found. The current `bit` function returns 0 for bits beyond string length.
    // This implicitly handles prefixes by treating the shorter string as padded with zeros.
    // Therefore, if `i` goes beyond the length of both keys (padded with zeros), it implies they are identical after padding,
    // which should ideally have been caught by the `strcmp` earlier.
    // If we reach here, `i` is the index of the first differing bit.
    return (insertBetween(k, t, i, idDoc, 1)); // Pass 1 for initial occurrence in new external node
}
