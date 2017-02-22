/* 
 * Name:    postfix.c
 * Author:  Jakub Sochůrek, David Smékal, Karel Onřej
 * Email:   xsochu01@stud.fit.vutbr.cz, xsmeka13@...
 * Login:   xsochu01, xsmeka13
 * Date:    10/10/2016
 * Last modificated: 11/11/16
 * Team members:    Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 * 
 * Description: File includes converter to postfix notation, 
 *              from postfix to 3AK code generator.
*/

#include "postfix.h"

extern token_t *error_token;
extern node_t *global_table;
extern node_data_t *counter;
extern ilist_t* instruction_list;
extern char *class_id;

// ************  Stack functions declaration ******************

void stack_push_token(stack_t *S, token_t *token)
{
    stackItem* new_item = (stackItem*)GBC_malloc(sizeof(stackItem));
    token_t *new_token = token_ctor();

    if( new_item == NULL) { set_error(INTERN_ERROR); }

    if ( new_token == NULL ) { set_error(INTERN_ERROR); }

    if (token->type == SEMI){
        new_token->type = SEMI;
    } else {
        if (token->type == INTEGER){
            new_token->type = INTEGER;
            new_token->data.ivar = token->data.ivar;
        } else if (token->type == DOUBLE){
            new_token->type = DOUBLE;
            new_token->data.dvar = token->data.dvar;
        } else if (token->type == STRING || token->type == SIMPLE_ID ||
                   token->type == FULLY_QUALIFIED_ID){
            token_set_string(new_token, token->data.svar, token->type);
        } else {
            new_token->type = token->type;
        }
    }

    new_item->data = new_token;
    new_item->lessthan = false;
    new_item->isExpression = false;
    new_item->next = S->First;

    S->First = new_item;
    S->Top = new_item;
}

void stack_pop(stack_t* S)
{
    if( S->First != NULL) {
        stackItem* del = S->First;
        S->First = S->First->next;
        GBC_free(del->data);
        GBC_free(del);
    }
}

stackItem* stack_top(stack_t* S)
{
    return S->Top;
}

void stack_init(stack_t* S)
{
    S->First = NULL;
    S->Top = NULL;
}

void stack_delete(stack_t* S)
{
    while( S->First != NULL) { stack_pop(S); }
}

void stack_push(stack_t *S, stackItem *I){
    I->next = S->First;
    S->First = I;
    S->Top = I;
}

void stack_change(stack_t *S){
    stackItem* top = stack_top(S);
    top->lessthan = true;
}

// ************************************************************

// ********* Double linked list functions declaration *********

void list_init(list_t *L){
    L->First = NULL;
    L->Current = NULL;
    L->Last = NULL;
}

void list_activate(list_t *L){
    if (L->Current == NULL){}
        L->Current = L->First;
}

void list_delete(list_t *L){
    while (L->First != NULL){
        listItem *next = L->First->next;
        GBC_free(L->First);
        L->First = next;
    }
    L->Last = L->First;
    L->Current = L->First;
}

void list_succ(list_t* L) {
    if ( L->Current != NULL) { L->Current = L->Current->next; }
}

void list_actualize(list_t* L, token_t T) {
    if (L->Current != NULL) {
        // clean_token(&(L->Current->data));
        L->Current->data.type = T.type;
        if ( T.type == STRING || T.type == SIMPLE_ID || T.type == FULLY_QUALIFIED_ID) {
            L->Current->data.data.svar = GBC_malloc(sizeof(char) * (strlen(T.data.svar) +1));
            strcpy(L->Current->data.data.svar, T.data.svar);
        }
    }
}

void list_copy(list_t *L, token_t *T){
    if ( L->Current != NULL) {
        *T = L->Current->data;
    } else {
        T->type = EMPTY;
        T->data.svar = NULL;
    }
}

void list_copy_pre(list_t *L, token_t *T){
    if ( L->Current != NULL && L->Current->previous != NULL) {
        *T = L->Current->previous->data;
    } else {
        T->type = EMPTY;
        T->data.svar = NULL;
    }
}

void list_insert_pre(list_t *L, token_t T){
    if ( L->Current != NULL ) {
        listItem *item = (listItem*)GBC_malloc(sizeof(listItem));
        if (item == NULL){
            set_error(INTERN_ERROR);
        } else {
            item->data = T;
            item->next = L->Current;
            item->previous = L->Current->previous;
            if (item->previous != NULL) {
                item->previous->next = item;
            } else {
                L->First = item;
            }
            L->Current->previous = item;
        }
    }
}

void list_delete_pre(list_t *L) {
    if ( L->Current != NULL && L->Current->previous != NULL) {
        listItem *del = L->Current->previous;
        L->Current->previous = del->previous;
        if (L->Current->previous != NULL) {
            del->previous->next = L->Current;
        } else {
            L->First = L->Current;
        }
        GBC_free(del);
    }
}

