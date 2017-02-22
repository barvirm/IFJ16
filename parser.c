/*
 * Name: parser.c
 * Author: Marek Barvir
 * Email: barvirm@seznam.cz
 * Login: xbarvi00
 * Date: 25.9.2016
 * Team members:  Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 *
 * Description: Parser
*/

#include "parser.h"

node_t *global_table;
node_data_t *counter;
token_t *token;
token_t *error_token;

ilist_t *instruction_list;

char *current_func;
char *class_id;
const char *buildin_function[] = { "ifj16.print","ifj16.readInt","ifj16.readString","ifj16.sort","ifj16.find","ifj16.compare", "ifj16.readDouble", "ifj16.length", "ifj16.substr" };

#define GET_TOKEN() do { if ( get_token(token) != NO_ERROR ) { set_error(LEX_ERROR); return false; } } while(0)
#define EXPECTED(lex_type) do { if ( token->type != lex_type ) { set_error(SYNTAX_ERROR); save_error_token(); return false; } } while(0)
#define ACCEPT(lex_type) do { GET_TOKEN(); EXPECTED(lex_type); } while(0)
#define BUILDIN_SIZE sizeof(buildin_function)/sizeof(buildin_function[0])


void save_error_token() {
    error_token->type = token->type;
    error_token->line = token->line;
}

char *create_id_record(char *s) {

    size_t id_size = strlen(s)+1;

    char *tmp_id =(char *) GBC_malloc(sizeof(char)*(id_size));
    if ( tmp_id == NULL ) {return NULL;}

    strncpy(tmp_id,s,id_size);
    return tmp_id;
}

void init_table_data(node_data_t *data) {

    data->struct_type = '\0';
    data->inicializate = false;
    data->offset = 0;
    data->defined = false;
    data->func_type =(char *) GBC_malloc(sizeof(char)*2);
    data->type_size_act = 0;
    data->type_size_max = 2;
    data->func_type[data->type_size_max-1] = '\0';
    data->data_u.svar = NULL;
}

char *data_add_type(node_data_t *tmp, char t) {

    if ( tmp->type_size_act == tmp->type_size_max-1 ) { // small size
        tmp->func_type=(char *)GBC_realloc(tmp->func_type,tmp->type_size_max*2); //double it
        if ( tmp->func_type == NULL ) { // error
                set_error(RUNTIME_OTHERS_ERROR);
                return NULL;
        }
        tmp->type_size_max *=2;
        tmp->func_type[tmp->type_size_max-1] = '\0';
    }

    tmp->func_type[tmp->type_size_act++] = t; // add type
    tmp->func_type[tmp->type_size_act] = '\0';
    return tmp->func_type;
}

char *create_full_id(char *class_id,char *simple_id) {

    size_t size = strlen(class_id) + strlen(simple_id) + 2;

    char *full_id =(char *) GBC_malloc(sizeof(char)*size);
    if ( full_id == NULL ) { set_error(RUNTIME_OTHERS_ERROR); return NULL; }

    sprintf(full_id,"%s.%s",class_id,simple_id); 
    return full_id;
}

bool TYPE(node_data_t *tmp) {
    
    GET_TOKEN();
    switch(token->type) {
        case RES_INT:
            if ( data_add_type(tmp,'i') == NULL ) return false;
            break;
        case RES_STRING:
            if ( data_add_type(tmp,'s') == NULL ) return false;
            break;
        case RES_DOUBLE:
            if ( data_add_type(tmp,'d') == NULL ) return false;
            break;
        case RES_VOID:
            if ( data_add_type(tmp,'v') == NULL ) return false;
                break;
        default:
            set_error(SYNTAX_ERROR);
            return false;
    }

    return true;
}

bool skip_expresions(char e, node_t **local_table,node_data_t **result) {
    int r = processExpression(0,local_table,result);

    if ( e == 'e' ) { // can be empty
        if ( r == 2 ) { *result = NULL; return true; } // empty expression
        else if ( r == 1 ) {  return true; } // no problem with expression
        else { return false; }
    }

    // cant be empty
    if ( r == 1 ) { return true; } // no problem with expression
    else if ( r == -1 || r == 2) { set_error(SYNTAX_ERROR); return false; } // its empty or synatax error
    else { return false; }
}

