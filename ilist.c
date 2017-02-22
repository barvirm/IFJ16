/*
 * Name:    ilist.c
 * Author:  Karel ONDŘEJ, FIT
 * Email:   xondre09@stud.fit.vutbr.cz
 * Login    xondre09
 * Date:    6. 10. 2016
 * Team members:  Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 *
 * Description: 
 * ...
 */

#include "ilist.h"

int ilist_active(ilist_t* L);
void ilist_disactive(ilist_t* L);
void ilist_init(ilist_t* L);
int ilist_copy (ilist_t* L, instruction_t* val);
void ilist_add(ilist_t* L, ilist_item_t* new);
void ilist_succ (ilist_t *L);
void ilist_first (ilist_t *L);
void ilist_dispose_list (ilist_t *L);
void ilist_delete_first (ilist_t *L);

ilist_item_t* init_instruction(ins_type_t type, void* op1, void* op2, void* result) {
    instruction_t i;   
    i.type = type;
    i.op1 = op1;
    i.op2 = op2;
    i.result = result;
  
    ilist_item_t* new = (ilist_item_t*)GBC_malloc(sizeof(ilist_item_t));

    if (new == NULL) {
        return new;
    }
    new->ins  = i;
    new->next = NULL;
    
    return new;

}

void generate_instruction(ilist_t* L, ins_type_t type, void* op1, void* op2, void* result) {
    
    ilist_item_t* i = init_instruction(type, op1, op2, result);

    ilist_add(L, i);
    ilist_succ(L);

}

void generate_function(ilist_t* L, void* op1, void* space) {

    if( L->cf % FCHUNCK == 0) {
        L->Function = (ilist_item_t**)GBC_realloc(L->Function, (L->cf + FCHUNCK) * sizeof(ilist_item_t*));
    }

    ilist_item_t* lab1 = init_instruction(INS_LAB, op1, NULL, NULL);

    L->Function[L->cf] = lab1;
    ilist_add(L, lab1);
    ilist_succ_generate(L);
    generate_instruction(L, BEGIN_FUNC, space, NULL, NULL);
    ((node_data_t*)op1)->offset = L->cf++;
}
void generate_while(ilist_t* L,  void* op1) {

    ilist_item_t* lab1 = init_instruction(INS_LAB, NULL, NULL, NULL);
    ilist_item_t* lab2 = init_instruction(INS_LAB, NULL, NULL, NULL);
    ilist_item_t* cond = init_instruction(JMPZ, op1, NULL, lab2);
    ilist_item_t* jump = init_instruction(JMP, NULL, NULL, lab1);

    ilist_add(L, lab1);
    ilist_succ_generate(L);


    ilist_add(L, lab2);
    ilist_add(L, jump);
    ilist_add(L, cond);
}

void generate_if_else(ilist_t* L, void* op1) {
    ilist_item_t* lab1 = init_instruction(INS_LAB, NULL, NULL, NULL);
    ilist_item_t* lab2 = init_instruction(INS_LAB, NULL, NULL, NULL);
    ilist_item_t* cond = init_instruction(JMPZ, op1, NULL, lab1);
    ilist_item_t* jump = init_instruction(JMP, NULL, NULL, lab2);

    ilist_add(L, cond);
    ilist_succ_generate(L);

    ilist_add(L, lab2);
    ilist_add(L, lab1);
    ilist_add(L, jump);
}

void generate_if_end(ilist_t* L) {
    ilist_succ(L);
    ilist_succ(L);
}

void generate_else_end(ilist_t* L) {
    ilist_succ(L);
}

void generate_while_condition_end(ilist_t* L) {
    ilist_succ(L);
}

void generate_while_end(ilist_t* L) {
    ilist_succ(L);
    ilist_succ(L);
}

void ilist_init(ilist_t* L) {
    L->First = L->Last = L->Actual = NULL;
    L->Function = NULL;
    L->cf = 0;
}

int ilist_active(ilist_t* L) {
    return L->Actual != NULL;
}

void ilist_disactive(ilist_t* L) {
    L->Actual = NULL;
}

int ilist_copy(ilist_t *L, instruction_t *val) {

    if (L->Actual == NULL) {
        return INTERN_ERROR;
    }
    else {
        *val = L->Actual->ins;
    }
    return NO_ERROR;
}

void ilist_add(ilist_t* L, ilist_item_t* new) {

    if (ilist_active(L)) {
        new->next = L->Actual->next;
        L->Actual->next = new;
        if (L->Actual == L->Last) {
            L->Last = L->Actual->next;
        }
    }
    else {
        if (L->Last == NULL) {
            L->First = L->Last = new;
        }
        else {
            L->Last->next = new;
            L->Last = new;
        }
    }
}

void ilist_succ (ilist_t *L) {
    if (L->Actual != NULL){
        L->Actual = L->Actual->next;
    }
}

void ilist_succ_generate(ilist_t *L) {
    if (L->Actual != NULL){
        L->Actual = L->Actual->next;
    }
    else {
        L->Actual = L->Last;
    }
}

void ilist_first (ilist_t *L) {
    L->Actual = L->First;
}

void ilist_dispose_list (ilist_t *L) {
    if (L != NULL)
    {
        while(L->First != NULL ) {
            ilist_delete_first(L);
        }
    }
    GBC_free(L->Function);
}

void ilist_delete_first (ilist_t *L) {
    if (L->First != NULL) {
        ilist_item_t* next = L->First->next;

        if (L->Actual == L->First) {
            L->Actual = NULL;
        }
        GBC_free(L->First);
        L->First = next;
    }
}

