#include "parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "web.h"
#include "redis-slave.h"


regex_t preg0;
regex_t preg1;
regex_t preg2;
char fullUrlBuffer[LONGEST_CAPTURED_URL];

// expressions taken from  http://www.the-art-of-web.com/php/parse-links/

// flags to regcomp(): 
//   REG_ICASE - Caseless
//   REG_STARTEND - Use PMATCH[0] to delimit the start and end of the search in the buffer. 
//                - this one makes it go really fast!!

int initParse( void )
{
  int rc;
  const char *pattern0 = "<a[^>]*href\\s*=\\s*\"\\([^\" >]*\\)\"[^>]*>.*<\\/a>";



  if (0 != (rc = regcomp(&preg0, pattern0, REG_ICASE|REG_STARTEND))) {
    printf("INVALID regex: regcomp() failed, returning nonzero (%d)\n", rc);
    return(EXIT_FAILURE);
  }
  

  return 0;
}
int freeParse( void )
{
  // for now we can only return 0 (success)
  regfree(&preg0);
  return 0;
}


// takes relative urls and tries to construct an absolute url
int getFullUrl( char* base, char* url, char* ret )
{
  char* domainEnd;
  char* http;
  char* domain;
  unsigned length;
  if( globalVerbose )
  printf("base %s\nurl %s\n", base, url);
  
  // check length
  if( ( strlen( base ) + strlen( url ) ) > LONGEST_CAPTURED_URL )
    {
      ret[0] = '\0';
      return 1;
    }


  // url is relative to base of server
  if( url[0] == '/' )
    {
      // find beginning of http url
      http = strstr( base, (char*)"http://" );
      domain = http + 7; //strlen("http://");
      if( http != 0 )
	{

	  // calculate the ending character of domain
	  domainEnd = strchr( domain, '/' );

	  // if no slash is found, it's possible we have www.google.com and not www.google.com/
	  if( domainEnd == 0 )
	    {
	      // assume end of base string is end of url
	      domainEnd = domain + strlen(domain);
	    }
	  // calculate the length of domain
	  length = domainEnd-http;
	  // copy domain into return string
	  strncpy( ret, http, length );
	  // copy path in domain 
	  strcpy( ret+length, url);
	} //if http != 0
    } //if url[0] == /
  else
    {
      // looks like the url is fully qualified, so just return it
      strcpy( ret, url );
    }
 return 0;
}


int doParse( char* m, unsigned s, unsigned pregNum, char* baseUrl )
{
  int ret;
  if( pregNum == 0 )
    {
      ret =  extractLinks( m, s, &preg0, baseUrl );
 
#ifndef CU_UNIT_TEST
      redisCompletedUrl(baseUrl);
#endif
      return ret;

    }

  return 0;
}



// TODO -
//  - Spaces in the url?
//  - 

// temporary measures to deal with the number of extracted substrings and how
// this affects the code
#define NUMMATCHES (2)
#define MATCH_LOCATION (NUMMATCHES-1)

int extractLinks( char* m, unsigned s, regex_t* pregIn, char* baseUrl )
{
  int        rc;
  size_t     nmatch = NUMMATCHES;
  regmatch_t pmatch[NUMMATCHES];
  unsigned start, end, len;
  char saved;
  int totalUrlCount;
  totalUrlCount = 0;

  // set to 1 when rdy to exit
  int flag;
  // offset to character at end of last matching link
  unsigned lastOffset;
  lastOffset = 0;
  flag = 0;

  while( flag == 0 )
    {
      // offset into string for next search
      m += lastOffset;
      
      if (0 != (rc = regexec(pregIn, m, nmatch, pmatch, 0))) {
	flag = 1;
      }
      else {  
	// take pointer to result
	// this saves multiple dereferences
	regmatch_t result;
	result = pmatch[MATCH_LOCATION];

	// save unsigned positions
	start = result.rm_so;
	end = result.rm_eo - 1;
	len = result.rm_eo - result.rm_so;

	// take a pointer to the start of the url string
	char* capturedUrl = (char*)&m[result.rm_so];
	
	// save this character before we destroy it in the buffer
	saved = capturedUrl[len];
	// add null character after match so that we can print
	capturedUrl[len] = '\0';

	//printf("%s\n", capturedUrl);
	getFullUrl( baseUrl, capturedUrl, fullUrlBuffer );
#ifndef CU_UNIT_TEST
	redisAddUrl(fullUrlBuffer);
#endif
	totalUrlCount++;
	// restore destroyed character
	capturedUrl[len] = saved;


	// %.*s  is a cool trick.  use it like printf("%.*s", 6, str);
	// limits printing of str to length 6
	/*		printf("\n\nWith the whole expression, "
			"a matched substring \"%.*s\" is found at position %d to %d.\n\n",
			result.rm_eo - result.rm_so, &m[result.rm_so],
			start, end);
	*/
	//printf("start %u end %u diff %u\n", start, end, result.rm_eo - result.rm_so); 


	// end is calculated as result.rm_eo -1.  If we want to continue after the null inserted
	// above, we must do end+2
	lastOffset = end+2;
      }
    }
  
  // if lastOffset is still 0 it means we failed
  // on the first match
  if( lastOffset == 0 )
    {
      printf("Failed to find any urls, returning %d.\n", rc);
      //printf("Failed to match '%s' with '%s',returning %d.\n",
      //string, pattern, rc);
      
      return 1;
    }
  else
    {
      return 0;
    }
}