bool exec_expressions(char e,  node_data_t **result, ilist_t* temp_ilist) {
    // init
    node_t* local_table = NULL;
    // generate 3AK
    ilist_t* temp = instruction_list;
    instruction_list = temp_ilist;
    skip_expresions(e, &local_table, result);
    instruction_list = temp;
    
    return error_number == NO_ERROR ? true : false;
}

bool skip_condition(node_t **local_table,node_data_t **result) {
    int r = processExpression(1,local_table,result);
    if ( r == 1 ) { return true; } // no problem with condition
    else if ( r == -1 ) { return false; } // empty condition
    else { return false; }
}

node_data_t *search_local_global(node_t **local_table, char *id) {

    node_t *tmp = search_note(local_table,id); // search note in local table
    if ( tmp != NULL )
        return tmp->data; // find return

    char *global_tmp = create_full_id(class_id,id); // create full_id
    tmp = search_note(&global_table,global_tmp); // search in global table
    GBC_free(global_tmp); // free full_id
    if ( tmp != NULL )
        return tmp->data; // return data

    return NULL; // not found
}


bool buildin_func(node_t **local_table, char f,node_data_t *func) {
    GET_TOKEN();
    if( token->type != BRACKET_RIGHT) {
        while( true ) {
            if ( token->type == STRING ) {  // parameter is string -> create const add string and push
                char *tmp = create_id_record(token->data.svar);
                node_data_t *s =(node_data_t *) GBC_malloc(sizeof(node_data_t));
                if ( s == NULL ) { set_error(INTERN_ERROR); return false; }
                init_const(s,'s');
                s->data_u.svar =(char *) GBC_malloc(sizeof(char)*(strlen(token->data.svar)+1));
                if ( s->data_u.svar == NULL ) { set_error(INTERN_ERROR); return false; }
                s->data_u.svar = tmp;
                generate_instruction(instruction_list,PUSH_PARAM,s,NULL,NULL);
            }
            else if ( token->type == INTEGER ) { // parameter is integer
                node_data_t *i =(node_data_t *) GBC_malloc(sizeof(node_data_t));
                if ( i == NULL ) { set_error(INTERN_ERROR); return false; }
                init_const(i,'i');
                i->data_u.ivar = token->data.ivar;
                generate_instruction(instruction_list,PUSH_PARAM,i,NULL,NULL);
            }
            else if ( token->type == DOUBLE ) { // parameter is double
                node_data_t *d =(node_data_t *) GBC_malloc(sizeof(node_data_t));
                if ( d == NULL ) { set_error(INTERN_ERROR); return false; }
                init_const(d,'d');
                d->data_u.dvar = token->data.dvar;
                generate_instruction(instruction_list,PUSH_PARAM,d,NULL,NULL);
            }
            else if ( token->type == SIMPLE_ID ) { // paramter is simple id, find in tables and push
                node_data_t *v = search_local_global(local_table,token->data.svar);
                if ( v == NULL ) {
                    char* id = create_full_id(class_id, token->data.svar);
                    v = (node_data_t*)GBC_malloc(sizeof(node_data_t));
                    init_table_data(v);
                    v->defined = false;
                    v->struct_type = 'g';
                    bin_tree_insert(&global_table, id, v);
                }
                else if( v->struct_type == 'f') {
                    GET_TOKEN();
                    if( token->type == COMMA || token->type == BRACKET_RIGHT)  {
                        set_error(SEM_UNDEF_ERROR); 
                    }
                    else {
                        set_error(SYNTAX_ERROR); 
                    }
                    return false;
                }
                generate_instruction(instruction_list,PUSH_PARAM,v,NULL,NULL);
            }
            else if ( token->type == FULLY_QUALIFIED_ID ) { // parameter is full id, find in table and push
                node_data_t *v;
                node_t *t = search_note(&global_table,token->data.svar);
                if ( t == NULL ) { 
                    v = (node_data_t*)GBC_malloc(sizeof(node_data_t));
                    char* id = (char*)GBC_malloc(sizeof(char) * strlen(token->data.svar)+1);
                    strcpy(id, token->data.svar);
                    init_table_data(v);
                    v->defined = false;
                    v->struct_type = 'g';
                    bin_tree_insert(&global_table, id, v);
                }
                else if( t->data->struct_type == 'f') {
                    GET_TOKEN();
                    if( token->type == COMMA || token->type == BRACKET_RIGHT) {
                        set_error(SEM_UNDEF_ERROR);
                    }
                    else {
                        set_error(SYNTAX_ERROR); 
                    }
                    return false;
                }
                else {
                    v = t->data;
                }
                generate_instruction(instruction_list,PUSH_PARAM,v,NULL,NULL);
            }

            else { set_error(SYNTAX_ERROR);  return false; }
            
            GET_TOKEN();

            if ( token->type == COMMA ) { GET_TOKEN(); } // next parameter
            else if ( token->type == BRACKET_RIGHT ) { break; }
            else if ( token->type == PLUS && f == 'p' ) { GET_TOKEN(); } // next parameter if its print
            else { set_error(SYNTAX_ERROR); return false; }
        }
    }
    switch ( f ) { // build in function

        case 'i': // readint
                    generate_instruction(instruction_list,READ_INT,NULL,NULL,NULL);
                    break;
        case 'd': // readdouble
                    generate_instruction(instruction_list,READ_DOUBLE,NULL,NULL,NULL);
                    break;
        case 's': // readstring
                    generate_instruction(instruction_list,READ_STRING,NULL,NULL,NULL);
                    break;
        case 'l': // length
                    generate_instruction(instruction_list,LENGTH,NULL,NULL,NULL);
                    break;
        case 'u': // substr
                    generate_instruction(instruction_list,SUBSTR,NULL,NULL,NULL);
                    break;
        case 'c': // compare
                    generate_instruction(instruction_list,COMPARE,NULL,NULL,NULL);
                    break;
        case 'f': // find
                    generate_instruction(instruction_list,FIND,NULL,NULL,NULL);
                    break;
        case 'o': // sort
                    generate_instruction(instruction_list,SORT,NULL,NULL,NULL);
                    break;
        case 'x': // defined by source code
                    generate_instruction(instruction_list,CALL,func,NULL,NULL);
                    break;
        case 'p': // print
                    generate_instruction(instruction_list,PRINT,NULL,NULL,NULL);
                    break;
        default:
                    printf("Unknown function - param\n");
                    set_error(INTERN_ERROR);
                    return false;
    }
    return true;
}

