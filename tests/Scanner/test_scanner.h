/* 
 * Name: 	Tests for scanner
 * Author: 	Petr Malaník
 * Email: 	xmalan02@stud.fit.vutbr.cz
 * Login:	xmalan02
 * Date:	17.10.2016
 * Team members:	Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 * 
 * Description:	Pack of test for scanner
 * Usage: type -l for short(less) report
 * 		  type number for run on specific test
*/

#ifndef test_scanner_h_xmalan02
#define test_scanner_h_xmalan02

#include "../../scanner.h"
#include "../../buffer.h"
#include "../../token.h"

#include <stdbool.h>


// comments for doxygen

void init_test(int order);

void start_test();

void print_result(bool error_flag,int test_number);

void print_summary(float failed,float passed);

int test_next();


#endif