/*
 * Name:    interpret.h
 * Author:  Karel ONDŘEJ, FIT
 * Email:   xondre09@stud.fit.vutbr.cz
 * Login    xondre09
 * Date:    15. 11. 2016
 * Team members:  Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 *
 * Description: 
 * ...
 */


#ifndef INTERPRET_H_INCLUDED
#define INTERPRET_H_INCLUDED

#include <string.h>

#include "ilist.h"
#include "ial.h"
#include "error.h"
#include "storage.h"
#include "ilist.h"
#include "buildin.h"



int interpret_run(ilist_t* ilist);

#endif
