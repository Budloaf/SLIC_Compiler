/*
 *-------------------------------------------------------------------------
 * Project   - SLIC Compiler
 * Course    - CS4223
 * Developer - Hayden Berry
 * 
 * Description - This is the symbol table .h file for the compiler.
 * Due - 02.04.2025
 *-------------------------------------------------------------------------
 */
#include <stdlib.h>
#include <stdio.h>

#define DT_VOID     0
#define DT_INTEGER  1
#define DT_REAL     2

#define VT_SCALAR  0
#define VT_ARRAY   1

// symbol table structs
struct SymbolTableEntry {
    char  *  name;
    char dataType;
    char  varType;
    int      size;
    int   address;
    int int_value;
    float float_value;
};

struct SymbolTable {
    int capacity;
    int count;
    int nextAddress;
    struct SymbolTableEntry * entry;
};


// symbol table operations/functions
struct SymbolTable * initializeST();
int lookupST(struct SymbolTable * st, char * name);
int lookupSTbyAddress(struct SymbolTable * st, int address);
void displayST(struct SymbolTable * st);
void insertST(struct SymbolTable * st, char * name, char dataType, char varType, int size);
int addressofST(struct SymbolTable * st, char * name);

extern struct SymbolTable * st;
