/*
 * Name:    interpret.c
 * Author:  Karel ONDŘEJ, FIT
 * Email:   xondre09@stud.fit.vutbr.cz
 * Login    xondre09
 * Date:    6. 10. 2016
 * Team members:  Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 *
 * Description: 
 * ...
 */

#include "interpret.h"


#define type(op, t) ((op)->func_type != NULL && (op)->func_type[0] == t && (op)->func_type[1] == '\0')

int get_variable(storage_t* s, node_data_t* var, var_t* dest) {
    int err = NO_ERROR;

    if (var->struct_type == LOCAL) {
        err = storage_get_local(s, var->offset, dest);
        return err;
    }
    else if (var->struct_type == GLOBAL) {
        dest->type = var->func_type[0];
        dest->data = var->data_u;
        if(var->inicializate == 0 ) {
        	fprintf(stderr, "Error: No inicializate variable\n");
        }
        return var->inicializate == 0 ? RUNTIME_UNINIT_ERROR : NO_ERROR;
    }
    else if( var->struct_type == CONST) {
        dest->type = var->func_type[0];
        dest->data = var->data_u;
        return NO_ERROR;
    }
    fprintf(stderr, "Error: Unknown variable (%c, %s)\n", var->struct_type, var->func_type);
    return INTERN_ERROR;
}

int set_variable(storage_t* s, node_data_t* var, var_t val) {
    if (var->struct_type == GLOBAL) {
        var->inicializate = 1;
        if( type(var, tSTRING)) {
        	if(var->data_u.svar != NULL) {
        		GBC_free(var->data_u.svar);
        	}
            var->data_u.svar = GBC_malloc(sizeof(char)*(strlen(val.data.svar)+1));
            strcpy(var->data_u.svar, val.data.svar);
        }
        else {
            var->data_u = val.data;
        }
        return NO_ERROR;
    }
    else if (var->struct_type == LOCAL) {
        return storage_set_local(s, var->offset, val);
    }
    else if (var->struct_type == CONST) {   
        fprintf(stderr, "Error: Constant change\n");
        return INTERN_ERROR;
    }
    fprintf(stderr, "Error: Unknown variable (%c)\n", var->struct_type);
    return INTERN_ERROR;
}

static inline int ins_store(storage_t* storage, node_data_t* op1, node_data_t* result) {
    var_t a;
    var_t c;
    c.type = result->func_type[0];

    int err;

    if( (err = get_variable(storage, op1, &a))  ) {
        return err;
    }

    if(type(result, tINTEGER) && type(op1, tINTEGER) ) {
        c.data.ivar = a.data.ivar;
    }
    else if(type(result, tDOUBLE) && type(op1, tINTEGER)) { 
        c.data.dvar = (double)a.data.ivar;
    }
    else if(type(result, tDOUBLE) && type(op1, tDOUBLE)) {
        c.data.dvar = a.data.dvar;
    }
    else if(type(result, tSTRING)  && type(op1, tSTRING)) {     // TODO
        c.data.svar = a.data.svar;
    }
    else {
        fprintf(stderr, "Error: incompatible type for assignments\n");
        return SEM_UNCOMP_ERROR;
    }

    if ( (err = set_variable(storage, result, c)) ) {
        return err;
    }
    return NO_ERROR;
}

static inline int jmp_zero(storage_t* storage, ilist_t* ilist, node_data_t* op1, ilist_item_t* result) {
    var_t a;

    int err;

    if( (err = get_variable(storage, op1, &a)) ) {
        return err;
    }
    if (a.data.ivar == 0) {
        ilist->Actual = result;
    }
    return NO_ERROR;
}

static inline int jmp(ilist_t* ilist, ilist_item_t* result) {
    
    ilist->Actual = result;
    return NO_ERROR;  
}

