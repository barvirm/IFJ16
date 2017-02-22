
#include "test_scanner.h"


const char* tokens[] = {
/*0 */"EMPTY",              ///< Empty
/*1 */"INTEGER",            ///< Integer
/*2 */"DOUBLE",             ///< Double
/*3 */"STRING",             ///< String
/*4 */"SIMPLE_ID",          ///< Simple identifier
/*5 */"FULLY_QUALIFIED_ID", ///< Fully qualified identifier

/*6 */"ASIGN",              ///< =
/*7 */"MUL",                ///< *
/*8 */"DIV",                ///< /
/*9 */"PLUS",               ///< +
/*10*/"MINUS",              ///< -
/*11*/"LESS",               ///< <
/*12*/"ELESS",              ///< <=
/*13*/"GREAT",              ///< >
/*14*/"EGREAT",             ///< >=
/*15*/"NOT",                ///< !=
/*16*/"EQUAL",              ///< ==
/*17*/"BRACKET_LEFT",       ///< (
/*18*/"BRACKET_RIGHT",      ///< )
/*19*/"BRACE_LEFT",       	///< {
/*20*/"BRACE_RIGHT",      	///< }
/*21*/"SEMI",               ///< ;
/*22*/"COMMA",              ///< ,

/*23*/"RES_BOOLEAN",        ///< boolean
/*24*/"RES_BREAK",          ///< break
/*25*/"RES_CLASS",          ///< class
/*26*/"RES_CONTINUE",       ///< continue
/*27*/"RES_DO",             ///< do
/*28*/"RES_DOUBLE",         ///< double
/*29*/"RES_ELSE",           ///< else
/*30*/"RES_FALSE",          ///< false
/*31*/"RES_FOR",            ///< for
/*32*/"RES_IF",             ///< if
/*33*/"RES_INT",            ///< int
/*34*/"RES_RETURN",         ///< return
/*35*/"RES_STRING",         ///< String
/*36*/"RES_STATIC",         ///< static
/*37*/"RES_TRUE",           ///< true
/*38*/"RES_VOID",           ///< void
/*39*/"RES_WHILE",          ///< while

/*40*/"END_OF_FILE"        ///< End of file    
    };

token_t *token;

FILE *result;
FILE *input;
data_t token_data;
bool less;

int main(int argc,char *argv[]){

	int input_file = 0;



	if (argc == 2 && (!(strcmp(argv[1],"-l"))) )
		less = true;
	else if(argc == 2 && (input_file=strtol(argv[1],NULL,10)) )
		printf("Test %d\n",input_file);

	if (argc==3){
		if (( (!(strcmp(argv[1],"-l"))) &&  (input_file=strtol(argv[2],NULL,10))  )  )
			less = true;
		else if (( (!(strcmp(argv[2],"-l"))) &&  (input_file=strtol(argv[1],NULL,10))  )  )
			less = true;
	}

    token_t t;
    init_token(&t);
    token = &t;

    if (input_file!=0){
    	init_test(input_file);
    	init_scanner(input);
    	start_test();
    }
	else {
		for (int i = 1; i <= 3; ++i){
			init_test(i);
    		init_scanner(input);
    		printf("------------------------TEST %i----------------------\n",i);
    		start_test();
		}
	}

	return 0;
}

void init_test(int order){
	char buffer[100];

	sprintf(buffer, "./tests/Scanner/test_scanner_%i.ifj16", order);

	input = fopen(buffer,"r");


	sprintf(buffer, "./tests/Scanner/test_scanner_%i_result", order);

	result = fopen(buffer,"r");

}

void start_test(){
	int file_end = 0;

	if (less != true)
    	printf("--------+----------+----+--------------------\n");

    while(file_end==0){
    	file_end = test_next();
    }

}

void print_result(bool error_flag,int test_number){
	if (error_flag==true){
		printf("\n  %i\t|   TEST FAIL   |     XXXXXXXXXX     ",test_number);
	}
	else{
		printf("\n  %i\t|   TEST PASS   |                    ",test_number);
	}
}

