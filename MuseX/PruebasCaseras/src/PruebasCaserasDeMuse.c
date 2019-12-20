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
	muse_init(1412,"127.0.0.1",6521);
	muse_alloc(10);
	muse_close();
	return EXIT_SUCCESS;
}