void list_delete_last_item(list_t *L){
    listItem *pre_last = L->Last->previous;
    pre_last->next = NULL;
    listItem *del = L->Last;
    L->Last = pre_last;
    GBC_free(&del->data);
}

int insert_copy_of_token_last(list_t *L, token_t *T){
    listItem *item = (listItem*)GBC_malloc(sizeof(listItem));
    if (item == NULL){
        set_error(INTERN_ERROR);
        return -1;
    }

    item->data = *T;
    item->next = NULL;

    if (L->First == NULL){
        item->previous = NULL;
        L->First = item;
        L->Last = item;
    } else {
        item->previous = L->Last;
        L->Last->next = item;
        L->Last = L->Last->next;
    }

    return 1;
}

// ************************************************************

/* Picking rule for postfix conversion
 * Rules:
 *  1. E -> E+E or E-E
 *  2. E -> E*E or E/E
 *  3. E -> (E)
 *  4. E -> i
 *  5. E -> E<E or E>E or E<=E or E>=E or E!=E or E==E
 *  default: SYNTAX_ERROR
 */
int isRule(stack_t *S){
    stackItem *top = stack_top(S);
    int rule;
    switch (top->data->type){
        case LESS:
        case ELESS:
        case GREAT:
        case EGREAT:
        case NOT:
        case EQUAL:
            rule = 5;
            break;
        case INTEGER:
        case DOUBLE:
        case STRING:
        case SIMPLE_ID:
        case FULLY_QUALIFIED_ID:
            rule = 4;
            break;
        case BRACKET_RIGHT:
            rule = 3;
            break;
        case DIV:
        case MUL:
            rule = 2;
            break;
        case PLUS:
        case MINUS:
            rule = 1;
            break;
        default:
            rule = 0;       // Syntax error
            break;
    }
    return rule;
}

/* Returns count number of comparison in expression
*/
int includeComparison(list_t *postfixList){
    int i = 0;
    list_activate(postfixList);
    while (postfixList->Current != NULL){
        if (11 <= postfixList->Current->data.type &&
            postfixList->Current->data.type <= 16){
            i++;
        }
        list_succ(postfixList);
    }
    return i;
}

/* Precedential table of syntax analysis
    
    EMPTY,              ///< Empty
    INTEGER,            ///< Integer
    DOUBLE,             ///< Double
    STRING,             ///< String
    SIMPLE_ID,          ///< Simple identifier
    FULLY_QUALIFIED_ID, ///< Fully qualified identifier
*/

#define V 1     // >
#define R 0     // =
#define M -1    // <
#define N -2    // Error
#define F 7     // Function

const int p_table[t_size][t_size]={
//      1   2       5            9  10                  17       20
/*   EM IN DO ST SI FQ  =  *  /  +  -  < <=  > >= != ==  (  )  {  }  ;  , ---- input token
*/  { N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N}, // EM
    { N, N, N, N, N, N, N, V, V, V, V, V, V, V, V, V, V, N, V, N, N, V, N}, // IN
    { N, N, N, N, N, N, N, V, V, V, V, V, V, V, V, V, V, N, V, N, N, V, N}, // DO
    { N, N, N, N, N, N, N, V, V, V, V, V, V, V, V, V, V, N, V, N, N, V, N}, // ST
    { N, N, N, N, N, N, N, V, V, V, V, V, V, V, V, V, V, F, V, N, N, V, N}, // SI
    { N, N, N, N, N, N, N, V, V, V, V, V, V, V, V, V, V, F, V, N, N, V, N}, // FQ
    { N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N}, // =
    { N, M, M, M, M, M, N, V, V, V, V, V, V, V, V, V, V, M, V, N, N, V, N}, // *
    { N, M, M, M, M, M, N, V, V, V, V, V, V, V, V, V, V, M, V, N, N, V, N}, // /
    { N, M, M, M, M, M, N, M, M, V, V, V, V, V, V, V, V, M, V, N, N, V, N}, // +
    { N, M, M, M, M, M, N, M, M, V, V, V, V, V, V, V, V, M, V, N, N, V, N}, // -
    { N, M, M, M, M, M, N, M, M, M, M, R, R, R, R, V, V, M, V, N, N, V, N}, // <
    { N, M, M, M, M, M, N, M, M, M, M, R, R, R, R, V, V, M, V, N, N, V, N}, // <=
    { N, M, M, M, M, M, N, M, M, M, M, R, R, R, R, V, V, M, V, N, N, V, N}, // >
    { N, M, M, M, M, M, N, M, M, M, M, R, R, R, R, V, V, M, V, N, N, V, N}, // >=
    { N, M, M, M, M, M, N, M, M, M, M, M, M, M, M, R, R, M, V, N, N, V, N}, // !=
    { N, M, M, M, M, M, N, M, M, M, M, M, M, M, M, R, R, M, V, N, N, V, N}, // ==
    { N, M, M, M, M, M, N, M, M, M, M, M, M, M, M, M, M, M, R, N, N, N, N}, // (
    { N, N, N, N, N, N, N, V, V, V, V, V, V, V, V, V, V, N, V, N, N, V, N}, // )
    { N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N}, // {
    { N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N}, // }
    { N, M, M, M, M, M, N, M, M, M, M, M, M, M, M, M, M, M, N, N, N, N, N}, // ;
    { N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N, N}  // ,
};                                                                          // ||
                                                                            // terminal on stack

