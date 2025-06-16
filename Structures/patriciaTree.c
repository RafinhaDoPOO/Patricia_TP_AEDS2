#include <stdio.h>
#include <stdlib.h>

#define D 8 // aparentemente para ler a palavra de 8 bits
/*
Implementação da estrutura do ziviani por partes para concretizar as ideias
ainda nao e a versao do trabalho, vou seguir adaptando conforme o vasco vence
*/


typedef char* Key;//palavra
typedef unsigned char Index;
typedef unsigned char Dib;//death in bits?????
//define o tipo do no da arvore
typedef enum {
    internal,
    external
}nodeType;

typedef struct patNode* patTree;
typedef struct patNode{
    nodeType nt;
    union {
        struct{
            Index index;
            patTree left, right;
        }internalNode;
        Key key;
    }Node;
}patNode;


//retorna o iesimo  da chave k a partir da esquerda
Dib bit(Index i, Key k){
    int c;
    if(i == 0){
        return 0;
    }else{
        c = k;
        for(int j = 1; j <= D - i;j++){ // percorrer os 8 bits 
            c /=2;
        }
        return (c & 1);//Aplica uma máscara AND com 1 para isolar o último bit
    }
}

short isExternal (patTree p){
    return (p->nt == external);
}

patTree createInternalNode( int i, patTree *left, patTree *right){
    patTree p;
    p = (patTree) malloc(sizeof(patNode));
    p->nt = internal;
    p->Node.internalNode.left = *left;// armazena subarvore a esq
    p->Node.internalNode.right = *right;//mema coisa
    p->Node.internalNode.index = i; //armazena o indice de diferenca
    return p;
}
patTree createExternalNode(Key k){
    patTree p;
    p = (patTree) malloc(sizeof(patNode));
    p->nt = external;
    p->Node.key = k;
    return p;
}

void search(Key k, patTree t){
    if (isExternal(t)){
        if(k == t->Node.key){
            print("Found\n");
        }
        else{
        printf("Not found\n");
        }
        return;
    }
    if (bit(t->Node.internalNode.index, k) == 0){
        //extrai o bit da posicao indicada pelo indice do no interno
        //0 esquerda e 1 direita
        search(k, t->Node.internalNode.left);
    }else{
        search(k, t->Node.internalNode.right);
    }
}

/*
insere uma nova chave k na árvore, criando nós internos e externos conforme necessário,
 usando o bit na posição i como ponto de divisão.
*/
patTree insertBetween( Key k, patTree *t, int i){
    patTree p;
    if(isExternal(*t) || i < (*t)->Node.internalNode.index){
        //cria um novo no externo 
        p = createExternalNode(k);
        if (bit(i,k) == 1){
            return (createInternalNode(i,t, &p));
        }else{
            return (createInternalNode(i, &p, t));
        }
    }else{
        if(bit((*t)->Node.internalNode.index, k) == 1){
            (*t)->Node.internalNode.right = insertBetween(k,&(*t)->Node.internalNode.right, i);
        }else{
            (*t)->Node.internalNode.left = insertBetween(k,&(*t)->Node.internalNode.left, i);
        }
        return (*t);
    }
}

patTree insert(Key k, patTree *t){
    patTree p;
    int i;
    if (*t == NULL){
        return (createExternalNode(k));
    }else{
        p = *t;
        while(!isExternal(p)){
            if(bit(p->Node.internalNode.index, k) == 1){
                p = p->Node.internalNode.right;
            }else{
                p = p->Node.internalNode.left;
            }
        }
    }
    //apos achar o primeiro bit que difere
    i = 1;
    while((i <= D) & (bit((int)i,k) == bit((int)i, p->Node.key)))i++;
    if (i > D){
        printf("Error, already in tree\n");
        return (*t);
    }else{
        return (insertBetween(k,t,i));
    }
}

