#include "test_parser.h"

FILE *input_list;
FILE *input;

extern token_t *token;
extern token_t *error_token;

int correct_line = 0;
int correct_error = 0;
int input_file = 0;
bool more = false;
char test[99];
char actual_test[99];

#define NUMBER_OF_TESTS 84
int main(int argc, char const *argv[])
{

	if (argc == 2 && (!(strcmp(argv[1],"-m"))) )
		more = true;
	else if(argc == 2 && (input_file=strtol(argv[1],NULL,10)) ){
		printf("Testing only file %d\n",input_file);
		more = true;
	}

	if (argc==3){
		if (( (!(strcmp(argv[1],"-m"))) &&  (input_file=strtol(argv[2],NULL,10))  )  )
			more = true;
		else if (( (!(strcmp(argv[2],"-m"))) &&  (input_file=strtol(argv[1],NULL,10))  )  )
			more = true;
	}

	input_list = fopen("./tests/Parser/List_of_tests","r");

	if(input_file==0){
		print_summary(NUMBER_OF_TESTS,test_loop());
	}
	else{
		print_summary(1,test_loop());
	}
	

	return 0;

}
void open_next_file(){
	
	char buffer[99];

	fscanf(input_list,"%s\n",actual_test);

	sprintf(buffer, "./tests/Parser/%s",actual_test);
	input = fopen(buffer,"r");
	fscanf(input,"%i %i %[^\n]",&correct_error,&correct_line,test);
}

int test_loop(){
	int test_passed = 0;
	int test_number = 1;
	for (; test_number < NUMBER_OF_TESTS+1; test_number++){
		error_number = 0;
		open_next_file();
		if (input_file != 0){
			if (input_file != test_number)
			{
				continue;
			}
		}

		parser(input,NULL);

		int token_line=error_token->line; //TODO 

		if (more!=true){

			if (correct_line!= token_line || correct_error!=error_number){
				printf("\n\t\tTEST %i\t\tFILE: %s\n",test_number,actual_test);
				printf("EXPECTED\tERROR: %i\tError on line: %i\t%s\n",correct_error,correct_line,test);
				printf("OBTAINED\tERROR: %i\tError on line: %i\n",error_number,token_line);
			}
			else
				test_passed++;
		}	
		else{
			printf("\n\t\tTEST %i\t\tFILE: %s\n",test_number,actual_test);
			printf("EXPECTED\tERROR: %i\tError on line: %i\t%s\n",correct_error,correct_line,test);
			printf("OBTAINED\tERROR: %i\tError on line: %i\n",error_number,token_line);
			if (correct_line== token_line && correct_error==error_number){
					test_passed++;
			}
		}
		error_token->line = 0;
	}
	return test_passed;
}

void print_summary(float total,float passed){
	printf("   _____                                            \n");
	printf("  / ____|                                           \n");
	printf(" | (___  _   _ _ __ ____ _ __ ____ ___ _ _ __ _   _ \n");
	printf(" |____  | | | | '_ ` _  | '_ ` _  V  _  | '__| | | |\n");
	printf("  ____) | |_| | | | | | | | | | | | (_| | |  | |_| |\n");
	printf(" |______|_____|_| |_| |_|_| |_| |_|___,_|_|  |___, |\n");
	printf("                                               __/ |\n");
	printf("                                              |___/ \n");

	printf("\t\t TOTAL  :%.0f\n",total);
	printf("\t\t PASSED :%.0f\n",passed);
	printf("\t\t FAILED :%.0f\n",total-passed);

	printf("\t\t SUCCESS RATE :%.2f%%\n\n",(passed/(passed+(total-passed)))*100);

}
