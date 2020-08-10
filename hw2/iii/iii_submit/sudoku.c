/*
    Authors: Mico Theogene Micomyiza
             Yves Chen

    COMP40 FALL 2019
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>


#include <uarray2.h>
#include "pnmrdr.h"

void Populate_uarray(Pnmrdr_T PnmPayload, UArray2_T uarray);
void check_row(int row, int col, UArray2_T uarray, void *cl2,void *cl);
void check_col(int row, int col, UArray2_T uarray, void *cl2,void *cl);
void check_box_3x3(UArray2_T uarray, int *temparray, int row, int col);
void check_box_helper(UArray2_T uarray, int *array);
void init_arr(int * array);


int const MAX_SIZE = 9;

int main(int argc, char const *argv[])
{

        UArray2_T values = UArray2_new(MAX_SIZE,MAX_SIZE,sizeof(int));
        FILE *firstfile;
        //in case more than one argument is given
        if(argc > 2){
            printf("To many arguments!\n");
            exit(EXIT_FAILURE);
        }

        if (argc > 1)
        {
            firstfile = fopen(argv[1], "r");

            assert(firstfile != NULL);
        
            Pnmrdr_T PnmPayload = Pnmrdr_new(firstfile);
            Populate_uarray(PnmPayload, values);

            Pnmrdr_free(&PnmPayload);
            fclose(firstfile);
        } 
        else{
            Pnmrdr_T PnmPayload = Pnmrdr_new(stdin);
            Populate_uarray(PnmPayload, values);
            Pnmrdr_free(&PnmPayload);
        } 

        UArray2_free(&values);

        return EXIT_SUCCESS;
}


void Populate_uarray(Pnmrdr_T PnmPayload, UArray2_T uarray){

        int input;
        
        Pnmrdr_mapdata info = Pnmrdr_data(PnmPayload);

        assert((int)info.width == MAX_SIZE && (int)info.height == MAX_SIZE);

        for (int i = 0; i < (int)info.width; i++){
            for (int j = 0; j < (int)info.height; j++){
                
                input = Pnmrdr_get(PnmPayload);
                /*enforces that every element is range 1...9 inclusive*/
                assert(input > 0 && input <= MAX_SIZE);
                
                *(int *)UArray2_at(uarray, i, j) = input;
                
            }
        }


        /*aux array to store information of current row or column to help
        identify collision*/
        int* nineArray = malloc(9*sizeof(int));
        init_arr(nineArray);

        /*check whether columns, rows, and sub_matrices were solved well*/
        UArray2_map_col_major(uarray, check_row, nineArray);
        UArray2_map_row_major(uarray, check_col, nineArray);
        check_box_helper(uarray, nineArray);

        free(nineArray);


}

void check_box_helper(UArray2_T uarray, int * temparray){
 
    for (int i = 0; i <= 6; i +=3 ){
        for (int j = 0; j <= 6; j += 3){
            init_arr(temparray);
            check_box_3x3(uarray, temparray, i , j);

        }
    }
    
}

void init_arr(int * array){
    for (int i = 0 ; i < MAX_SIZE; i++)
        array[i] = -1;
}


void check_box_3x3(UArray2_T uarray, int *temparray, int row, int col){

        int colOne = col;
        int rowOne = row;

        int value;

        /*evaluates whether the sub_matrices were solved as well*/
        for (int i = 0; i < 3; i++){
            colOne = col;
            for (int j = 0; j < 3; j++){

                value = *((int*)UArray2_at(uarray, rowOne, colOne));

                if(temparray[value - 1] == -1){
                    /*no collision, every element in the box is differnt*/
                    temparray[value - 1] = value;
                }

                else{
                    /*Soduko not solved*/
                     exit(EXIT_FAILURE);
                }

               colOne++;
            } 
             rowOne++;
        }
}


void check_row(int row, int col, UArray2_T uarray, void *cl2,void *cl){
    
        assert( *((int *)UArray2_at(uarray, row, col)) == *((int *)cl2));
        int value = *((int *)cl2);

        /*Note: cl is initialized to -1, if the value is not -1, then
        there is a collision. which means that Sudoku isn't solved*/
        if( ((int*)cl)[value - 1] == -1){
            ((int *)cl)[value - 1] = value;
        }
        else{
            exit(EXIT_FAILURE);
        }
        

        if (col == MAX_SIZE - 1){
            //initialize our temp array
            init_arr(cl);
            
        }   

}

void check_col(int row, int col, UArray2_T uarray, void *cl2,void *cl){
        /*check that cl2 has correct information*/
        assert(*((int *)UArray2_at(uarray, row, col)) == *((int *)cl2));
        int value = *((int *)cl2);

        /*Note: cl is initialized to -1, if the value is not -1, then
        there is a colision. which means that Sudoku isn't solved*/
        if(((int*)cl)[value - 1] == -1){
            ((int *)cl)[value - 1] = value;
        }
        else {
            /*Sudoku isn't solved*/
            exit(EXIT_FAILURE);
        }
        

        if (row == MAX_SIZE - 1){
            //initialize temp array
            init_arr(cl);
        }   

}


