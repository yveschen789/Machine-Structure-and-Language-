/*
    Authors: Mico Theogene Micomyiza
             Yves Chen

    COMP40 FALL 2019
*/


#ifndef Bit2_INCLUDED
#define Bit2_INCLUDED


#include "bit.h"
#include <stdio.h>

typedef Bit_T Bit2_T;

/*
Description: creates an unboxed bit array of size height*width.

Parameters: integer value for height, width will be stored in the
array in order to correctly allocate enough memory.

Returns: a Bit2_T to client
*/
Bit2_T Bit2_new( int height, int width);


/* 
Description: applies the given function to a group of elements in the given
unboxed array

Parameters: Bit2_T, function pointer, and row index. A cl void
pointer as a check

Returns: nothing since it’s a void function
*/

void Bit2_map_row_major(Bit2_T barray, void apply
    (int row, int col, Bit2_T barray, int cl2,void *cl), void *cl);



/*
Description: applies the given function to every element in the column of
the unboxed
array
Parameters: Bit2_T, function pointer, and column index. A cl void pointer
as a check
Returns: nothing since it’s a void function
*/
 void Bit2_map_col_major(Bit2_T barray, void apply
    (int row, int col, Bit2_T array, int cl2,void *cl), void *cl);



/*
Description: free all memory that was allocated to barray

Parameters: a Bit2_T pointer to the unboxed array to be freed
Return: none
*/
void Bit2_free(Bit2_T *barray);



/*
Description: takes a barray2 and returns the width of that array. 

Parameters: a Bit2_T

Returns: an int representing the width of the barray2
*/
int Bit2_width(Bit2_T barray);


/*
Description: finds element at array[row][col] and returns it. 

Parameters: Bit2_T array, row index and column index;

Returns: 0 or 1 
*/
int Bit2_get(Bit2_T barray, int row, int col);


/*
Description:  finds element at array[row][col] and changes it to the value
parameter provided by the client.

Parameters: Bit2_T array, row index and column index, as well as a value
to replace;

Returns : 0 or 1, which is the value at the array [row, col] before
modification
*/
int Bit2_put(Bit2_T barray, int row, int col, int value);


/*
Description: takes a barray2 and returns the height of that array. 

Parameters: a Bit2_T

Returns: an int representing the height of the barray2
*/
int Bit2_height(Bit2_T barray);





#undef Bit2_T
#endif