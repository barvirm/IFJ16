/*
 * Name:    semantic.c
 * Author:  Karel ONDŘEJ, FIT
 * Email:   xondre09@stud.fit.vutbr.cz
 * Login    xondre09
 * Date:    6. 10. 2016
 * Team members:  Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 *
 * Description: 
 * ...
 */

#include "sematic.h"
#define type(op, t) ((op)->func_type != NULL && (op)->func_type[0] == t && (op)->func_type[1] == '\0')

#define pretypovani(op1, op2) (type(op1, tDOUBLE) || type(op2, tDOUBLE))

int expr = 0;


/*
 * Sematic check for add or cat operation.
 */
int operation_add_or_cat(int expr, node_data_t* op1, node_data_t* op2, node_data_t* result) {
    char ret;

    // if initialization instruction list and don't inilialization static variable
    if(expr && ((op1->struct_type == 'g' && !op1->inicializate) || ( op2->struct_type == 'g' && op2->inicializate))) {
        return SEM_OTHER_ERROR;
    }
    //  operand type check
    if(type(op1, tSTRING) || type(op2, tSTRING)) {
        ret = tSTRING;
    }
    else if(type(op1, tDOUBLE) || type(op2, tDOUBLE)) {
        ret = tDOUBLE;
    }
    else {
        ret = tINTEGER;
    }

    // result type check
    if(type(result, tEMPTY)) {
        result->func_type[0] = ret;
    }
    else if(!(type(result, ret) || (type(result, tDOUBLE) && ret == tINTEGER))) {
        fprintf(stderr, "%s\n", "Sematic error: Addition");
        return SEM_UNCOMP_ERROR;
    }
    return NO_ERROR;
}

/*
 * Semtic check for aritmetic oparation
 */
int aritmetic_operation(int expr, node_data_t* op1, node_data_t* op2, node_data_t* result) {
    char ret;

    // if initialization instruction list and don't inilialization static variable
    if(expr && ((op1->struct_type == 'g' && !op1->inicializate) || ( op2->struct_type == 'g' && op2->inicializate))) {
        return SEM_OTHER_ERROR;
    }

    //  operand type check
    if(type(op1, tSTRING) || type(op2, tSTRING)) {
        fprintf(stderr, "%s\n", "Sematic error: unknown aritmetic operation with String");
        return SEM_UNCOMP_ERROR;
    }
    else if(type(op1, tDOUBLE) || type(op2, tDOUBLE)) {
        ret = tDOUBLE;
    }
    else {
        ret = tINTEGER;
    }

    if(type(result, tEMPTY)) {
        result->func_type[0] = ret;
    }
    else if(!(type(result, ret) || (type(result, tDOUBLE) && ret == tINTEGER))) {
        fprintf(stderr, "Sematic error: aritmetic\n");
        return SEM_UNCOMP_ERROR;
    }
    return NO_ERROR;
}
/*
 * Semtic check for relation oparation
 */
int relation_operation(int expr, node_data_t* op1, node_data_t* op2, node_data_t* result) {
    char ret;

    if(expr && ((op1->struct_type == 'g' && !op1->inicializate) || ( op2->struct_type == 'g' && op2->inicializate))) {
        return SEM_OTHER_ERROR;
    }

    //  operand type check
    if(type(op1, tSTRING) || type(op2, tSTRING)) {
        fprintf(stderr, "Sematic error: unknown relation operation with String\n");
        return SEM_UNCOMP_ERROR;
    }
    else {
        ret = tINTEGER;
    }

    if(type(result, tEMPTY)) {
        result->func_type[0] = tINTEGER;
    }
    else if(!(type(result, ret) || (type(result, tDOUBLE) && ret == tINTEGER))) {
        return SEM_UNCOMP_ERROR;
    }
    return NO_ERROR;
}

/*
 * Check number and type paramets of function
 */
