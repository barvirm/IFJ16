/* 
 * Name: 	Tests for parser
 * Author: 	Petr Malaník
 * Email: 	xmalan02@stud.fit.vutbr.cz
 * Login:	xmalan02
 * Date:	1.11.2016
 * Team members:	Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 * 
 * Description:	Pack of test for parser
 
*/

#ifndef test_parser_h_xmalan02
#define test_parser_h_xmalan02



#include "../../parser.h"
#include "../../scanner.h"
#include "../../buffer.h"
#include "../../token.h"
#include "../../error.h"
#include "../../postfix.h"


#include <stdbool.h>


// comments for doxygen
void open_next_file();

int test_loop();

void print_summary(float total,float passed);

#endif