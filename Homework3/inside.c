#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// Maximum number of circles we can store in the circle list.  The list is
// allocated statically, so workers can start working on circles as soon as we
// read them in.
#define CAP 50

// Simple struct for representing a circle.
typedef struct {
  int x, y;
  int r;
} Circle;

// List of all circles.
Circle *cList;

//position of threads
int pos = 0;

// all semaphores
sem_t semCount;
sem_t semSize;
sem_t semWait;

// Number of circles on our list.
int cCount = 0;

// Total number of points we've found that are inside a circle.
int total = 0;

int size = CAP;

// Is readcircles still reading circles
bool moreCircles;

// Read the lsit of circles.
void readCircles() {
  // Read circle definitions until we can't read any more.
  Circle c;
  moreCircles = true;
  while ( scanf( "%d%d%d", &c.x, &c.y, &c.r ) == 3 ) {
    if ( cCount >= size) {
      printf("1");
      sem_wait(&semSize);
	  printf("2");
	  size *= 2;
	  cList = (Circle *)realloc(cList, sizeof(Circle)*size);
	  sem_post(&semSize);
	}

    // Add the circle we just read to the lsit.
    cList[ cCount ] = c;
    cCount++;
    sem_post(&semWait);
  }
  moreCircles = false;
}

// Return true if the given x, y location is inside circle c.
bool inside( Circle *c, int x, int y )
{
  int dx = x - c->x;
  int dy = y - c->y;
  return dx * dx + dy * dy <= c->r * c->r;
}

/**Assigns worker thread to circle*/
int getWork(){
  while(true){
    if(!moreCircles && pos >= cCount){
      //no circles left
      return -1;
    }else if(pos >= cCount){
      //waits for circle
      sem_wait(&semWait);
      continue;
    }else{
      //returns circle
      pos++;
      return (pos-1);
    }   
  }
}

/** Start routine for each worker. */
void *workerRoutine(void *arg) {
  int position; 
  while ((position = getWork()) != -1) {
	sem_wait(&semSize);
    Circle *c = &cList[position];
	sem_post(&semSize);
    for (int x = c->x - c->r; x <= c->x + c->r; x++) {
      for (int y = c->y - c->r; y <= c->y + c->r; y++) {
        if (inside(c, x, y)) {
          bool counted = false;
          // Make sure we haven't already counted this point.
          for (int j = 0; !counted && j < position; j++) {
            if (inside(cList + j, x, y))
              counted = true;
          }
          // Count the point if it isn't inside an earlier circle.
          if (!counted) {
            sem_wait( &semCount);
            total++;
            sem_post( &semCount);
          }
        }
      }
    }
  }
return NULL;
}
  
int main( int argc, char *argv[] ) {
  int workers = 4;
  if ( argc != 2 ||
       sscanf( argv[ 1 ], "%d", &workers ) != 1 ||
       workers < 1 )
    fail( "usage: inside <workers>" );

  // Allocate our statically-sized list circles.
  cList = (Circle *) malloc( size * sizeof( Circle ) );

  //initialize semaphores
  sem_init(&semCount, 0, 1);
  sem_init(&semCount, 0 ,1);
  sem_init(&semWait, 0, 1);

  // Make each of the workers.
  pthread_t worker[ workers ];
  for ( int i = 0; i < workers; i++ )
    pthread_create(&worker[i], NULL, workerRoutine, NULL);

  // Then, start getting work for them to do.
  readCircles();

  // Wait until all the workers finish.
  for ( int i = 0; i < workers; i++ )
    pthread_join(worker[i], NULL);

  // Report the total and release the semaphores.
  printf( "Total: %d\n", total );
  
  return EXIT_SUCCESS;
}

