/* 
 * Name: 	Garbage Collector Lite
 * Author: 	Petr Malaník
 * Email: 	xmalan02@stud.fit.vutbr.cz
 * Login:	xmalan02
 * Date:	7.10.2016
 * Team members: Barvíř, Malaník, Ondřej, Sochůrek, Smékal	
 * 
 * Description:	Store pointers of allocated memory for case of error
 * 				
*/

#ifndef GBC_h_xmalan02
#define GBC_h_xmalan02

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>


struct GBC_root GBC_List;

typedef struct GBC_part {
    void *ptr;
    bool seen;
    bool sleep;
    struct GBC_part *next;
} GBC_part_t;

typedef struct GBC_root {
	int size;
    struct GBC_part *First;
}GBC_root_t;

/**
 * @brief      Inicializate GBC List
 */
void GBC_Init();

/**
 * @brief      same as malloc + save pointer to GBC list
 *
 * @param[in]  size  size of memory, that you want to allocate
 *
 */
void *GBC_malloc(int size);

/**
 * @brief      append new pointer to GBC List
 *
 * @param      ptr   pointer to memory
 *
 * @return     size of GBC list
 */
int GBC_List_append(void *ptr);

/**
 * @brief      free all pointer in GBC List
 *
 * @return     number of pointer before dispose
 */
int GBC_Dispose();

/**
 * @brief      same as free + delete pointer in GBC List
 *
 * @param      ptr   pointer to memory
 */
void GBC_free(void *ptr);

/**
 * @brief      Display the CBG list with pointers
 */
void GBC_Display_ptr();


/**
 * @brief      print number of pointers in GBC List
 *
 * @return     size of GBC list
 */
int GBC_Print_size();

/**
 * @brief      same as realloc + save pointer to GBC list and delete old pointer
 *
 * @param      ptr   pointer to old memory
 * @param[in]  size  new size of realocated memmory
 *
 */
void *GBC_realloc(void*ptr,int size);


#endif