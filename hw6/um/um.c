/******************************************************************************
 *
 *     um.c
 *
 *     Assignment: Comp40 HW6 (UM)
 *     Authors:    Era Iyer (eiyer01) and Yves Chen (ychen22)
 *     Date:       11/19/2019
 *
 *     This file serves as the main for our um program. It is responsible
 *     for creating and allocating space for the overall memory structure
 *     as well as segment zero. It calls the appropriate functions to load
*      the um file and execute each instruction. All memory is freed at the end
 *
 *****************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "assert.h"
#include "seq.h"
#include "load.h"
#include "execute.h"
#include "skeleton.h"

#define INIT_SIZE 10

int main(int argc, char const *argv[]) {

    assert(argc == 2);
    FILE *fp = NULL;
    fp = fopen(argv[1], "r");
    assert(fp != NULL);

    struct stat st;
    stat(argv[1], &st);
    int size = st.st_size;

    /* allocating space for skeleton struct and all sequences within it */
    struct Skeleton *skeleton = malloc(sizeof(struct Skeleton));

    Seq_T unmap = Seq_new(INIT_SIZE);
    skeleton->unmapped = unmap;

    Seq_T skeletonSeq = Seq_new(INIT_SIZE);
    skeleton->skeleton_seq = skeletonSeq;

     Seq_T segmentZero = Seq_new(size/4);
     Seq_addlo(skeleton->skeleton_seq, segmentZero);


     /* calling appropriate modules for loading and executing instructions */
     loadAndStore(skeleton->skeleton_seq, fp);
     execute(skeleton);

     /* freeing memory allocated for sequences and closing files */
     fclose(fp);
     Seq_free(&skeleton->unmapped);

    int skel_len = Seq_length(skeleton->skeleton_seq);
     for(int i = 0; i < skel_len; i++){
      void *s = Seq_get(skeleton->skeleton_seq, i);

      if(s != NULL){
          Seq_T seg = (Seq_T)s;
          Seq_free(&seg);
      }

    }
     Seq_free(&skeleton->skeleton_seq);
     free(skeleton);

    return 0;
}