static inline int ins_add(storage_t* storage, node_data_t* op1, node_data_t* op2, node_data_t* result) {
    var_t a;
    var_t b;
    var_t c;
    c.type = result->func_type[0];

    int err;
    char buffer[256];
    int lena = 0;
    int lenb = 0;
    int conva = 0;
    int convb = 0;

    if( (err = get_variable(storage, op1, &a)) || (err = get_variable(storage, op2, &b)) ) {
        return err;
    }

    if(type(result, tINTEGER) && type(op1, tINTEGER) && type(op2, tINTEGER) ) {      // int = int + int i
        c.data.ivar = a.data.ivar + b.data.ivar;
    }
    else if(type(result, tDOUBLE)&& type(op1, tINTEGER) && type(op2, tINTEGER)) {    // double = int + int
        c.data.dvar = a.data.ivar + b.data.ivar;
    }
    else if(type(result, tDOUBLE)&& type(op1, tINTEGER) && type(op2, tDOUBLE)) {     // double = int + double
        c.data.dvar = a.data.ivar + b.data.dvar;
    }
    else if(type(result, tDOUBLE) && type(op1, tDOUBLE) && type(op2, tINTEGER)) {    // double = double + int
        c.data.dvar = a.data.dvar + b.data.ivar;
    }
    else if(type(result, tDOUBLE) && type(op1, tDOUBLE) && type(op2, tDOUBLE)) {     // double = double + double
        c.data.dvar = a.data.dvar + b.data.dvar;
    }
    else if(type(result, tSTRING)){

        if(type(op1, tINTEGER)) {
            sprintf(buffer, "%i", a.data.ivar);
            lena = strlen(buffer);
            a.data.svar = (char*)malloc(sizeof(char) * (lena+1));
            strcpy(a.data.svar, buffer);
            conva = 1;
        }
        else if(type(op1, tDOUBLE)){
            sprintf(buffer, "%g", a.data.dvar);
            lena = strlen(buffer);
            a.data.svar = (char*)malloc(sizeof(char) * (lena+1));
            strcpy(a.data.svar, buffer);
            conva = 1;
        }
        else {
            lena = strlen(a.data.svar);
        }

        if(type(op2, tINTEGER)) {
            sprintf(buffer, "%i", b.data.ivar);
            lenb = strlen(buffer);
            b.data.svar = (char*)malloc(sizeof(char) * (lenb+1));
            strcpy(b.data.svar, buffer);
            convb = 1;
        }
        else if(type(op2, tDOUBLE)){
            sprintf(buffer, "%g", b.data.dvar);
            lenb = strlen(buffer);
            b.data.svar = (char*)malloc(sizeof(char) * (lenb+1));
            strcpy(b.data.svar, buffer);
            convb = 1;
        }
        else {
            lenb = strlen(b.data.svar);
        }

        c.type = tSTRING;
        c.data.svar = (char*)malloc(sizeof(char) * (lena + lenb + 1));
        strcpy(c.data.svar, a.data.svar);
        strcat(c.data.svar, b.data.svar);
        if(conva == 1) { free(a.data.svar); }
        if(convb == 1) { free(b.data.svar); }
        if ( (err = set_variable(storage, result, c)) ) {
            return err;
        }
        free(c.data.svar);
        return NO_ERROR;
    }
    else {
        fprintf(stderr, "Error: incompatible type for the addition operation: %s = %s + %s\n", op1->func_type, op2->func_type, result->func_type);
        return SEM_UNCOMP_ERROR;
    }
    
    if ( (err = set_variable(storage, result, c)) ) {
        return err;
    }
    return NO_ERROR;
}

static inline int ins_sub(storage_t* storage, node_data_t* op1, node_data_t* op2, node_data_t* result){
    var_t a;
    var_t b;
    var_t c;
    c.type = result->func_type[0];

    int err;

    if( (err = get_variable(storage, op1, &a)) || (err = get_variable(storage, op2, &b)) ) {
        return err;
    }

    if(type(result, tINTEGER) && type(op1, tINTEGER) && type(op2, tINTEGER) ) {      // int = int - int
        c.data.ivar = a.data.ivar - b.data.ivar; 
    }
    else if(type(result, tDOUBLE) && type(op1, tINTEGER) && type(op2, tINTEGER)) {   // double = int - int
        c.data.dvar = a.data.ivar - b.data.ivar;
    }
    else if(type(result, tDOUBLE) && type(op1, tINTEGER) && type(op2, tDOUBLE)) {    // double = int - double
        c.data.dvar = a.data.ivar - b.data.dvar;
    }
    else if(type(result, tDOUBLE) && type(op1, tDOUBLE)  && type(op2, tINTEGER) ) {  // double = double - int
        c.data.dvar = a.data.dvar - b.data.ivar;
    }
    else if(type(result, tDOUBLE) && type(op1, tDOUBLE)  && type(op2, tDOUBLE)) {    // double = double - double
        c.data.dvar = a.data.dvar - b.data.dvar;
    }
    else {
        fprintf(stderr, "Error: incompatible type for the subtraction operation\n");
        return SEM_UNCOMP_ERROR;
    }

    if ( (err = set_variable(storage, result, c)) ) {
        return err;
    }
    
    return NO_ERROR;
}