void set_offset(node_data_t *td) {
    td->offset = counter->data_u.ivar;
    counter->data_u.ivar++;
}

bool PARAMS_N(node_data_t *data,node_t **local_table) {

    GET_TOKEN();

    node_data_t *tmp =(node_data_t *)GBC_malloc(sizeof(node_data_t));
    if ( tmp == NULL ) { set_error(INTERN_ERROR); return false; }
    init_table_data(tmp);
    tmp->struct_type = 'l';

    switch( token->type ) {
        case RES_INT:
                    if ( data_add_type(data,'i') == NULL ) return false; // function param
                    if ( data_add_type(tmp,'i') == NULL ) return false; // local table
                    break;
        case RES_STRING:
                    if ( data_add_type(data,'s') == NULL ) return false; // function param
                    if ( data_add_type(tmp,'s') == NULL ) return false; // local table
                    break;
        case RES_DOUBLE:
                    if ( data_add_type(data,'d') == NULL ) return false; // function param
                    if ( data_add_type(tmp,'d') == NULL ) return false;  // local table
                    break;
        default:
                    set_error(SYNTAX_ERROR);
                    return false;
    }

    ACCEPT(SIMPLE_ID);
    char *id = create_id_record(token->data.svar);
    bin_tree_insert(local_table,id,tmp);
    set_offset(tmp);

    GET_TOKEN();
    if ( token->type == BRACE_RIGHT ) {
        return true;
    }
    else if ( token->type == COMMA ) { // next parameter
        if ( !PARAMS_N(data,local_table) ) { set_error(SYNTAX_ERROR); return false; }
    }

    return true;
}

