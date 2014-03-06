

#include "symbolTable.h"
#include <math.h>

// Top should point to the top of the scope stack,
// which is the most recent scope pushed

SymbolTableStackEntryPtr symbolStackTop;

int scopeDepth;


/* global function prototypes */

//allocate the global scope entry and symbol table --and set scopeDepth to 0
// The global scope remains till the end of the program
// return 1 on success, 0 on failure (such as a failed malloc)
int		initSymbolTable() {

    //allocation for global scope and symbol table.
    symbolStackTop = (SymbolTableStackEntryPtr) malloc(sizeof(SymbolTableStackEntry));
    if(symbolStackTop == NULL)
        return 0;
    symbolStackTop->symbolTablePtr = (SymbolTablePtr)malloc(sizeof(SymbolTable));
    if(symbolStackTop->symbolTablePtr == NULL)
        return 0;
    // previousScope ptr should be NULL to indicate global scope
    symbolStackTop->prevScope = NULL;
    scopeDepth = 0;

    return 1;

}


int hash(char *id) {

    int hashNum = 0, i;
    for (i=0; i < strlen(id); i++) {

        hashNum += pow(31, strlen(id)-(i+1)) * id[i];

    }

    return hashNum;

}

// Look up a given entry
ElementPtr		symLookup(char *name) {

    int key = abs(hash(name))%MAXHASHSIZE;

    SymbolTableStackEntryPtr previousScope;
    HashTableEntry temp;

    previousScope = symbolStackTop;
    // loop through all the scopes
    while(previousScope != NULL) {

        temp = previousScope->symbolTablePtr->hashTable[key];
        // loop through all the elements with same hash
        while(temp != NULL) {

            if(strcmp(temp->id, name) == 0)
                return temp;
            else
                temp = temp->next;

        }

        previousScope = previousScope->prevScope;

    }

    return NULL;

}


// Insert an element with a specified type in a particular line number
// initialize the scope depth of the entry from the global var scopeDepth
ElementPtr		symInsert(char *name, struct type *type, int line) {

    HashTableEntry newElement;
    newElement = (HashTableEntry)malloc(sizeof(Element));
    if(newElement == NULL)
        return NULL;

    newElement->key = abs(hash(name))%MAXHASHSIZE;
    newElement->id = name;
    newElement->linenumber = line;
    newElement->scope = scopeDepth;
    // newElement->stype = (TypePtr) malloc(sizeof(Type));
    newElement->stype = type;
    // if(newElement->stype->dimension > 0)
    //     printf("Array %s dimension: %d", newElement->id, newElement->stype->dimension);

    HashTableEntry temp;
    temp = symbolStackTop->symbolTablePtr->hashTable[newElement->key];
    if(temp == NULL)
        symbolStackTop->symbolTablePtr->hashTable[newElement->key] = newElement;
    else {

        while(temp->next != NULL)
            temp = temp->next;

        temp->next = newElement;

    }

    return newElement;

}


//push a new entry to the symbol stack
// This should modify the variable top and change the scope depth
// return 1 on success, 0 on failure (such as a failed malloc)
int			enterScope() {

    SymbolTableStackEntryPtr newScope;
    newScope = (SymbolTableStackEntryPtr)malloc(sizeof(SymbolTableStackEntry));
    if(newScope == NULL)
        return 0;
    newScope->symbolTablePtr = (SymbolTablePtr)malloc(sizeof(SymbolTable));
    if(newScope->symbolTablePtr == NULL)
        return 0;

    newScope->prevScope = symbolStackTop;
    symbolStackTop = newScope;

    scopeDepth++;

    return 1;

}

void freeElement(HashTableEntry elem) {

    if(elem->next == NULL) {

        free(elem->stype);
        free(elem);

        return;

    }

    freeElement(elem->next);
    free(elem);

}

void freeElements(HashTableEntry * hashtable) {

    int hashIter;
    for(hashIter=0; hashIter<MAXHASHSIZE; hashIter++) {

        if(hashtable[hashIter] != NULL)
            freeElement(hashtable[hashIter]);

    }

}

//pop an entry off the symbol stack
// This should modify top and change scope depth
void			leaveScope() {

    if(scopeDepth == 0) {

        printf("Depth already at global.\n");
        return;

    }

    SymbolTableStackEntryPtr temp;
    temp = symbolStackTop;

    symbolStackTop = symbolStackTop->prevScope;

    freeElements(temp->symbolTablePtr->hashTable);
    free(temp->symbolTablePtr);
    free(temp);
    scopeDepth--;

}


// Do not modify this function
void printElement(ElementPtr symelement) {
    if (symelement != NULL) {
        printf("Line %d: %s\n", symelement->linenumber,symelement->id);
    }
    else printf("Wrong call! symbol table entry NULL\n");
}

void printElements(HashTableEntry entry) {

    if(entry == NULL)
        return;

    printElements(entry->next);
    printElement(entry);

}

//should traverse through the entire symbol table and print it
// must use the printElement function given above
void printSymbolTable(SymbolTablePtr symtable) {

    int i;
    for(i=0; i < MAXHASHSIZE; i++) {

        if(symtable->hashTable[i] != NULL)
            printElements(symtable->hashTable[i]);

    }

}
