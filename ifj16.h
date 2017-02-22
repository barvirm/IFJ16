/* 
 * Name: 	Main
 * Author: 	Petr Malaník
 * Email: 	xmalan02@stud.fit.vutbr.cz
 * Login:	xmalan02
 * Date:	1.11.2016
 * Team members:	Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 * 
 * Description:	Top level part of whole program. 
 *				Initialization and launching of single parts.
 *				Opening file and argument check.
 *
 */

#ifndef test_ifj16_h_xmalan02
#define test_ifj16_h_xmalan02

const char* Error_name[] = {
/*0 */"NO_ERROR",              	///< Empty
/*1 */"LEX_ERROR",            	///< Integer
/*2 */"SYNTAX_ERROR",           ///< Double
/*3 */"SEM_UNDEF_ERROR",        ///< String
/*4 */"SEM_UNCOMP_ERROR",       ///< Simple identifier
/*5 */"", 						///< Fully qualified identifier
/*6 */"SEM_OTHER_ERROR",        ///< =
/*7 */"RUNTIME_INPUT_ERROR",    ///< *
/*8 */"RUNTIME_UNINIT_ERROR",   ///< /
/*9 */"RUNTIME_DIV_ZERO_ERROR", ///< +
/*10*/"RUNTIME_OTHERS_ERROR",   ///< -
    };

#include "token.h"
#include "parser.h"
#include "ial.h"
#include "ilist.h"
#include "interpret.h"

#include "scanner.h"
#include "buffer.h"

#include "error.h"
#include "postfix.h"
#include "gbc.h"

#include "sematic.h"


#include <stdbool.h>



#endif