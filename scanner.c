/*
 * Name:    scanner.c
 * Author:  Karel ONDŘEJ, FIT
 * Email:   xondre09@stud.fit.vutbr.cz
 * Login    xondre09
 * Date:    25. 9. 2016
 * Team members:  Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 *
 * Description: 
 * ...
 */

#include "scanner.h"

#define RESERVED_SIZE 17

// Reserved words.
const char* RESERVED_LIST[] = { "boolean", "break", "class", "continue", "do",
                                "double", "else", "false", "for", "if", "int",
                                "return", "String", "static", "true", "void", "while"};

// States of automaton.
typedef enum {
    STATE_START,              ///< Start state
    STATE_INTEGER,            ///< Integer
    STATE_DOUBLE,             ///< Double
    STATE_STRING,             ///< String
    STATE_SIMPLE_ID,          ///< Simple identifier
    STATE_FULLY_ID,           ///< Fully qualified identifier
    STATE_FULLY_ID_PRE,
    STATE_ERROR,              ///< Lexical error
    STATE_DOUBLE_PRE,
    STATE_EXPONENT,
    STATE_EXPONENT_PRE,
    STATE_EXPONENT_PRE2,
    STATE_STRING_PRE,
    STATE_ESCAPE,
    STATE_ESCAPE_OCT1,
    STATE_ESCAPE_OCT2,
    STATE_ESCAPE_OCT3,
    STATE_DIV_PRE,
    STATE_LESS_PRE,
    STATE_GREAT_PRE,
    STATE_NOT_PRE,
    STATE_ASIGN_PRE,
    STATE_LINE_COMMENT,
    STATE_BLOCK_COMMENT,
    STATE_BLOCK_COMMENT2
}state_t;

FILE* file; // File for scane
int line;

#define delimiter(c) ((c) == '=' || (c) == '*' || (c) == '/' || (c) == '+' || \
                      (c) == '-' || (c) == '<' || (c) == '>' || (c) == '!' || \
                      (c) == ';' || (c) == ',' || (c) == '{' || (c) == '}' || \
                      (c) == '(' || (c) == ')' || isspace(c) || (c) == EOF)

/*
 * Initialization scanner.
 */
void init_scanner(FILE *f)
{
    file = f;
    line = 1;
}

/**
 * Lexeme retrieves and converts it to a token.
 */