bool PARAMS(node_data_t *data,node_t **local_table) {

    node_data_t *tmp; // local table

    GET_TOKEN();
    switch( token->type ) {
        case RES_INT:
                    tmp =(node_data_t *) GBC_malloc(sizeof(node_data_t));
                    if ( tmp == NULL ) { set_error(INTERN_ERROR); return false; }
                    init_table_data(tmp);
                    tmp->struct_type = 'l';
                    if ( data_add_type(data,'i') == NULL ) return false; // func
                    if ( data_add_type(tmp,'i') == NULL ) return false;  // local table
                    break;
        case RES_STRING:
                    tmp =(node_data_t *) GBC_malloc(sizeof(node_data_t));
                    if ( tmp == NULL ) { set_error(INTERN_ERROR); return false; }
                    init_table_data(tmp);
                    tmp->struct_type = 'l';
                    if ( data_add_type(data,'s') == NULL ) return false; // func
                    if ( data_add_type(tmp,'s') == NULL ) return false; // local table
                    break;
        case RES_DOUBLE:
                    tmp =(node_data_t *) GBC_malloc(sizeof(node_data_t));
                    if ( tmp == NULL ) { set_error(INTERN_ERROR); return false; }
                    init_table_data(tmp);
                    tmp->struct_type = 'l';
                    if ( data_add_type(data,'d') == NULL ) return false; // func
                    if ( data_add_type(tmp,'d') == NULL ) return false; // local table
                    break;
        case BRACKET_RIGHT:
                    return true;
        default:
                    set_error(SYNTAX_ERROR);
                    return false;
    }

    ACCEPT(SIMPLE_ID);
    char *id = create_id_record(token->data.svar);
    bin_tree_insert(local_table,id,tmp);
    set_offset(tmp);

    GET_TOKEN();
    if ( token->type == BRACE_RIGHT ) {
        return true;
    }
    else if ( token->type == COMMA ) { // next param
        if ( !PARAMS_N(data,local_table) ) { set_error(SYNTAX_ERROR); return false; }
    }

    return true;
}

bool VAR_TYPE(node_data_t *data) {

    switch(token->type) {
        case RES_INT:
                    if ( data_add_type(data,'i') == NULL ) return false;
                    break;
        case RES_STRING:
                    if ( data_add_type(data,'s') == NULL ) return false;
                    break;
        case RES_DOUBLE:
                    if ( data_add_type(data,'d') == NULL ) return false;
                    break;
        default:
                    set_error(SYNTAX_ERROR);
                    return false;
    }
    return true;
}


bool WHILE(node_t *local_table) {

    ACCEPT(BRACKET_LEFT);

    node_data_t *tmp_r =(node_data_t *) GBC_malloc(sizeof(node_data_t));
    if ( tmp_r == NULL ) { set_error(INTERN_ERROR); return false; }

    init_table_data(tmp_r);
    set_offset(tmp_r);
    tmp_r->struct_type = 'l';
    data_add_type(tmp_r,'i');
    tmp_r->inicializate = true;

    generate_while(instruction_list,tmp_r); 
    if ( !skip_condition(&local_table,&tmp_r) ) { return false; };
    generate_while_condition_end(instruction_list);
    if ( !FUNC_BODY(&local_table,'w') ) { return false;}
    generate_while_end(instruction_list);
    return true;
}

bool IF(node_t *local_table) {

    ACCEPT(BRACKET_LEFT);

    node_data_t *tmp_r =(node_data_t *)GBC_malloc(sizeof(node_data_t));
    if (tmp_r == NULL ) { set_error(INTERN_ERROR); return false;}

    init_table_data(tmp_r);
    set_offset(tmp_r);
    tmp_r->struct_type = 'l';
    data_add_type(tmp_r, 'e');
    tmp_r->inicializate = true;

    if (!skip_condition(&local_table,&tmp_r) ) { return false; };
    generate_if_else(instruction_list,tmp_r);
    if ( !FUNC_BODY(&local_table,'i') ) { return false;}
    ACCEPT(RES_ELSE);
    generate_if_end(instruction_list);
    if ( !FUNC_BODY(&local_table,'i') ) { return false;}
    generate_else_end(instruction_list);
    return true;
}


bool is_buildin_func(char *s) {

    for ( size_t i = 0; i < BUILDIN_SIZE; i++ ) {
        if ( strcmp(s,buildin_function[i]) == 0 )
            return true;
    }

    return false;
}

void set_table_data_init(node_t **t,char *r) {

    node_t *tmp = search_note(t,r);
    tmp->data->inicializate = true;

}

void init_const(node_data_t *data,char type) {
    data->struct_type = 'c';
    data->inicializate = true;
    data->offset = 0;
    data->defined = true;
    data->func_type =(char *) GBC_malloc(sizeof(char)*2);
    data->type_size_act = 0;
    data->type_size_max = 2;
    data->func_type[data->type_size_max-1] = '\0';
    data_add_type(data,type);
}