void print_summary(float failed,float passed){
	printf("   _____                                            \n");
	printf("  / ____|                                           \n");
	printf(" | (___  _   _ _ __ ____ _ __ ____ ___ _ _ __ _   _ \n");
	printf(" |____  | | | | '_ ` _  | '_ ` _  V  _  | '__| | | |\n");
	printf("  ____) | |_| | | | | | | | | | | | (_| | |  | |_| |\n");
	printf(" |______|_____|_| |_| |_|_| |_| |_|___,_|_|  |___, |\n");
	printf("                                               __/ |\n");
	printf("                                              |___/ \n");

	printf("\t\t TOTAL  :%.0f\n",passed+failed);
	printf("\t\t PASSED :%.0f\n",passed);
	printf("\t\t FAILED :%.0f\n",failed);

	printf("\t\t SUCCESS RATE :%.2f%%\n\n",(passed/(passed+failed))*100);

}

int test_next(){
	static int test_number = 1;
	int token_type = 0;
	int token_data_int = 0;
	double token_data_double = 0;
	char token_data_string[100] = "";
	int end_flag = 0;
	static float failed = 0;
	static float passed = 0;

	fscanf(result,"%i",&token_type);	

	if (token_type == 40){
		end_flag = 2;
	}
	get_token(token);

	if (token->type == 40){
		end_flag = 1;
	}


	
	if(less!=true)printf("  %i\t| OBTAINED | %i\t| %s",test_number,token->type,tokens[token->type]);

	//print token data
	switch((int)token->type){
		case 1:	//Integer
			if(less!=true)printf("\t%i\t\t",token->data.ivar);
			break;
		case 2:	//Double
			if(less!=true)printf("\t%lf\t\t",token->data.dvar);
			break;
		case 3:	//String
			break;
		case 4: //Simple_ID
		case 5:	//Fully_qualified_ID
			if(less!=true)printf("\t%s\t\t",token->data.svar);
			break;
		default:
			if(less!=true)printf("\t\t\t");
			break;

	}
	if(less!=true)printf("\n");

	//print result data
	if(less!=true)printf("  %i\t| EXPECTED | %i\t| %s",test_number,token_type,tokens[token_type]);
	switch((int)token_type){
		case 1:	//Integer
			fscanf(result,"|%i\n",&token_data_int);
			if(less!=true)printf("\t%i",token_data_int);
			break;
		case 2:	//Double
			fscanf(result,"|%lf\n",&token_data_double);
			if(less!=true)printf("\t%lf",token_data_double);
			break;
		case 3:	//String
			fscanf(result,"|\"%[^\n]\"\"\n",token_data_string);
			break;
		case 4:	//Simple_ID
		case 5:	//Fully_qualified_ID
			fscanf(result,"|%s\n",token_data_string);
			if(less!=true)printf("\t%s",token_data_string);
			break;
		default: //Others
			fscanf(result,"\n");
			break;

	}

	bool error_flag = false;
	if ((unsigned int)token_type == token->type)
	{
		switch((int)token_type){
			case 1:	//Integer
				if (token->data.ivar != token_data_int){error_flag = true;}
				break;
			case 2:	//double
				if (token->data.dvar != token_data_double){error_flag = true;}
				break;
			case 4:	//Simple_ID
			case 5:	//Fully_qualified_ID
				if (strcmp(token_data_string,token->data.svar)!=0){error_flag = true;}
				break;
			default: //Others
				break;
		}
	}
	else{
		error_flag = true;
	}	

	if(error_flag==true){
		failed++;
	}
	else{
		passed++;
	}

	if(less!=true)print_result(error_flag,test_number);

	if(less!=true)printf("\n--------+----------+----+--------------------\n");

	test_number++;

	if (end_flag == 2){
		if(less!=true)printf("End of result file\n");
		print_summary(failed,passed);
		test_number=0;passed=0;failed=0;
		return 2;
	}
	
	if (end_flag == 1){
		if(less!=true)printf("End of source file\n");
		print_summary(failed,passed);
		test_number=0;passed=0;failed=0;
		return 1;
	}
	
	return 0;
}

