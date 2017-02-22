/*
 * Name:    storage.c
 * Author:  Karel ONDŘEJ, FIT
 * Email:   xondre09@stud.fit.vutbr.cz
 * Login    xondre09
 * Date:    6. 10. 2016
 * Team members:  Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 *
 * Description: 
 * ...
 */

#include "storage.h"

/*
 * Initialization data storage
 */
void storage_init(storage_t* s) {
    s->Local  = NULL;
    s->Temp   = frame_ctor();
}

void storage_disponse(storage_t* s) {
    while(!storage_empty(s)) {
        storage_pop(s);
    }
    if (s->Temp != NULL) {
        frame_dtor(s->Temp);
    }
}

void storage_push(storage_t* s, frame_t* frame) {
    frame->next = s->Local;
    s->Local = frame;
}

void storage_push_temp(storage_t* s) {
    storage_push(s, s->Temp);
    s->Temp = frame_ctor();
}

void storage_pop(storage_t* s) {
    if (!storage_empty(s)) {
        frame_t* del = s->Local;
        s->Local = s->Local->next;
        frame_dtor(del);
    }
}

int storage_empty(storage_t* s) {
    return s->Local == NULL;
}

frame_t* frame_ctor() {
    frame_t* new = malloc(sizeof(frame_t));

    if (new == NULL) {
        return new;
    }
    new->Count  = 0;
    new->Data   = NULL;
    new->Result = NULL;
    new->next   = NULL;

    return new;
}

void frame_dtor(frame_t* f) {
    frame_clear(f);
    free(f);
}

void frame_clear(frame_t* f) {
 
    while ( f->Count > 0) {
        f->Count--;
        if (f->Data[f->Count].type == tSTRING && f->Data[f->Count].data.svar != NULL) { 
            free(f->Data[f->Count].data.svar); 
        } 
    }
    free(f->Data);
    f->Count  = 0;
    f->Data   = NULL;
    f->Result = NULL;
}


int frame_append(frame_t* f, var_t v) {
    if ( f->Count % (FRAME_CHUNK - 1) == 0) {
        var_t* new = realloc(f->Data, sizeof(var_t)*(f->Count + FRAME_CHUNK));
        if (new == NULL) {
            return INTERN_ERROR;
        }
        f->Data = new;
    }
    if(v.type == tSTRING) {
        f->Data[f->Count].type = tSTRING;
        f->Data[f->Count].data.svar = malloc(sizeof(char)*(strlen(v.data.svar)+1));
        strcpy(f->Data[f->Count].data.svar, v.data.svar); 
    }
    else {
        f->Data[f->Count] = v;
    }
    f->Count++;
    return NO_ERROR;
}

/*
 * Reserved empty place in frame
 */
int frame_reserve(frame_t* f, int val) {
    int err = NO_ERROR;
    var_t v;
    v.type = tEMPTY;
    v.data.svar = NULL; 

    while( val-- > 0) {
        if( (err = frame_append(f, v)) != NO_ERROR ) {
            return err;
        }
    }
    return NO_ERROR;
}
/*
 * Get value in frame on offset
 */
int frame_get(frame_t* f, int offset,  var_t* dest) {
    if (f == NULL || (0 > offset || offset >= f->Count)) {
        fprintf(stderr, "Error: Neoprávněný přístup do paměti\n");
        return INTERN_ERROR;
    }
    
    if (f->Data[offset].type == tEMPTY) {
        fprintf(stderr, "Error: work with uninicializated variable (offset in frame: %i)\n", offset);
        return RUNTIME_UNINIT_ERROR;
    }

    *dest = f->Data[offset];
    return NO_ERROR;
}

/*
 * Get value in frame on offset
 */
int frame_set(frame_t* f, int offset, var_t val) {
    if (f == NULL || (0 > offset || offset >= f->Count) ) {
        fprintf(stderr, "Error: Neoprávněný přístup do paměti\n");
        return INTERN_ERROR;
    }
    if(val.type == tSTRING) {
        free(f->Data[offset].data.svar);
        f->Data[offset].type = tSTRING;
        f->Data[offset].data.svar = malloc(sizeof(char)*(strlen(val.data.svar)+1));
        strcpy(f->Data[offset].data.svar, val.data.svar);
    }
    else {
        f->Data[offset] = val;
    }
    return NO_ERROR;
}

/*
 * Get value in top frame on offset
 */
int storage_get_local(storage_t* s, int offset,  var_t* dest) {
    return frame_get(s->Local, offset, dest);
}


/*
 * Set value in top frame on offset
 */
int storage_set_local(storage_t* s, int offset,  var_t val) {
    return frame_set(s->Local, offset, val);
}