/**
  @program client.c
  @author Jeremy Timperio (jmtimper)
  Client program that sends commands to server
*/

#include "common.h"
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
 
// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}
 
/**
  runs client program and connects to server
 @argc number of command line arguments
 @argv char * to an array of command line arguments
*/
int main( int argc, char *argv[] ) {
 
  // Make both the server and client message queues.
  mqd_t serverQueue = mq_open( SERVER_QUEUE, O_WRONLY );
  mqd_t clientQueue = mq_open( CLIENT_QUEUE, O_RDONLY );
  if ( serverQueue == -1 || clientQueue == -1 )
    fail( "Can't create the needed message queues" );
  
  char word[MESSAGE_LIMIT];

  //checks for query command
  if( argc == 2 && strcmp(argv[1], "query") == 0){
    mq_send( serverQueue, argv[1] , strlen( argv[1] ), 0 );
    mq_send( serverQueue, argv[1] , strlen( argv[1] ), 0 );
    for(int i = 0; i < LIST_CAP; i++){
      memset(word, 0, MESSAGE_LIMIT);
      mq_receive( clientQueue, word , MESSAGE_LIMIT , NULL);
      if(!(strcmp("empty", word)==0))
        printf("%s\n", word);
    }
  }
  
  if( argc == 3 ){
    //checks for up command
    if( strcmp(argv[1],"up") == 0){
      //sends up command
      mq_send( serverQueue, argv[1] , strlen( argv[1] ), 0 );
      //sends word to be used
      mq_send( serverQueue, argv[2] , strlen( argv[2] ), 0 );
      char rt[MESSAGE_LIMIT];
      memset(rt, 0, MESSAGE_LIMIT);
      //recieves updated position
      if((mq_receive( clientQueue, rt , MESSAGE_LIMIT , NULL)) == -1)
        fail("can't get number" );
      printf("%s\n", rt);
    } 
    //checks for down command
    if(strcmp(argv[1], "down")==0){
      //sends up command
      mq_send( serverQueue, argv[1] , strlen( argv[1] ), 0 );
      //sends word to be used
      mq_send( serverQueue, argv[2] , strlen( argv[2] ), 0 );
      char rt[MESSAGE_LIMIT];
      memset(rt, 0, MESSAGE_LIMIT);
      //recieves updated position
      if((mq_receive( clientQueue, rt , MESSAGE_LIMIT , NULL)) == -1)
        fail("can't get number" );
      printf("%s\n", rt);
    }    
  }
  //closes message queues
  mq_close( serverQueue );
  mq_close( clientQueue );
}
