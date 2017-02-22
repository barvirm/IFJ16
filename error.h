#ifndef error_h_xbarvi00
#define error_h_xbarvi00

#include <stdbool.h>

/*
 * Name: error.h
 * Author: Marek Barvir
 * Email: barvirm@seznam.cz
 * Login: xbarvi00
 * Date: 25.9.2016
 * Team members:  Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 *
 * Description: define errors by specification
*/

bool set_error(int e);

#define DEBUG_INFO(string) do { printf("%s\n",string); } while(0)

int error_number;


#define NO_ERROR 0
// Lexical analysis error
#define LEX_ERROR 1
// Syntax analysis error ( wrong synatax )
#define SYNTAX_ERROR 2
// Semantic error undefined or redifine class, function, variable
#define SEM_UNDEF_ERROR 3
// Semantic error in type uncompatibility ( aritmetic, strings and relation expresion )
// Wrong number of parameters at calling function
#define SEM_UNCOMP_ERROR 4
// Other semantic error
#define SEM_OTHER_ERROR 6
// Runtime error at reading input
#define RUNTIME_INPUT_ERROR 7
// Runtime error at uninicializated variable
#define RUNTIME_UNINIT_ERROR 8
// Runtime error - division by zero
#define RUNTIME_DIV_ZERO_ERROR 9
// Other runtime error
#define RUNTIME_OTHERS_ERROR 10
// Internal error of interpret ( bad alocated memory, open file, bad terminal parameters )
#define INTERN_ERROR 99


#endif