/* Default function caled by parser.
 * Function checks correct of expression and convert it to postfix, than
 * creates 3AK code from it and insert it to target instruction list.
 */
int processExpression(int condition, node_t **local_table, node_data_t **target){
    list_t postfixList;
    list_init(&postfixList);

    error_token =(token_t *) GBC_malloc(sizeof(token_t));
    if (error_token == NULL){
        set_error(INTERN_ERROR);
        return false;
    }
    init_token(error_token);
    error_token->line = 0;
    
    // call postfix converter
    int postfixOK = createPostfix(&postfixList, condition);

    // syntax error
    if (postfixOK == -1){
        set_error(SYNTAX_ERROR);
        #ifdef DEBUG_POSTFIX
        printf("syntax error\n");
        printf("error token type: %d\n", error_token->type);
        printf("error token line: %d\n", error_token->line);
        #endif
        list_delete(&postfixList);
        return false;
    // intern error
    } else if (postfixOK == -2){
        set_error(INTERN_ERROR);
        #ifdef DEBUG_POSTFIX
        printf("Intern error\n");
        #endif
        list_delete(&postfixList);
        return false;
    // semantic error
    } else if (postfixOK == -3){
        set_error(SEM_UNCOMP_ERROR);
        #ifdef DEBUG_POSTFIX
        printf("semantic error\n");
        #endif
        list_delete(&postfixList);
        return -2;
    // lexical error
    } else if (postfixOK ==  -4){
        set_error(LEX_ERROR);
        #ifdef DEBUG_POSTFIX
        printf("lexical error\n");
        #endif
        list_delete(&postfixList);
        return false;
    } else if (postfixOK ==  -5){
        set_error(SEM_UNDEF_ERROR);
        list_delete(&postfixList);
        return false;
    // function
    } else if (postfixOK == 5){
        #ifdef DEBUG_POSTFIX
        printf("nazev funkce: >%s<\n", postfixList.First->data.data.svar);
        #endif
        int f3AK = function3AK(&postfixList, local_table, target);
        if (f3AK == -1){
            set_error(SEM_UNCOMP_ERROR);
            list_delete(&postfixList);
            return false;
        } else if (f3AK == -2){
            set_error(INTERN_ERROR);
            list_delete(&postfixList);
            return false;
        }
        list_delete(&postfixList);
        return true;
    // comparison
    } else if (postfixOK == 6){
        #ifdef DEBUG_POSTFIX
        printf("je to porovnavani tu\n");
        #endif

        if (includeComparison(&postfixList) != 1){
            #ifdef DEBUG_POSTFIX
            printf("je to spatne porovnani\n");
            #endif
            set_error(SEM_UNCOMP_ERROR );
            list_delete(&postfixList);
            return false;
        }
    // empty semicolon
    } else if (postfixOK == 9){
        list_delete(&postfixList);
        return 2;
    }

    // call 3AK code generator
    int c3AK = create3AK(&postfixList, local_table, target);

    if (c3AK == -1){
        set_error(SEM_UNCOMP_ERROR);
        list_delete(&postfixList);
        return false;
    } else if (c3AK == -2){
        set_error(INTERN_ERROR);
        list_delete(&postfixList);
        return false;
    }

    list_delete(&postfixList);
    return true;
}

