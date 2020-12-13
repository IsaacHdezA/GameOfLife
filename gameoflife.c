#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// include certain library depending the OS.
#ifdef _WIN32
	#include <windows.h>
#elif __linux__
	#include <unistd.h>
#endif

// Function prototypes.
int **createMatrix( const int, const int );
void printMatrix( const int **, const int, const int );
void freeMatrix( int **, const int );

// Manejo de archivo.
unsigned int countRows( FILE * );
unsigned int countColumns( FILE * );
int **importMatrix( FILE * );
void copyMatrix( int **, const int **, const int, const int );
unsigned int countNeighbors( const int **, const int , const int, const int, const int );

int main( void ) {

	char filename[100];
	printf( "Enter the name with the initial state: " );
	scanf( "%s", filename );

	FILE *file = fopen( filename, "rt" );

	if( file ) {
		unsigned int rows = 0, cols = 0;
		int **matrix = NULL, **newMatrix = NULL;

		// Counting the rows and columns included in the file.
		rows = countRows( file ); cols = countColumns( file );

		// Import the initial state matrix from the file.
		matrix = importMatrix( file );		// The present step matrix.
		newMatrix = createMatrix( rows, cols );	// New matrix. It'll be the next to compare in each step.

		// Closing the file after using it.
		fclose( file );

		// This is the execution of the program, it will repeat itself 1000 times, you can change it for more/few..
		unsigned int gen = 0;
		while( gen < 1000 ) {
			// Visiting every cell looking for living neighbors.
			for( int i = 0; i < rows; i++ ) {
				for( int j = 0; j < cols; j++ ) {
					int state = matrix[i][j];	// The living/dead state of the current cell.

					// Counting the neighbors from the current cell.
					unsigned int vecinos = countNeighbors( (const int **) matrix, rows, cols, i, j );

					// Conway's Game of life conditions.
					if( state == 0 && vecinos == 3 ) newMatrix[i][j] = 1;
					else if( state == 1 && ( vecinos < 2 || vecinos > 3 ) ) newMatrix[i][j] = 0;
					else newMatrix[i][j] = state;
				}
			}

			#ifdef _WIN32
				Sleep( 10 );
				system( "cls" );
			#elif __linux__
				usleep( 60000 );
				system( "clear" );
			#endif
			printf( "Generation: %d\n", gen );
			printMatrix( (const int **) matrix, rows, cols );
			copyMatrix( matrix, (const int **) newMatrix, rows, cols );

			gen++;
		}
		
		// Reclaiming the memory used for the matrices.
		freeMatrix( matrix,    rows );
		freeMatrix( newMatrix, rows );
	} else printf( "The file %s doesn't exists.\n\n", filename );

	return 0;
}

// Function definitions.
int **createMatrix( const int rows, const int cols ) {
	// Allocating memory for a dinamic matrix.
	int **matrix = ( int ** ) malloc( sizeof( int * ) * rows );
	if( matrix ) {
		// Creating an array in every row.
		for( int i = 0; i < rows; i++ ) matrix[ i ] = ( int * ) malloc( sizeof( int ) * cols );

		// Initializing the matrix in a consistent state.
		for( int i = 0; i < rows; i++ )
			for( int j = 0; j < cols; j++ )
				matrix[i][j] = 0;
	} else return NULL;

	return matrix;
}

void printMatrix( const int **matrix, const int rows, const int cols ) {
	for( int i = 0; i < rows; i++ ) {
		for( int j = 0; j < cols; j++ ) {
			if( matrix[ i ][ j ] == 0 ) printf( " " );
			else printf( "*" );
		}

		printf( "\n" );
	}
}

void freeMatrix( int **matrix, const int rows ) {
	// Reclaiming the memory allocated for the matrix.
	for( int i = 0; i < rows; i++ ) free( matrix[ i ] );
	free( matrix );
}

// File management.
unsigned int countRows( FILE *file ) {
	char cad[ 999 ];
	unsigned int rows = 0;

	// Counting each row.
	rewind( file );
	while( fgets( cad, 999, file ) != NULL && cad[0] != '\n' ) rows++;
	return rows;
}

unsigned int countColumns( FILE *file ) {
	char cad[ 999 ];
	unsigned int cols = 1;

	// Counting how many elements (comma separated) there are in each row.
	rewind( file ); fgets( cad, 999, file );
	int k = 0;
	while( cad[k] )
		if( cad[k++] == ',' ) cols++;

	return cols;
}

// Import the matrix from the file in our dynamic memory matrix.
int **importMatrix( FILE *file ) {
	char cad[ 999 ];
	unsigned int rows = 0, cols = 1;

	rows = countRows( file );
	cols = countColumns( file );
	int **matrix = createMatrix( rows, cols );

	rewind( file );

	// Copying each element from the file.
	int i = 0;
	while( fgets( cad, 999, file ) ) {
		int num = atoi( strtok( cad, "," ) );
		matrix[i][0] = num;

		for( int j = 1; j < cols; j++ ) {
			num = atoi( strtok( NULL, "," ) );
			matrix[i][j] = num;
		}
		i++;
	}

	return matrix;
}

// Copy a matrix from source to dest (both have to be same size)..
void copyMatrix( int **dest, const int **source, int rows, int cols ) {
	for( int i = 0; i < rows; i++ )
		for( int j = 0; j < cols; j++ )
			dest[i][j] = source[i][j];
}

// Counting neighbots function.
unsigned int countNeighbors( const int **matrix, const int rows, const int cols, const int y, const int x ) {
	unsigned int sum = 0;
	for( int i = -1; i < 2; i++ ) {
		for( int j = -1; j < 2; j++ ) {
			int row    = ( y + i + rows ) % rows;
			int column = ( x + j + cols ) % cols;
			sum += matrix[row][column];
		}
	}

	sum -= matrix[y][x];

	return sum;
}
