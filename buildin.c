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


#include "buildin.h"


int readInt() {

    int number = 0;
    int base = 10;
    char c;

    while( (c = fgetc(stdin)) != EOF && c != '\n') {
        if ('0' <= c && c <= '9' && ((INT_MAX - (c - '0')) / base) >= number) {
            number *= base;
            number += (c - '0');
        }
        else {
            set_error(RUNTIME_INPUT_ERROR); // error code 7
            return -1;
        }
    }

    return number;
}

char *readString() {

    int i = 0, o = 1, escaped = false, ERROR = false;
    int c;
    char *buffer =(char *) malloc(100), octal_string[4] = "";

    while( (c = fgetc(stdin)) != EOF && c != '\n') {
        // nejvetsi int ma 10 cifer takze kdyz tam nekdo zada vic jak 10 znaku tak je to automaticky error nebo pokud zadate neco jinyho nez cislo tak je error
        if (c < 32)
            ERROR = true;
        if (ERROR == false){

            if (c == '\\' && escaped == false){
                escaped = true;
                continue;
            }
            if (escaped){       // escape sequence
                if ('0' <= c && c <= '3'){ // write escaped octal char
                    octal_string[0] = c;
                    while(o < 3 && (c = fgetc(stdin)) != EOF){
                        if ('0' <= c && c <= '7'){
                            octal_string[o] = c;
                            ++o;
                        } else {
                            ERROR = true;
                            break;
                        }
                    }
                    if (o != 3){
                        ERROR = true;
                        continue;
                    }
                    if (ERROR == true)
                        continue;
                    buffer[i] = (int)strtol(octal_string, NULL, 8);
                    o = 0;
                }
                else{
                    switch(c){      // other posible escaped chars
                        case '"':
                            buffer[i] = '"';
                            break;
                        case '\\':
                            buffer[i] = '\\';
                            break;
                        case 'n':
                            buffer[i] = '\n';
                            break;
                        case 't':
                            buffer[i] = '\t';
                            break;
                        default:
                            ERROR = true;
                            continue;
                    }
                }
                i++;
                escaped = false;
                continue;
            }
            buffer[i++] = c;
        }
    }

    buffer[i] = '\0';
    
    if (ERROR == false)
        return buffer;
    else {
        set_error(RUNTIME_INPUT_ERROR);
        return NULL; // errro 7
    }
}


double readDouble() {

    double ret;
    buffer_t str;
    init_buffer(&str);
    int c;

    while((c = fgetc(stdin)) != EOF && c != '\n') {
        buffer_char_add(c, &str);
    }

    if(sscanf( str.data, "%lf", &ret) != 1  || errno){
        set_error(RUNTIME_INPUT_ERROR);
    }

    buffer_free(&str);

    return ret;
    /*
    double number = 0;
    char input[60] = "";
    int i = 0;
    int ERROR = false;
    char c;

    while( (c = fgetc(stdin)) != EOF && c != '\n' && i < 60){
        if ( ('0' <= c && c <= '9') || c == 'e' || c == 'E' || c == '.' || c == '+' || c == '-'){
            input[i] = c;
            i++;
        } else {
            ERROR = true;
        }
    }

    if (!ERROR && sscanf(input, "%lf", &number) == 1)
        return number;
    else {
        set_error(RUNTIME_INPUT_ERROR);
        return -1;  // error code 7
    }
    */
}


void reverse(char *s) {

    int i = 0;
    int j = strlen(s)-1;
    char c;

    while ( i<j ) {
        c = s[i];
        s[i++] = s[j];
        s[j--] = c;
    }
}


char *double_to_string(double d) {

    int size = snprintf(NULL,0,"%g",d); // return size request for malloc

    // It will be mess with malloc -> free
    // TODO Better solution
    char *s = (char *) malloc (size+1);
    if ( s == NULL ) return NULL;

    if ( 0 > sprintf(s,"%g",d)) return NULL; // fill buffer with double

    return s;
}

char *int_to_string(int i) {

    int tmp = i;
    int lenght = 0;

    while ( tmp != 0 ) { // count number of digits
        tmp/=10;
        lenght++;
    }

    // It will be mess with malloc -> free
    // TODO Better solution
    char *s =(char *) malloc(sizeof(char)*lenght);
    if ( s == NULL ) return NULL;

    sprintf(s,"%i",i);
    return s;
}


int length(char *string) {
    return (int)strlen(string);
}


int compare(char *s1, char *s2) {

    while ( *s1 != '\0' && *s2 != '\0' ) {

        if ( *s1 > *s2 ) return 1;
        else if( *s1 < *s2) return -1;
        else { s1++; s2++; }
    }

    if ( *s1 != '\0' ) return 1;
    else if( *s2 != '\0') return -1;
    return 0;
}

char *substr(char *string, int i, int n) {

    char *s =(char *) malloc(sizeof(char) *(n+1));
    if ( s == NULL ){
        set_error(INTERN_ERROR);
        return NULL;
    }

    if(i < 0) {
        set_error(RUNTIME_OTHERS_ERROR);
        return NULL;
    }
    int c = 0;
    for (; c < n; c++, i++) {
        s[c] = string[i];
        if ( string[i] == '\0' ) {
            free(s);
            set_error(RUNTIME_OTHERS_ERROR);
            return NULL;
        }
    }
    s[c] = '\0';
    return s;
}

#ifdef DEBUG

int main() {

    // int a = 123456;
    // double w = 123.98798456;
    // char *s = "123";
    // printf("%s\n",substr(s,0,50));
    // printf("%s\n",int_to_string(a));
    // printf("%s\n",double_to_string(w));
    // printf("%d\n", lenght("x\n\tz"));
    // printf("cislo je: %d\n", readInt());
    // printf("%s",readString());
    // readString();
    printf("%lf\n", readDouble());
    return 0;

}

#endif