/* Main function checking and converting expression to 
 * postfix notation or function calling.
 * Allowed operations sorted by priority:
 * 3. * / 
 * 4. + -
 * 6. < > <= >=
 * 7. == != 
*/
int createPostfix(list_t *postfixList, int condition){
    
    #ifdef DEBUG_POSTFIX
    FILE *file;
    file = fopen("test1", "r");
    init_scanner(file);
    #endif
    stack_t stack;
    stack_init(&stack);
    token_t* end_token = token_ctor();
    token_set_type(end_token, SEMI);
    token_t* b = token_ctor();
    stackItem *top;
    int rule = 0;
    int syntaxError = false;
    int semanticError = false;
    int comparison = false;
    int left_bracket_counter = 0;

    stack_push_token(&stack, end_token);
    type_t a = stack_top(&stack)->data->type;
    if (get_token(b) != NO_ERROR){
        stack_delete(&stack);
        return -4;
    }
    int i = 0;
    // empty semicolon
    if (a == SEMI && b->type == SEMI){
        stack_delete(&stack);
        return 9;
    }
    while ((a != SEMI || b->type != SEMI) || 
        (condition && left_bracket_counter > 0 && (a != SEMI && b->type != BRACKET_RIGHT))){
        int precedenc = p_table[a][b->type];
        #ifdef DEBUG_POSTFIX
        // printf("have: %2d , read:%2d | ", a, b->type);
        // printf("line: %2d | ", b->line);
        // printf("%d\n", precedenc);
        #endif

        if (condition){
            if (b->type == BRACKET_LEFT){
                left_bracket_counter++;
            }
            // in condition expression can't be semicolon
            if (b->type == SEMI){
                syntaxError = true;
                error_token->type = b->type;
                error_token->line = b->line;
                break;
            }
        }
        // conmparison in expression and not condition is Error
        if (!condition && 11 <= a && a <= 16){
            stack_delete(&stack);
            return -3;
        }
        // if more than one comparison in condition expression is Error
        if (11 <= a && a <= 16 && ((b->type == SEMI) || b->type == BRACKET_RIGHT)){
            if (comparison == true){
                syntaxError = true;
                error_token->type = b->type;
                error_token->line = b->line;
                break;
            }
            else
                comparison = true;
        }
        // special case of calling function
        if (precedenc == F && stack.First->next->data->type == SEMI){
            insert_copy_of_token_last(postfixList, stack.First->data);
            int functionOK = checkFunctionSyntax(b, &stack, postfixList);
            if (functionOK == 1){
                stack_delete(&stack);
                return 5;
            } else if (functionOK == 0){
                stack_delete(&stack);
                return -1; // syntax error
            } else if (functionOK == -4){
                // lex error
                stack_delete(&stack);
                return functionOK;
            }
        }
        switch (precedenc){
            case R: // =
                stack_push_token(&stack, b);
                if (b->type == BRACKET_RIGHT)
                    stack_change(&stack);
                if (b->type != SEMI)
                    if (get_token(b) != NO_ERROR){
                        stack_delete(&stack);
                        return -4;
                    }
                break;
            case M: // <
                // if expression starts with operator is Error
                if (5 < b->type && b->type < 17 && stack.First->next == NULL && stack.First->data->type == SEMI){
                    syntaxError = true;
                    error_token->type = b->type;
                    error_token->line = b->line;
                    break;
                }
                stack_push_token(&stack, b);
                stack_change(&stack);
                if (b->type != SEMI){
                    if (get_token(b) != NO_ERROR){
                        stack_delete(&stack);
                        return -4;
                    }
                    if (stack_top(&stack) != NULL){
                        a = stack_top(&stack)->data->type;
                        i++;
                    } else {
                        syntaxError = true;
                        printf("%d\n", b->line);
                        error_token->type = a;
                        error_token->line = b->line;
                    }
                }
                continue;
            case V: // >
                top = stack_top(&stack);
                if (top->lessthan && (rule = isRule(&stack))){
                    // comparison
                    if (rule == 5){
                        stack_pop(&stack);
                        insert_copy_of_token_last(postfixList, stack.First->data);
                        stack_pop(&stack);
                        stack_pop(&stack);
                        stackItem *E = (stackItem*)GBC_malloc(sizeof(stackItem));
                        if (E == NULL){
                            return -2;
                        }
                        token_t *new_token = token_ctor();
                        E->data = new_token;
                        E->isExpression = true;
                        stack_push(&stack, E);
                        stack.Top = stack.First->next;
                    }
                    // variable or number
                    if (rule == 4){
                        top->isExpression = true;
                        top->lessthan = false;
                        insert_copy_of_token_last(postfixList, top->data);
                        stack.Top = stack.Top->next;
                        // kdyz je if vyraz s vice zavorkami tak ,kdyz se 
                        // aplikuje toto pravidlo dvakrat zapocitava
                        if (condition && left_bracket_counter != 0){
                            left_bracket_counter++;
                        } //*/
                    }
                    // brackets
                    if (rule == 3){
                        stack_pop(&stack);
                        stack_pop(&stack);
                        stack_pop(&stack);
                        stackItem *E = (stackItem*)GBC_malloc(sizeof(stackItem));
                        if (E == NULL){
                            return -2;
                        }
                        token_t *new_token = token_ctor();
                        E->data = new_token;
                        E->isExpression = true;
                        stack_push(&stack, E);
                        stack.Top = stack.First->next;
                    }
                    // multiplication and division
                    if (rule == 2){
                        stack_pop(&stack);
                        insert_copy_of_token_last(postfixList, stack.First->data);
                        stack_pop(&stack);
                        stack_pop(&stack);
                        stackItem *E = (stackItem*)GBC_malloc(sizeof(stackItem));
                        if (E == NULL){
                            return -2;
                        }
                        token_t *new_token = token_ctor();
                        E->data = new_token;
                        E->isExpression = true;
                        stack_push(&stack, E);
                        stack.Top = stack.First->next;
                        if (condition && left_bracket_counter != 0){
                            left_bracket_counter++;
                        } //*/
                    }
                    // addition and subtraction
                    if (rule == 1){
                        stack_pop(&stack);
                        insert_copy_of_token_last(postfixList, stack.First->data);
                        stack_pop(&stack);
                        stack_pop(&stack);
                        stackItem *E = (stackItem*)GBC_malloc(sizeof(stackItem));
                        if (E == NULL){
                            return -2;
                        }
                        token_t *new_token = token_ctor();
                        E->data = new_token;
                        E->isExpression = true;
                        stack_push(&stack, E);
                        stack.Top = stack.Top->next;
                        if (condition && left_bracket_counter != 0){
                            left_bracket_counter++;
                        } //*/
                    }
                }
                else{
                    syntaxError = true;
                    error_token->type = b->type;
                    error_token->line = b->line;
                }
                // printf(" => rule: %d\n", rule); // check used rule
                break;
            default:
                if (b->type == 3){
                    semanticError = true;
                }
                else{
                    syntaxError = true;
                }
                error_token->type = b->type;
                error_token->line = b->line;
                break;
        } // END OF SWITCH
        // check if expression ends with operator -> Error
        if (stack_top(&stack) != NULL){
            a = stack_top(&stack)->data->type;
            i++;
            if (condition && a == SEMI && b->type == BRACKET_RIGHT){
                break;
            }
        } else {
            syntaxError = true;
            error_token->type = a;
            error_token->line = b->line;
        }

        if (syntaxError || semanticError){
            break;
        }
    }
    #ifdef DEBUG_POSTFIX
    fclose(file);
    #endif
    stack_delete(&stack);

    if (syntaxError){
        return -1;
    } else if (semanticError){
        return -3;
    } else if (condition){
        return 6;
    } else {
        return 1;
    }
}