static inline int ins_mul(storage_t* storage, node_data_t* op1, node_data_t* op2, node_data_t* result){
    var_t a;
    var_t b;
    var_t c;
    c.type = result->func_type[0];

    int err;

    if( (err = get_variable(storage, op1, &a)) || (err = get_variable(storage, op2, &b)) ) {
        return err;
    }
    
    if(type(result, tINTEGER) && type(op1, tINTEGER) && type(op2, tINTEGER) ) {      // int = int * int
        c.data.ivar = a.data.ivar * b.data.ivar; 
    }
    else if(type(result, tDOUBLE) && type(op1, tINTEGER) && type(op2, tINTEGER)) {   // double = int * int
        c.data.dvar = a.data.ivar * b.data.ivar;
    }
    else if(type(result, tDOUBLE) && type(op1, tINTEGER) && type(op2, tDOUBLE)) {    // double = int * double
        c.data.dvar = a.data.ivar * b.data.dvar;
    }
    else if(type(result, tDOUBLE) && type(op1, tDOUBLE) && type(op2, tINTEGER) ) {   // double = double * int
        c.data.dvar = a.data.dvar * b.data.ivar;
    }
    else if(type(result, tDOUBLE) && type(op1, tDOUBLE) && type(op2, tDOUBLE)) {     // double = double * double
        c.data.dvar = a.data.dvar * b.data.dvar;
    }
    else {
        fprintf(stderr, "Error: incompatible type for multiplication operation %c = %i * %i\n", result->func_type[0],op1->data_u.ivar,op2->data_u.ivar);
        return SEM_UNCOMP_ERROR;
    }
    
    if ( (err = set_variable(storage, result, c)) ) {
        return err;
    }

    return NO_ERROR;
}

static inline int ins_div(storage_t* storage, node_data_t* op1, node_data_t* op2, node_data_t* result){
    var_t a;
    var_t b;
    var_t c;
    c.type = result->func_type[0];

    int err;

    if( (err = get_variable(storage, op1, &a)) || (err = get_variable(storage, op2, &b)) ) {
        return err;
    }

    if((b.type == tINTEGER && b.data.ivar == 0) || (b.type == tDOUBLE && b.data.dvar == 0.0) ) {
        return RUNTIME_DIV_ZERO_ERROR;
    }

    if(type(result, tINTEGER) && type(op1, tINTEGER) && type(op2, tINTEGER)) {      // int = int / int
        c.data.ivar = a.data.ivar / b.data.ivar;
    }
    else if(type(result, tDOUBLE) && type(op1, tINTEGER) && type(op2, tINTEGER)) {  // double = int / int
        c.data.dvar = a.data.ivar / b.data.ivar;
    }
    else if(type(result, tDOUBLE) && type(op1, tINTEGER) && type(op2, tDOUBLE)) {   // double = int / double
        c.data.dvar = a.data.ivar / b.data.dvar;
    }
    else if(type(result, tDOUBLE) && type(op1, tDOUBLE)  && type(op2, tINTEGER)) {  // double = double / int
        c.data.dvar = a.data.dvar / b.data.ivar;
    }
    else if(type(result, tDOUBLE) && type(op1, tDOUBLE)  && type(op2, tDOUBLE)) {   // double = double / double
        c.data.dvar = a.data.dvar / b.data.dvar;
    }
    else {
        fprintf(stderr, "Error: incompatible type for division operation\n");
        return SEM_UNCOMP_ERROR;
    }
    
    if ( (err = set_variable(storage, result, c)) ) {
        return err;
    }
    
    return NO_ERROR;
}

static inline int ins_less(storage_t* storage, node_data_t* op1, node_data_t* op2, node_data_t* result) {
    var_t a;
    var_t b;
    var_t c;
    c.type = result->func_type[0];

    int err;

    if( (err = get_variable(storage, op1, &a)) || (err = get_variable(storage, op2, &b)) ) {
        return err;
    }

    if(type(result, tSTRING) || type(op1, tSTRING) || type(op2, tSTRING)) {
        fprintf(stderr, "Error: incompatible type for < operation\n");
        return SEM_UNCOMP_ERROR;
    }
    

    if(type(op1, tINTEGER) && type(op2, tINTEGER)) {        // int = int < int
        c.data.ivar = a.data.ivar < b.data.ivar;
    }
    else if(type(op1, tINTEGER) && type(op2, tDOUBLE)) {    // int = int < double
        c.data.ivar = a.data.ivar < b.data.dvar;
    }
    else if(type(op1, tDOUBLE) && type(op2, tINTEGER)) {    // int = double < int
        c.data.ivar = a.data.dvar < b.data.ivar;
    }
    else if(type(op1, tDOUBLE) && type(op2, tDOUBLE)) {     // int = double < tdouble
        c.data.ivar = a.data.dvar < b.data.dvar;
    }

    if(type(result, tDOUBLE)) {
        c.data.dvar = (double)c.data.ivar;
    }

    if ( (err = set_variable(storage, result, c)) ) {
        return err;
    }
    
    return NO_ERROR;
}

