/*
 Authors: Yves Chen , ychen22
          Theogene Micomyiza, tmicom01

 COMP40 HW1 Filesnpix

 9/16/2019

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "pnmrdr.h"


void brightness(Pnmrdr_T);

int main (int argc, char *argv[]) {
        if (argc > 1){
                FILE *firstfile = fopen(argv[1], "r");

                if (firstfile == NULL){
                    printf("error handling input file. Terminating");
                    exit(EXIT_FAILURE);
                }    
               
                Pnmrdr_T PnmPayload = Pnmrdr_new(firstfile);
                brightness(PnmPayload);
                Pnmrdr_free(&PnmPayload);

                fclose(firstfile);
                return EXIT_SUCCESS;

        }
        else {

                Pnmrdr_T PnmPayload = Pnmrdr_new(stdin);
                brightness(PnmPayload);
                Pnmrdr_free(&PnmPayload);
                return EXIT_SUCCESS;
        }
       

    return 0;
}

void brightness(Pnmrdr_T PnmPayload){

                unsigned sum = 0;
                double counter = 0;
                Pnmrdr_mapdata info = Pnmrdr_data(PnmPayload);
                double size = info.width * info.height;

                do{
                    unsigned temp = Pnmrdr_get(PnmPayload);
                    sum += temp;
                    counter ++;

                }while(counter < size);

                float denominator = info.denominator;

                float Average = (float)((sum/(counter * denominator)));
                printf(" %.3f\n", Average);
}