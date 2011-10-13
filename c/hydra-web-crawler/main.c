/***************************************************************************
 *                                  _   _ ____  _
 *                              ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <curl/curl.h>
#include "web.h"
#include "redis-slave.h"
#include "hiredis/hiredis.h"

#include "sys/stat.h"



int main (int argc, char *argv[])
{
  //  basicstart();
  //curl_global_cleanup();
  redisReply *curUrl;
  if( 0 != redisInit() )
    {
      printf("ERROR: Is redis-server started?\n\n");
      exit(1);
    }

  struct stat st;
  if(stat("dirout",&st) == -1)
    {
      printf("ERROR: dirout sym link not present.\nCreate it with 'ln -s xfs dirout' or ./mount.xfs\n\n");
      exit(1);
    }

  globalVerbose = 0;
  if( argc > 1 )
    {
      if( strcmp( argv[1], "-v" ) == 0 )
	{
	  printf( "Starting Verbose...\n" );
	  globalVerbose = 1;
	}

      if( strcmp( argv[1], "-pps" ) == 0 )
        {
	  redisPrintPerSecond();
	  exit(0);
        }

    }


  redisPing();

  //  redisAddUrl((char*)"hi");
  //  int i;
  

  while( 1 )
    {
      curUrl = redisGetUrl();
      if( curUrl->str != 0 )
	{
	  printf("Downloading %s...\n", curUrl->str);
	  basicstart( curUrl->str );
	  redisFreeGetUrl(curUrl);
	}
      // 5 seconds
      //usleep(2 * 1000000);
      //return 0;
    }

  return 0;
}
