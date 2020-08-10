/******************************************************************************
 *
 *     load.h
 *
 *     Assignment: Comp40 HW6 (UM)
 *     Authors:    Era Iyer (eiyer01) and Yves Chen (ychen22)
 *     Date:       11/19/2019
 *
 *    header file for load.c
 *
 *****************************************************************************/
#ifndef LOAD_INCLUDED
#define LOAD_INCLUDED
#include <stdbool.h>
#include <stdint.h>

#include "bitpack.h"
#include "seq.h"

/*
 * Name: loadAndStore
 * Parameters: Seq_T skeletonSeq, FILE *fp
 * Purpose: loads instructions from file into segmentZero
 * Returns: nothing
*/
void loadAndStore(Seq_T skeletonSeq, FILE *fp);

#endif