static inline int ins_eless(storage_t* storage, node_data_t* op1, node_data_t* op2, node_data_t* result) {
    var_t a;
    var_t b;
    var_t c;
    c.type = result->func_type[0];

    int err;

    if( (err = get_variable(storage, op1, &a)) || (err = get_variable(storage, op2, &b)) ) {
        return err;
    }

    if(type(result, tSTRING) || type(op1, tSTRING) || type(op2, tSTRING)) {
        fprintf(stderr, "Error: incompatible type for <= operation\n");
        return SEM_UNCOMP_ERROR;
    }

    if(type(op1, tINTEGER) && type(op2, tINTEGER)) {        // int = int <= int
        c.data.ivar = a.data.ivar <= b.data.ivar;
    }
    else if(type(op1, tINTEGER) && type(op2, tDOUBLE)) {    // int = int <= double
        c.data.ivar = a.data.ivar <= b.data.dvar;
    }
    else if(type(op1, tDOUBLE) && type(op2, tINTEGER)) {    // int = double <= int
        c.data.ivar = a.data.dvar <= b.data.ivar;
    }
    else if(type(op1, tDOUBLE) && type(op2, tDOUBLE)) {     // int = double <= tdouble
        c.data.ivar = a.data.dvar <= b.data.dvar;
    }

    if(type(result, tDOUBLE)) {
        c.data.dvar = (double)c.data.ivar;
    }

    if ( (err = set_variable(storage, result, c)) ) {
        return err;
    }
    
    return NO_ERROR;
}

static inline int ins_great(storage_t* storage, node_data_t* op1, node_data_t* op2, node_data_t* result) {
    var_t a;
    var_t b;
    var_t c;
    c.type = result->func_type[0];

    int err;

    if( (err = get_variable(storage, op1, &a)) || (err = get_variable(storage, op2, &b)) ) {
        return err;
    }

    if(type(result, tSTRING) || type(op1, tSTRING) || type(op2, tSTRING)) {
        fprintf(stderr, "Error: incompatible type for > operation\n");
        return SEM_UNCOMP_ERROR;
    }


    if(type(op1, tINTEGER) && type(op2, tINTEGER)) {        // int = int > int
        c.data.ivar = a.data.ivar > b.data.ivar;
    }
    else if(type(op1, tINTEGER) && type(op2, tDOUBLE)) {    // int = int > double
        c.data.ivar = a.data.ivar > b.data.dvar;
    }
    else if(type(op1, tDOUBLE) && type(op2, tINTEGER)) {    // int = double > int
        c.data.ivar = a.data.dvar > b.data.ivar;
    }
    else if(type(op1, tDOUBLE) && type(op2, tDOUBLE)) {     // dint = double > tdouble
        c.data.ivar = a.data.dvar > b.data.dvar;
    }

    if(type(result, tDOUBLE)) {
        c.data.dvar = (double)c.data.ivar;
    }

    if ( (err = set_variable(storage, result, c)) ) {
        return err;
    }
    
    return NO_ERROR;
}

static inline int ins_egreat(storage_t* storage, node_data_t* op1, node_data_t* op2, node_data_t* result) {
    var_t a;
    var_t b;
    var_t c;
    c.type = result->func_type[0];

    int err;

    if( (err = get_variable(storage, op1, &a)) || (err = get_variable(storage, op2, &b)) ) {
        return err;
    }

    if(type(result, tSTRING) || type(op1, tSTRING) || type(op2, tSTRING)) {
        fprintf(stderr, "Error: incompatible type for >= operation\n");
        return SEM_UNCOMP_ERROR;
    }


    if(type(op1, tINTEGER) && type(op2, tINTEGER)) {        // int = int >= int
        c.data.ivar = a.data.ivar >= b.data.ivar;
    }
    else if(type(op1, tINTEGER) && type(op2, tDOUBLE)) {    // double = int >= double
        c.data.ivar = a.data.ivar >= b.data.dvar;
    }
    else if(type(op1, tDOUBLE) && type(op2, tINTEGER)) {    // double = double >= int
        c.data.ivar = a.data.dvar >= b.data.ivar;
    }
    else if(type(op1, tDOUBLE) && type(op2, tDOUBLE)) {     // double = double >= tdouble
        c.data.ivar = a.data.dvar >= b.data.dvar;
    }

    if(type(result, tDOUBLE)) {
        c.data.dvar = (double)c.data.ivar;
    }

    if ( (err = set_variable(storage, result, c)) ) {
        return err;
    }
    
    return NO_ERROR;
}