bool FUNC_BODY(node_t **local_table,char block) {
    ACCEPT(BRACE_LEFT);

    node_data_t *table_data;
    char *tmp_id;

    GET_TOKEN();

    while ( token->type != BRACE_RIGHT ) {
        switch( token->type) {
            case RES_INT:
            case RES_STRING:
            case RES_DOUBLE:
                            if ( block == 'i' || block == 'w' ) { set_error(SYNTAX_ERROR); return false; } // cant init in if or while
                            table_data =(node_data_t *) GBC_malloc(sizeof(node_data_t));
                            if ( table_data == NULL ) {set_error(RUNTIME_OTHERS_ERROR); return false;}
                            init_table_data(table_data);

                            table_data->struct_type = 'l'; // as local
                            table_data->defined = true;

                            VAR_TYPE(table_data);
                            set_offset(table_data);
                            ACCEPT(SIMPLE_ID);
                            tmp_id = create_id_record(token->data.svar);

                            if ( tmp_id == NULL ) { set_error( INTERN_ERROR ); return false;}

                            // redfined function or local variable
                            node_data_t* redef = search_local_global(local_table, tmp_id);
                            if(redef != NULL && (redef->struct_type == 'f' || redef->struct_type == 'l')) {
                                set_error(SEM_UNDEF_ERROR);
                                return false;
                            }

                            // block create function with same name
                            if( redef == NULL) {
                                redef = (node_data_t *) GBC_malloc(sizeof(node_data_t));
                                if ( redef == NULL ) {set_error(INTERN_ERROR); return false;}
                                init_table_data(redef);

                                redef->struct_type = 'u'; // as local
                                redef->defined = true;
                                bin_tree_insert(&global_table, create_full_id(class_id, tmp_id), redef); // add record to bin tree
                            }

                            bin_tree_insert(local_table,tmp_id,table_data); // add record to bin tree

                            GET_TOKEN();
                            if ( token ->type == SEMI ) // double a;
                                break;
                            else if ( token->type == ASIGN ) { // double a = <expression>
                                set_table_data_init(local_table,tmp_id);
                                if (!skip_expresions('n',local_table,&table_data) ) { return false; }; 
                            }
                            else { set_error(SYNTAX_ERROR); return false; }
                            break;
            case RES_WHILE:
                            if ( !WHILE((*local_table)) ) { return false;}  
                            break;
            case RES_IF:
                            if ( !IF(*local_table) ) {return false;}
                            break;
            case RES_RETURN:
                            table_data =(node_data_t *) GBC_malloc(sizeof(node_data_t)); // return data
                            if ( table_data == NULL ) { set_error(INTERN_ERROR); return false; }
                            init_table_data(table_data);
                            set_offset(table_data);
                            table_data->struct_type = 'l';
                            data_add_type(table_data,'e');
                            table_data->inicializate = true;
                            if ( !skip_expresions('e',local_table,&table_data) ) { return false; }
                            generate_instruction(instruction_list,RET,table_data,NULL,NULL);
                            break;
            case FULLY_QUALIFIED_ID:
                            if ( is_buildin_func(token->data.svar) ) {  // buildin func
                                if ( strcmp(token->data.svar,"ifj16.print") == 0 ) {
                                    ACCEPT(BRACKET_LEFT);
                                    if(!buildin_func(local_table,'p',NULL)) {
                                        return false;
                                    }
                                }
                                else if ( strcmp(token->data.svar,"ifj16.readInt") == 0 ) {
                                    ACCEPT(BRACKET_LEFT);
                                    if(!buildin_func(local_table,'i',NULL)) {
                                        return false;
                                    }
                                }
                                else if ( strcmp(token->data.svar,"ifj16.readString") == 0 ) {
                                    ACCEPT(BRACKET_LEFT);
                                    if(!buildin_func(local_table,'s',NULL)) {
                                        return false;
                                    }
                                }
                                else if ( strcmp(token->data.svar,"ifj16.readDouble") == 0 ) {
                                    ACCEPT(BRACKET_LEFT);
                                    if(!buildin_func(local_table,'d',NULL)) {
                                        return false;
                                    }
                                }
                                else if ( strcmp(token->data.svar,"ifj16.length") == 0 ) {
                                    ACCEPT(BRACKET_LEFT);
                                    if(!buildin_func(local_table,'l',NULL)) {
                                        return false;
                                    }
                                }
                                else if ( strcmp(token->data.svar,"ifj16.substr") == 0 ) {
                                    ACCEPT(BRACKET_LEFT);
                                    if(!buildin_func(local_table,'u',NULL)) {
                                        return false;
                                    }
                                }
                                else if ( strcmp(token->data.svar,"ifj16.compare") == 0 ) {
                                    ACCEPT(BRACKET_LEFT);
                                    if(!buildin_func(local_table,'c',NULL)) {
                                        return false;
                                    }
                                }
                                else if ( strcmp(token->data.svar,"ifj16.find") == 0 ) {
                                    ACCEPT(BRACKET_LEFT);
                                    if(!buildin_func(local_table,'f',NULL)) {
                                        return false;
                                    }
                                }
                                else if ( strcmp(token->data.svar,"ifj16.sort") == 0 ) {
                                    ACCEPT(BRACKET_LEFT);
                                    if(!buildin_func(local_table,'o',NULL)) {
                                        return false;
                                    }
                                }
                                ACCEPT(SEMI);
                                break;
                            }
                            tmp_id = create_id_record(token->data.svar);
                            GET_TOKEN();
                            if ( token->type == BRACKET_LEFT ) { // Math.sqrt(
                                node_t *f = search_note(&global_table,tmp_id);
                                node_data_t *data;
                                if ( f == NULL ) { // not defined yet -> create record
                                    data =(node_data_t *)GBC_malloc(sizeof(node_data_t));
                                    if ( data == NULL ) { set_error(INTERN_ERROR); return false; }
                                    init_table_data(data);
                                    data->struct_type = 'f';
                                    bin_tree_insert(&global_table,tmp_id,data);
                                }
                                else {
                                    data = f->data; // defined
                                }

                                if(!buildin_func(local_table,'x',data)) { // generate func
                                    return false;
                                }

                                ACCEPT(SEMI);

                            }
                            else if ( token->type == ASIGN ) { // Main.x = <expression>
                                node_t *f = search_note(&global_table,tmp_id);
                                node_data_t *table_data;

                                if ( f == NULL ) { // not defined yet -> add record to table
                                    table_data =(node_data_t *)GBC_malloc(sizeof(node_data_t));
                                    if ( table_data == NULL ) { set_error(INTERN_ERROR); return false; }
                                    init_table_data(table_data);
                                    table_data->struct_type = 'g';
                                    table_data->defined = false;
                                    bin_tree_insert(&global_table,tmp_id,table_data);
                                }
                                else {
                                    table_data = f->data; // defined
                                }
                                if (!skip_expresions('n',local_table,&table_data) ) {return false; }; // <expression>
                            }
                            else { set_error(SYNTAX_ERROR); return false; }
                            break;
            case SIMPLE_ID:
                            tmp_id = create_id_record(token->data.svar);
                            table_data = search_local_global(local_table,tmp_id);
                            if ( table_data == NULL ) {
                                node_data_t *data =(node_data_t *) GBC_malloc(sizeof(node_data_t));
                                init_table_data(data);
                                tmp_id = create_full_id(class_id,tmp_id);
                                GET_TOKEN();
                                if ( token->type == ASIGN ) { // asign to unknow var -> error
                                    data->struct_type = 'g';
                                    bin_tree_insert(&global_table,tmp_id,data);
                                    if (!skip_expresions('n',local_table,&table_data) ) { return false; };
                                    break;
                                }
                                else if ( token->type == BRACKET_LEFT ) { // call unknow function -> add record to table
                                    data->struct_type = 'f';
                                    bin_tree_insert(&global_table,tmp_id,data);
                                    if(!buildin_func(local_table,'x',data)) {
                                        return false;
                                    }
                                    ACCEPT(SEMI);
                                    break;
                                }
                                else { set_error(SYNTAX_ERROR); return false; }
                            }

                            GET_TOKEN();
                            if ( token->type == BRACKET_LEFT ) { // call func
                                if(table_data->struct_type == 'g' || table_data->struct_type == 'l') {
                                    set_error(SEM_UNDEF_ERROR);
                                    return false;
                                }
                                if(!buildin_func(local_table,'x',table_data) ){
                                    return false;
                                } 
                                ACCEPT(SEMI);
                                break;
                            }
                            else if ( token->type == ASIGN ) { // asign
                                if(table_data->struct_type == 'f') {
                                    set_error(SEM_UNDEF_ERROR);
                                    return false;
                                }
                                if (!skip_expresions('n',local_table,&table_data) ) { return false; };
                            }
                            else { set_error(SYNTAX_ERROR); return false; }
                            break;
            default:
                            set_error(SYNTAX_ERROR);
                            return false;
                            break;
        }
        GET_TOKEN();
    }


    return true;

}