int get_token(token_t* Token)
{
    buffer_t buffer; // were static

    clean_token(Token);
    if( init_buffer(&buffer) ) goto ERROR;
    state_t status = STATE_START;
    int escape = 0;
    int c;

    while(Token->type == EMPTY)
    {
        c = fgetc(file);
        switch(status)
        {
            // Start state

            case STATE_START:
                if (isspace(c)) {           // skip white space
                    if(c == '\n') {
                        line += 1;
                    }
                    break;
                }
                else if (c == '_' || c == '$' || isalpha(c)) {  // identifier or reserved word 
                    status = STATE_SIMPLE_ID;
                }
                else if (isdigit(c)) {      // number
                    status = STATE_INTEGER;
                }
                else if (c == EOF) {        // end of file
                    token_set_type(Token, END_OF_FILE);
                }
                else if (c == '*') {
                    token_set_type(Token, MUL);
                }
                else if (c == '/') {        // div or comment
                    status = STATE_DIV_PRE;
                }
                else if (c == '+') {
                    token_set_type(Token, PLUS);
                }
                else if (c == '-') {
                    token_set_type(Token, MINUS);
                }
                else if (c == '<') {        // < or <=
                    status = STATE_LESS_PRE;
                }
                else if (c == '>') {        // > or >=
                    status = STATE_GREAT_PRE;
                }
                else if (c == '=') {        // = or ==
                    status = STATE_ASIGN_PRE;
                }
                else if (c == '!') {        // !=
                    status = STATE_NOT_PRE;
                }
                else if (c == '(') {
                    token_set_type(Token, BRACKET_LEFT);
                }
                else if (c == ')') {
                    token_set_type(Token, BRACKET_RIGHT);
                }
                else if (c == '{') {
                    token_set_type(Token, BRACE_LEFT);
                }
                else if (c == '}') {
                    token_set_type(Token, BRACE_RIGHT);
                }
                else if (c == ';') {
                    token_set_type(Token, SEMI);
                }
                else if (c == ',') {
                    token_set_type(Token, COMMA);
                }
                else if (c == '\"') {       // string
                    status = STATE_STRING_PRE; 
                    break;
                }
                else {
                    fprintf(stderr, "Lex error: unknown token\n");
                    status = STATE_ERROR;
                }
                
                if( buffer_char_add(c, &buffer) ) goto ERROR;
                break;

            // Identifier

            case STATE_SIMPLE_ID:     // simple identifier
                if(c == '.') { 
                    status = STATE_FULLY_ID_PRE;  
                    if( buffer_char_add(c, &buffer) ) goto ERROR;
                }
                else if (isalnum(c) || c == '_' || c == '$') {
                    if( buffer_char_add(c, &buffer) ) goto ERROR;
                }
                else if (delimiter(c)) {
                    ungetc(c, file);
                    token_set_string(Token, buffer.data, SIMPLE_ID);

                    // Reserved words

                    for( int i = 0 ; i < RESERVED_SIZE; i++) {
                        if (strcmp(RESERVED_LIST[i], buffer.data) == 0) {
                            token_set_type(Token, RES_BOOLEAN + i);
                        }
                    }
                }
                else {
                    status = STATE_ERROR;
                }
                break;
            case STATE_FULLY_ID_PRE:
                if(isalpha(c) || c == '_' || c == '$') { // set [a-zA-Z_$]
                    if( buffer_char_add(c, &buffer) ) goto ERROR;
                    status = STATE_FULLY_ID;
                }
                else {
                    status = STATE_ERROR;
                }
                break;
            case STATE_FULLY_ID:       // fully qualified identifier
                if(isalnum(c) || c == '_' || c == '$') { // set [a-zA-Z_$]
                    if( buffer_char_add(c, &buffer) ) goto ERROR;
                }
                else if( delimiter(c)) {
                    ungetc(c, file);
                    token_set_string(Token, buffer.data, FULLY_QUALIFIED_ID);
                }
                else {
                    status = STATE_ERROR;
                }
                break;

            // Number

            case STATE_INTEGER:   // integer or double
                if(c == '.') { 
                    status = STATE_DOUBLE_PRE; 
                    if( buffer_char_add(c, &buffer) ) goto ERROR;
                }
                else if (c == 'e' || c == 'E') { // double with exponent
                    status = STATE_EXPONENT_PRE;
                    if( buffer_char_add(c, &buffer) ) goto ERROR;
                }
                else if(isdigit(c)) {
                    if( buffer_char_add(c, &buffer) ) goto ERROR;
                }
                else if(delimiter(c)) {   
                    if(  token_set_integer(Token, buffer.data) ) {
                        status = STATE_ERROR;
                    }
                    ungetc(c, file);
                }
                else {
                    status = STATE_ERROR;
                }
                break;

            case STATE_EXPONENT_PRE:  // optional + or - sign
                if( c == '+' || c == '-') {
                    if( buffer_char_add(c, &buffer) ) goto ERROR;
                }
                else {
                    ungetc(c,file);
                }
                status = STATE_EXPONENT_PRE2; 
                break;

            case STATE_EXPONENT_PRE2: // exponent must have at least one digit
                if(isdigit(c)) {
                    status = STATE_EXPONENT;
                }
                else {
                    status = STATE_ERROR;
                }
                ungetc(c, file);
                break;

            case STATE_EXPONENT:      // exponent
                if( isdigit(c)) {
                    if( buffer_char_add(c, &buffer) ) goto ERROR;
                }
                else if(delimiter(c)) {
                    if(  token_set_double(Token, buffer.data) ) {
                        status = STATE_ERROR;
                    }
                    ungetc(c, file);
                }
                else {
                    status = STATE_ERROR;
                }
                break;

            case STATE_DOUBLE_PRE: // double must have at least one digit in the decimal part
                if(isdigit(c)) {
                    status = STATE_DOUBLE;
                }
                else {
                    status = STATE_ERROR;
                }
                ungetc(c, file);
                break;

            case STATE_DOUBLE:   // double
                if(c == 'e' || c == 'E') { // double with exponent 
                    status = STATE_EXPONENT_PRE; 
                    if( buffer_char_add(c, &buffer) ) goto ERROR;
                }
                else if(isdigit(c)) {
                    if( buffer_char_add(c, &buffer) ) goto ERROR;
                }
                else if(delimiter(c)) {
                    token_set_double(Token, buffer.data);
                    ungetc(c, file);
                }
                else {
                    status = STATE_ERROR;
                }
                break;

            case STATE_STRING_PRE:    // string
                if(c == '\"') { // end of string
                    token_set_string(Token, buffer.data, STRING);
                }
                else if (c <= 31 || 255 < c) { // TODO upřesnit zadani 31-127 nebo 31-255
                    status = STATE_ERROR;
                }
                else if (c == '\\') {
                    status = STATE_ESCAPE;
                }
                else {
                    if( buffer_char_add(c, &buffer) ) goto ERROR;
                }
                break;
            
            // Escape sequence (\", \n, \t, \\ and octan number \ooo)
            
            case STATE_ESCAPE:    // TODO Escape \ddd 
                status = STATE_STRING_PRE;
                if( c == '\\' ) {
                    if( buffer_char_add('\\', &buffer) ) goto ERROR; 
                }
                else if( c == 'n'  ) {
                    if( buffer_char_add('\n', &buffer) ) goto ERROR; 
                } 
                else if( c == '\"' ) {
                    if( buffer_char_add('\"', &buffer) ) goto ERROR;
                }
                else if( c == 't'  ) {
                    if( buffer_char_add('\t', &buffer) ) goto ERROR;
                } 
                else if( '0' <= c && c < '4') { // first octan digit 
                    escape = (c-'0');
                    status = STATE_ESCAPE_OCT1;
                }
                else {
                    if( buffer_char_add(c, &buffer) ) goto ERROR;
                    status = STATE_ERROR;
                }
                break;
            case STATE_ESCAPE_OCT1:
                if( '0' <= c && c < '8') {  // second octan digit 
                    escape = escape * 8 + (c-'0');
                    status = STATE_ESCAPE_OCT2;
                }
                else {
                    if( buffer_char_add(c, &buffer) ) goto ERROR;
                    status = STATE_ERROR;
                }
                break;

            case STATE_ESCAPE_OCT2: // third octan digit
                if( '0' <= c && c < '8' ) { 
                    escape = escape*8 + (c-'0');
                    status = STATE_ESCAPE_OCT3;
                }
                else {
                    if( buffer_char_add(c, &buffer) ) goto ERROR;
                    status = STATE_ERROR;
                }
                break;
            case STATE_ESCAPE_OCT3:     // correct octan number
                ungetc(c, file);
                if( 0 < escape ) {
                    buffer_char_add(escape, &buffer);
                    status = STATE_STRING_PRE;
                }
                else {
                    if( buffer_char_add(c, &buffer) ) goto ERROR;
                    fprintf(stderr, "%s\n", "Lex error: invalid octan sequence");
                    status = STATE_ERROR;
                }
            
            // Comment
            
            case STATE_LINE_COMMENT:    // in line comment

                if(c == '\n' || c == EOF) {         // end of line comment
                    status = STATE_START;
                    ungetc(c, file);
                }
                break;

            case STATE_BLOCK_COMMENT:   // in block comment
                if( c == EOF) {
                    status = STATE_ERROR;
                }
                else if(c == '*') {
                    status = STATE_BLOCK_COMMENT2;
                }
                break;

            case STATE_BLOCK_COMMENT2:  // end of block comment
                if( c == EOF) {
                    status = STATE_ERROR;
                }
                else if(c == '/') {
                    status = STATE_START;
                }
                else {
                    status = STATE_BLOCK_COMMENT; // not yet
                }
                break;

            //Operators
             
            case STATE_NOT_PRE:       // !=
                if( c == '=') {
                    token_set_type(Token, NOT);
                }
                else {
                    status = STATE_ERROR;
                }
                break;

            case STATE_ASIGN_PRE:     // = or ==
                if( c == '=') {
                    token_set_type(Token, EQUAL);
                }
                else {
                    token_set_type(Token, ASIGN);
                    ungetc(c, file);
                }
                break;

            case STATE_DIV_PRE:       // / or comment
                if( c == '/') {
                    status = STATE_LINE_COMMENT;
                    clean_buffer(&buffer);
                }
                else if( c == '*') {
                    status = STATE_BLOCK_COMMENT;
                    clean_buffer(&buffer);
                }
                else {
                    token_set_type(Token, DIV);
                    ungetc(c, file);
                }
                break;
            case STATE_LESS_PRE:      // < or <=
                if( c == '=') {
                    token_set_type(Token, ELESS);
                }
                else {
                    token_set_type(Token, LESS);
                    ungetc(c, file);
                }
                break;      
            case STATE_GREAT_PRE:     // > or >=
                if( c == '=') {
                    token_set_type(Token, EGREAT);
                }
                else {
                    token_set_type(Token, GREAT);
                    ungetc(c, file);
                }
                break;

            // Lexikal error
            case STATE_ERROR:
                ungetc(c, file);
                buffer_free(&buffer);
                clean_token(Token);
                Token->line = line;
                set_error(LEX_ERROR);
                fprintf(stderr, "Lexical error on line %i\n", line);
                return LEX_ERROR;
                break;

            default:
                goto ERROR;
        }
    }
    Token->line = line;
    buffer_free(&buffer);
    return NO_ERROR;

ERROR:      // Intern error (allocated memory, ...)
    buffer_free(&buffer);
    clean_token(Token);
    set_error(INTERN_ERROR);
    return INTERN_ERROR;      

}
