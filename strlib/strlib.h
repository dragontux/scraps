#ifndef _strlib_h
#define _strlib_h

// Basic data structure types
typedef enum encode {
	E_ASCII	= 1,
	E_UTF8	= 1,
	E_UTF16	= 2,
	E_UTF32	= 4
} encode_t;

typedef struct string {
	char *str;
	unsigned int 	size, 
			alloced;
	encode_t encoding;
} string_t;

typedef struct array {
	void **ray;
	unsigned int size;
} array_t;

typedef struct dict {
	array_t *key,
		*index;
	unsigned int size;
} dict_t;

typedef struct _strlib_link_node {
	struct _strlib_link_node *prev;
	struct _strlib_link_node *next;
	void *value;
} _link_node_t;

typedef struct llist { 
	_link_node_t *root;
	unsigned int size;
} llist_t;

// Function stubs for basic structures
// strings
string_t *newstr	( );
string_t *mkstr		( char *str );
string_t *mkenstr	( char *str, encode_t encoding );
string_t *cpcstr	( char *str );
string_t *cpstr		( string_t *str );
string_t *setstr	( string_t *str, char *set );
string_t *joinstr	( string_t *str1, string_t *str2 );
string_t *substr	( string_t *str,  unsigned int start, unsigned long end );
int	 *lenstr	( string_t *str );
char 	 *rawstr	( string_t *str );
void  	 freestr	( string_t **str );

// arrays
array_t *newarray	( );
array_t	*splitstr	( string_t *str, char tok );
array_t *joinray	( array_t *ray1, array_t *ray2 );
array_t *pushray	( array_t *ray,  void *thing );
void 	*popray		( array_t *ray );
unsigned int lenray	( array_t *ray );
void	 freeray	( array_t **ray );
void 	 dumpray	( array_t *ray );

// dictionaries
dict_t	*newdict	( );
dict_t	*adddict	( dict_t *dict, void *key, void *hash );
dict_t	*addraydict	( dict_t *dict, array_t *key, array_t *hash );
dict_t  *joindict	( dict_t *dict1, dict_t *dict2 );
void 	*searchdict	( dict_t *dict, void *key );
void 	 freedict	( dict_t **dict );

// linked list
llist_t *newllist	( );
llist_t *addnode	( llist_t *list, void *thing );
llist_t *addrayllist	( llist_t *list, array_t *ray );
llist_t *nextnode	( llist_t *list );
llist_t *prevnode	( llist_t *list );
void    *listindex	( llist_t *list, unsigned int index );
void 	 freellist	( llist_t **list );

#endif
