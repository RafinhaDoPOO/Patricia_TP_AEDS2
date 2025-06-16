#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

//estruturas para o id invertido
//struct for <qtde,idDoc> pair

typedef struct Occurence{
    int qtde;
    int idDoc;
}occurence;

//lista encadeada de ocorrencias
typedef struct occurenceList {
    occurence data;
    struct occurenceList* next;
}occurenceList;


typedef struct patNode* patTree;
typedef struct patNode{
    nodeType nt;
    union {
        struct{
            Index index;
            patTree left, right;
        }internalNode;
        struct{
            occurenceList* occurences;// lista  de  <qtde, idDoc> para esta palavra
            Key key;
        }externalNodeData;
    }Node;
}patNode;

occurenceList* createOcurrenceNode(int qtde, int idDoc){
    occurenceList* newNode = (occurenceList*)malloc(sizeof(occurenceList));
    if(newNode == NULL){
        perror("failed to allocate memory to occurencelist node");
        exit(EXIT_FAILURE);
    }
    newNode->data.qtde = qtde;
    newNode->data.idDoc = idDoc;
    newNode->next = NULL;
    return newNode;
}
void addOrUpdateOccurence( occurenceList** head,int qtde,int idDoc){

    occurenceList* current = *head;
    occurenceList* prev = NULL;

    while(current != NULL && current->data.idDoc < idDoc){
        prev = current;
        current = current->next;
    }
    if (current != NULL && current->data.idDoc  == idDoc){
        current->data.qtde += qtde; // atualiza quantidade se o iddoc ja existir
    }else{
        occurenceList* newNode = createOcurrenceNode(qtde,idDoc);
        if( prev == NULL){
            newNode->next = *head;//insere no comeco
            *head = newNode;
        }else{
            newNode->next = current;//insere no meio ou final
            prev->next = newNode;
        }
    }
}
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

patTree createInternalNode( int i, patTree *left, patTree *right){
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
patTree createExternalNode(Key k, patTree left, patTree right){
    patTree p;
    p = (patTree)malloc(sizeof(patNode));
    if (p == NULL) {
        perror("Failed to allocate memory for external node");
        exit(EXIT_FAILURE);
    }
    p->nt = external;
    p->Node.externalNodeData.key = strdup(k); //duplica a string pra arranjar memoria sla
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
            print("Found\n");
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
        search(k, t->Node.internalNode.left);
    }else{
        search(k, t->Node.internalNode.right);
    }
    return NULL;
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