static inline int ins_equal(storage_t* storage, node_data_t* op1, node_data_t* op2, node_data_t* result) {
    var_t a;
    var_t b;
    var_t c;
    c.type = result->func_type[0];

    int err;

    if( (err = get_variable(storage, op1, &a)) || (err = get_variable(storage, op2, &b)) ) {
        return err;
    }

    if(type(result, tSTRING) || type(op1, tSTRING) || type(op2, tSTRING)) {
        fprintf(stderr, "Error: incompatible type for the comparison operation: %s = %s == %s\n", result->func_type, op1->func_type, op2->func_type);
    }

    if(type(op1, tINTEGER) && type(op2, tINTEGER)) {        // int = int == int
        c.data.ivar = a.data.ivar == b.data.ivar;
    }
    else if(type(op1, tINTEGER) && type(op2, tDOUBLE)) {    // double = int == double
        c.data.ivar = a.data.ivar == b.data.dvar;
    }
    else if(type(op1, tDOUBLE) && type(op2, tINTEGER)) {    // double = double == int
        c.data.ivar = a.data.dvar == b.data.ivar;
    }
    else if(type(op1, tDOUBLE) && type(op2, tDOUBLE)) {     // double = double == tdouble
        c.data.ivar = a.data.dvar == b.data.dvar;
    }

    if(type(result, tDOUBLE)) {
        c.data.dvar = (double)c.data.ivar;
    }
    
    if ( (err = set_variable(storage, result, c)) ) {
        return err;
    }
    
    return NO_ERROR;
}
static inline int ins_not(storage_t* storage, node_data_t* op1, node_data_t* op2, node_data_t* result) {
    var_t a;
    var_t b;
    var_t c;
    c.type = result->func_type[0];

    int err = NO_ERROR;

    if( (err = get_variable(storage, op1, &a)) || (err = get_variable(storage, op2, &b)) ) {
        return err;
    }

    if(type(result, tSTRING) || type(op1, tSTRING) || type(op2, tSTRING)) {
        fprintf(stderr, "Error: incompatible type for != operation\n");
        return SEM_UNCOMP_ERROR;
    }

    if(type(op1, tINTEGER) && type(op2, tINTEGER)) {        // int = int != int
        c.data.ivar = a.data.ivar != b.data.ivar;
    }
    else if(type(op1, tINTEGER) && type(op2, tDOUBLE)) {    // int = int != double
        c.data.ivar = a.data.ivar != b.data.dvar;
    }
    else if(type(op1, tDOUBLE) && type(op2, tINTEGER)) {    // int = double != int
        c.data.ivar = a.data.dvar != b.data.ivar;
    }
    else if(type(op1, tDOUBLE) && type(op2, tDOUBLE)) {     // int = double != tdouble
        c.data.ivar = a.data.dvar != b.data.dvar;
    } 

    if(type(result, tDOUBLE)) {
        c.data.dvar = (double)c.data.ivar;
    }

    if ( (err = set_variable(storage, result, c)) ) {
        return err;
    }

    return NO_ERROR;
}

int push_param(storage_t* storage, node_data_t* op1) {
    var_t a;

    int err;
    if( (err = get_variable(storage, op1, &a))  ) {
        return err;
    }

    if ( (err = frame_append(storage->Temp, a)) ) {
        return err;
    }
    return NO_ERROR;
}

int end_func(storage_t* storage, ilist_t* ilist, node_data_t* op1) {
    if(op1->func_type[0] != tVOID) {
        fprintf(stderr, "Error: missing return in non-void function.\n");
        return RUNTIME_UNINIT_ERROR;
    }
    ilist->Actual = storage->Local->Function;
    storage_pop(storage);
    return NO_ERROR;
}

int call(storage_t* storage, ilist_t* ilist, node_data_t* op1, node_data_t* result) {
    int err = NO_ERROR;
    var_t a;

    if(op1 == NULL || op1->func_type == NULL){
        return INTERN_ERROR;
    }

    if( result != NULL) {
        if( (op1->func_type[0] == tVOID && result != NULL) ||
            (op1->func_type[0] != result->func_type[0] && !(op1->func_type[0] == tINTEGER && type(result, tDOUBLE)))) {
            fprintf(stderr, "Error: assigning of void value %c = %c\n", result->func_type[0], op1->func_type[0]);
            return SEM_UNCOMP_ERROR;
        }
    }

    storage->Temp->Result = result;
    storage->Temp->Function = ilist->Actual;

    storage_push_temp(storage);

    ilist->Actual = ilist->Function[op1->offset];
    
    node_data_t T0 = {.struct_type = 'l', .func_type = "e" };
    
    if(((int)strlen(op1->func_type)-1 ) != storage->Local->Count) {
        if(((int)strlen(op1->func_type)-1 ) < storage->Local->Count) {
            fprintf(stderr, "Error: too few arguments to function\n");
        }
        else {
            fprintf(stderr, "Error: too many arguments to function\n");
        }
        return SEM_UNCOMP_ERROR;
    }

    for( int i = 1; op1->func_type[i] != '\0'; i++) {
        T0.offset = i - 1;

        if( (err = get_variable(storage, &T0, &a))  ) {
            return err;
        }
        if(op1->func_type[i] == tDOUBLE && a.type == tINTEGER) {
            a.type = tDOUBLE;
            a.data.dvar = (double)a.data.ivar;
            if( (err = set_variable(storage, &T0, a))) {
                return err;
            }           
        }
        else if(op1->func_type[i] != a.type) {
            fprintf(stderr, "Error: wrong type of arguments to function\n");
            return SEM_UNCOMP_ERROR;
        }

    }
    return NO_ERROR;
}

