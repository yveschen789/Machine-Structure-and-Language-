/***********************************************************************
 *                              ArrayHandling.c
 * Assignment: Homework 4 for Comp 40, Fall 2019
 * Authors: Philip Miljanic (pmilja01) and Yves Chen (ychen22)
 * Date: October 24, 2019
 *
 * Summary:  header file for methods that handle transfer of data from
             PPMread to RGB sequence
 ***********************************************************************/
 #ifndef ARRAYHANDLING_INCLUDED
 #define ARRAYHANDLING_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

typedef struct Seq_closure {
    Seq_T seq;
    unsigned denominator;
    unsigned count;
} *Seq_closure;

typedef struct RGB_values {
    float red,green,blue;
} *RGB_values;

//array handling
A2Methods_UArray2 trim_array(Pnm_ppm og);
void apply_fill_modified(int col, int row, A2Methods_UArray2 array, void *elem,
                void *cl);
Pnm_ppm modify_array(Pnm_ppm original);
void RGB_UArray_fill(int col, int row, A2Methods_UArray2 array, void *elem,
                void *cl);

#endif
