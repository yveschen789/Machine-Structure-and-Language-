/***********************************************************************
 *                              ArrayHandling.c
 * Assignment: Homework 4 for Comp 40, Fall 2019
 * Authors: Philip Miljanic (pmilja01) and Yves Chen (ychen22)
 * Date: October 24, 2019
 *
 * Summary:  handles transfer of data from PPMread to RGB sequence
 ***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "pnm.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "a2plain.h"
#include "seq.h"
#include "uarray.h"
#include <arith40.h>
#include "CalculationHeader.h"
#include "ArrayHandling.h"
#include "Check_Range.h"
#include "WordPack.h"
#include <math.h>

/*
 * Name: DCT_Calc
 * Parameters: (Seq_T) sequence, (int) height, width
 * Purpose: Converts RGB values stored within sequence into cosine coefficients,
 *          then packed and stored into the UArray_T returned
 * Returns: (UArray_T) of 32 bit codewords
*/
UArray_T DCT_Calc(Seq_T sequence, int height, int width)
{
    assert(Seq_length(sequence) == height*width);
    UArray_T wordArray = UArray_new(height * width /4, sizeof(uint32_t));

    float r, g, b, y, pb, pr;
    float pb_sum = 0;
    float pr_sum = 0;

    /* Initializes UArray storing y values */
    UArray_T bright = UArray_new(4, sizeof(float));
    int count = 0;
    /* Iterates over sequence length, calculating cosine coeffs */
    while(Seq_length(sequence) != 0) {
        for(int i = 0; i < 4; i++) {
            float *elem = UArray_at(bright, i);
            RGB_values curr = Seq_remlo(sequence);

            /* Stores current rgb vals */
            r = curr->red;
            b = curr->blue;
            g = curr->green;

            /* Calculates component values */
            y = 0.299 * r + 0.587 * g + 0.114 * b;
            pb = -0.168736 * r - 0.331264 * g + 0.5 * b;
            pr = 0.5 * r - 0.418688 * g - 0.081312 * b;
            /* Checks component values are within expected range */
            y = check_urange(y,1);
            pr = check_range(pr,0.5);
            pb = check_range(pb,0.5);

            *elem = y;
            pb_sum += pb;
            pr_sum += pr;
            free(curr);
        }
        pr = pr_sum / 4;
        pb = pb_sum / 4;


        uint32_t * temp = UArray_at(wordArray, count);
        count++;
        *temp = Cosine_Calc(bright, pb, pr);
        pb_sum = 0;
        pr_sum = 0;
        pb = 0;
        pr = 0;
    }
    UArray_free(&bright);
    return wordArray;
}

/*
 * Name: Cosine_Calc
 * Parameters: (UArray_T)bright, (float) pb, pr
 * Purpose: Converts chroma and luma values to cosine coefficients, and calls
 *          packword()
 * Returns: (uint32_t) packed 32 bit code word
*/
uint32_t Cosine_Calc(UArray_T bright, float pb, float pr)
{
    float y1 = *(float*)UArray_at(bright, 0);
    float y2 = *(float*)UArray_at(bright, 1);
    float y3 = *(float*)UArray_at(bright, 2);
    float y4 = *(float*)UArray_at(bright, 3);

    float a = (y4 + y3 + y2 + y1)/4.0;
    float b = (y4 + y3 - y2 - y1)/4.0;
    float c = (y4 - y3 + y2 - y1)/4.0;
    float d = (y4 - y3 - y2 + y1)/4.0;
    /* Checks coefficients are within the expected bounds*/
    a = check_urange(a,1);
    b = check_range(b,.3);
    c = check_range(c,.3);
    d = check_range(d,.3);

    return packWord(a, b, c, d, pb, pr);
}

/*
 * Name: decompress_dct
 * Parameters: (int) col, row, (A2Methods_UArray2) array, void *elem, *cl
 * Purpose: used by map to take dct_values from struct and convert to RGB
            unsigned to store in pnm_ppm pixel array
 * Returns: (void)
*/
void decompress_dct(int col, int row, A2Methods_UArray2 array, void *elem,
                void *cl)
{
    (void)col;
    (void)row;
    (void)array;
    /* Extracts Seq_closure info */
    Seq_closure seq_clos = cl;
    Seq_T sequence = seq_clos->seq;
    unsigned count = seq_clos->count;
    unsigned denominator = seq_clos->denominator;
    dct_values dct_struct = Seq_get(sequence, floor(count/4));
    /* Assigns chroma values */
    float a, b, c, d, y, pr, pb;
    a = dct_struct->a;
    b = dct_struct->b;
    c = dct_struct->c;
    d = dct_struct->d;
    pr = dct_struct->pr;
    pb = dct_struct->pb;

    /* Calculates distinct y values for each pixel based on position*/
    if ((count % 4) == 0) {
        y = a - b - c + d;
    } else if((count % 4) == 1) {
        y = a - b + c - d;
    } else if((count % 4) == 2) {
        y = a + b - c - d;
    } else if((count % 4) == 3) {
        y = a + b + c + d;
    }

    y = check_urange(y,1);
    pr = check_range(pr,0.5);
    pb = check_range(pb,0.5);

    /* Calculates r g b values */
    float red, green, blue;
    red = (1.0 * y) + (0.0 * pb) + (1.402 * pr);
    green = 1.0 * y - 0.344136 * pb - 0.714136 * pr;
    blue = 1.0 * y + 1.772 * pb + 0.0 * pr;

    /* Ensures RGB Values are within correct range */
    red = check_urange(red,1);
    green = check_urange(green,1);
    blue = check_urange(blue,1);

    Pnm_rgb curr = elem;

    curr->red = (red*denominator);
    curr->green = (green*denominator);
    curr->blue = (blue*denominator);
    seq_clos->count = seq_clos->count + 1;
}
