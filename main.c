/*
 *-------------------------------------------------------------------------
 * Project   - Checkpoint #7 / while, exit, modulo, arrays
 * Course    - CS4223
 * Developer - Hayden Smith
 * 
 * Description - This is the main file for checkpoint #6.
 * Due - 02.04.2025
 *-------------------------------------------------------------------------
 */

#include <stdio.h>
#include "symbolTable.h"
#include "ast.h"
#include <string.h>


int yyparse(void);
void exprgen(char *,struct expression *);
char * codegen(char *, struct listnode *);
void typecast(char *,struct expression *,char);
void get_operands(char *, struct expression *,char);
int get_line_number(char *);
void insert_jump(char * program_string,char);

int main()
{
   st = initializeST();
   if (yyparse()) {
      printf("Syntax error\n");
   }
   else {
      char * program_string = malloc(MAX_PROG_LENGTH*sizeof(char));
      struct listnode * p;
      p = list;
      printf("%s",codegen(program_string,p));
   }
   return 0;
}

// don't print GSTAL code to the screen
char * codegen(char * program_string, struct listnode * p) {
   char num_str[10];
   char main_scope = 0;
   if (p == list) {
      int i;
      int isp = 0; // ISP <isp>
      for (i = 0; i < st->count; i++) {
         isp += st->entry[i].size;
      }
      strcat(program_string,"ISP ");
      sprintf(num_str,"%d\n",isp);
      strcat(program_string,num_str);
      main_scope = 1;
   }
   
   while (p != NULL) { 
      int jump_address;
      if (&p->stmt->sk != NULL) {
         if (p->stmt->sk == SK_P) {
            while (p->stmt->args != NULL) {
               if (p->stmt->args->item->pk == PK_E) {
                  exprgen(program_string,p->stmt->args->item->exp);
                  if (p->stmt->args->item->exp->kind == EK_FLO || p->stmt->args->item->exp->return_type == EK_FLO) {
                     strcat(program_string,"PTF\n");
                  }
                  else {
                     strcat(program_string,"PTI\n");
                  }
               }
               else if (p->stmt->args->item->pk == PK_NL) { // print new line
                  strcat(program_string,"PTL\n");
               }
               else if (p->stmt->args->item->pk == PK_CS) { // print character string
                  int i;
                  int each_char;
                  for (i = 0; i < strlen(p->stmt->args->item->char_string); i++) {
                     strcat(program_string,"LLI ");
                     each_char = p->stmt->args->item->char_string[i];
                     sprintf(num_str,"%d\n",each_char);
                     strcat(program_string,num_str);
                     strcat(program_string,"PTC\n");
                  }
               }
               p->stmt->args = p->stmt->args->link;
            }
         }
         else if (p->stmt->sk == SK_A) { // assignment statement
            char dest_type = st->entry[lookupSTbyAddress(st,p->stmt->dest_base_address)].dataType;
            if (p->stmt->index_exp == NULL) { // not an array variable
               strcat(program_string,"LAA ");
               sprintf(num_str,"%d\n",p->stmt->dest_base_address);
               strcat(program_string,num_str);
               exprgen(program_string,p->stmt->arg_exp);
               
               typecast(program_string,p->stmt->arg_exp,dest_type);
               strcat(program_string,"STO\n");
            }
            else { // array variable
               strcat(program_string,"LLI ");
               sprintf(num_str,"%d\n",p->stmt->dest_base_address);
               strcat(program_string,num_str);

               exprgen(program_string,p->stmt->index_exp);

               strcat(program_string,"ADI\n");

               exprgen(program_string,p->stmt->arg_exp);
               typecast(program_string,p->stmt->arg_exp,dest_type);

               strcat(program_string,"STO\n");
            }
         }
         else if (p->stmt->sk == SK_EX) { // exit statement
            strcat(program_string,"HLT\n");
            break;
         }
         else if (p->stmt->sk == SK_R) { // read statement
            if (p->stmt->index_exp == NULL) { // not an array reference
               // get address of variable
               strcat(program_string,"LAA ");
               sprintf(num_str,"%d\n",p->stmt->dest_base_address);
               strcat(program_string,num_str);
            }
            else { // an array reference
               strcat(program_string,"LLI ");
               sprintf(num_str,"%d\n",p->stmt->dest_base_address);
               strcat(program_string,num_str);
               
               exprgen(program_string,p->stmt->index_exp);
               typecast(program_string,p->stmt->index_exp,EK_INT);
               strcat(program_string,"ADI\n");
            }
            char var_data_type = st->entry[lookupSTbyAddress(st,p->stmt->dest_base_address)].dataType;
            // get input from keyboard
            if (var_data_type == DT_REAL) {
               strcat(program_string,"INF\n");
            }
            else if (var_data_type == DT_INTEGER) {
               strcat(program_string,"INI\n");
            }
            // put into variable
            strcat(program_string,"STO\n");
         }
         else if (p->stmt->sk == SK_IF || p->stmt->sk == SK_IFE) {
            exprgen(program_string,p->stmt->arg_exp);
            strcat(program_string,"LLI 0\n");
            strcat(program_string,"NEI\n");

            strcat(program_string,"JPF -\n"); // hyphen for the if jump
            codegen(program_string,p->stmt->body);
            if (p->stmt->sk == SK_IFE) {
               strcat(program_string,"JMP _\n"); // underscore for the else jump
            }
            insert_jump(program_string,'-');
            if (p->stmt->sk == SK_IFE) {
               codegen(program_string,p->stmt->elsebody);
               insert_jump(program_string,'_');
            }
         }
         else if (p->stmt->sk == SK_W) { // while loop
            jump_address = get_line_number(program_string);
            exprgen(program_string,p->stmt->arg_exp);
            strcat(program_string,"LLI 0\n");
            strcat(program_string,"NEI\n");

            strcat(program_string,"JPF !\n"); // exclamation mark for the while jump
            codegen(program_string,p->stmt->body);

            strcat(program_string,"JMP ");
            sprintf(num_str,"%d\n",jump_address);
            strcat(program_string,num_str);

            insert_jump(program_string,'!');
         }
         else if (p->stmt->sk == SK_CU || p->stmt->sk == SK_CD) { // counting loop
            // initialize i to start_exp (pre-loop)
            strcat(program_string,"LAA ");
            sprintf(num_str,"%d\n",p->stmt->iterator_address);
            strcat(program_string,num_str);
            exprgen(program_string,p->stmt->start_exp);
            strcat(program_string,"STO\n");

            jump_address = get_line_number(program_string);
            // loop starts
            // get iterator 
            strcat(program_string,"LAA ");
            sprintf(num_str,"%d\n",p->stmt->iterator_address);
            strcat(program_string,num_str);
            strcat(program_string,"LOD\n");

            exprgen(program_string,p->stmt->finish_exp);
            if (p->stmt->sk == SK_CU) {
               strcat(program_string,"LEI\n"); // i <= finish_exp
               strcat(program_string,"LLI 0\n");
               strcat(program_string,"NEI\n");
            }
            else {
               strcat(program_string,"GEI\n");
               strcat(program_string,"LLI 0\n");
               strcat(program_string,"NEI\n");
            }
            strcat(program_string,"JPF *\n"); // jump if i <= finish_exp is false

            // execute loop body
            codegen(program_string,p->stmt->body);
            // increment iterator
            // get iterator
            strcat(program_string,"LAA ");
            sprintf(num_str,"%d\n",p->stmt->iterator_address);
            strcat(program_string,num_str);

            // get iterator +/- 1
            strcat(program_string,"LAA ");
            sprintf(num_str,"%d\n",p->stmt->iterator_address);
            strcat(program_string,num_str);
            strcat(program_string,"LOD\n");
            strcat(program_string,"LLI 1\n");
            if (p->stmt->sk == SK_CU) {
               strcat(program_string,"ADI\n");
            }
            else {
               strcat(program_string,"SBI\n");
            }
            // iterator := iterator + 1
            strcat(program_string,"STO\n");

            // jump to beginning of loop
            strcat(program_string,"JMP ");
            sprintf(num_str,"%d\n",jump_address);
            strcat(program_string,num_str);
            // insert jump address for end of loop (if loop conditional is false)
            insert_jump(program_string,'*');
         }
      }
      p = p->link;
   }
   if (main_scope) {
      strcat(program_string,"HLT\n");
   }
   return program_string;
}

