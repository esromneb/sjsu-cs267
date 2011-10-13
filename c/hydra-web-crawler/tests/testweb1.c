#include "cu/cu.h"
#include "../web.h"
#include "../web.c"

#include <stdio.h>

#define min(a,b) ((a)>(b)?(b):(a))



TEST(testweb1) {
  int ret;
  char myUrl[4096];
  
  strcpy(myUrl, "http://www.sfgate.com/cgi-bin/object/article?f=/g/a/2010/12/13/winter_getaways_snow_ski.DTL&amp;object=&amp;type=ski");

  fixUrlFilename( myUrl );

  ret = strcmp( myUrl, "http___www.sfgate.com_cgi-bin_object_article_f__g_a_2010_12_13_winter_getaways_snow_ski.DTL_amp_object__amp_type_ski");

  puts(myUrl);

  assertFalse(ret);
  //  assertEquals(1, 1);
}


