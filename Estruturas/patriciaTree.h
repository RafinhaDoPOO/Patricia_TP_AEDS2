// patricia.h
#ifndef PATRICIATREE_H
#define PATRICIATREE_H

#define D 8 // aparentemente para ler a palavra de 8 bits

// Tipos definidos
typedef char* Key; // palavra
typedef unsigned char Index;
typedef unsigned char Dib; // death in bits?????

typedef enum {
    internal,
    external
} nodeType;

// struct for <qtde,idDoc> pair
typedef struct Occurence {
    int qtde;
    int idDoc;
} occurence;

// lista encadeada de ocorrencias
typedef struct occurenceList {
    occurence data;
    struct occurenceList* next;
} occurenceList;

typedef struct patNode* patTree;

typedef struct patNode {
    nodeType nt;
    union {
        struct {
            Index index;
            patTree left, right;
        } internalNode;
        struct {
            occurenceList* occurences; // lista  de  <qtde, idDoc> para esta palavra
            Key key;
        } externalNodeData;
    } Node;
} patNode;

// Funcoes principais
occurenceList* createOcurrenceNode(int qtde, int idDoc);
void addOrUpdateOccurence(occurenceList** head, int qtde, int idDoc);
Dib bit(Index i, Key k);
short isExternal(patTree p);
patTree createInternalNode(int i, patTree left, patTree right);
patTree createExternalNode(Key k);
patTree search(Key k, patTree t);
patTree insertBetween(Key k, patTree* t, Index i, int idDocNew, int qtdeNew);
patTree insert(Key k, patTree* t, int idDoc);

#endif