/* Check if expression is correct call of function
 */
int checkFunctionSyntax(token_t *b, stack_t *stack, list_t *postfixList){
    if (get_token(b) != NO_ERROR){
        return -4;
    }
    while (b->type != BRACKET_RIGHT){
        if (b->type != INTEGER && b->type != DOUBLE && b->type != STRING &&
            b->type != SIMPLE_ID && b->type != FULLY_QUALIFIED_ID &&
            b->type != COMMA)
            return 0;
        if (b->type != COMMA){
            stack_push_token(stack, b);
            insert_copy_of_token_last(postfixList, stack->First->data);
            stack_pop(stack);
        }
        if (get_token(b) != NO_ERROR){
            return -4;
        }
    }
    if (get_token(b) != NO_ERROR){
        return -4;
    }
    if (b->type == SEMI)
        return 1; // OK
    return 0; // Syntax Error
}


/* Token operations enum to ilist operations enum converter
 */
ins_type_t get_ilist_operator(type_t operator){
    switch (operator){
        case ASIGN:
            return INS_ST;
        case MUL:
            return INS_MUL;
        case DIV:
            return INS_DIV;
        case PLUS:
            return INS_ADD;
        case MINUS:
            return INS_SUB;
        case LESS:
            return INS_LESS;
        case ELESS:
            return INS_ELESS;
        case GREAT:
            return INS_GREAT;
        case EGREAT:
            return INS_EGREAT;
        case NOT:
            return INS_NOT;
        case EQUAL:
            return INS_EQUAL;
        default:
            return -1;  // error
    }
}

/* Creates 3AK code for function calling
 */
