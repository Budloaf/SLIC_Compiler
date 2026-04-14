/*
 *-------------------------------------------------------------------------
 * Project   - Checkpoint #7 / while, exit, modulo, arrays
 * Course    - CS4223
 * Developer - Hayden Smith
 * 
 * Description - This is the symbol table .c file for checkpoint #6.
 * Due - 02.04.2025
 *-------------------------------------------------------------------------
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symbolTable.h"
#include "ast.h"

// symbol table operations/functions
struct SymbolTable * initializeST() {
    struct SymbolTable * st = malloc(sizeof(struct SymbolTable));
    st->capacity = 2000;
    st->count = 0;
    st->nextAddress = 0;
    st->entry = malloc(st->capacity*sizeof(struct SymbolTableEntry));
    return st;
}

int lookupST(struct SymbolTable * st, char * name) {
    int i;
    for(i = 0; i < st->count; i++) {
        if (!strcmp(st->entry[i].name,name)) {
            return i; // return entry number where the entry is located
        }
    }
    return -1; // return a negative value for not found
}
int lookupSTbyAddress(struct SymbolTable * st, int address) {
    int i;
    for(i = 0; i < st->count; i++) {
        if (address == st->entry[i].address) {
            return i;
        }
    }
    return -1;
}
void displayST(struct SymbolTable * st) {
    if (st == NULL) {
        printf("An error occurred. The symbol table is not initialized properly\n");
        return;
    }
    printf("Entries: %d\n",st->count);
    int i;
    for (i = 0; i < st->count; i++) {
        printf("Entry %d\n----------------------------\n",i);
        printf("    Name          : %s\n",st->entry[i].name);
        printf("    Data Type     : ");
        if (st->entry[i].dataType == DT_INTEGER) {
            printf("Integer\n");
            printf("    Value         : %d",st->entry[i].int_value);
        }
        else if (st->entry[i].dataType == DT_REAL) {
            printf("Real\n");
            printf("    Value         : %f",st->entry[i].float_value);
        }
        else {
            printf("Data type not recognized. Entry %d's data type is being read as %d",i,st->entry[i].dataType);
        }
        printf("\n");

        printf("    Variable Type : ");
        if (st->entry[i].varType == VT_SCALAR) {
            printf("Scalar");
        }
        else if (st->entry[i].varType == VT_ARRAY) {
            printf("Array");
        }
        else { printf("Not recognized. Entry %d's variable type is %d",i,st->entry[i].varType); }
        printf("\n");

        printf("    Size          : %d\n",st->entry[i].size);
        printf("    Address       : %d\n",st->entry[i].address);
        printf("\n");
    }
    return;
}

void insertST(struct SymbolTable * st, char * name, char dataType, char varType, int size) {
    if (lookupST(st,name) >= 0) {
        printf("An error occurred. Failed to insert entry. Duplicate.\n");
        return;
    }
    else if (st->count == st->capacity) {
        printf("An error occurred. Failed to insert entry. Capacity reached.\n");
        return;
    }
    st->entry[st->count].varType  = varType;

    st->entry[st->count].name     = strdup(name);
    st->entry[st->count].dataType = dataType;
    st->entry[st->count].size     = size;
    st->entry[st->count].address  = st->nextAddress;
    st->nextAddress += size;
    st->count += 1;
    return;
}

int addressofST(struct SymbolTable * st, char * name) {
    // given the name of an entry, return its address
    return st->entry[lookupST(st,name)].address;
}
