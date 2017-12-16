/**
  @program server.c
  @author Jeremy Timperio (jmtimper)
  Server that manages list and recieves commands from client program
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

// List of words maintained by the server.
char list[ LIST_CAP ][ WORD_MAX + 1 ];
 
// Number of words in the list.
int wcount = 0;
 
//Handles signal for ctrl-c @credit http://stackoverflow.com/questions/4217037/catch-ctrl-c-in-c
void sig_handler(int signo){
  if( signo == SIGINT){
    for(int i = 0; i < wcount; i++){
      printf("%s\n",list[i]);
    } 
  }
}

/**
  runs server and manages all commands recieved from client
  @param argc number of command line arguments
  @param argv array of command line arguments
*/
int main( int argc, char *argv[] ) {
  // Remove both queues, in case, last time, this program terminated
  // abnormally with some queued messages still queued.
  mq_unlink( SERVER_QUEUE );
  mq_unlink( CLIENT_QUEUE );
 
  // Prepare structure indicating maximum queue and message sizes.
  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 1;
  attr.mq_msgsize = MESSAGE_LIMIT;
 
  // Make both the server and client message queues.
  mqd_t serverQueue = mq_open( SERVER_QUEUE, O_RDONLY | O_CREAT, 0600, &attr );
  mqd_t clientQueue = mq_open( CLIENT_QUEUE, O_WRONLY | O_CREAT, 0600, &attr );
  if ( serverQueue == -1 || clientQueue == -1 )
    fail( "Can't create the needed message queues" );

  //holds message being sent
  char buffer[ MESSAGE_LIMIT ];
  char commandBuffer[ MESSAGE_LIMIT ];

  //Handles signal for ctrl-c @credit http://stackoverflow.com/questions/4217037/catch-ctrl-c-in-c
  signal(SIGINT, sig_handler);

  // Repeatedly read and process requests from the client.
  while ( true ) {

    //clears buffers
    memset(buffer, 0 , MESSAGE_LIMIT);
    memset(commandBuffer, 0 , MESSAGE_LIMIT);

    //receives command from client
    if(mq_receive( serverQueue , commandBuffer, sizeof(commandBuffer) , NULL )==-1){
      fail("");
    }

    //receives word from client
    mq_receive( serverQueue , buffer, sizeof(buffer) , NULL );

    if(strncmp("query", commandBuffer, MESSAGE_LIMIT) == 0){
      for(int i = 0; i < LIST_CAP; i++){
         //printf("%d %s | wcount = %d\n", i, list[i], wcount);
         if(wcount > i)
           mq_send(clientQueue, list[i], sizeof(list[i]), 0);
         else
           mq_send(clientQueue, "empty", sizeof("empty"), 0);
      }
    }
    //checks for up command from client
    if(strncmp("up", commandBuffer, MESSAGE_LIMIT) == 0){
      //0 if word isn't in list, 1 if word is in list
      int foundWord = 0;
      for(int i = 0; i < wcount; i++){
        //compares string in buffer to string in list
        if(strncmp(buffer, list[i], sizeof( buffer )) == 0){
          foundWord = 1;
          if(i == 0){
            mq_send(clientQueue, "0", sizeof("0"), 0);
          }
          if(i != 0){
            //swaps positions of two words in the list
            char temp[ MESSAGE_LIMIT ];
            strcpy(temp, list[i-1]);
            strcpy(list[i-1], buffer);
            strcpy(list[i], temp);
            
            //converts number to string to pass to client
            int k = i-1;
            char num[1];
            sprintf(num, "%d", k);
            mq_send(clientQueue, num, sizeof(num), 0);
          }
        }
      }
      //if word was not found in up call
      if(foundWord == 0){
        //if list is empty
        if(wcount == 0){
          strncpy(list[0], buffer, MESSAGE_LIMIT);
          char num[1];
          sprintf(num, "%d", wcount);
          wcount++;
          mq_send(clientQueue, num, sizeof(num), 0);
        } else {
          //if list has words in it but not full
          if(wcount < LIST_CAP){
            strcpy(list[wcount], buffer);
            char num[1];
            sprintf(num, "%d", wcount);
            wcount++;
            mq_send(clientQueue, num, sizeof(num), 0);
          } else { //if list is full
            strcpy(list[wcount-1], buffer);
            mq_send(clientQueue, "5", sizeof("5"), 0);
          }
        }
      }
    }
    if(strncmp("down", commandBuffer, sizeof(commandBuffer)) == 0){
      //0 if word isn't in list, 1 if word is in list
      int foundWord = 0;
      for(int i = 0; i < wcount; i++){
        //compares string in buffer to string in list
        if(strncmp(buffer, list[i], sizeof( buffer )) == 0){
          foundWord = 1;
          if(i == (wcount-1)){
            int k = wcount-1;
            char num[2];
            sprintf(num, "%d", k);
            mq_send(clientQueue, num, sizeof(num), 0);
          }
          if(i != (wcount-1)){
            //swaps positions of two words in the list
            char temp[ MESSAGE_LIMIT ];
            strcpy(temp, list[i+1]);
            strcpy(list[i+1], buffer);
            strcpy(list[i], temp);
            i++;
            //converts number to string to pass to client
            int k = i;
            char num[2];
            sprintf(num, "%d", k);
            mq_send(clientQueue, num, sizeof(num), 0);
          }
        }
      }
      //if word was not found in up call
      if(foundWord == 0){
        mq_send(clientQueue, "-1", sizeof("-1"), 0);
      }
    }
  }
 
  // This code isn't reachable right now, but it may be useful once you add
  // support for catching sigint.
 
  // Close our two message queues (and delete them).
  mq_close( clientQueue );
  mq_close( serverQueue );
 
  // Delete the message queues, this is for normal server termination.
  mq_unlink( SERVER_QUEUE );
  mq_unlink( CLIENT_QUEUE );
 
  return EXIT_SUCCESS;
}
