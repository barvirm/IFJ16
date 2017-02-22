/* 
 * Name:	buildin.c
 * Author:	David Smékal,Marek Barvir
 * Email:	xsmeka13@stud.fit.vutbr.cz,barvirm@seznam.cz
 * Login:	xsmeka13,xbarvi00
 * Date:	28/9/2016
 * Team members:	Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 * 
 * Description:	Build-in functions
*/

#ifndef BUILDIN_H_INCLUDED
#define BUILDIN_H_INCLUDED
// TODO include limit.h

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
 
#include "error.h"
#include "ial.h"
#include "buffer.h"

/*
 * Reverse string
 * @param s string that will be reversed
 * @return reversed string
*/
void reverse(char *s);							// custom

/*
 * Transform integer to string
 * @param i Integer
 * @return string
 * @return NULL Error raise by malloc
*/
char *int_to_string(int i);						// custom

/*
 * Transform double to string
 * @param d Double
 * @return stirng
 * @return NULL Error raise by malloc
*/
char *double_to_string(double d);				// custom

int readInt();									// task
    
double readDouble();							// task

char *readString();								// task

void print(int argc, char *argv);				// task

/*
 * Calculate lenght of string
 * @param string
 * @return lenght of string
*/
int length(char *string);						// task

/*
 * Create substring from input string
 * @param string String
 * @param i starting index ( start at 0 )
 * @param n long of substring
 * @return String
 * @return NULL Error raise by malloc
*/
char *substr(char *string, int i, int n);		// task

/*
 * Lexicaly compare two strings
 * @param s1 first string
 * @param s2 second string
 * @return 0 On match
 * @return -1 If s1 is before s2
 * @return 1 If s2 is before s1
*/
int compare(char *s1, char *s2);				// task

#endif
