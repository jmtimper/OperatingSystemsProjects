#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static void fail( char *msg ) {
  printf( "Error: %s\n", msg );
  exit( EXIT_FAILURE );
}

int main( int argc, char *argv[] ) {
  // Try to make a child process.
  pid_t p1 = fork();
  if ( p1 == -1)
    fail( "Can't create child process" );

  // Print out a report from that child.
  if ( p1 == 0) {
    sleep( 1 );
    printf("I am %d, child of %d\n", getpid(), getppid());
    exit( EXIT_SUCCESS );
  } 
  
    pid_t p2 = fork();
  if ( p2 == -1)
    fail( "Can't create child process" );

  // Print out a report from that child.
  if ( p2 == 0) {
    sleep( 1 );
    printf("I am %d, child of %d\n", getpid(), getppid());
    exit( EXIT_SUCCESS );
  }
  
      pid_t p3 = fork();
  if ( p3 == -1)
    fail( "Can't create child process" );

  // Print out a report from that child.
  if ( p3 == 0) {
    sleep( 1 );
    printf("I am %d, child of %d\n", getpid(), getppid());
    exit( EXIT_SUCCESS );
  }
  if (p1 > 0) {
    wait(NULL);
    printf( "Done\n" );
    return EXIT_SUCCESS;
  }
  return EXIT_SUCCESS;
}