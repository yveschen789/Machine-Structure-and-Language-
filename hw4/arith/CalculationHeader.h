/***********************************************************************
 *                              CalculationHeader.c
 * Assignment: Homework 4 for Comp 40, Fall 2019
 * Authors: Philip Miljanic (pmilja01) and Yves Chen (ychen22)
 * Date: October 24, 2019
 *
 * Summary:  header file for methods that handle calculations from RGB space
             to Cosine Calc
 ***********************************************************************/
 #ifndef CALCULATIONHEADER_INCLUDED
 #define CALCULATIONHEADER_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "seq.h"
#include "a2methods.h"


typedef struct dct_values{
    float a, b, c, d, pr, pb;
} *dct_values;

//calc header
UArray_T DCT_Calc(Seq_T sequence, int height, int width);
uint32_t Cosine_Calc(UArray_T bright, float pb, float pr);
void decompress_dct(int col, int row, A2Methods_UArray2 array, void *elem,
                void *cl);

#endif
