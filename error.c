/*
 * Name: error.c
 * Author: Marek Barvir
 * Email: barvirm@seznam.cz
 * Login: xbarvi00
 * Date: 25.9.2016
 * Team members:  Barvíř, Malaník, Ondřej, Sochůrek, Smékal
 *
 * Description: define errors by specification
*/

#include "error.h"

bool set_error(int e) {
    error_number = e;
    return true;
}