bool CLASS_BODY(ilist_t* init_ilist) {
    if ( token->type == END_OF_FILE ) 
        return true;

    GET_TOKEN();
    if(token->type == BRACE_RIGHT) return true;

    if(token->type != RES_STATIC) {
        set_error(SYNTAX_ERROR);
        return false;
    }
        
    node_data_t *table_data;
    table_data =(node_data_t *)GBC_malloc(sizeof(node_data_t));
    if ( table_data == NULL ) { set_error(INTERN_ERROR); return false; }
    init_table_data(table_data);

    TYPE(table_data);
    char save = table_data->func_type[0];
    table_data->defined = true;
    ACCEPT(SIMPLE_ID);

    char *tmp_id = create_id_record(token->data.svar);
    if ( tmp_id == NULL ) { set_error(INTERN_ERROR); return false; }

    char *full_id = create_full_id(class_id,tmp_id);
    if ( full_id == NULL ) { set_error(INTERN_ERROR); return false; }
    GBC_free(tmp_id);


    GET_TOKEN();
    if ( token->type == SEMI ) { // static <type> id a;
        table_data->struct_type = 'g';
        node_t *tmp = search_note(&global_table,full_id);

        if( tmp == NULL) {
            bin_tree_insert(&global_table,full_id,table_data);
            tmp = search_note(&global_table,full_id);
        }
        else if( tmp->data->struct_type == 'u')  {

            tmp->data->struct_type = 'g';
            tmp->data->func_type[0] = save;
        }
        else if (tmp->data->defined == 0 && tmp->data->struct_type == 'g') {
            tmp->data->func_type[0] = save;
        }
        else  {
            set_error(SEM_UNDEF_ERROR); 
            return false; 
        }
        if ( table_data->func_type[0] == 'v' ) { set_error(SYNTAX_ERROR); return false; }
        tmp->data->defined = 1;
    }
    else if ( token->type == ASIGN ) { // static <type> id a = <expression>
        table_data->struct_type = 'g';
        table_data->inicializate = true;

        node_t *tmp = search_note(&global_table,full_id);

        if( tmp == NULL) { // not defined  
            bin_tree_insert(&global_table,full_id,table_data); // add to local table
            tmp = search_note(&global_table,full_id);
        }
        else if( tmp->data->struct_type == 'u')  {
            tmp->data->struct_type = 'g';
            tmp->data->func_type[0] = save;
        }
        else if (tmp->data->defined == 0 && tmp->data->struct_type == 'g') {
            tmp->data->func_type[0] = save;
        }
        else  {
            set_error(SEM_UNDEF_ERROR); 
            return false; 
        }
        if ( table_data->func_type[0] == 'v' ) { set_error(SYNTAX_ERROR); return false; } // void variable -> error
        tmp->data->defined = 1;
        if(!exec_expressions('n', &table_data, init_ilist)) {return false;}
    }
    else if ( token->type == BRACKET_LEFT ) { // static <type> id(<param>) <func_body>
        counter =(node_data_t *) GBC_malloc(sizeof(node_data_t)); // new counter for new function
        if ( counter == NULL ) { set_error(INTERN_ERROR); return false; }
        counter->data_u.ivar = 0;

        node_t *tmp = search_note(&global_table,full_id); 
        if ( tmp == NULL ) { // not in table -> add record
            table_data->struct_type = 'f';
            table_data->defined = true;
            bin_tree_insert(&global_table,full_id,table_data);

            generate_function(instruction_list,table_data,counter);
        }
        else {
            if ( tmp->data->defined == true || tmp->data->struct_type == 'g') { set_error(SEM_UNDEF_ERROR); return false; } // redefined -> error
            else { 
                tmp->data->defined = true;
                table_data = tmp->data;
                data_add_type(table_data,save);
                generate_function(instruction_list,table_data,counter);
            }
        }
        current_func = create_id_record(full_id);
        node_t *local_table = NULL; // new local table
        if ( current_func == NULL ) { set_error(RUNTIME_OTHERS_ERROR); return false; }
        if ( !PARAMS(table_data,&local_table) ) { set_error(SYNTAX_ERROR); return false; }
        if ( !FUNC_BODY(&local_table, 'f') ) {return false; }

        generate_instruction(instruction_list,END_FUNC,table_data,NULL,NULL);
        // bin_tree_dispose(&local_table);
    }
    else { set_error(SYNTAX_ERROR); return false; }
    return CLASS_BODY(init_ilist); // end of class
}

