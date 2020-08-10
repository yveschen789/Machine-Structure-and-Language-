/******************************************************************************
 *
 *     operations.c
 *
 *     Assignment: Comp40 HW6 (UM)
 *     Authors:    Era Iyer (eiyer01) and Yves Chen (ychen22)
 *     Date:       11/19/2019
 *
 *     This file contains definitions for each of the 14 possible instructions.
 *     This module communicates with the execute module, and performs the
 *     appropriate instruction and modifying the necessary segments.
 *
 *****************************************************************************/

#include "assert.h"
#include "operations.h"

/*
 * Name: halt
 * Parameters: nothing
 * Purpose: returns true to execute and sets local boolean to true and execute
 *          exits while loop
 * Returns: true boolean value
*/
bool halt(){
  return true;
}

/*
 * Name: add
 * Parameters: uint32_t *a, uint32_t *b, uint32_t *c
 * Purpose: sets the value at register a equal to the value at register b plus
 *          the value at register c
 * Returns: nothing
*/
void add(uint32_t *a, uint32_t *b, uint32_t *c){
  *a = (*b + *c) % 4294967296;
}

/*
 * Name: loadval
 * Parameters: uint32_t *reg, uint32_t value
 * Purpose: sets the value at the register given in the parameter to the value
 *          provided in the parameter
 * Returns: nothing
*/
void loadval(uint32_t *reg, uint32_t value){
    *reg = value;
}

/*
 * Name: multiply
 * Parameters uint32_t *a, uint32_t *b, uint32_t *c
 * Purpose: takes 3 registers and multiplies register b and c and stores it
 *          in memory where c points to after by modding 2^32
 * Returns: nothing
*/
void multiply(uint32_t *a, uint32_t *b, uint32_t *c){
  *a = (*b * *c) % 4294967296;
}

/*
 * Name: divide
 * Parameters: uint32_t *a, uint32_t *b, uint32_t *c
 * Purpose: sets the value at register a equal to the value at register b
 *          divided by the value at register c
 * Returns: nothing
*/
void divide(uint32_t *a, uint32_t *b, uint32_t *c){
  *a = (*b / *c);
}

/*
 * Name: bitwiseNand
 * Parameters: uint32_t *reg, uint32_t value
 * Purpose: gets the not of register b and register c and sets value at
 *          regsiter a equal to this
 * Returns: nothing
*/
void bitwiseNand(uint32_t *a, uint32_t *b, uint32_t *c){
  *a = ~(*b & *c);
}

/*
 * Name: conditional_move
 * Parameters: uint32_t *a, uint32_t *b, uint32_t *c
 * Purpose: checks if the memory pointed to by pointer c contains 0, if not
            it puts the value pointed to by b in a.
 * Returns: nothing
*/
void conditional_move(uint32_t *a, uint32_t *b, uint32_t *c){
  if(*c != 0){
    *a = *b;
  }
}

/*
 * Name: input
 * Parameters: uint32_t *reg
 * Purpose: reads in from stdin and sets value at reg equal to value read
 * Returns: nothing
*/
void input(uint32_t *reg){
      int c = fgetc(stdin);
      if(c <= 255 && c >=0)
        *reg = (uint32_t)c;
      if(c == EOF)
        *reg = ~0;
}


/*
 * Name: output
 * Parameters: uint32_t *reg
 * Purpose: outputs value at register given to stdout
 * Returns: nothing
*/
void output(uint32_t *reg){
  if((int)*reg <= 255 && (int)*reg >=0)
    fputc((char)*reg, stdout);
}

/*
 * Name: load_program
 * Parameters: Seq_T skeletonSeq, uint32_t *b
 * Purpose: duplicates segment at register b and replaces segment 0 with this
 * Returns: nothing
*/
void load_program(Seq_T skeletonSeq, uint32_t *b){
    if(*b != 0){
        void *s = Seq_get(skeletonSeq, *b);
        Seq_T segement_b = (Seq_T)s;
        int blength = Seq_length(segement_b);
        Seq_T newSeq = Seq_new(blength);
        for(int i = 0; i < blength; i ++){
          uint32_t instruction = (uintptr_t)Seq_get(segement_b, i);
          Seq_addhi(newSeq, (void*)(uintptr_t)instruction);
        }
        Seq_remlo(skeletonSeq);
        Seq_addlo(skeletonSeq, newSeq);
    }
}

/*
 * Name: map_segment
 * Parameters: uint32_t *b, uint32_t *c, struct Skeleton *skeleton
 * Purpose: creates a new segement within skeleton segment that is the size of
 *          the value at register c. Sets value at register b equal to the
 *          index at which the new segement was mapped to
 * Returns: nothing
*/
void map_segment(uint32_t *b, uint32_t *c, struct Skeleton *skeleton){
    Seq_T skeletonSeq = skeleton->skeleton_seq;
    Seq_T newSeg = Seq_new(*c);
    int newlength = *c;

    for(int i = 0 ; i < newlength; i++)
      Seq_addhi(newSeg, (void *)(uintptr_t)0);

    if (Seq_length(skeleton->unmapped) == 0){
      Seq_addhi(skeletonSeq, newSeg);
      *b = Seq_length(skeletonSeq)-1;
    }
    else{
      uint32_t indice = (uintptr_t)Seq_remlo(skeleton->unmapped);
      Seq_put(skeletonSeq, indice, newSeg);
      *b = indice;
    }
}
/*
 * Name: unmap_segment
 * Parameters: uint32_t *c, struct Skeleton *skeleton
 * Purpose: appends uint32_t value at register c to the unmapped sequence
 *          and frees value at register c in the skeleton sequene
 * Returns: nothing
*/
void unmap_segment(uint32_t *c, struct Skeleton *skeleton){
    Seq_addlo(skeleton->unmapped, (void*)(uintptr_t)*c);
    Seq_T skeletonSeq = skeleton->skeleton_seq;
    void *s = Seq_get(skeletonSeq, *c);
    Seq_T getSeg = (Seq_T)s;
    Seq_free(&getSeg);
    Seq_put(skeletonSeq, *c, NULL);
}

/*
 * Name: segmented_load
 * Parameters: uint32_t *a, uint32_t *b, uint32_t *c, Seq_T skeletonSeq
 * Purpose: sets value at register a equal to the value at register c within
 *          the sequence at register b
 * Returns: nothing
*/
void segmented_load(uint32_t *a, uint32_t *b, uint32_t *c, Seq_T skeletonSeq){
  void *s = Seq_get(skeletonSeq, *b);
  Seq_T seg = (Seq_T)s;

  uint32_t length = Seq_length(seg);

  if (*c <= length){
    uint32_t instruction = (uintptr_t)Seq_get(seg, *c);
    *a = instruction;
    }
}

/*
 * Name: segmented_store
 * Parameters: uint32_t *a, uint32_t *b, uint32_t *c, Seq_T skeleton
 * Purpose: gets value at register a equal to the value at register c within
 *          the sequence at register b
 * Returns: nothing
*/
void segmented_store(uint32_t *a, uint32_t *b, uint32_t *c, Seq_T skeletonSeq){
  Seq_T seg = (Seq_T)Seq_get(skeletonSeq, *a);

  uint32_t length = Seq_length(seg);

    if (*b <= length)
        Seq_put(seg, *b, (void*)(uintptr_t)*c);
}