void exprgen(char * program_string, struct expression * p) {
   char num_str[10];
   if (p->var && p->index_exp == NULL) { // variable
         strcat(program_string,"LAA ");
         sprintf(num_str,"%d\n",p->src_base_address);
         strcat(program_string,num_str);
         strcat(program_string,"LOD\n");
   }
   else if (p->var && p->index_exp != NULL) { // array reference
      strcat(program_string,"LLI ");
      sprintf(num_str,"%d\n",p->src_base_address);
      strcat(program_string,num_str);
      exprgen(program_string,p->index_exp);
      typecast(program_string,p->index_exp,EK_INT);
      strcat(program_string,"ADI\n");
      strcat(program_string,"LOD\n");
   }
   else if (p->kind != EK_OP) { // constant
      char num_str[10];
      if (p->kind == EK_FLO) {
         strcat(program_string,"LLF ");
         sprintf(num_str, "%f\n", p->float_value);
         strcat(program_string, num_str);
      }
      else if (p->kind == EK_INT) {
         strcat(program_string,"LLI ");
         sprintf(num_str, "%d\n", p->int_value);
         strcat(program_string, num_str);
      }
   }
   else { // operation
      switch (p->operator) {
         case OP_ANI:
            get_operands(program_string,p,EK_INT);
            strcat(program_string,"MLI\n");
            strcat(program_string,"LLI 0\n");
            strcat(program_string,"NEI\n");
            break;
         case OP_ANF:
            get_operands(program_string,p,EK_FLO);
            strcat(program_string,"MLF\n");
            strcat(program_string,"LLF 0.0\n");
            strcat(program_string,"NEF\n");
            break;
         case OP_ORI:
            get_operands(program_string,p,EK_INT);
            strcat(program_string,"ADI\n");
            strcat(program_string,"LLI 0\n");
            strcat(program_string,"NEI\n");
            break;
         case OP_ORF:
            get_operands(program_string,p,EK_FLO);
            strcat(program_string,"ADF\n");
            strcat(program_string,"LLF 0.0\n");
            strcat(program_string,"NEF\n");
            break;
         case OP_NTI:
            exprgen(program_string,p->r_operand);
            strcat(program_string,"LLI 0\n");
            strcat(program_string,"EQI\n");
            // do the not
            break;
         case OP_NTF:
            exprgen(program_string,p->r_operand);
            strcat(program_string,"LLF 0.0\n");
            strcat(program_string,"EQF\n");
            break;
         case OP_LTI:
            get_operands(program_string,p,EK_INT);
            strcat(program_string,"LTI\n");
            break;
         case OP_LTF:
            get_operands(program_string,p,EK_FLO);
            strcat(program_string,"LTF\n");
            break;
         case OP_LEI:
            get_operands(program_string,p,EK_INT);
            strcat(program_string,"LEI\n");
            break;
         case OP_LEF:
            get_operands(program_string,p,EK_FLO);
            strcat(program_string,"LEF\n");
            break;
         case OP_GTI:
            get_operands(program_string,p,EK_INT);
            strcat(program_string,"GTI\n");
            break;
         case OP_GTF:
            get_operands(program_string,p,EK_FLO);
            strcat(program_string,"GTF\n");
            break;
         case OP_GEI:
            get_operands(program_string,p,EK_INT);
            strcat(program_string,"GEI\n");
            break;
         case OP_GEF:
            get_operands(program_string,p,EK_FLO);
            strcat(program_string,"GEF\n");
            break;
         case OP_EQI:
            get_operands(program_string,p,EK_INT);
            strcat(program_string,"EQI\n");
            break;
         case OP_EQF:
            get_operands(program_string,p,EK_FLO);
            strcat(program_string,"EQF\n");
            break;
         case OP_NEI:
            get_operands(program_string,p,EK_INT);
            strcat(program_string,"NEI\n");
            break;
         case OP_NEF:
            get_operands(program_string,p,EK_FLO);
            strcat(program_string,"NEF\n");
            break;
         case OP_ADI:
            get_operands(program_string,p,EK_INT);
            strcat(program_string,"ADI\n");
            break;
         case OP_ADF:
            get_operands(program_string,p,EK_FLO);
            strcat(program_string,"ADF\n");
            break;
         case OP_SBI:
            get_operands(program_string,p,EK_INT);
            strcat(program_string,"SBI\n");
            break;
         case OP_SBF:
            get_operands(program_string,p,EK_FLO);
            strcat(program_string,"SBF\n");
            break;
         case OP_MLI:
            get_operands(program_string,p,EK_INT);
            strcat(program_string,"MLI\n");
            break;
         case OP_MLF:
            get_operands(program_string,p,EK_FLO);
            strcat(program_string,"MLF\n");
            break;
         case OP_DVI:
            get_operands(program_string,p,EK_INT);
            strcat(program_string,"DVI\n");
            break;
         case OP_DVF:
            get_operands(program_string,p,EK_FLO);
            strcat(program_string,"DVF\n");
            break;
         case OP_RMI:
            // % operator coerces all non-integers into integers.
            // NOTE:
            // the % operator executes a remainder operation (as opposed to a mathematical/Euclidean modulo operation)

            exprgen(program_string,p->l_operand);
            typecast(program_string,p->l_operand,EK_INT);
            exprgen(program_string,p->r_operand);
            typecast(program_string,p->r_operand,EK_INT);

            exprgen(program_string,p->l_operand);
            typecast(program_string,p->l_operand,EK_INT);
            exprgen(program_string,p->r_operand);
            typecast(program_string,p->r_operand,EK_INT);

            strcat(program_string,"DVI\n");
            strcat(program_string,"MLI\n");
            strcat(program_string,"SBI\n");
            break;
         case OP_NGI:
            exprgen(program_string,p->r_operand);
            strcat(program_string,"NGI\n");
            break;
         case OP_NGF:
            exprgen(program_string,p->r_operand);
            strcat(program_string,"NGF\n");
            break;
      } 
   }
   return;
}

