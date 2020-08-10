/******************************************************************************
 *
 *     load.c
 *
 *     Assignment: Comp40 HW6 (UM)
 *     Authors:    Era Iyer (eiyer01) and Yves Chen (ychen22)
 *     Date:       11/19/2019
 *
 *     takes in instructions from file and bitpacks the pulled chars into
 *     a 32 bit instruction and stores it in segmentZero of skeleton seq
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "assert.h"
#include "load.h"

/*
 * Name: loadAndStore
 * Parameters: Seq_T skeletonSeq, FILE *fp
 * Purpose:  loads instructions from file into segmentZero
 * Returns: nothing
*/
void loadAndStore(Seq_T skeletonSeq, FILE *fp){

    assert(fp);
    assert(Seq_length(skeletonSeq)>0);
    uint32_t instruction = 0;
    uint32_t curr_byte = 0;
    void *s = Seq_get(skeletonSeq, 0);
    Seq_T segmentZero = (Seq_T)s;

    while((int)curr_byte != EOF){

    /* pulling and creating valid instruction */
      for (unsigned long i = 0; i < 4; i++) {
              curr_byte = getc(fp);
              if (Bitpack_fitsu(curr_byte, 8)){
                instruction = Bitpack_newu(instruction, 8, 24 - (i * 8),
                                curr_byte);
              }else{
                  return;
              }
      }

      /* storing instruction in segmentZero */
        Seq_addhi((Seq_T)segmentZero, (void*)(uintptr_t)instruction);
      }

}