int function3AK(list_t *postfixList, node_t **local_table, node_data_t **target){

    #ifdef DEBUG_POSTFIX
    list_activate(postfixList);
    while (postfixList->Current != NULL){
        printf("function3AK: %d\n", postfixList->Current->data.type);
        postfixList->Current = postfixList->Current->next;
    }
    printf("\n");
    #endif
    
    list_activate(postfixList);
    char buffer[256];

    node_data_t* function = NULL;
    char* name = NULL;

    node_t* temp_node;

    while (postfixList->Current != NULL){
        buffer[0] = '\0';
        node_data_t* temp;
        token_t op;

        list_copy(postfixList, &op);

        if( op.type == SIMPLE_ID) {
            if (search_note(local_table, op.data.svar) == NULL){
                char* id = create_full_id(class_id, op.data.svar);
                temp_node = search_note(&global_table, id);
                if(temp_node == NULL) {
                    temp = (node_data_t*)GBC_malloc(sizeof(node_data_t));
                    init_table_data(temp);
                    temp->defined = false;
                    temp->struct_type = (name == NULL ? 'f' : 'g');
                    bin_tree_insert(&global_table, id, temp);
                }
                else if((temp_node->data->struct_type == 'g' &&  name == NULL) || (temp_node->data->struct_type == 'f' && name != NULL)) {
                    fprintf(stderr, "%c\n", temp_node->data->struct_type);
                    set_error(SEM_UNDEF_ERROR);
                    return -5;
                }
                GBC_free(op.data.svar);
                op.type = FULLY_QUALIFIED_ID;
                op.data.svar = id;
                list_actualize(postfixList, op);
            }
        }
        else if(op.type == FULLY_QUALIFIED_ID) {
            if (is_buildin_func(op.data.svar)) {
                name = op.data.svar;
                list_succ(postfixList);
                continue;
            }
            else if(search_note(&global_table, op.data.svar) == NULL) {
                temp = (node_data_t*)GBC_malloc(sizeof(node_data_t));
                init_table_data(temp);
                temp->defined = false;
                temp->struct_type = (name == NULL ? 'f' : 'g');
                bin_tree_insert(&global_table, op.data.svar, temp);
            }
        }
        else if(op.type == INTEGER) {
            int temp_i = op.data.ivar;
            sprintf(buffer, "%i", op.data.ivar);

            op.type = FULLY_QUALIFIED_ID;
            op.data.svar = (char*)GBC_malloc(sizeof(char) * (strlen(buffer) + 4));

            strcpy(op.data.svar, "&I_");
            strcat(op.data.svar, buffer);
            if(search_note(&global_table, op.data.svar) == NULL) {
                temp = (node_data_t*)GBC_malloc(sizeof(node_data_t));
                init_table_data(temp);
                data_add_type(temp, 'i');
                temp->defined = true;
                temp->struct_type = 'c';
                temp->data_u.ivar = temp_i;
                bin_tree_insert(&global_table, op.data.svar, temp);
            }
            
            list_actualize(postfixList, op);
        }
        else if(op.type == DOUBLE) {
            double temp_d = op.data.dvar;
            sprintf(buffer, "%.12g", op.data.dvar);

            op.type = FULLY_QUALIFIED_ID;
            op.data.svar = (char*)GBC_malloc(sizeof(char) * (strlen(buffer) + 4));

            strcpy(op.data.svar, "&D_");
            strcat(op.data.svar, buffer);
            if(search_note(&global_table, op.data.svar) == NULL) {

                temp = (node_data_t*)GBC_malloc(sizeof(node_data_t));
                init_table_data(temp);
                data_add_type(temp, 'd');
                temp->defined = true;
                temp->struct_type = 'c';
                temp->data_u.dvar = temp_d;
                bin_tree_insert(&global_table, op.data.svar, temp);
            }
            list_actualize(postfixList, op);
        }
        else if(op.type == STRING) {
            char* temp_s = op.data.svar;

            op.type = FULLY_QUALIFIED_ID;
            op.data.svar = (char*)GBC_malloc(sizeof(char) * (strlen(op.data.svar) + 4));

            strcpy(op.data.svar, "&S_");
            strcat(op.data.svar, temp_s);
            if(search_note(&global_table, op.data.svar) == NULL) {

                temp = (node_data_t*)GBC_malloc(sizeof(node_data_t));
                init_table_data(temp);
                data_add_type(temp, 's');
                temp->defined = true;
                temp->struct_type = 'c';
                temp->data_u.svar = temp_s;
                bin_tree_insert(&global_table, op.data.svar, temp);
            }
            else {
                GBC_free(temp_s);
            }
            list_actualize(postfixList, op);
        }
        else {
            set_error(INTERN_ERROR);
            return -2;
        }

        if(op.type == SIMPLE_ID){
            temp = search_note(local_table, op.data.svar)->data;
        }
        else if(!is_buildin_func(op.data.svar)){
            temp = search_note(&global_table, op.data.svar)->data;
        }

        if( name == NULL) {
            name = op.data.svar;
            function = temp;
        }
        else {
            generate_instruction(instruction_list, PUSH_PARAM, temp, NULL, NULL);
        }
        list_succ(postfixList);
    }

    if ( strcmp(name,"ifj16.print") == 0 ) {
        generate_instruction(instruction_list, PRINT, NULL, NULL, *target);
    }
    else if ( strcmp(name, "ifj16.readInt") == 0 ) {
        generate_instruction(instruction_list, READ_INT, NULL, NULL, *target);
    }
    else if ( strcmp(name, "ifj16.readString") == 0 ) {
        generate_instruction(instruction_list, READ_STRING, NULL, NULL, *target);
    }
    else if ( strcmp(name,"ifj16.readDouble") == 0 ) {
        generate_instruction(instruction_list, READ_DOUBLE, NULL, NULL, *target);
    }
    else if ( strcmp(name,"ifj16.length") == 0 ) {
        generate_instruction(instruction_list, LENGTH, NULL, NULL, *target);
    }
    else if ( strcmp(name,"ifj16.substr") == 0 ) {
        generate_instruction(instruction_list, SUBSTR, NULL, NULL, *target);
    }
    else if ( strcmp(name,"ifj16.compare") == 0 ) {
        generate_instruction(instruction_list, COMPARE, NULL, NULL, *target);
    }
    else if ( strcmp(name,"ifj16.find") == 0 ) {
        generate_instruction(instruction_list, FIND, NULL, NULL, *target);
    }
    else if ( strcmp(name,"ifj16.sort") == 0 ) {
        generate_instruction(instruction_list, SORT, NULL, NULL, *target);
    }
    else {
        generate_instruction(instruction_list, CALL, function, NULL, *target);
    }
    return 1;

}

