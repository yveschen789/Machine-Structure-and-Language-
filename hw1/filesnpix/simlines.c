/*
 Authors: Yves Chen , ychen22
          Theogene Micomyiza, tmicom01

 COMP40 HW1 Filesnpix

 9/16/2019

*/



#include <stdio.h>
#include <stdlib.h>
#include "readaline.h"
#include "seq.h"
#include "table.h"
#include "atom.h"

#define true 1
#define false 0

int const Hint = 100;

struct fileInfo{
    char *filename;
    int lineNum;
};

typedef struct fileInfo fileInfo;

struct matchingGroup {
    char const * atomLine;
    Seq_T fileInfo;
};

typedef struct matchingGroup matchingGroup;

void main_helper(Table_T *dataTable, FILE *file, char *fileName);

/*formats line to the format described in SimLine Spec*/
void FormatLine(char **, size_t size);

/*in charge of managing hashtable*/
void Hashtable(Table_T *table, char const * atom, fileInfo *);
void printSim(const void *key, void **value, void *cl);

int FileHandle(int argc, char *argv[]);

//used to free memory allocated to every struct
void FreeStructs(const void *key, void **value, void *cl);

int main(int argc, char *argv[])
{

    if(FileHandle(argc, argv)){

        Table_T dataTable = Table_new(Hint, NULL , NULL);
        for (int i = 1; i < argc; i++){
            FILE *filename = fopen(argv[i], "r");
            main_helper(&dataTable, filename, argv[i]);
            fclose(filename);
        }
        Table_map(dataTable, printSim, NULL);

        //free memory for structs in the table
        Table_map(dataTable, FreeStructs, NULL);



        Table_free(&dataTable);

    }

    return 0;

}

void main_helper(Table_T *dataTable, FILE *file, char *fileName){

    char const *atom;
    size_t size = 0;
    char *datapp = NULL;
    char **datappPtr = &datapp;
    fileInfo *fileStruct;
    
    int count = 0;

    do {
        count ++;
        fileStruct = malloc(sizeof(fileInfo));
        fileStruct->filename = fileName;
        fileStruct-> lineNum =  count;

        size = readaline(file, datappPtr);
        
    if(size != 0){
        FormatLine(datappPtr, size);
        const char *atomPtr = *datappPtr;
        atom  = Atom_string(atomPtr);
        Hashtable(dataTable, atom, fileStruct);
    }

        free(datapp);

    }while(size != 0);

}

//eliminates all char element in the line that
// are not valid as specified in spec of the hw
void FormatLine(char **line, size_t size){

    if (*line == NULL)
        return;

    char *newArray = malloc(size * sizeof(char));

    int count = 0;
    int index_newArray = 0;

    int Prev_space = false;
    while (*(*line + count) != '\n' && count < (int)size){
        char element = *(*line + count);
        
        //in case first elements of the line are spaces
        if(element == 32 && index_newArray == 0){
            count ++;
            continue;
        }

        else {

            if (element == 32)
                Prev_space = true;

            if ((element >= 65 && element <= 90) || 
                (element >= 48 && element <= 57) || 
                (element >= 97 && element <= 122) || (element == 95) )
            {
                if(!Prev_space){
                    newArray[index_newArray] = element;
                    index_newArray ++;
                }
                else{

                    newArray[index_newArray] = 32;

                    newArray[index_newArray + 1] = element;
                    Prev_space = false;
                    index_newArray += 2;
                }
            }


            count++;
        }
    }

    newArray[index_newArray] = '\0';
    free(*line);
    *line = newArray;
}

//manages hashtable
void Hashtable(Table_T *table, char const * atom, fileInfo *file){
    if (table != NULL && atom != NULL && file != NULL)
    {
        void *IsIn = Table_get(*table, atom);
        if (IsIn == NULL)
        {
            matchingGroup *new_line = malloc(sizeof(matchingGroup));
            new_line->atomLine = atom;
            new_line->fileInfo = Seq_new(5);
            Seq_addhi(new_line->fileInfo, file);
            table = Table_put(*table, atom, new_line);

        }
        else{

            matchingGroup *exitGroup = IsIn;
            Seq_addhi(exitGroup->fileInfo, file);
        }
    }

    else exit(EXIT_FAILURE);
}

//checks validity of input files
int FileHandle(int argc, char *argv[]){


    if (argc > 1){
        for (int i = 0; i < argc; i++){
            FILE *file = fopen(argv[i], "r");
                if (file == NULL){
                    printf("error handling input file. Terminating");
                    exit(EXIT_FAILURE);
                }

                else {
                    fclose(file);
                }
        }
        return true;
    }  

    return false;
}  

//it's only used by Table_map
//prints the output on stderr in the specified format
void printSim(const void *key, void **value, void *cl){
    
   
    (void) cl;

    
    if(key == NULL || value == NULL || *value == NULL)
        return;

    if (*((char*)key) =='\0')
        return;

    printf("%s\n", (char*)key);

    Seq_T fileSeq = ((matchingGroup *)(*value))->fileInfo;
    
    int count = 0;
    int length = Seq_length(fileSeq);

    while (length > count)
    {
        fileInfo *temp = Seq_get(fileSeq, count);
        printf("%s------------------", temp->filename);
        printf("%d\n", temp->lineNum);


        free((fileInfo*)Seq_get(fileSeq, count));
        count++;
    }
} 


//function used by Table_map to free memory
void FreeStructs(const void *key, void **value, void *cl){

    (void) cl;
    (void)key;
    Seq_T fileSeq = ((matchingGroup *)(*value))->fileInfo;
    

    Seq_free(&fileSeq);

    free(*value);
}