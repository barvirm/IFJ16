/*
 * Name:    semantic.h
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
#include <stdio.h>
#include "error.h"
#include "ial.h"
#include "storage.h"

/*
 * Sematic check for initialization instruction list.
 */
int sematic_run(ilist_t* ilist);

/*
 * Sematic check for program instruction list.
 */
int sematic_expression(ilist_t* ilist);