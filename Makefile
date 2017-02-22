# 	
# Name: 	Makefile
# Author: 	Petr Malaník
# Email: 	xmalan02@stud.fit.vutbr.cz
# Login:	xmalan02
# Date:		19. 11. 2016
# Team members:	Barvíř, Malaník, Ondřej, Sochůrek, Smékal
#

CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -g3 

#   $< - prvni zavyslost
#   $^ - vsechyn zavyslosati
#   $@ - label

#-----------------------groups of targets------------------------

all: main

test: update_tests_database run_full_tests diff_all_tests

single: parser postfix gbc interpret

main: ifj16

#-----------------------Single parts-----------------------------

ifj16: error.o gbc.o buffer.o token.o scanner.o ilist.o buildin.o ial.o parser.o postfix.o storage.o interpret.o ifj16.o sematic.o
		$(CC) $(CFLAGS) $^ -o run_$@

parser:	error.o buffer.o scanner.o token.o ilist.o postfix.o parser.d.o
		$(CC) $(CFLAGS) $^ -o run_$@

postfix: error.o buffer.o scanner.o parser.o token.o ilist.o gbc.o sematic.o interpret.o storage.o ial.o buildin.o  postfix.d.o 
		$(CC) $(CFLAGS) $^ -o run_$@

gbc: gbc.d.o
		$(CC) $(CFLAGS) $^ -o run_$@

interpret: error.o interpret.d.o storage.o ilist.o buildin.o gbc.o ial.o
		$(CC) $(CFLAGS) $^ -o run_$@

sematic: error.o sematic.o ilist.o storage.o
		$(CC) $(CFLAGS) $^ -o run_$@

sort: error.o gbc.o ial.d.o
		$(CC) $(CFLAGS) $^ -o run_$@

tree: error.o token.o tree.d.o
		$(CC) $(CFLAGS) $^ -o run_$@

#-----------------------TESTS-----------------------------

test_scanner: error.o test_scanner.o scanner.o token.o buffer.o
		$(CC) $(CFLAGS) $^ -o run_$@

test_parser:  error.o buffer.o token.o scanner.o ilist.o ial.o gbc.o parser.o postfix.o test_parser.o 
		$(CC) $(CFLAGS) $^ -o run_$@ 

test_sort: error.o test_sort.o ial.o gbc.o
		$(CC) $(CFLAGS) $^ -o run_$@

test_interpret: error.o interpret.o storage.o ilist.o buildin.o gbc.o ial.o test_interpret.o
		$(CC) $(CFLAGS) $^ -o run_$@

test_interpret2: error.o interpret.o storage.o ilist.o buildin.o gbc.o ial.o test_interpret2.o
		$(CC) $(CFLAGS) $^ -o run_$@

test_interpret3: error.o interpret.o storage.o ilist.o buildin.o gbc.o ial.o test_interpret3.o
		$(CC) $(CFLAGS) $^ -o run_$@

test_interpret4: error.o interpret.o storage.o ilist.o buildin.o gbc.o ial.o test_interpret4.o
		$(CC) $(CFLAGS) $^ -o run_$@

test_interpret5: error.o interpret.o storage.o ilist.o buildin.o gbc.o ial.o test_interpret5.o
		$(CC) $(CFLAGS) $^ -o run_$@

test_interpret7: error.o interpret.o storage.o ilist.o buildin.o gbc.o ial.o test_interpret7.o
		$(CC) $(CFLAGS) $^ -o run_$@

test_interpret8: error.o interpret.o storage.o ilist.o buildin.o gbc.o ial.o test_interpret8.o
		$(CC) $(CFLAGS) $^ -o run_$@

test_interpret9: error.o interpret.o storage.o ilist.o buildin.o gbc.o ial.o test_interpret9.o
		$(CC) $(CFLAGS) $^ -o run_$@
#----------------Files without DEBUG----------------------

%.o: %.c %.h
		$(CC) $(CFLAGS) -c $<	

test_scanner.o: tests/Scanner/test_scanner.c tests/Scanner/test_scanner.h
		$(CC) $(CFLAGS) -c $<

test_parser.o:  tests/Parser/test_parser.c tests/Parser/test_parser.h
		$(CC) $(CFLAGS) -c $<

test_sort.o:  tests/Sort/test_sort.c tests/Sort/test_sort.h
		$(CC) $(CFLAGS) -c $<

sematic.o: sematic.c sematic.h
		$(CC) $(CFLAGS) -c $<

token.o: token.c token.h
		$(CC) $(CFLAGS) -c $<

parser.o: parser.c parser.h
		$(CC) $(CFLAGS) -c $<

scanner.o: scanner.c scanner.h
		$(CC) $(CFLAGS) -c $<

buffer.o: buffer.c buffer.h
		$(CC) $(CFLAGS) -c $<

ial.o: ial.c ial.h
		$(CC) $(CFLAGS) -c $<

error.o: error.c error.h
		$(CC) $(CFLAGS) -c $<

gbc.o: gbc.c gbc.h
		$(CC) $(CFLAGS) -c $<

interpret.o: interpret.c interpret.h
		$(CC) $(CFLAGS) -c $<

postfix.o: postfix.c postfix.h
		$(CC) $(CFLAGS) -c $<

ilist.o: ilist.c ilist.h
		$(CC) $(CFLAGS) -c $<

storage.o: storage.c storage.h
		$(CC) $(CFLAGS) -c $<

buildin.o: buildin.c buildin.h
		$(CC) $(CFLAGS) -c $<

ifj16.o: ifj16.c ifj16.h
		$(CC) $(CFLAGS) -c $<

#----------------Files with DEBUG------------------------------

interpret.d.o: interpret.c interpret.h
		$(CC) $(CFLAGS) -DDEBUG_INTERPRET -c $< -o $@

postfix.d.o: postfix.c postfix.h
		$(CC) $(CFLAGS) -DDEBUG_POSTFIX -c $< -o $@

parser.d.o:	parser.c parser.h
		$(CC) $(CFLAGS) -DDEBUG_PARSER -c $< -o $@

ial.d.o: ial.c ial.h
		$(CC) $(CFLAGS) -DDEBUG_SORT -c $< -o $@

gbc.d.o: gbc.c gbc.h
		$(CC) $(CFLAGS) -DDEBUG_SORT -c $< -o $@

tree.d.o: ial.c ial.h
		$(CC) $(CFLAGS) -DDEBUG_TREE -c $< -o $@

#------------------others--------------------------------------

test_diff:
	diff tests/Full/Outputs/merged_outputs tests/Full/Correct/merged_corrects

run_full_tests:
	@bash full_test_run.sh

make diff_all_tests:
	bash diff_all_tests.sh

pack:
	zip xbarvi00 * 

test_output_clean:
	rm tests/Full/Outputs/*.output

update_tests_database:
	@ls tests/Parser | grep 'parser_test_' > tests/Parser/List_of_tests
	@cat tests/Parser/List_of_tests | wc -l
	@bash tests/Full/update_database.sh

number_of_test:
	@cat tests/Parser/List_of_tests | wc -l

.PHONY clean:
	rm *.o run*

#Pro spusteni kompletnich testi
#
#-Aktualizace satabaze testu -
#make update_tests_database
#
#-Vytvoreni vystupu testu-
#make run_full_tests
#
#-Spusteni rozdilu mezi vystupy a spravnami vystupy-
#make diff_all_tests
#
#-Spusteni rozdilu mezi spojenými soubory-
#make test_diff
