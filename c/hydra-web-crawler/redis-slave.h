#ifndef _REDIS_SLAVE_H_
#define _REDIS_SLAVE_H_
#include "hiredis/hiredis.h"

int redisInit( void );
void redisPing( void );
int redisCompletedUrl( char* url );
int redisAddUrl( char* url );
redisReply* redisGetUrl();
void redisFreeGetUrl( redisReply *r );
void redisPrintPerSecond();

#define URL_SET "urls"
#define URL_DONE_SET "xurls"

#define PPS_TIME_AVERAGE (15)

#endif
