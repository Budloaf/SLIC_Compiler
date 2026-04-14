%{
/*
 *-------------------------------------------------------------------------
 * Project   - Checkpoint #7 / while, exit, modulo, arrays
 * Course    - CS4223
 * Developer - Hayden Smith
 * 
 * Description - This is the scanner for checkpoint #6.
 * Due - 02.04.2025
 *-------------------------------------------------------------------------
 */
#include "y.tab.h"
#include <stdio.h>
#include <string.h>

#define DEBUG 0
%}

%%

[mM][aA][iI][nN]                        { if (DEBUG) printf("MAIN: %s\n", yytext); 
                                          return(MAIN); }
[eE][xX][iI][tT]                        { if (DEBUG) printf("EXIT: %s\n", yytext); 
                                          return(EXIT); }
[eE][nN][dD]                            { if (DEBUG) printf("END: %s\n", yytext); 
                                          return(END); }
[iI][fF]                                { if (DEBUG) printf("IF: %s\n", yytext);
                                          return(IF); }
[eE][lL][sS][eE]                        { if (DEBUG) printf("ELSE: %s\n", yytext);
                                          return(ELSE); }
[wW][hH][iI][lL][eE]                    { if (DEBUG) printf("WHILE: %s\n", yytext);
                                          return(WHILE); }
[dD][aA][tT][aA]                        { if (DEBUG) printf("DATA: %s\n", yytext); 
                                          return(DATA); }
[aA][lL][gG][oO][rR][iI][tT][hH][mM]    { if (DEBUG) printf("ALGORITHM: %s\n", yytext);
                                          return(ALGORITHM); }
[iI][nN][tT][eE][gG][eE][rR]            { if (DEBUG) printf("INTEGER_TYPE: %s\n", yytext);
                                          return(INTEGER_TYPE); }
[rR][eE][aA][lL]                        { if (DEBUG) printf("REAL_TYPE: %s\n", yytext);
                                          return(REAL_TYPE); }
[cC][oO][uU][nN][tT][iI][nN][gG]        { if (DEBUG) printf("COUNTING: %s\n", yytext);
                                          return(COUNTING); }
[uU][pP][wW][aA][rR][dD]                { if (DEBUG) printf("UPWARD: %s\n", yytext);
                                          return(UPWARD); }
[dD][oO][wW][nN][wW][aA][rR][dD]        { if (DEBUG) printf("DOWNWARD: %s\n", yytext);
                                          return(DOWNWARD); }
[tT][oO]                                { if (DEBUG) printf("TO: %s\n", yytext);
                                          return(TO); }
[rR][eE][aA][dD]                        { if (DEBUG) printf("READ: %s\n", yytext);
                                          return(READ); }
[pP][rR][iI][nN][tT]                    { if (DEBUG) printf("PRINT: %s\n", yytext);
                                          return(PRINT); }

"+"                                     { if (DEBUG) printf("PLUS: %s\n", yytext);
                                          return(PLUS); }
"-"                                     { if (DEBUG) printf("MINUS: %s\n", yytext);
                                          return(MINUS); }
"*"                                     { if (DEBUG) printf("MULTIPLY: %s\n", yytext);
                                          return(MULTIPLY); }
"/"                                     { if (DEBUG) printf("DIVIDE: %s\n", yytext);
                                          return(DIVIDE); }
"%"                                     { if (DEBUG) printf("MODULO: %s\n", yytext);
                                          return(MODULO); }
"="                                     { if (DEBUG) printf("EQUAL: %s\n", yytext);
                                          return(EQUAL); }
"<"                                     { if (DEBUG) printf("LESS_THAN: %s\n", yytext);
                                          return(LESS_THAN); }
">"                                     { if (DEBUG) printf("GREATER_THAN: %s\n", yytext);
                                          return(GREATER_THAN); }
"<="                                    { if (DEBUG) printf("LESS_THAN_EQUAL: %s\n", yytext);
                                          return(LESS_THAN_EQUAL); }
