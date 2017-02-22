/* 
 * Name:    ial.h
 * Author:  David Smékal, Jakub Sochůrek, Marek Barvíř
 * Email:   xsmeka13@stud.fit.vutbr.cz, xsochu01@stud.fit.vutbr.cz, xbarvi00@stud.fit.vutbr.cz
 * Login:   xsmeka13, xsochu01, xbarvi00
 * Date:    28/9/2016
 * Team members:    Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 * 
 * Description: File include source code for biuld-in functions that uses IAL algorithms and binary tree
*/

#include "ial.h"


// get maximum of two integers
int max(int a, int b)
{
    return (a > b) ? a : b;
}

//calculates the first jump
void bma(char *str, int chr[STR_MAX], int size)
{
    int i;
    for (i = 0; i < STR_MAX; i++) 
    {
        chr[i] = -1;
    }

    for (i = 0; i < size; i++) 
    {
        chr[(int)str[i]] = i;
    }
}
/*-------------------------------------------------------------------------
*                               Binary tree
*--------------------------------------------------------------------------
*/


void init_data(node_data_t *d) {
    
    d->struct_type = 'x';
    d->inicializate = false;
    d->defined = false;
    
    d->data_u.ivar = 0;
    d->func_type = (char *) GBC_malloc(sizeof(char)*d->type_size_max);
    if ( d->func_type == NULL )
        set_error(RUNTIME_OTHERS_ERROR);

    d->type_size_max = 1;
    d->type_size_act = 0;

}

node_t *bin_tree_insert(node_t **root, char *id, node_data_t *data) {

    if ( *root == NULL ) {
        *root =(node_t *) GBC_malloc(sizeof(node_t));
        (*root)->id = id;
        (*root)->data = data;
        (*root)->left = NULL;
        (*root)->right = NULL;
        
    }
    else if ( strcmp(id,(*root)->id) < 0 )  // z < a - z to the left side
        bin_tree_insert(&(*root)->left,id,data);
    else if ( strcmp(id,(*root)->id) > 0 )  // z > a - z to the right side
        bin_tree_insert(&(*root)->right,id,data);
    else if ( strcmp(id,(*root)->id) == 0 )  // a == a, duplicate error
        return NULL;

    return NULL;
}

node_t *search_note(node_t **root,char *id) {
    if ( root == NULL ) { return NULL ;}    
    if ( *root != NULL ) {
        if ( strcmp(id,(*root)->id) == 0 )  // a == a, duplicate error
            return (*root);
        else if ( strcmp(id,(*root)->id) < 0 )  // z < a - z to the left side
            return search_note(&(*root)->left,id);
        else // z > a - z to the right side
            return search_note(&(*root)->right,id);
    }
    else 
        return NULL;
}

void free_node(node_t *node) {
    
    GBC_free(node->data->func_type);
    GBC_free(node->data);
    GBC_free(node->id);
    node->data = NULL;
    node->left = NULL;
    node->right = NULL;

    GBC_free(node);
    node = NULL;
}

void ReplaceByRightmost (node_t *PtrReplaced, node_t **RootPtr) {

    if ( (*RootPtr)->right == NULL ) {
        PtrReplaced->id = (*RootPtr)->id;
        PtrReplaced->data = (*RootPtr)->data;
        PtrReplaced = (*RootPtr);
        (*RootPtr) = (*RootPtr)->left;
        free_node( PtrReplaced );
    }
    else { 
        ReplaceByRightmost(PtrReplaced,&(*RootPtr)->right);
    }
}

void delete_note (node_t **Root, char *id)      {

    if ( (*Root) == NULL )
        return;

    if ( strcmp(id,(*Root)->id) < 0 )  // z < a - z to the left side
        delete_note(&(*Root)->left,id);
    if ( strcmp(id,(*Root)->id) > 0 )  // z < a - z to the left side
        delete_note(&(*Root)->right,id);
    else {
        node_t *tmp = (*Root);
        if ( tmp->right == NULL ) {
            (*Root)=tmp->left;
            free_node(tmp);
        }
        else if ( tmp->left == NULL ) {
            (*Root)=tmp->right;
            free_node(tmp);
        }
        else 
            ReplaceByRightmost(tmp,&(*Root)->left);
    }
} 


