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

#include "gbc.h"

void GBC_Init(){
	GBC_List.size = 0;
	GBC_List.First = NULL;
}

void *GBC_malloc(int size){
	if(size < 0|| size > INT_MAX)
		return NULL;
	void *tmp = (void*) malloc(size);
	if (tmp == NULL){
		return NULL;
	}
	GBC_List_append(tmp);
	return tmp;
}

void *GBC_realloc(void*ptr,int size){
	if(size < 0|| size > INT_MAX)
		return NULL;
	void *tmp = (void*) realloc(ptr,size);
	if (tmp == NULL){
		return NULL;
	}

	//remove old pointer from list
	GBC_part_t *tmp_shift = GBC_List.First;
	GBC_part_t *tmp_shift2;

	//if first record in list is wanted pointer
	if(tmp_shift->ptr==ptr){
		GBC_List.First=tmp_shift->next;
		free(tmp_shift);
		GBC_List.size--;
	}
	else{
		//check all pointers in list
		while(tmp_shift->next!= NULL){
			if (tmp_shift->next->ptr==ptr){
				
				tmp_shift2 = tmp_shift->next;
				tmp_shift->next=tmp_shift2->next;

				free(tmp_shift2);

				GBC_List.size--;
				break;
			}
			tmp_shift=tmp_shift->next;
		}
	}
	//append new pointer to list
	GBC_List_append(tmp);
	return tmp;
}

int GBC_List_append(void *ptr){
	//creation if new membor
	GBC_part_t *tmp = (GBC_part_t *) malloc(sizeof(GBC_part_t));
	tmp->next = NULL;
	tmp->ptr = ptr;

	//if list is completly empty
	if(GBC_List.First == NULL){
 		GBC_List.First = tmp;
 		GBC_List.size++;
 		return GBC_List.size;
	}

	GBC_part_t *tmp_shift = GBC_List.First;

	//go to end of list
	while(tmp_shift->next != NULL){
		tmp_shift = tmp_shift->next;
	}
	tmp_shift->next = tmp;
	GBC_List.size++;
 	return GBC_List.size;
}

int GBC_Dispose(){
#ifdef GDB_DEBUG
	int size_of_leaked = 0;
#endif
	int part_number = 0;
	if(GBC_List.First == NULL){
 		return part_number;
	}

	//inicialization of temporary variables
	GBC_part_t *tmp_shift = GBC_List.First;
	GBC_part_t *tmp2_shift;

	//go through the whole list and free every member
	while(tmp_shift!= NULL){
		tmp2_shift = tmp_shift->next;
		free(tmp_shift->ptr);
		free(tmp_shift);
		GBC_List.size--;
		tmp_shift = tmp2_shift;
		part_number++;
	}

	GBC_List.First = NULL;

	#ifdef DEBUG_GBC
	printf("Leaks = %i\n",part_number);
	printf("Leaked = %i\n",size_of_leaked);
	#endif

	return part_number;
}

void GBC_free(void *ptr){
	if(GBC_List.First == NULL){
 		return;
	}

	//inicialization of temporary variables
	GBC_part_t *tmp_shift = GBC_List.First;
	GBC_part_t *tmp_shift2;

	//if first record in list is wanted pointer
	if(tmp_shift->ptr==ptr){
		GBC_List.First=tmp_shift->next;
		free(tmp_shift->ptr);
		free(tmp_shift);
		GBC_List.size--;
		return;
	}

	//check all pointers in list
	while(tmp_shift->next!= NULL){
		if (tmp_shift->next->ptr==ptr){
			
			tmp_shift2 = tmp_shift->next;
			tmp_shift->next=tmp_shift2->next;

			free(tmp_shift2->ptr);
			free(tmp_shift2);

			GBC_List.size--;
			return;
		}
		tmp_shift=tmp_shift->next;
	}
}

void GBC_Display_ptr(){
	printf("|");
	GBC_part_t *tmp_shift = GBC_List.First;

	//go through the whole list and print every member
	while(tmp_shift!= NULL){
		printf(" %li |",(long)tmp_shift->ptr);
		tmp_shift=tmp_shift->next;
	}
	printf("\n");
	return;
}

int GBC_Print_size(){
	printf("Size of GBC is %i\n",GBC_List.size);
	return GBC_List.size;
}


//test program
#ifdef DEBUG_GBC

int main(void){
	GBC_Init();
	GBC_List.size = 0;
	int* h1 = (int *)GBC_malloc(10);
	GBC_Display_ptr();
	int* h2 = (int *)GBC_malloc(10);
	GBC_Display_ptr();
	int* h3 = (int *)GBC_malloc(10);
	GBC_Display_ptr();

	h3=GBC_realloc(h3,10000);

	GBC_Display_ptr();
	GBC_free(h1);
	GBC_Display_ptr();
	GBC_free(h2);
	GBC_Display_ptr();
	GBC_Dispose();
	printf("Size of GBC is %i\n",GBC_List.size);
	GBC_Display_ptr();
	return 0;
}

#endif