bool program(ilist_t* init_ilist) {

    GET_TOKEN();
    while ( token->type != END_OF_FILE ) { // class id { <class_body> }
        EXPECTED(RES_CLASS);
        ACCEPT(SIMPLE_ID);
        class_id = create_id_record(token->data.svar);
        if ( class_id == NULL ) { set_error(INTERN_ERROR); return false; }

        if ( search_note(&global_table,class_id) == NULL ) {
            node_data_t *d =(node_data_t *)GBC_malloc(sizeof(node_data_t));
            if ( d == NULL ) { set_error(INTERN_ERROR); return false; }
            init_table_data(d);
            d->struct_type = 's';
            d->defined = true;
            bin_tree_insert(&global_table,class_id,d);
        }
        else { set_error(SEM_UNDEF_ERROR); save_error_token(); return false; }

        ACCEPT(BRACE_LEFT);
        if ( !CLASS_BODY(init_ilist) ) { save_error_token(); return false; }
        EXPECTED(BRACE_RIGHT);
        class_id = NULL;

        GET_TOKEN(); // EOF or CLASS
    }

    return true;
}

bool check_main_function() {
    char f[] = "Main.run";
    node_t* tmp = search_note(&global_table,f);
    if ( tmp != NULL && tmp->data != NULL && tmp->data->defined == true && !strcmp(tmp->data->func_type, "v"))
        return true;
    return false;
}