void bin_tree_dispose(node_t **root) {
    if ( *root != NULL ) {
        bin_tree_dispose(&(*root)->left);
        bin_tree_dispose(&(*root)->right);
        free_node(*root);
    }
}

bool bin_tree_trought(node_t **root,int (*fce)(node_t **root)) {

    if ( *root == NULL ) { return true; }
    if ( !bin_tree_trought(&(*root)->left,fce) ) return false;
    if ( !bin_tree_trought(&(*root)->right,fce) ) return false;

    if ( !fce(root) ) { return false; }
    /*
    if ( (*root)->data->struct_type == 'f' )
        return (*root)->data->defined;
    */

    return true;
}


#ifdef DEBUG_TREE

int main() {
    data_t *b = (data_t *) GBC_malloc(sizeof(data_t));
    b->i = 25;
    data_t *c = (data_t *) GBC_malloc(sizeof(data_t));
    c->i = 5;
    data_t *a = (data_t *) GBC_malloc(sizeof(data_t));
    a->i = 25;
    node_t *root = NULL;
    insert(&root,"e",b);
    insert(&root,"a",a);
    insert(&root,"j",c);
    insert(&root,"l",c);
    delete_note(&root,"e");
    return 0;
}

#endif

/*-------------------------------------------------------------------------
*                  Boyer–Moore string search algorithm
*--------------------------------------------------------------------------
*/


//Boyer–Moore string search bad char algorithm 
int find(char *string, char *search)
{
    int size_sea = strlen(search);      //size of search string
    int size_str = strlen(string);      //size of input string
    int chr[STR_MAX];

    bma(search, chr, size_sea);

    int pos = 0;    //shift of pattern
    while (pos <= (size_str - size_sea))    
    {
        int pointer = size_sea - 1;     //position of the current character
        while (pointer >= 0 && search[pointer] == string[pos+pointer]) 
        {
            pointer--;
        }

        if (pointer < 0)
        {
            return pos;     //find searching string
        }

        else 
        {
            pos += max(1, pointer - chr[(int)string[pos+pointer]]);       //moves pattern after wrong char in text
        }
    }

    return -1;
}

/*-------------------------------------------------------------------------
*                               List-merge sort
*--------------------------------------------------------------------------
*/
// creates pom array
void prepare(mainList *L, int string_lenght, int *pom, char *string){

    int i = 1;
    InsertLast(&*L, 0);
    while (i < string_lenght){
        if (string[i-1] < string[i]){
            pom[i-1] = i;
        } else {
            InsertLast(&*L, i);
            pom[i-1] = -1;
        }
        i++;
    }
    pom[i-1] = -1;
}

// fills subList with elemLists 
int createLists(mainList *L, int *pom) {

    subListPtr actual = L->First;
    subListPtr nextList = actual->nextList;
    while (actual != NULL){
        int index = actual->index;
        while (index != -1) {
            elemListPtr newElemList = GBC_malloc(sizeof(struct elemList));
            if (newElemList == NULL){
                GBC_free(newElemList);  // error 99
                return -1;
            }
            else {
                newElemList->index = index;
                newElemList->nextElement = NULL;
                if (actual->thisList == NULL){
                    actual->thisList = newElemList;
                    actual->last = newElemList;
                } else {
                    actual->last->nextElement = newElemList;
                    actual->last = newElemList;
                }
                index = pom[index];
            }
        }
        actual->index = -1;
        actual = nextList;
        if (actual != NULL)
            nextList = actual->nextList;
    }
    return 1;
}

// merging two elemList to new subList contaning new sorted elemList
subListPtr merge(elemListPtr first, elemListPtr second, char *string){
    subListPtr mergedList = GBC_malloc(sizeof(struct subList));
    if (mergedList == NULL){
        GBC_free(mergedList); // error 99
        return NULL;
    }
    else {
        mergedList->thisList = NULL;
        while (first != NULL || second != NULL){
            elemListPtr newElemList = GBC_malloc(sizeof(struct elemList));
            if (newElemList == NULL){
                GBC_free(mergedList); // error 99
                GBC_free(newElemList);
                return NULL;
            }
            else {
                if (first != NULL){
                    if (second != NULL){
                        if (string[first->index] <= string[second->index]){
                            newElemList->index = first->index;
                            first = first->nextElement;
                        } else {
                            newElemList->index = second->index;
                            second = second->nextElement;
                        }
                    } else {
                        newElemList->index = first->index;
                        first = first->nextElement;
                    }
                } else {
                    newElemList->index = second->index;
                    second = second->nextElement;
                }

                newElemList->nextElement = NULL;
                if (mergedList->thisList == NULL){
                    mergedList->thisList = newElemList;
                    mergedList->last = newElemList;
                } else {
                    mergedList->last->nextElement = newElemList;
                    mergedList->last = newElemList;
                }
            }
        }
    }
    return mergedList;
}