int ret(storage_t* storage, ilist_t* ilist, node_data_t* op1) {
    int err = NO_ERROR;
    var_t a;
    var_t temp;
    node_data_t* result = storage->Local->Result;

    if(op1 == NULL && result == NULL) {
        ilist->Actual = storage->Local->Function;
        storage_pop(storage);       
    }
    else {
        if((err = get_variable(storage, op1, &temp))) {
            return err;
        }

        if(temp.type == tSTRING){
        	a.type = temp.type;
        	a.data.svar = (char*)malloc(strlen(temp.data.svar) + 1);
        	strcpy(a.data.svar, temp.data.svar);
        }	
        else{
        	a = temp;
        }

        ilist->Actual = storage->Local->Function;
        storage_pop(storage);

        if( result != NULL) {
            if(result->func_type[0] == tDOUBLE && a.type == tINTEGER) {
                a.type = tDOUBLE;
                a.data.dvar = (double)a.data.ivar;       
            }
            else if(result->func_type[0] != a.type) {
                fprintf(stderr, "Error: function return uncompatibility type\n");
                return SEM_UNCOMP_ERROR;
            }

            if( result != NULL && (err = set_variable(storage, result, a))) {
                return err;
            }
        }
         if(temp.type == tSTRING){
         	free(a.data.svar);
         }
    }
    return NO_ERROR;   
}

/*
 * BUILT-IN FUNCTIONS
 */
int ifj16_print(storage_t* storage) {

    int err = NO_ERROR;
    var_t a;

    node_data_t T0 = {.struct_type = 'l'};
    T0.func_type = GBC_malloc(sizeof(char)*2);
    T0.func_type[0] = tEMPTY;
    T0.func_type[1] = '\0';
    node_data_t temp = {.struct_type = 'g', .inicializate = 1, .func_type = "s"};
    temp.data_u.svar = GBC_malloc(sizeof(char));
    temp.data_u.svar[0] = '\0';

    storage_push_temp(storage);

    if(storage->Local->Count == 0) {
        fprintf(stderr, "Error: too few arguments to built-in function 'print'\n");
        return SEM_UNCOMP_ERROR;
    }

    // Konkatenance
    for( int i = 0; i < storage->Local->Count; i++) {
        T0.func_type[0] = tEMPTY;
        T0.offset = i;
        if((err = get_variable(storage, &T0, &a))) {
            return err;
        }
        T0.func_type[0] = a.type;
        if((err = ins_add(storage, &temp, &T0, &temp))){
            return err;
        }
    }
    // Tisk
    printf("%s", temp.data_u.svar);
    // Uvolneni pomocnych promenych
    GBC_free(T0.func_type);
    GBC_free(temp.data_u.svar);
    // Odstraneni rámce
    storage_pop(storage);
    return NO_ERROR;
}

int ifj16_readInt(storage_t* storage, node_data_t* result) {
    int err = NO_ERROR;
    var_t c;
    
    storage_push_temp(storage);

    if(storage->Local->Count != 0) {
        fprintf(stderr, "Error: too many arguments to built-in function 'ifj16.readInt'\n");
        return SEM_UNCOMP_ERROR;
    }

    c.type = tINTEGER;
    c.data.ivar = readInt();
    
    if(error_number != NO_ERROR) {
    	fprintf(stderr, "Error: wrong input\n");
        return error_number;
    }
    storage_pop(storage);

    if( result != NULL) {
        if((err = set_variable(storage, result, c))) {
            return err;
        }
    }
    return NO_ERROR;
}
int ifj16_readDouble(storage_t* storage, node_data_t* result) {
    int err = NO_ERROR;
    var_t c;
    
    storage_push_temp(storage);

    if(storage->Local->Count != 0) {
        fprintf(stderr, "Error: too many arguments to built-in function 'ifj16.readDouble'\n");
        return SEM_UNCOMP_ERROR;
    }

    c.type = tDOUBLE;
    c.data.dvar = readDouble();
    
    if(error_number != NO_ERROR) {
        return error_number;
    }
    storage_pop(storage);

    if( result != NULL) {
        if((err = set_variable(storage, result, c))) {
            return err;
        }
    }
    return NO_ERROR;
}
int ifj16_readString(storage_t* storage, node_data_t* result) {
    int err = NO_ERROR;
    var_t c;

    storage_push_temp(storage);

    if(storage->Local->Count != 0) {
        fprintf(stderr, "Error: too many arguments to built-in function 'ifj16.readString'\n");
        return SEM_UNCOMP_ERROR;
    }

    c.type = tSTRING;
    c.data.svar = readString();

    if(error_number != NO_ERROR) {
        return error_number;
    }

    storage_pop(storage);

    if( result != NULL) {
        if((err = set_variable(storage, result, c))) {
            return err;
        }
    }
    free(c.data.svar);
    return NO_ERROR;
}