">="                                    { if (DEBUG) printf("GREATER_THAN_EQUAL: %s\n", yytext);
                                          return(GREATER_THAN_EQUAL); }
"<>"                                    { if (DEBUG) printf("NOT_EQUAL: %s\n", yytext);
                                          return(NOT_EQUAL); }
";"                                     { if (DEBUG) printf("SEMICOLON: %s\n", yytext);
                                          return(SEMICOLON); }
"!"                                     { if (DEBUG) printf("EXCLAMATION_MARK: %s\n", yytext);
                                          return(EXCLAMATION_MARK); }
","                                     { if (DEBUG) printf("COMMA: %s\n", yytext);
                                          return(COMMA); }
":"                                     { if (DEBUG) printf("COLON: %s\n", yytext);
                                          return(COLON); }
"["                                     { if (DEBUG) printf("OPEN_BRACKET: %s\n", yytext);
                                          return(OPEN_BRACKET); }
"]"                                     { if (DEBUG) printf("CLOSE_BRACKET: %s\n", yytext);
                                          return(CLOSE_BRACKET); }
"&"                                     { if (DEBUG) printf("BOOL_AND: %s\n", yytext);
                                          return(BOOL_AND); }
"|"                                     { if (DEBUG) printf("BOOL_OR: %s\n", yytext);
                                          return(BOOL_OR); }
"~"                                     { if (DEBUG) printf("BOOL_NOT: %s\n", yytext);
                                          return(BOOL_NOT); }
":="                                    { if (DEBUG) printf("ASSIGNMENT: %s\n", yytext);
                                          return(ASSIGNMENT); }
"("                                     { if (DEBUG) printf("OPEN_PARENTHESIS: %s\n", yytext);
                                          return(OPEN_PARENTHESIS); }
")"                                     { if (DEBUG) printf("CLOSE_PARENTHESIS: %s\n", yytext);
                                          return(CLOSE_PARENTHESIS); }

[\t ]+                                                      { if (DEBUG) printf("WHITESPACE: %s\n", yytext);
                                                            }
[a-zA-Z][a-zA-Z0-9]*                                        { if (DEBUG) printf("VARIABLE_NAME: %s\n", yytext);
                                                              yylval.strval = strdup(yytext);
                                                              return(VARIABLE_NAME); }
\"([^\"\n]|(\"\"))*\"                                       { if (DEBUG) printf("CHAR_STRING: %s\n", yytext);
                                                                char * copy = strdup(yytext);
                                                                // get rid of first quotation mark by moving pointer
                                                                copy++; 
                                                                // get rid of last quotation mark by setting it equal to the end of string
                                                                yylval.strval = malloc(strlen(copy)*sizeof(char));
                                                                int i;
                                                                int strval_index = 0;
                                                                for (i = 0; i < strlen(copy) - 1; i++) {
                                                                    if (copy[i] == 34 && copy[i+1] == 34) {
                                                                        i++; // skip the next "
                                                                    }
                                                                    yylval.strval[strval_index] = copy[i];
                                                                    strval_index++;
                                                                    // remove " around string
                                                                }
                                                                yylval.strval[strval_index] = '\0';
                                                                return(CHAR_STRING);
                                                            }
[0-9]+                                                      { if (DEBUG) printf("INTEGER_CONSTANT: %s\n", yytext);
                                                              yylval.intval = atoi(yytext);
                                                              return(INTEGER_CONSTANT);  }
([0-9]+)(("."[0-9]+)?)((([eE][+-]?)|([eE]?))([0-9]+))?      { if (DEBUG) printf("FLOAT_CONSTANT: %s\n", yytext);
                                                              yylval.floval = atof(yytext);
                                                              return(FLOAT_CONSTANT);  }
"#".*                                                       { if (DEBUG) printf("COMMENT: %s\n", yytext);
                                                            }
\n                                                          { if (DEBUG) printf("NEWLINE: %s\n", yytext);
                                                            }

.                                                           { if (DEBUG) printf("TRASH: %s\n", yytext);
                                                              return(TRASH); }

%%
