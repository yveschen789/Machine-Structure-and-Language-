/*
    Authors: Mico Theogene Micomyiza
             Yves Chen

    COMP40 FALL 2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include <stack.h>
#include <bit2.h>
#include "pnmrdr.h"



/*identify an element in a stack*/
struct Edge_Indices{
    int row;
    int col;
};

typedef struct Edge_Indices Edge_Indices;

void Populate_Bit2(Pnmrdr_T PnmPayload, Bit2_T barray);
Bit2_T pbmread(FILE *inputfp);
void check_Width(int init_row, int init_col, Bit2_T barray, int cl2,void *cl);
void check_Height(int init_row, int init_col, Bit2_T barray, 
                    int cl2, void *cl);
void UnBlackenEdges(int init_row, int init_col, Bit2_T barray);
void stack_handler(Bit2_T barray, Stack_T EdgeStack, int row, int col);
void check_and_print(int i, int j, Bit2_T a, int b, void *p1);
void print(Bit2_T barray);

int main(int argc, char const *argv[])
{
    
        Bit2_T bitArray;
        FILE *firstfile;

        //in case more than one argument is given
        if(argc > 2){
            printf("To many arguments!\n");
            exit(EXIT_FAILURE);
        }

        if (argc > 1)
        {
            firstfile = fopen(argv[1], "r");
            if(firstfile == NULL){
                printf("File not found!\n");
                exit(EXIT_FAILURE);
            }

            bitArray = pbmread(firstfile);
            fclose(firstfile); 
        } 
        else{
             bitArray = pbmread(stdin);  
        } 

        /*process the input to remove black edges*/
        Bit2_map_col_major(bitArray, check_Width, NULL);
        Bit2_map_row_major(bitArray, check_Height, NULL);

        /*print results*/
        print(bitArray);

        Bit2_free(&bitArray);


        return 0;
}

/*in charge of processing the file and read info into pnmrdr
that would be used to populate bit array*/
Bit2_T pbmread(FILE *inputfp){

        Pnmrdr_T PnmPayload;

        PnmPayload = Pnmrdr_new(inputfp);

        Pnmrdr_mapdata info = Pnmrdr_data(PnmPayload);

        if (info.height <= 0 || info.width <= 0){
            printf("Invalid height and/or width values!\n");
            exit(EXIT_FAILURE);
        }

        Bit2_T values = Bit2_new(info.width, info.height);

        Populate_Bit2(PnmPayload, values);
        Pnmrdr_free(&PnmPayload);


        return values;
}


void Populate_Bit2(Pnmrdr_T PnmPayload, Bit2_T barray){

        int input;
        
        Pnmrdr_mapdata info = Pnmrdr_data(PnmPayload);
        /*read bits into barray in their corresponding positions*/
        for (int i = 0; i < (int)info.width; i++){
            for (int j = 0; j < (int)info.height; j++){
                
                input = Pnmrdr_get(PnmPayload);
                Bit2_put(barray, i, j, input);

                /*make sure that we are getting the correct element*/
                assert(Bit2_get(barray, i, j) == input);
            }
        }

}

/*find black edges on top row and bottom row*/
void check_Width(int row, int col, Bit2_T barray, int cl2,void *cl){

        (void)cl;

        if (row == 0 || row == Bit2_width(barray) - 1){
            if (cl2 == 1)
                UnBlackenEdges(row, col, barray);
        }
}

/*find black edges on left column and right column*/
void check_Height(int row, int col, Bit2_T barray, int cl2,void *cl){

        (void)cl;
        if (col == 0 || col == Bit2_height(barray) - 1){
            if (cl2 == 1)
                UnBlackenEdges(row, col, barray);
        }
}


/*this function uses BFS inspired algorithm in order to acomplish the task*/
void UnBlackenEdges(int init_row, int init_col, Bit2_T barray){

        int curr_row = init_row;
        int curr_col = init_col;

        /*aux variable to help store indices of neighboring edges of the 
        current black edge*/
        int top_row, top_col, bot_row, bot_col, right_row, right_col,
        left_row, left_col;


        /*stack to store neighboring edges*/
        Stack_T EdgeStack = Stack_new();

        /**/
        Edge_Indices *value;


        Edge_Indices *temp = malloc(sizeof(temp));
        temp->row = curr_row;
        temp->col = curr_col;

        /*add current pixel to the stack*/
        Stack_push(EdgeStack, temp);

        /*turn it into a white edge*/
        Bit2_put(barray, temp->row, temp->col, 0);

        while (true){
            /*identify neighbors of current pixel*/
            top_row = curr_row - 1;
            top_col = curr_col;
            bot_row = curr_row + 1;
            bot_col = curr_col;
            right_row = curr_row;
            right_col = curr_col + 1;
            left_row = curr_row;
            left_col = curr_col - 1;

            /*check if any neighbors are black edges or not. if yes,
            then proceed to turn into a white edge*/
            stack_handler(barray, EdgeStack, top_row, top_col);
            stack_handler(barray, EdgeStack, bot_row, bot_col);
            stack_handler(barray, EdgeStack, right_row, right_col);
            stack_handler(barray, EdgeStack, left_row, left_col);

            /*pop edge that has been processed so that it won't be repeated*/
            value = (Edge_Indices*)(Stack_pop(EdgeStack));
            curr_row = value->row;
            curr_col = value->col;

            /*check if the poped element was last element in the stack,
            which was the first element to be added. If yes, break the loop.
            Otherwise, continue looping*/
            if (curr_row == init_row && curr_col == init_col){
                free(temp);
                break;
            }
            free(value);
        }

        Stack_free(&EdgeStack);
}


void stack_handler(Bit2_T barray, Stack_T EdgeStack, int row, int col){
        /*check if indices are not out of bounds*/
        if ((row >= 0 && row <= Bit2_width(barray) - 1) &&
            (col >= 0 && col <= Bit2_height(barray) - 1)){
                /*check if the current element is a black pixel and add
                it to the stack of neighbors of black edge
                , and turn it into a white
                edge*/
                if (Bit2_get(barray, row, col) == 1){
                    Edge_Indices *temp = malloc(sizeof(temp));
                    temp-> row = row;
                    temp -> col = col;
                    Stack_push(EdgeStack, temp);
                    Bit2_put(barray, temp->row, temp->col, 0);
                }
            }

}


void print(Bit2_T barray){
        //print results on stderr
        printf("P1\n");
        printf("%d ", Bit2_width(barray));
        printf("%d\n", Bit2_height(barray));

        Bit2_map_col_major(barray, check_and_print, NULL);
}



void check_and_print(int i, int j, Bit2_T a, int b, void *p1){
    
        /*intentionally silenced the warning because p1 is unused*/
        (void)p1;
        assert(b == Bit2_get(a, i, j));

        printf("%d", b );

        /*at the end of line print new line*/
        if(j == Bit2_height(a)-1){
            printf("%c",'\n'); 
        }
}