/* List-merge-sort algorithm used to sort string (char array)
 * to upward form
 */
char *sort(char *string) {

    int string_lenght = (int)strlen(string);
    int pom[string_lenght];
    mainList L;
    elemListPtr first;
    elemListPtr second;
    subListPtr mergedList;

    InitList(&L);
    prepare(&L, string_lenght, pom, string);
    int createListsOK = createLists(&L, pom);
    if (createListsOK == -1) {
        set_error(INTERN_ERROR);
        return NULL;
    }

    setFirstActive(&L);
    while(L.First != L.Last){
        getElemList(&L, &first);
        NextIsFirst(&L);
        setFirstActive(&L);
        if (Active(&L)){
            getElemList(&L, &second);
            mergedList = merge(first, second, string);  // merging
            if (mergedList == NULL){    // Intern error
                GBC_free(L.First);
                set_error(INTERN_ERROR);
                return NULL;
            }
            L.Last->nextList = mergedList;      // connect new list to last
            L.Last = mergedList;                // mark new list as Last
            clearElemList(first);               // destroy used lists
            clearElemList(second);
            NextIsFirst(&L);
            setFirstActive(&L);
        }
    }
    // Create new sorted string
    char *newString = (char*)GBC_malloc(sizeof(char) * (string_lenght + 1));
    if (newString == NULL) {        // Intern error
        clearElemList(L.Last->thisList);
        GBC_free(L.Last);
        set_error(INTERN_ERROR);
        return NULL;
    }
    int i = 0;
    elemListPtr next = L.Last->thisList;
    while (next != NULL){
        newString[i] = string[next->index]; // Copying from list to array
        next = next->nextElement;
        i++;
    }
    newString[i] = '\0';
    clearElemList(L.Last->thisList);
    GBC_free(L.Last);
    return newString;
}

/*-------------------------------------------------------------------------
*                  Linked list's functions declaration
*--------------------------------------------------------------------------
*/

void InitList (mainList *L) {
    L->First = NULL;
    L->Act = NULL;
    L->Last = NULL;
}

void InsertLast (mainList *L, int val) {
    subListPtr new = GBC_malloc(sizeof(struct subList));
    if (new == NULL)
        ;   // chyba 99
    else {
        new->nextList = NULL;
        new->thisList = NULL;
        new->index = val;
        if (L->First == NULL)
            L->First = new;
        if (L->Last != NULL)
            L->Last->nextList = new;
        L->Last = new;
    }
}

void setFirstActive (mainList *L) {
    L->Act = L->First;
}

void getElemList (mainList *L, elemListPtr *ptr){
    if (L->First == NULL)
        *ptr = NULL;
    else{
        *ptr = L->First->thisList;
    }
}

void clearElemList(elemListPtr ptr) {
    elemListPtr tmp = ptr;
    while (tmp != NULL){
        tmp = ptr->nextElement;
        GBC_free(ptr);
        ptr = tmp;
    }
}

void NextIsFirst (mainList *L) {
    if (L->First == L->Act)
        L->Act = NULL;
    subListPtr next = L->First->nextList;
    if (L->First != NULL && L->First != L->Last)
        GBC_free(L->First);
    L->First = next;
}

int Active (mainList *L) {
    return (L->Act != NULL);
}

/*-------------------------------------------------------------------------
*                       Test program
*--------------------------------------------------------------------------
*/
#ifdef DEBUG_SORT

int main() {
    GBC_Init();
    char keyboard[] ="qwertzuiopasdfghjklyxcvbnm";
    char* str = sort(keyboard);
    printf("result:  %s\n",str);
    GBC_free(str);
    printf("----------GBC INFO----------\n");
    GBC_Print_size();
    GBC_Display_ptr();
    GBC_Dispose();
    GBC_Print_size();
    GBC_Display_ptr();
    return 0;

}

#endif