int corect_parametrs(char* reference, char* str, char* Merr) {
    int err = NO_ERROR;
    if(str[0] != tVOID && reference[0] == tVOID) {
        fprintf(stderr, "%s\n", "Sematic error: assignment void function");
        return RUNTIME_UNINIT_ERROR;
    }

    if( str[0] == tEMPTY) {
        fprintf(stderr, "%s\n", str);
        *str = reference[0];
        fprintf(stderr, "%s\n", str);
    }
    if(str[0] != tVOID && str[0] != reference[0] && !(str[0] == tDOUBLE && reference[0] == tINTEGER)) {
        fprintf(stderr, "Sematic error: wrong type of return value function %s = %s\n", str, reference);
        return SEM_UNCOMP_ERROR;
    } 
    int i = 1;
    for(; str[i] != '\0' && reference[i] != '\0'; i++) {
        if( str[i] != reference[i] && !(reference[i] == tDOUBLE && str[i] == tINTEGER)) {
            fprintf(stderr, "Sematic error: wrong type of parametrs to %s\n", Merr);
            err = SEM_UNCOMP_ERROR;
            break;
        }
    }
    if( str[i] == '\0' && reference[i] != '\0') {
        fprintf(stderr, "Sematic error: too many arguments to %s\n", Merr);
    }
    else if( str[i] != '\0' && reference[i] == '\0') {
        fprintf(stderr, "Sematic error: too few arguments to %s\n", Merr);
    }
    return err;
}

/*
 * Check assigment operation
 */
int store(int expr, node_data_t* op1, node_data_t* result) {
    if(expr && (op1->struct_type == 'g' && !op1->inicializate)) {
        return SEM_OTHER_ERROR;
    }

    if(type(result, tEMPTY)) {
        result->func_type[0] = op1->func_type[0];
    }
    else if(!(type(result, op1->func_type[0]) || (type(result, tDOUBLE) && op1->func_type[0] == tINTEGER))) {
        fprintf(stderr, "Sematic error: store %s = %s\n", result->func_type, op1->func_type);
        return SEM_UNCOMP_ERROR;
    }
    return NO_ERROR;
}

/*
 * Set correct type of variable/function
 */
void set_return(node_data_t* op1, char result) {
    if( op1 != NULL && op1->func_type[0] == tEMPTY ) {
       op1->func_type[0] = result;
    }
}

/*
 * Check correct return type of function
 */
int corect_return_type(node_data_t* op1, node_data_t* function) {
    if(!(function->func_type[0] == tVOID && op1 == NULL) && (function->func_type[0] != op1->func_type[0])) {
        return SEM_UNCOMP_ERROR;
    }
    return NO_ERROR;
}

/*
 * Check function with paramets
 */
