#ifndef _WEB_C_
#define _WEB_C_

#include "web.h"
#include "parse.h"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <curl/curl.h>

struct MemoryStruct {
  char *memory;
  size_t size;
};

char fileName[LONGEST_FILE_NAME];
 
int globalVerbose;

/*
// for writing files to disk
static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{

  int written = fwrite(ptr, size, nmemb, (FILE *)stream);
  // printf("%d written, %d size, %d nmemb\n", written, size, nmemb);

  return written;
}*/

static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)data;
  
  mem->memory = (char*) realloc(mem->memory, mem->size + realsize + 1);
   if (mem->memory == NULL) {
    // out of memory! 
    printf("not enough memory (realloc returned NULL)\n");
    exit(EXIT_FAILURE);
  }
 
  memcpy(&(mem->memory[mem->size]), ptr, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}


int basicstart( char* url )
{
  CURL *curl_handle;
  /*
  static const char *headerfilename = "dirout/head.out";
  FILE *headerfile;
  static const char *bodyfilename = "dirout/body.out";
  FILE *bodyfile;
  */
  struct MemoryStruct chunk;
 
  chunk.memory = (char*) malloc(1);  /* will be grown as needed by the realloc above */
  chunk.size = 0;    /* no data at this point */ 


  curl_global_init(CURL_GLOBAL_ALL);

  /* init the curl session */
  curl_handle = curl_easy_init();

  /* set URL to get */

  // curl_easy_setopt(curl_handle, CURLOPT_URL, "http://localhost:10080/2link.html");
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);


  /* no progress meter please */
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

  /* send all data to this function  */
  //write to file
  // curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

  // save in memory
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

  /* open the files */
  /*headerfile = fopen(headerfilename,"w");
  if (headerfile == NULL) {
    curl_easy_cleanup(curl_handle);
    return -1;
  }
  bodyfile = fopen(bodyfilename,"w");
  if (bodyfile == NULL) {
    curl_easy_cleanup(curl_handle);
    return -1;
    }*/

  /* we want the headers to this file handle */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&chunk);

  // bjm: this line directs data into a file vs stdout
  //  curl_easy_setopt(curl_handle,   CURLOPT_WRITEDATA, headerfile);


  /*
   * Notice here that if you want the actual data sent anywhere else but
   * stdout, you should consider using the CURLOPT_WRITEDATA option.  */

  /* get it! */
  curl_easy_perform(curl_handle);

  /* close the header file */
  //  fclose(headerfile);

  //fclose(bodyfile);

  /* cleanup curl stuff */
  curl_easy_cleanup(curl_handle);



 
  printf("Done, size %lu bytes.  ", (long)chunk.size);
  
  printf("Parsing...\n");
  // extract links!!
  initParse();
  doParse( chunk.memory, chunk.size, 0, url );
  freeParse();

  writeToDisk( url, chunk.memory, chunk.size );

  if(chunk.memory)
    free(chunk.memory);
  
  printf("\n\n");
  return 0;
}

// FIXME length limit?
// replaces single instance of string
char *replace(char *st, char *orig, char *repl) {
  static char buffer[LONGEST_FILE_NAME];
  char *ch;
  if (!(ch = strstr(st, orig)))
    return st;
  strncpy(buffer, st, ch-st);  
  buffer[ch-st] = 0;
  sprintf(buffer+(ch-st), "%s%s", repl, ch+strlen(orig));
  return buffer;
}


// does inplace modify of passed in url
void fixUrlFilename( char *url )
{
  char *c;

  c = url;
  while( *c != 0 )
    {
      c++;

      if( *c == '/' ||
	  *c == ';' ||
	  *c == '(' ||
	  *c == ')' ||
	  *c == '?' ||
	  *c == '%' ||
	  *c == '&' ||
	  *c == '=' ||
	  *c == ':'
	  )
	{
	  *c = '_';
	}
    }
} //fixUrlFilename

// modifies url string and limits to 255
void trimUrlFilename( char *url )
{
  if( strlen( url ) > 255 )
    url[255] = '\0';
}


int writeToDisk( char* url, char* memory, unsigned size )
{
  FILE * pFile;
  
  char safeFilename[LONGEST_FILE_NAME];

  // copy the file to our buffer
  strcpy( safeFilename, url);

  // do inline modify for unsafe characters
  fixUrlFilename(safeFilename);

  // do inline trim of filename length
  trimUrlFilename(safeFilename);

  // append on the correct directory
  sprintf( fileName, "%s%s", FILE_OUTPUT_FOLDER, safeFilename);


  pFile = fopen ( fileName, "wb" );
  fwrite (memory , 1 , size , pFile );
  fclose (pFile);

  return 0;
}



#endif