int function_defined(node_t **root) {
    if ( (*root)->data->struct_type == 'f' || (*root)->data->struct_type == 'g') {
        if ( (*root)->data->defined ) {
            return true;
        }
        else {
            fprintf(stderr, "Sematic error: undef %s\n",(*root)->id);
            return false;
        }
    }
    return 2;
}


void init_counter() {
    init_table_data(counter);
    counter->inicializate = true;
    counter->data_u.ivar = 0;
    counter->struct_type = 'c';
    counter->defined = "true";
    data_add_type(counter,'i');
}

int parser( FILE *file_for_read, ilist_t *inst_l, ilist_t* init_ilist) {
    token_t t;
    init_token(&t);
    token = &t;
    instruction_list = inst_l;
    error_token =(token_t *) GBC_malloc(sizeof(token_t));
    if ( error_token == NULL ) { return INTERN_ERROR; }
    error_token->line = 0;

    counter =(node_data_t*) GBC_malloc(sizeof(node_data_t));
    if ( counter == NULL ) { return INTERN_ERROR; }
    init_counter();

    global_table = NULL;
    char ifj[] = "ifj16";
    char *i = create_id_record(ifj);
    node_data_t *tmp =(node_data_t*) GBC_malloc(sizeof(node_data_t));
    init_table_data(tmp);
    tmp->defined = true;
    tmp->struct_type = 's';
    bin_tree_insert(&global_table,i,tmp);

    FILE *f = file_for_read;

    node_data_t *main_run =(node_data_t *)GBC_malloc(sizeof(node_data_t));
    if ( main_run == NULL ) { return INTERN_ERROR; }
    init_table_data(main_run);
    main_run->struct_type = 'f';
    char *main_id = create_id_record("Main.run");
    bin_tree_insert(&global_table,main_id,main_run);
    generate_instruction(instruction_list,CALL,main_run,NULL,NULL);
    generate_instruction(instruction_list,EXIT,NULL,NULL,NULL);
    //printf("counter 1 - %i\n",counter->data_u.ivar);

    if ( !f )
        return RUNTIME_OTHERS_ERROR;
    
    init_scanner(f);
    program(init_ilist);
    if ( error_number == NO_ERROR ) {
        //printf("aaseaseae\n");
        if ( !check_main_function() ) { set_error(SEM_UNDEF_ERROR); }
        if ( !bin_tree_trought(&global_table,&function_defined) ) { set_error(SEM_UNDEF_ERROR); }
    }


    return error_number;   
}

#ifdef DEBUG_PARSER

int main() {
    FILE *f = fopen("./tests/Parser/parser_test_1-0.ifj16","r");
    return parser(f);
}
#endif
