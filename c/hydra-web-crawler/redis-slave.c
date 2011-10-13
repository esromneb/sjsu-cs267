#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "web.h"
#include "redis-slave.h"
#include "hiredis/hiredis.h"

redisContext *redisC;
redisReply *redisRep;

int redisInit( void )
{
    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    redisC = redisConnectWithTimeout((char*)"127.0.0.1", 6379, timeout);
    if (redisC->err) {
        printf("Connection error: %s\n", redisC->errstr);
        return 1;
    }
    return 0;
}

void redisPing( void )
{
  // PING server 
  redisRep = (redisReply*) redisCommand(redisC,"PING");
  printf("PING: %s\n", redisRep->str);
  freeReplyObject(redisRep);
}


// This sets a key with a ttl of PPS_TIME_AVERAGE seconds
int redisPerSecond( char* url )
{
  redisRep = redisCommand(redisC,"SET pps:%s 1", url );
  // returns true when it went in ok
  // returns false with error or when key already exists
  //printf("REPLY: %lld\n", redisRep->integer);
  freeReplyObject(redisRep);

  redisRep = redisCommand(redisC,"EXPIRE pps:%s %d", url, PPS_TIME_AVERAGE );
  // returns true when it went in ok
  // returns false with error or when key already exists
  //printf("REPLY: %lld\n", redisRep->integer);
  freeReplyObject(redisRep);

  return 0;
}

// queries the number of keys set by the redisPerSecond() function
// that haven't expired yet
// divides by PPS_TIME_AVERAGE to get urls crawled per second
void redisPrintPerSecond()
{
  float pagesPerSecond;

  redisRep = redisCommand(redisC,"keys pps:*");
  if (redisRep->type == REDIS_REPLY_ARRAY)
    {
      pagesPerSecond = (float) redisRep->elements / PPS_TIME_AVERAGE;
      printf("Crawling %.3f pages per second\n", pagesPerSecond);
    }
  freeReplyObject(redisRep);
}

// call when url has been crawled
int redisCompletedUrl( char* url )
{
    redisPerSecond( url );
    redisRep = redisCommand(redisC,"SADD %s %s", URL_DONE_SET, url);
    // returns true when it went in ok
    // returns false with error or when key already exists
    //printf("REPLY: %lld\n", redisRep->integer);
    freeReplyObject(redisRep);
    if( redisRep->integer == 0 )
      {
	if( globalVerbose )
	  printf("(Completed) Key %s already exists in set\n", url);

	return 1;
      }
    else
      {
	return 0;
      }
}


// returns 1 for yes
// returns 0 for no
int redisAlreadyCrawled( char* url )
{
  redisRep = redisCommand(redisC,"SISMEMBER %s %s", URL_DONE_SET, url);
  freeReplyObject(redisRep);
  if( redisRep->integer == 1 )
    {
      return 1;
    }
  else
    {
      return 0;
    }
}

int redisAddUrl( char* url )
{
  if( redisAlreadyCrawled( url ) )
    {
       if( globalVerbose )
	printf("Url has already been succesfully crawled.\n");
      return 1;
    }

    redisRep = redisCommand(redisC,"SADD %s %s", URL_SET, url);
    // returns true when it went in ok
    // returns false with error or when key already exists
    //printf("REPLY: %lld\n", redisRep->integer);
    freeReplyObject(redisRep);
    if( redisRep->integer == 0 )
      {
	if( globalVerbose )
	printf("Key %s already exists in set\n", url);
	return 1;
      }
    else
      {
	return 0;
      }
}

redisReply* redisGetUrl( void )
{
  redisReply *r;
  r = redisCommand(redisC,"SPOP %s", URL_SET);
  // returns 0x0 string on emtpy set
  //printf("POPING: got status %x string\n", r->str);
  if( r->str == 0 )
    {
      printf("No urls in set\n");
      usleep(1 * 1000000);
    }
  else
    {
      if( globalVerbose )
      printf("got %s\n", r->str);
    }
  return r;
}

void redisFreeGetUrl( redisReply *r )
{
  freeReplyObject( r );
}

int main2(void) {
  unsigned int j;
  redisInit();
  redisPing();
    
 
    // Set a key 
    redisRep = redisCommand(redisC,"SET %s %s", "foo", "hello world");
    printf("SET: %s\n", redisRep->str);
    freeReplyObject(redisRep);

    // Set a key using binary safe API 
    redisRep = redisCommand(redisC,"SET %b %b", "bar", 3, "hello", 5);
    printf("SET (binary API): %s\n", redisRep->str);
    freeReplyObject(redisRep);

    // Try a GET and two INCR
    redisRep = redisCommand(redisC,"GET foo");
    printf("GET foo: %s\n", redisRep->str);
    freeReplyObject(redisRep);

    redisRep = redisCommand(redisC,"INCR counter");
    printf("INCR counter: %lld\n", redisRep->integer);
    freeReplyObject(redisRep);
    // again ... 
    redisRep = redisCommand(redisC,"INCR counter");
    printf("INCR counter: %lld\n", redisRep->integer);
    freeReplyObject(redisRep);

    // Create a list of numbers, from 0 to 9
    redisRep = redisCommand(redisC,"DEL mylist");
    freeReplyObject(redisRep);
    for (j = 0; j < 10; j++) {
        char buf[64];

        snprintf(buf,64,"%d",j);
        redisRep = redisCommand(redisC,"LPUSH mylist element-%s", buf);
        freeReplyObject(redisRep);
    }

    // Let's check what we have inside the list
    redisRep = redisCommand(redisC,"LRANGE mylist 0 -1");
    if (redisRep->type == REDIS_REPLY_ARRAY) {
        for (j = 0; j < redisRep->elements; j++) {
            printf("%u) %s\n", j, redisRep->element[j]->str);
        }
    }
    freeReplyObject(redisRep);
    
    return 0;
}
