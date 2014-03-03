

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

    int key = hash(name)%32;

    SymbolTableStackEntryPtr previousScope;
    HashTableEntry temp;

    previousScope = symbolStackTop;
    // loop through all the scopes
    while(previousScope != NULL) {

        temp = previousScope->symbolTablePtr->hashTable[key];

        if(temp != NULL) {

            if(strcmp(temp->id, name) == 0)
                return temp;
            // loop through all the elements with same hash
            while(strcmp(temp->id, name) != 0) {

                if(temp->next != NULL)
                    temp = temp->next;
                else {

                    temp = NULL;
                    break;

                }

            }

        }

        previousScope = previousScope->prevScope;

    }

    return temp;

}


// Insert an element with a specified type in a particular line number
// initialize the scope depth of the entry from the global var scopeDepth
ElementPtr		symInsert(char *name, struct type *type, int line) {

    HashTableEntry newElement;
    newElement = (HashTableEntry)malloc(sizeof(Element));
    if(newElement == NULL)
        return NULL;

    newElement->key = abs(hash(name)%32);
    newElement->id = name;
    newElement->linenumber = line;
    newElement->scope = scopeDepth;
    // newElement->stype = (TypePtr) malloc(sizeof(Type));
    newElement->stype = type;
    // if(newElement->stype->dimension > 0)
    //     printf("Array %s dimension: %d", newElement->id, newElement->stype->dimension);

    HashTableEntry *hashtable;
    hashtable = symbolStackTop->symbolTablePtr->hashTable;
    if(hashtable[newElement->key] == NULL)
        hashtable[newElement->key] = newElement;
    else {

        HashTableEntry temp;
        temp = hashtable[newElement->key];
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
    else printf("Wrong call! symbol table entry NULL");
}

//should traverse through the entire symbol table and print it
// must use the printElement function given above
void printSymbolTable(SymbolTablePtr symtable) {



}
