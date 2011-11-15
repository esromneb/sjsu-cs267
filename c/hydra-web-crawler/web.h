#ifndef _WEB_H_
#define _WEB_H_
//size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);

int CrawlUrl( char* url );
int writeToDisk( char* url, char* memory, unsigned size );
extern int globalVerbose;

#define LONGEST_FILE_NAME (4096)

// this string must have a trailing /
#define FILE_OUTPUT_FOLDER "dirout/"


#endif
