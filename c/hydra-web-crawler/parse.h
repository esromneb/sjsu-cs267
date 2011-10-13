#ifndef _PARSE_H_
#define _PARSE_H_

#include <regex.h>

#define LONGEST_CAPTURED_URL (2048)

// call this with 0,1,2 to parse
int doParse( char* m, unsigned s, unsigned pregNum, char* baseUrl );

// internal fn which runs the passed in preg
int extractLinks( char* m, unsigned s, regex_t* pregIn, char* baseUrl );

// takes relative urls and tries to construct an absolute url
int getFullUrl( char* base, char* url, char* ret );

int initParse( void );
int freeParse( void );


extern regex_t preg0;
extern regex_t preg1;
extern regex_t preg2;
#endif