int function_with_parametrs(ilist_t* ilist) {
    instruction_t ins;

    int err = NO_ERROR;
    int stop = 0;
    char buffer[256];
    buffer[0] = tEMPTY;
    int i = 1;
    while(stop == 0 && ilist_active(ilist)) {
        ilist_copy(ilist, &ins);
        switch(ins.type) {
            case PUSH_PARAM:
                buffer[i++] = ((node_data_t*)ins.op1)->func_type[0];
                buffer[i] = '\0';
                break;
            case CALL:
                set_return(ins.result, ((node_data_t*)ins.op1)->func_type[0]);
                buffer[0] = (ins.result != NULL ? ((node_data_t*)ins.result)->func_type[0] : tVOID);
                err = corect_parametrs(((node_data_t*)ins.op1)->func_type, buffer, "function");
                stop = 1;
                break;
            case READ_INT:
                fprintf(stderr, "Sematic error: too many arguments to built-in function 'ifj16.readInt\n");
                err = SEM_UNCOMP_ERROR;
                stop = 1;
                break;
            case READ_DOUBLE:
                fprintf(stderr, "Sematic error: too many arguments to built-in function 'ifj16.readDouble\n");
                err = SEM_UNCOMP_ERROR;
                stop = 1;
                break;
            case READ_STRING:
                fprintf(stderr, "Sematic error: too many arguments to built-in function 'ifj16.readString\n");
                err = SEM_UNCOMP_ERROR;
                stop = 1;
                break;
            case PRINT:
                if(ins.result != NULL) {
                    fprintf(stderr, "%s\n", "print");
                    err = SEM_UNCOMP_ERROR;
                }
                stop = 1;
                break;
            case LENGTH:
                set_return(ins.result, 'i');
                buffer[0] = (ins.result != NULL ? ((node_data_t*)ins.result)->func_type[0] : tVOID);
                err = corect_parametrs("is", buffer, "build-in function 'ifj16.lenght'");
                stop = 1;
                break;
            case SUBSTR:
                set_return(ins.result, 's');
                buffer[0] = (ins.result != NULL ? ((node_data_t*)ins.result)->func_type[0] : tVOID);
                err = corect_parametrs("ssii", buffer, "build-in function 'ifj16.substr'");
                stop = 1;
                break;
            case COMPARE:
                set_return(ins.result, 'i');
                buffer[0] = (ins.result != NULL ? ((node_data_t*)ins.result)->func_type[0] : tVOID);
                err = corect_parametrs("iss", buffer, "build-in function 'ifj16.compare'");
                stop = 1;
                break;
            case FIND:
                set_return(ins.result, 'i');
                buffer[0] = (ins.result != NULL ? ((node_data_t*)ins.result)->func_type[0] : tVOID);
                err = corect_parametrs("iss", buffer, "build-in function 'ifj16.find'");
                stop = 1;
                break;
            case SORT:
                set_return(ins.result, 's');
                buffer[0] = (ins.result != NULL ? ((node_data_t*)ins.result)->func_type[0] : tVOID);
                err = corect_parametrs("ss", buffer, "build-in function 'ifj16.sort'");
                stop = 1;
                break;
            default:
                err = INTERN_ERROR;
                stop = 1;
                break;
        }
        if(stop == 0) {
            ilist_succ(ilist);
        }
    }
    return err;

}

/*
 * Run sematic check
 */
