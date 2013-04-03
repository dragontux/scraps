#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strlib.h"

#define new( n ) (memset(malloc(sizeof(n)),0,sizeof(n)))

void strcleanup( array_t **ray ){
	string_t *meh;
	while ((*ray)->size){
		meh = popray( *ray );
		freestr( &meh );
	}
	freeray( ray );
}

int main( int argc, char *argv[] ){
	string_t *foo;
	array_t  *bar;
	int i;

	// make new string from standard null-terminated c string
	foo = mkstr( "Some text" );

	// split string into spaces
	bar = splitstr( foo, ' ' );

	for ( i = 0; i < bar->size; i++ )
		printf( "%s\n", rawstr( bar->ray[i] ));

	strcleanup( &bar );
	freestr( &foo );
	return 0;
}