void typecast(char * program_string, struct expression * operand, char type) {
   // only converts type if operand does not match the desired type 
   if (operand->kind != type && operand->return_type != type) {
      if (type == EK_INT) {
         strcat(program_string,"FTI\n");
         operand->kind = EK_INT;
      }
      else if (type == EK_FLO) {
         strcat(program_string,"ITF\n");
         operand->kind = EK_FLO;
      }
   }  
   return;
}
void get_operands(char * program_string, struct expression * p, char type) {
   exprgen(program_string,p->l_operand);
   if (type == EK_FLO && (p->l_operand->return_type != EK_FLO && p->l_operand->kind != EK_FLO)) {
      typecast(program_string,p->l_operand,EK_FLO);
   }
   exprgen(program_string,p->r_operand);
   if (type == EK_FLO && (p->r_operand->return_type != EK_FLO && p->r_operand->kind != EK_FLO)) {
      typecast(program_string,p->r_operand,EK_FLO);
   }
   return;
}
int get_line_number(char * program_string) {
   // get the current line number for jump addresses
   int i;
   int count = 0; 
   for (i = 0; i < strlen(program_string); i++) {
      if (program_string[i] == '\n') {
         count++;
      }
   }
   return count;
}
void insert_jump(char * program_string, char ifelse_char) {
   // for inserting jump addresses.
   // the ifelse_char variable dictates what character the jump address should replace.
   // this algorithm finds the last instance of that character, and replaces it with the jump address.
   int jump_address = get_line_number(program_string);
   char num_str[MAX_LINE_NUMBER_LENGTH]; //
   sprintf(num_str,"%d",jump_address);
   int i;
   int j = strlen(program_string) - 1 + strlen(num_str);
   int k; 
   char * return_string = malloc(MAX_PROG_LENGTH*sizeof(char));
   char done = 0;
   int jump_address_index;
   for (i = 0; i < strlen(program_string); i++) {
      // get last instance of '-'
      if (program_string[i] == ifelse_char) {
         jump_address_index = i;
      }
   }
   // get program_string up until jump address index
   strncpy(return_string,program_string,jump_address_index);

   // replace jump address index character with the jump address
   strcat(return_string,num_str); 

   // start again after the jump address and copy the rest of the string
   strcat(return_string,program_string+jump_address_index+1);

   // replace program_string with updated string
   strcpy(program_string,return_string);

   return;
}