int ifj16_find(storage_t* storage, node_data_t* result) {
    int err = NO_ERROR;
    var_t a;
    var_t b;
    var_t c;

    node_data_t op1 = {.struct_type = 'l', .func_type = "s", .offset = 0};
    node_data_t op2 = {.struct_type = 'l', .func_type = "s", .offset = 1};

    storage_push_temp(storage);

    if(storage->Local->Count != 2) {
        if(storage->Local->Count < 2) {
            fprintf(stderr, "Error: too few arguments to built-in function 'ifj16.find'\n");
        }
        else {
            fprintf(stderr, "Error: too many arguments to built-in function 'ifj16.find'\n");
        }
        return SEM_UNCOMP_ERROR;
    }

    if( (err = get_variable(storage, &op1, &a)) || (err = get_variable(storage, &op2, &b)) ) {
        return err;
    }

    c.type = tINTEGER;
    c.data.ivar = find(a.data.svar, b.data.svar);

    storage_pop(storage);

    if( result != NULL) {
        if((err = set_variable(storage, result, c))) {
            return err;
        }
    }
    return NO_ERROR;
}

int ifj16_sort(storage_t* storage, node_data_t* result) {
    int err = NO_ERROR;
    var_t a;
    var_t c;

    node_data_t op1 = {.struct_type = 'l', .func_type = "s", .offset = 0};

    storage_push_temp(storage);
    
    if(storage->Local->Count != 1) {
        if(storage->Local->Count < 1) {
            fprintf(stderr, "Error: too few arguments to built-in function 'ifj16.sort'\n");
        }
        else {
            fprintf(stderr, "Error: too many arguments to built-in function 'ifj16.sort'\n");
        }
        return SEM_UNCOMP_ERROR;
    }

    if( (err = get_variable(storage, &op1, &a))) {
        return err;
    }

    c.type = tSTRING;
    c.data.svar = sort(a.data.svar);

    storage_pop(storage);

    if( result != NULL) {
        if((err = set_variable(storage, result, c))) {
            return err;
        }
    }
    GBC_free(c.data.svar); // TODO nechci to tu
    return NO_ERROR;
}

int ifj16_compare(storage_t* storage, node_data_t* result) {
    int err = NO_ERROR;
    var_t a;
    var_t b;
    var_t c;

    node_data_t op1 = {.struct_type = 'l', .func_type = "s", .offset = 0};
    node_data_t op2 = {.struct_type = 'l', .func_type = "s", .offset = 1};

    storage_push_temp(storage);

    if(storage->Local->Count != 2) {
        if(storage->Local->Count < 2) {
            fprintf(stderr, "Error: too few arguments to built-in function 'ifj16.compare'\n");
        }
        else {
            fprintf(stderr, "Error: too many arguments to built-in function 'ifj16.compare'\n");
        }
        return SEM_UNCOMP_ERROR;
    }

    if( (err = get_variable(storage, &op1, &a)) || (err = get_variable(storage, &op2, &b))) {
        return err;
    }

    c.type = tINTEGER;
    c.data.ivar = compare(a.data.svar, b.data.svar);

    storage_pop(storage);

    if( result != NULL) {
        if((err = set_variable(storage, result, c))) {
            return err;
        }
    }
    return NO_ERROR;
}

int ifj16_length(storage_t* storage, node_data_t* result) {
    int err = NO_ERROR;
    var_t a;
    var_t c;

    node_data_t op1 = {.struct_type = 'l', .func_type = "s", .offset = 0};

    storage_push_temp(storage);

    if(storage->Local->Count != 1) {
        if(storage->Local->Count < 1) {
            fprintf(stderr, "Error: too few arguments to built-in function 'ifj16.lenght'\n");
        }
        else {
            fprintf(stderr, "Error: too many arguments to built-in function 'ifj16.lenght'\n");
        }
        return SEM_UNCOMP_ERROR;
    }

    if( (err = get_variable(storage, &op1, &a))) {
        return err;
    }

    c.type = tINTEGER;
    c.data.ivar = length(a.data.svar);

    storage_pop(storage);

    if( result != NULL) {
        if((err = set_variable(storage, result, c))) {
            return err;
        }
    }
    return NO_ERROR;
}

int ifj16_substr(storage_t* storage, node_data_t* result) {
    int err = NO_ERROR;
    var_t s;
    var_t i;
    var_t n;
    var_t c;

    node_data_t op1 = {.struct_type = 'l', .func_type = "s", .offset = 0};
    node_data_t op2 = {.struct_type = 'l', .func_type = "i", .offset = 1};
    node_data_t op3 = {.struct_type = 'l', .func_type = "i", .offset = 2};

    storage_push_temp(storage);

    if(storage->Local->Count != 3) {
        if(storage->Local->Count < 3) {
            fprintf(stderr, "Error: too few arguments to built-in function 'ifj16.substr'\n");
        }
        else {
            fprintf(stderr, "Error: too many arguments to built-in function 'ifj16.substr'\n");
        }
        return SEM_UNCOMP_ERROR;
    }

    if( (err = get_variable(storage, &op1, &s)) || (err = get_variable(storage, &op2, &i)) || (err = get_variable(storage, &op3, &n))) {
        return err;
    }

    c.type = tSTRING;
    c.data.svar = substr(s.data.svar, i.data.ivar, n.data.ivar);

    if(error_number != NO_ERROR) {
    	return error_number;
    }

    storage_pop(storage);

    if( result != NULL) {
        if((err = set_variable(storage, result, c))) {
            return err;
        }
    }
    free(c.data.svar);
    return NO_ERROR;
}

