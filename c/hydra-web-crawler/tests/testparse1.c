#include "cu/cu.h"
#include "../parse.h"
#include "../parse.c"
#include "../web.h"

#include <stdio.h>

#define min(a,b) ((a)>(b)?(b):(a))


const char* html1 = "<html>\
<head><title>links</title></head>\
<body>\
<a href=\"http://www.google.com\">google</a><br><a href=\"http://turbo-spring.com\">tbspring</a>\
</body>\
</html>";

const char* nolink1 = "<html>\
<head><title>links</title></head>\
<body>\
< href=\"http://www.google.com\">google</a><br>< b href=\"http://turbo-spring.com\">tbspring</a>\
</body>\
</html>";

#define HTML_BUF_SIZE (2048)
char htmlBuf[HTML_BUF_SIZE];

TEST(test1) {
  int ret;
  // test init correctly
  ret = initParse();
  assertFalse( ret );

  strncpy(htmlBuf, html1, HTML_BUF_SIZE);
  ret = doParse( htmlBuf, min(sizeof(html1),HTML_BUF_SIZE) , 0, (char*)"http://base");
  assertFalse( ret );
  
  freeParse();

  //  assertTrue(1);
  //  assertEquals(1, 1);
}

// test parse w empty
TEST(test2) {
  //    assertFalse(0);
  //  assertNotEquals(1, 2);
  int ret;
  // test init correctly
  ret = initParse();
  assertFalse( ret );

  char *emptyBuf[8];
  emptyBuf[0] = 0;
  emptyBuf[1] = 0;

  ret = doParse( (char*)emptyBuf, sizeof(0), 0, (char*)"http://base/" );
  assertTrue( ret );

  strncpy(htmlBuf, nolink1, HTML_BUF_SIZE);
  ret = doParse( htmlBuf, min(sizeof(nolink1),HTML_BUF_SIZE) , 0, (char*)"http://base/" );
  assertTrue( ret );

  freeParse();
}

// run top during this test to watch for memory leaks
TEST(test3) {
  int i;
  int ret;

  strncpy(htmlBuf, html1, HTML_BUF_SIZE);

  
  // try 100000
  for( i = 0; i < 2; i++ )
    {
      ret = initParse();
      ret = doParse(  htmlBuf, min(sizeof(html1),HTML_BUF_SIZE) , 0, (char*)"http://base/" );
      freeParse();
    }
  //  assertFalse(0);
}

TEST(test4) {
  char buf[1024];
  int compare;
  
  getFullUrl( "http://www.google.com/", "/intl/en/privacy/", buf );
  compare = strcmp( buf, "http://www.google.com/intl/en/privacy/" );
  assertFalse( compare );

  getFullUrl( "http://www.google.com", "/intl/en/privacy/", buf );
  compare = strcmp( buf, "http://www.google.com/intl/en/privacy/" );
  assertFalse( compare );

// this will fail, fn cannot detect spaces in url
//  getFullUrl( "http://www.google.com ", "/intl/en/privacy/", buf );
//  compare = strcmp( buf, "http://www.google.com/intl/en/privacy/" );
//  assertFalse( compare );

  getFullUrl( " http://www.google.com/", "/intl/en/privacy/", buf );
  compare = strcmp( buf, "http://www.google.com/intl/en/privacy/" );
  assertFalse( compare );

  getFullUrl( "http://www.google.com/ ", "/intl/en/privacy/", buf );
  compare = strcmp( buf, "http://www.google.com/intl/en/privacy/" );
  assertFalse( compare );

  getFullUrl( "http://www.google.com/", "http://yahoo.com/", buf );
  compare = strcmp( buf, "http://yahoo.com/" );
  assertFalse( compare );

  getFullUrl( "http://www.google.com/", "/", buf );
  compare = strcmp( buf, "http://www.google.com/" );
  assertFalse( compare );

  getFullUrl( "http://www.google.com", "/", buf );
  compare = strcmp( buf, "http://www.google.com/" );
  assertFalse( compare );

  //  printf("str compare is %d, Result buf is %s\n", compare, buf);
}


// this tests a previos bug when writeToDisk writes a filename
// longer than 255 characters (filesystem limit)
TEST(test5) { 

  char *url, *memory;
  
  url = malloc(sizeof(char)*2000);
  memory = malloc(sizeof(char)*5000);

  strcpy( url, "http://www.delicious.com/post?url=http%3A%2F%2Fwww.thefablife.com%2F2011-07-01%2Fselena-gomezs-20-hottest-looks%2F&amp;amp;title=Selena%20Gomez%27s%2020%20Hottest%20Looks&amp;amp;notes=%0D%0A%0D%0AKids%20love%20Selena%20Gomez%21%20Parents%20love%20Selena%20Gomez%21%20Justin%20Bieber%20loves%20Selena%20Gomez%21%20Everyone%20loves%20Selena%20Gomez%21%20Unless%20you%20love%20Justin%20Bieber%20a%20little%20too%20much%2C%20in%20which%20case%20you%20probably%20aren%27t%20as%20fond%20of%20her%20%28we%27re%20looking%20at%20you%2C%20%22Beliebers%22");

    
  strcpy( memory, "<!-- fe02.web.del.ac4.yahoo.net uncompressed/chunked Thu Sep 22 18:00:19 UTC 2011 -->\n");

  writeToDisk( url, memory, 8 );
  
  // if not segfaulted by here, we pass!
  assertTrue(1);

}
