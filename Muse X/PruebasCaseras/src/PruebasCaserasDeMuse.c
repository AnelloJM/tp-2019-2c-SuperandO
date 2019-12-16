/*
 ============================================================================
 Name        : PruebasCaserasDeMuse.c
 Author      : Superand0
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <libmuse/libmuse.h>

int main(void) {
	muse_init(1,"127.0.0.1",6969);
	return EXIT_SUCCESS;
}
