/*
    Authors: Mico Theogene Micomyiza
             Yves Chen

    COMP40 FALL 2019
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "assert.h"

#include "bit2.h"


int HEIGHT = -1, WIDTH = -1;


Bit2_T Bit2_new(int height, int width){

    //check for valid input
    assert(height > 0 && width);

    HEIGHT = height;
    WIDTH = width;

    return (Bit2_T)(Bit_new(height * width));
}


/*frees the memory*/
void Bit2_free(Bit2_T *barray){
        Bit_free(barray);
}


/*private function to identify which cell in 1D array
that corresponds to the [row, col] in 2D array*/
int Convert_to_one_dim_index(int row,int col){

        assert(row >= 0 && col >= 0);
        assert(row < HEIGHT && col <  WIDTH);

        return (WIDTH * row) + col;
}

int Bit2_get(Bit2_T barray, int row, int col){
        return Bit_get(barray, Convert_to_one_dim_index(row, col));
}


int Bit2_put(Bit2_T barray, int row, int col, int value){

        assert(value == 1 || value == 0);

        return Bit_put(barray, Convert_to_one_dim_index(row, col), value);
}



int Bit2_width(Bit2_T barray){
        /*if width is <= 0, then the bit array doesn't exist*/
        assert(WIDTH > 0);

        int width = Bit_length((Bit2_T)barray) / WIDTH;
        return width;
}



int Bit2_height(Bit2_T barray){
        /*if height is <= 0, then the bit array doesn't exist*/
        assert(HEIGHT >0);
        
        int height = Bit_length((Bit_T)barray) / HEIGHT;

        return height;
}




void Bit2_map_col_major(Bit2_T barray, void apply
    (int row, int col, Bit2_T barray, int cl2,void *cl), void *cl){
    
        int row, col, length;

        length = Bit_length((Bit_T)barray);

        for (int i = 0; i < length; ++i)
        {
            /*Note: (WIDTH * row) + col to identity 1D array index,
            then row is quotient of the result, and col is remainder.
            So here we transform from 1D index to 2D indeces*/
            row = (i / WIDTH);
            col = (i % WIDTH);
            apply(row, col, barray, Bit_get(barray, 
                                Convert_to_one_dim_index(row, col)), cl);

        }

}


void Bit2_map_row_major(Bit2_T barray, void apply
    (int row, int col, Bit2_T barray, int cl2,void *cl), void *cl){
    
        int row, col;

        for (int i = 0; i < WIDTH; ++i)
        {
            /*j increments by Width because elements of same column are Width 
            distance apart from each other*/
            for (int j = i; j < HEIGHT * WIDTH; j += WIDTH)
            {
                /*Note: (WIDTH * row) + col to identity 1D array index,
                then row is quotient of the result, and col is remainder.
                So here we transform from 1D index to 2D indeces*/
                row = (j / WIDTH);
                col = (j % WIDTH);
                apply(row, col, barray, Bit_get(barray, 
                                    Convert_to_one_dim_index(row, col)), cl);
            }

        }

}