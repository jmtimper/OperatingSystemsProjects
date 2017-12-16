// Elapsed Real Time for input-c4.txt:
// Elapsed Real Time for input-c5.txt:

#include <stdio.h>
#include <stdbool.h>
#include <cuda_runtime.h>

// Simple struct for representing a circle.
typedef struct {
  int x, y;
  int r;
} Circle;

// List of all circles.
Circle *cList;

// Number of circles on our list.
int cCount = 0;

// Function run by each thread.  You're going to need to pass
// in some more parameters.
__global__ void countCircles( int n, Circle * list, int *out ) {

  // Determine a unique index for this worker.
  int i = blockDim.x * blockIdx.x + threadIdx.x;

  // counter
  int countTemp = 0;
  
  // Make sure I actually have something to work on.
  if ( i < n ) {
    // This thread has a circle to work on.
    Circle *c = list + i;
    for ( int x = c->x - c->r; x <= c->x + c->r; x++ ){
      for ( int y = c->y - c->r; y <= c->y + c->r; y++ ){
        int dx = x - c->x;
        int dy = y - c->y;
        if ( dx * dx + dy * dy <= c->r * c->r ) {
          bool counted = false;
          for ( int k = 0; !counted && k < i; k++ ){
            Circle *u = list + k;
            int dx = x - u->x;
            int dy = y - u->y;
            if (  dx * dx + dy * dy <= u->r * u->r  )
              counted = true;
          }
          if ( !counted )
            countTemp++;
        }
      }
    }
    out[i] = countTemp;
  }
}

// General function to report a failure and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message ); 
  exit( 1 );
}

// Read the list of circles.
void readCircles() {
  // Use a resizable array, increasing capacity as we read more values.
  int capacity = 10;
  cList = (Circle *) malloc( capacity * sizeof( Circle ) );

  // Read circle definitions until we can't read any more.
  Circle c;
  while ( scanf( "%d%d%d", &c.x, &c.y, &c.r ) == 3 ) {
    // Grow the array if needed.
    if ( cCount >= capacity ) {
      capacity *= 2;
      cList = (Circle *) realloc( cList, capacity * sizeof( Circle ) );
    }
    
    // Add the circle we just read to the lsit.
    cList[ cCount ] = c;
    cCount++;
  }
}

int main( ) {
  readCircles();

  // Block and grid dimensions.
  int threadsPerBlock = 250;
  // Round up to get the number of blocks we need.
  int blocksPerGrid = ( cCount + threadsPerBlock - 1 ) / threadsPerBlock;

  //copy over clist to GPU
  Circle *list = NULL;
  cudaMalloc( (void **)&list, cCount * sizeof(Circle));
  cudaMemcpy(list, cList, cCount * sizeof(Circle), cudaMemcpyHostToDevice);
  //int array for gpu output
  int *gpuOut = NULL;
  cudaMalloc( (void **)&gpuOut, cCount * sizeof(int));

  // Run our kernel on these block/grid dimensions.  You'll need to
  // pass in some more parameters.
  countCircles<<<blocksPerGrid, threadsPerBlock>>>( cCount, list , gpuOut);
  if ( cudaGetLastError() != cudaSuccess )
    fail( "Failure in CUDA kernel execution." );

  // int array of output from gpu
  int result[cCount];
  cudaMemcpy(result, gpuOut, cCount * sizeof(int), cudaMemcpyDeviceToHost);
  
  // Add up the total and report it.
  int total = 0;
  for(int i = 0; i < cCount; i++)
    total += result[i];
  printf( "Total: %d\n", total );

  // Free resources and reset the device.
  free( cList );
  cudaFree( list );
  cudaFree( gpuOut );
  cudaDeviceReset();
}
