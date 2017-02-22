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

#include "ifj16.h"

extern token_t *error_token;

int main(int argc, char const *argv[]){
	//check of arguments
	if (argc != 2){
		set_error(INTERN_ERROR);
		fprintf(stderr,"%s:ERROR:INTERN_ERROR\n",argv[1]);
		return 99;
	}
	GBC_Init();

	//inicialization of instruction list
	ilist_t ilist;
	ilist_init(&ilist);
	ilist_t init;
	ilist_init(&init);

	//opening of file
	FILE *input = fopen(argv[1],"r");
	if (input == NULL){
		set_error(INTERN_ERROR);
		fprintf(stderr,"%s:ERROR:INTERN_ERROR\n",argv[1]);
		return 99;
	}

	//run parser 
	parser(input, &ilist, &init);
	fclose(input);
	//run schemantic
	sematic_expression(&init);
	sematic_run(&ilist);
	//run interpret
	interpret_run(&init);
	interpret_run(&ilist);
	ilist_dispose_list(&ilist);

	//error number evaluation
	if (error_number >= 1 && error_number <= 10){
		fprintf(stderr,"%s:ERROR:%i:%s\n",argv[1],error_token->line,Error_name[error_number]);
	}
	else if (error_number == 99){
		fprintf(stderr,"%s:ERROR:INTERN_ERROR\n",argv[1]);
	}
	
	//Garbage collector - free all memory
	GBC_Dispose();

	return error_number;
}

