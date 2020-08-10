/*
 Authors: Yves Chen , ychen22
          Theogene Micomyiza, tmicom01

 COMP40 HW1 Filesnpix

 9/16/2019

*/


#include "readaline.h"
#include "seq.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>


size_t readaline(FILE *inputfd, char **datapp){

    //if file can't be opened
    if (inputfd == NULL)
    {
        exit(EXIT_FAILURE);
    }

    char input;
    
    Seq_T Darray = Seq_new(200);


    //in case there is error in memory allocation
    if (datapp == NULL){
        exit(EXIT_FAILURE);
    }

    while((input = getc(inputfd)) != '\n' && 
        input != EOF){

        char * temp = NULL;
        temp = malloc(sizeof(char));
        *temp = input;


        Seq_addhi(Darray, temp);
    }

    if (input == '\n'){
        char * temp = NULL;
        temp = malloc(sizeof(char));
        *temp = input;
        Seq_addhi(Darray, temp);
    }

    int lineSize = Seq_length(Darray);
    char * char_array = malloc(lineSize + 1);

    for (int i = 0; i < lineSize; i++){
        char * newChar = Seq_get(Darray, i);
        char_array[i] = *newChar;
        free((char*)Seq_get(Darray, i));
    }
    char_array[lineSize] = '\0';

    *datapp = char_array;
    Seq_free(&Darray);

    size_t size = strlen(*datapp);

    if (input == EOF){
        free(*datapp);
        *datapp = NULL;
        return 0;
    }

    return size;
}