int sematic_run(ilist_t* ilist) {
    instruction_t ins;
    expr = 0;
    if(error_number != NO_ERROR) {
        return error_number;
    }

    node_data_t* function = NULL;

    int err = NO_ERROR;
    ilist_first(ilist);
    while(err == NO_ERROR && ilist_active(ilist)) {
        ilist_copy(ilist, &ins);

        switch(ins.type) {
            case INS_ADD:
                err = operation_add_or_cat(0, (node_data_t*)ins.op1, (node_data_t*)ins.op2, (node_data_t*)ins.result );
                break;
            case INS_SUB:
            case INS_MUL:
            case INS_DIV:
                err = aritmetic_operation(0, (node_data_t*)ins.op1, (node_data_t*)ins.op2, (node_data_t*)ins.result );
                break;
            case INS_LESS:
            case INS_GREAT:
            case INS_ELESS:
            case INS_EGREAT:
            case INS_EQUAL:
            case INS_NOT:
                err = relation_operation(0, (node_data_t*)ins.op1, (node_data_t*)ins.op2, (node_data_t*)ins.result);
                break;
            case PUSH_PARAM:
                err = function_with_parametrs(ilist);
                break;
            case INS_ST:
                err = store(0, (node_data_t*)ins.op1, (node_data_t*)ins.result);
                break;
            case CALL:
                if(((node_data_t*)ins.result) != NULL) {
                    if(( (node_data_t*)ins.op1)->func_type[0] == tEMPTY ) {
                       ((node_data_t*)ins.op1)->func_type[0] = ((node_data_t*)ins.result)->func_type[0];
                    }
                    err = corect_parametrs(((node_data_t*)ins.op1)->func_type, ((node_data_t*)ins.result)->func_type, "function");
                }
                break;
            case READ_INT:
                err = corect_parametrs("i", ((node_data_t*)ins.result)->func_type, "build-in function 'ifj16.readInt'");
                break;
            case READ_DOUBLE:
                err = corect_parametrs("d", ((node_data_t*)ins.result)->func_type, "build-in function 'ifj16.readDouble'");
                break;
            case READ_STRING:
                err = corect_parametrs("s", ((node_data_t*)ins.result)->func_type, "build-in function 'ifj16.readString'");
                break;
            case PRINT:
                fprintf(stderr, "Sematic error: too few arguments to built-in function 'ifj16.print\n");
                err = SEM_UNCOMP_ERROR;
                break;
            case LENGTH:
                fprintf(stderr, "Sematic error: too few arguments to built-in function 'ifj16.lenght\n");
                err = SEM_UNCOMP_ERROR;
                break;
            case SUBSTR:
                fprintf(stderr, "Sematic error: too few arguments to built-in function 'ifj16.substr\n");
                err = SEM_UNCOMP_ERROR;
                break;
            case COMPARE:
                fprintf(stderr, "Sematic error: too few arguments to built-in function 'ifj16.compare\n");
                err = SEM_UNCOMP_ERROR;
                break;
            case FIND:
                fprintf(stderr, "Sematic error: too few arguments to built-in function 'ifj16.find\n");
                err = SEM_UNCOMP_ERROR;
                break;
            case SORT:
                fprintf(stderr, "Sematic error: too few arguments to built-in function 'ifj16.sort\n");
                err = SEM_UNCOMP_ERROR;
                break;
            case INS_LAB:
                if( (node_data_t*)ins.op1 != NULL) {
                    function = (node_data_t*)ins.op1;
                }
                break;
            case RET:
                err = corect_return_type((node_data_t*)ins.op1, function);
                break;
            case BEGIN_FUNC:
            case END_FUNC:
            case EXIT:
            case JMP:
            case JMPZ:
                break;
            default:
                err = INTERN_ERROR;
                break;
        }

        ilist_succ(ilist);
    }
    set_error(err);
    return err;
}

/*
 * run sematic check of initialization instructon list
 */
int sematic_expression(ilist_t* ilist) {
    instruction_t ins;

    expr = 1;
    if(error_number != NO_ERROR) {
        return error_number;
    }

    int err = NO_ERROR;
    ilist_first(ilist);
    while(err == NO_ERROR && ilist_active(ilist)) {
        ilist_copy(ilist, &ins);

        switch(ins.type) {
            case INS_ADD:
                err = operation_add_or_cat(1, (node_data_t*)ins.op1, (node_data_t*)ins.op2, (node_data_t*)ins.result );
                break;
            case INS_SUB:
            case INS_MUL:
            case INS_DIV:
                err = aritmetic_operation(1, (node_data_t*)ins.op1, (node_data_t*)ins.op2, (node_data_t*)ins.result );
                break;
            case INS_LESS:
            case INS_GREAT:
            case INS_ELESS:
            case INS_EGREAT:
            case INS_EQUAL:
            case INS_NOT:
                err = relation_operation(1, (node_data_t*)ins.op1, (node_data_t*)ins.op2, (node_data_t*)ins.result);
                break;
            case INS_ST:
                err = store(1, (node_data_t*)ins.op1, (node_data_t*)ins.result);
                break;
            case PUSH_PARAM:
            case CALL:
            case READ_INT:
            case READ_DOUBLE:
            case READ_STRING:
            case PRINT:
            case LENGTH:
            case SUBSTR:
            case COMPARE:
            case FIND:
            case SORT:
                err = SYNTAX_ERROR;
                break;
            case RET:
            case BEGIN_FUNC:
            case END_FUNC:
            case EXIT:
            case INS_LAB:
            case JMP:
            case JMPZ:
            default:
                err = INTERN_ERROR;
                break;
        }

        ilist_succ(ilist);
    }
    set_error(err);
    return err;
}