int interpret_run(ilist_t* ilist) {
    instruction_t ins;
    storage_t storage;

    if(error_number != NO_ERROR) {
        return error_number;
    }

    int err = NO_ERROR;
    storage_init(&storage);

    ilist_first(ilist);
    while(err == NO_ERROR && ilist_active(ilist)) {
        ilist_copy(ilist, &ins);

        // TODO !!!! VSE ZDE BUDOU FUNKCE !!!! 
        // POUZE TEST

        switch(ins.type) {
            case INS_LAB:
                break;
            case INS_ST:
                err = ins_store(&storage, ins.op1, ins.result);
                break;
            case INS_SUB:
                err = ins_sub(&storage, (node_data_t*)ins.op1, (node_data_t*)ins.op2, (node_data_t*)ins.result );
                break;
            case INS_ADD:
                err = ins_add(&storage, (node_data_t*)ins.op1, (node_data_t*)ins.op2, (node_data_t*)ins.result );
                break;
            case INS_MUL:
                err = ins_mul(&storage, (node_data_t*)ins.op1, (node_data_t*)ins.op2, (node_data_t*)ins.result );
                break;
            case INS_DIV:
                err = ins_div(&storage, (node_data_t*)ins.op1, (node_data_t*)ins.op2, (node_data_t*)ins.result );
                break;
            case INS_LESS:
                err = ins_less(&storage, (node_data_t*)ins.op1, (node_data_t*)ins.op2, (node_data_t*)ins.result );
                break;
            case INS_GREAT:
                err = ins_great(&storage, (node_data_t*)ins.op1, (node_data_t*)ins.op2, (node_data_t*)ins.result );
                break;
            case INS_ELESS:
                err = ins_eless(&storage, (node_data_t*)ins.op1, (node_data_t*)ins.op2, (node_data_t*)ins.result );
                break;
            case INS_EGREAT:
                err = ins_egreat(&storage, (node_data_t*)ins.op1, (node_data_t*)ins.op2, (node_data_t*)ins.result );
                break;
            case INS_EQUAL:
                err = ins_equal(&storage, (node_data_t*)ins.op1, (node_data_t*)ins.op2, (node_data_t*)ins.result );
                break;
            case INS_NOT:
                err = ins_not(&storage, (node_data_t*)ins.op1, (node_data_t*)ins.op2, (node_data_t*)ins.result );
                break;
            case JMP:
                err = jmp(ilist, ins.result);
                break;
            case JMPZ:
                err = jmp_zero(&storage, ilist, ins.op1, ins.result);
                break;
            case PUSH_PARAM:
                err = push_param(&storage, (node_data_t*)ins.op1);
                break;
            case BEGIN_FUNC:
                frame_reserve(storage.Local, ((node_data_t*)ins.op1)->data_u.ivar);
                break;
            case END_FUNC:
                err = end_func(&storage, ilist, (node_data_t*)ins.op1);
                break;
            case CALL:
                err = call(&storage, ilist, (node_data_t*)ins.op1, (node_data_t*)ins.result );
                break;
            case RET:
                err = ret(&storage, ilist, (node_data_t*)ins.op1);
                break;
            case PRINT:
                err = ifj16_print(&storage);
                break;
            case EXIT:
                ilist_disactive(ilist);
                break;
            case READ_INT:
                err = ifj16_readInt(&storage, (node_data_t*)ins.result);
                break;
            case READ_DOUBLE:
                err = ifj16_readDouble(&storage, (node_data_t*)ins.result);
                break;
            case READ_STRING:
                err = ifj16_readString(&storage, (node_data_t*)ins.result);
                break;
            case LENGTH:
                err = ifj16_length(&storage, (node_data_t*)ins.result);
                break;
            case SUBSTR:
                err = ifj16_substr(&storage, (node_data_t*)ins.result);
                break;
            case COMPARE:
                err = ifj16_compare(&storage, (node_data_t*)ins.result);
                break;
            case FIND:
                err = ifj16_find(&storage, (node_data_t*)ins.result);
                break;
            case SORT:
                err = ifj16_sort(&storage, (node_data_t*)ins.result);
                break;
            default:
                fprintf(stderr, "Error: Unknown operation\n");
                return INTERN_ERROR;
                break;
        }
        ilist_succ(ilist);
    }
    set_error(err);
    storage_disponse(&storage);
    return err;
}