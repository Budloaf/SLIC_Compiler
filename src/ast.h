/*
 *-------------------------------------------------------------------------
 * Project   - SLIC Compiler
 * Course    - CS4223
 * Developer - Hayden Berry
 * 
 * Description - This is the parser for the compiler.
 * Due - 02.04.2025
 *-------------------------------------------------------------------------
 */

#define MAX_PROG_LENGTH 65536
#define MAX_STRING_LENGTH 2048
#define MAX_READ_LENGTH 16
#define MAX_LINE_NUMBER_LENGTH 5
// kinds of expressions
#define EK_VOID  0
#define EK_INT   1
#define EK_FLO   2
#define EK_OP    3

// operators
//  arithmetic
#define OP_NGI   1
#define OP_NGF   2
#define OP_ADI   3
#define OP_ADF   4
#define OP_SBI   5
#define OP_SBF   6
#define OP_MLI   7
#define OP_MLF   8
#define OP_DVI   9
#define OP_DVF   10
#define OP_RMI   11

//  relational
#define OP_LTI   12
#define OP_LTF   13
#define OP_LEI   14
#define OP_LEF   15
#define OP_GTI   16
#define OP_GTF   17
#define OP_GEI   18
#define OP_GEF   19
#define OP_EQI   20
#define OP_EQF   21
#define OP_NEI   22
#define OP_NEF   23

//  boolean
#define OP_ANI   24
#define OP_ANF   25
#define OP_ORI   26
#define OP_ORF   27
#define OP_NTI   28
#define OP_NTF   29

#define SK_P     1     // print statement
   #define PK_NL    2  // print statement (new line)
   #define PK_E     3  // print statement (expression)
   #define PK_CS    4  // print statement (char string)
#define SK_A     5     // assignment statement 
#define SK_R     6     // read statement
#define SK_IF    7     // if statement
#define SK_IFE   8     // if-else statement
#define SK_W     9     // while loop
#define SK_EX    10    // exit statement
#define SK_CU    11    // counting upward
#define SK_CD    12    // counting downward

struct printitem {
   char pk;
   char * char_string;
   struct expression * exp;
};
struct plist { // printlist
   struct printitem * item;
   struct plist * link; 
};

struct statement {
   char sk;
   struct listnode * body;
   struct listnode * elsebody;
   struct plist * args; // print statement arguments
   struct expression * arg_exp; // argument of statement (if/while conditional, expression to assign)
   int dest_base_address; // base address of variable to be assigned
   struct expression * index_exp; // for array reference on left side of :=
   int iterator_address; // for counting loop
   struct expression * start_exp;
   struct expression * finish_exp;
};

struct listnode {
   struct listnode * link;
   struct statement * stmt;
};

struct expression {
   char kind; // EK_OP, EK_INT, EK_FLO
   float float_value;
   int int_value;
   char var; // 1 if variable, 0 if constant
   char operator; // if kind == EK_OP, what operation
   char return_type; // what type does the operation return
   struct expression *l_operand;
   struct expression *r_operand;
   int src_base_address; // for assignment statement. address of variable on the right side of assignment
   struct expression * index_exp; // for array reference within an expression
};

extern struct listnode *list;