/* Creates 3AK code for expression
 */
int create3AK(list_t *postfixList, node_t **local_table, node_data_t **target){
    #ifdef DEBUG_POSTFIX
    list_activate(postfixList);
    printf("create3AK:\n");
    while (postfixList->Current != NULL){
        printf("%d\n", postfixList->Current->data.type);
        postfixList->Current = postfixList->Current->next;
    }
    printf("\n");
    #endif

    list_activate(postfixList);

    char buffer[256];
    node_data_t* temp;
    node_t* temp_node;
    token_t op;
    while (postfixList->Current != NULL){
        buffer[0] = '\0';

        list_copy(postfixList, &op);
        
        if( op.type == SIMPLE_ID) {
            temp_node = search_note(local_table, op.data.svar);

            if (temp_node == NULL) { 
                char* id = create_full_id(class_id, op.data.svar);
                temp_node = search_note(&global_table, id);
                // doesn't exists
                if(temp_node == NULL) {
                    temp = (node_data_t*)GBC_malloc(sizeof(node_data_t));
                    init_table_data(temp);
                    temp->defined = false;
                    temp->struct_type = 'g';
                    bin_tree_insert(&global_table, id, temp);
                }
                else if( temp_node->data->struct_type == 'f') {
                    set_error(SEM_UNDEF_ERROR);
                    return -5;
                }
                GBC_free(op.data.svar);
                op.type = FULLY_QUALIFIED_ID;
                op.data.svar = id;
                list_actualize(postfixList, op);
            }
        }
        else if(op.type == FULLY_QUALIFIED_ID) {
            temp_node = search_note(local_table, op.data.svar);
            if(temp_node == NULL) {
                temp = (node_data_t*)GBC_malloc(sizeof(node_data_t));
                init_table_data(temp);
                temp->defined = false;
                temp->struct_type = 'g';
                bin_tree_insert(&global_table, op.data.svar, temp);
            }
            else if( temp_node->data->struct_type == 'f') {
                set_error(SEM_UNDEF_ERROR);
                return -5;
            }
        }
        else if(op.type == INTEGER) {
            int temp_i = op.data.ivar;
            sprintf(buffer, "%i", op.data.ivar);

            op.type = FULLY_QUALIFIED_ID;
            op.data.svar = (char*)GBC_malloc(sizeof(char) * (strlen(buffer) + 4));

            strcpy(op.data.svar, "&I_");
            strcat(op.data.svar, buffer);
            if(search_note(&global_table, op.data.svar) == NULL) {
                temp = (node_data_t*)GBC_malloc(sizeof(node_data_t));
                init_table_data(temp);
                data_add_type(temp, 'i');
                temp->defined = true;
                temp->struct_type = 'c';
                temp->data_u.ivar = temp_i;
                bin_tree_insert(&global_table, op.data.svar, temp);
            }
            else {
                char* del = op.data.svar;
                op.data.svar = search_note(&global_table, op.data.svar)->id;
                GBC_free(del);
            }
            list_actualize(postfixList, op);
        }
        // double constant
        else if(op.type == DOUBLE) {
            double temp_d = op.data.dvar;
            sprintf(buffer, "%.12g", op.data.dvar);

            op.type = FULLY_QUALIFIED_ID;
            op.data.svar = (char*)GBC_malloc(sizeof(char) * (strlen(buffer) + 4));

            strcpy(op.data.svar, "&D_");
            strcat(op.data.svar, buffer);
            if(search_note(&global_table, op.data.svar) == NULL) {

                temp = (node_data_t*)GBC_malloc(sizeof(node_data_t));
                init_table_data(temp);
                data_add_type(temp, 'd');
                temp->defined = true;
                temp->struct_type = 'c';
                temp->data_u.dvar = temp_d;
                bin_tree_insert(&global_table, op.data.svar, temp);
            }
            list_actualize(postfixList, op);
        }
        // String constant
        else if(op.type == STRING) {
            char* temp_s = op.data.svar;

            op.type = FULLY_QUALIFIED_ID;
            op.data.svar = (char*)GBC_malloc(sizeof(char) * (strlen(op.data.svar) + 4));

            strcpy(op.data.svar, "&S_");
            strcat(op.data.svar, temp_s);
            if(search_note(&global_table, op.data.svar) == NULL) {

                temp = (node_data_t*)GBC_malloc(sizeof(node_data_t));
                init_table_data(temp);
                data_add_type(temp, 's');
                temp->defined = true;
                temp->struct_type = 'c';
                temp->data_u.svar = temp_s;
                bin_tree_insert(&global_table, op.data.svar, temp);
            }
            else {
                GBC_free(temp_s);
            }
            list_actualize(postfixList, op);
        }
        else {
            int operace = get_ilist_operator(op.type);
            token_t a;
            token_t b;

            node_data_t* node_a;
            node_data_t* node_b;
            node_data_t* node_c;

            // operant b
            list_copy_pre(postfixList, &b);
            list_delete_pre(postfixList);
            if(b.type == FULLY_QUALIFIED_ID) {
                node_b = search_note(&global_table, b.data.svar)->data;
            }
            else if(b.type == SIMPLE_ID) {
                node_b = search_note(local_table, b.data.svar)->data;
            }
            else {
                set_error(INTERN_ERROR);
                return -2;
            }
            // operant a
            list_copy_pre(postfixList, &a);
            list_delete_pre(postfixList);
            if(a.type == FULLY_QUALIFIED_ID) {

                node_a = search_note(&global_table, a.data.svar)->data;
            }
            else if(a.type == SIMPLE_ID) {
                node_a = search_note(local_table, a.data.svar)->data;
            }
            else {
                set_error(INTERN_ERROR);
                return -2;
            }
            if(postfixList->Current == postfixList->Last){
                node_c = *target;
                op.data.svar = (*target)->func_type;
                list_delete(postfixList);
            }
            // Temp variable
            else {
                sprintf(buffer, "%i", counter->data_u.ivar);
                op.type = SIMPLE_ID;
                op.data.svar = (char*)GBC_malloc(sizeof(char) * (strlen(buffer) + 4));
                strcpy(op.data.svar, "&T_");
                strcat(op.data.svar, buffer);
                if(search_note(local_table, op.data.svar) == NULL) {
                    temp = (node_data_t*)GBC_malloc(sizeof(node_data_t));
                    init_table_data(temp);
                    //data_add_type(temp, 'i');
                    data_add_type(temp, 'e');
                    temp->defined = true;
                    temp->struct_type = 'l';
                    temp->offset = counter->data_u.ivar;
                    bin_tree_insert(local_table, op.data.svar, temp);
                    counter->data_u.ivar++;
                }
                else {
                    fprintf(stderr, "Error\n");
                }
                list_actualize(postfixList, op);
                node_c = temp;
            }
            
            generate_instruction(instruction_list, operace, node_a, node_b, node_c);

        }
        list_succ(postfixList);
        
    }
    // dest. variable
    if( postfixList->First != NULL && postfixList->First == postfixList->Last ) {

        list_activate(postfixList);
        list_copy(postfixList, &op);
        if(op.type == FULLY_QUALIFIED_ID) {
            temp = search_note(&global_table, op.data.svar)->data;
        }
        else if(op.type == SIMPLE_ID) {
            temp = search_note(local_table, op.data.svar)->data;
        }
        else {
            set_error(INTERN_ERROR);
            return -2;
        }
        generate_instruction(instruction_list, INS_ST, temp, NULL, *target);
    }
    return 1;
}

#ifdef DEBUG_POSTFIX

int main(){

    global_table = NULL;
    node_t* local_table = NULL;
    node_data_t* target = NULL;
    counter = GBC_malloc(sizeof(node_data_t));
    counter->data_u.ivar = 0;
    class_id = "Main";
    ilist_t i;
    ilist_init(&i);
    instruction_list = &i;
    int b = processExpression(0, &local_table, &target);

    bin_tree_dispose(&local_table);
    bin_tree_dispose(&global_table);
    printf("vracim: %d\n", b);
    return 0;

}

#endif
