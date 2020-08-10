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
#include "ArrayHandling.h"



/*
 * Name: apply_fill_modified
 * Parameters: (int)col, row, (A2Methods_UArray2) array, (void) elem, cl
 * Purpose: Fills array (of trimmed size) with elements within the original
 *          Pnm_ppm struct pixel array passed as closure
 * Returns: (void)
*/
void apply_fill_modified(int col, int row, A2Methods_UArray2 array, void *elem,
                void *cl)
{
        (void)array;
        Pnm_rgb curr_rgb = elem;
        Pnm_ppm og = cl;
        Pnm_rgb og_rgb = og->methods->at(og->pixels, col, row);
        curr_rgb->red = og_rgb->red;
        curr_rgb->blue = og_rgb->blue;
        curr_rgb->green = og_rgb->green;
}

/*
 * Name: RGB_UArray_fill
 * Parameters: (int)col, row, (A2Methods_UArray2) array, (void) elem, cl
 * Purpose: Populates the sequence (within the sequence closure passed in) with
 *          RGB_values structs containing unscaled rgb values
 * Returns: (void)
*/
void RGB_UArray_fill(int col, int row, A2Methods_UArray2 array, void *elem,
                    void *cl)
{
    (void)array;
    (void)col;
    (void)row;
    Seq_closure closure = cl;
    Seq_T sequence = closure->seq;
    unsigned denominator = closure->denominator;
    Pnm_rgb curr = elem;

    RGB_values temp_struct = calloc(1, sizeof(struct RGB_values));
    assert(temp_struct);
    temp_struct->red = (float)curr->red / (float)denominator;
    temp_struct->blue = (float)curr->blue / (float)denominator;
    temp_struct->green = (float)curr->green / (float)denominator;
    Seq_addhi(sequence, temp_struct);
}

/*
 * Name: trim_array
 * Parameters: (Pnm_ppm) og
 * Purpose: Trims the pixel array of og to an even height and width,
 *          subtracting from the dimension as needed, and updates the height &
 *          width of og
 * Returns: (A2Methods_UArray2) of updated dimension
*/
A2Methods_UArray2 trim_array(Pnm_ppm og)
{
    int upd_s = og->methods->size(og->pixels);
    int upd_bs = 2;
    int upd_h, upd_w;
    /* Updates height and width based on flip_check */
    if (((og->methods->height(og->pixels)) % 2) == 1) {
        upd_h = og->methods->height(og->pixels) - 1;
    } else {
        upd_h = og->methods->height(og->pixels);
    }
    if (((og->methods->width(og->pixels)) % 2) == 1) {
        upd_w = og->methods->width(og->pixels) - 1;
    } else {
        upd_w = og->methods->width(og->pixels);
    }

    A2Methods_UArray2 upd = og->methods->new_with_blocksize
    (upd_w, upd_h, upd_s, upd_bs);
    /* Updates the attributes of the Pnm_ppm object */
    og->width = upd_w;
    og->height = upd_h;
    return upd;
}

/*
 * Name: modify_array
 * Parameters: (Pnm_ppm) original
 * Purpose: Instatiates new blocked array, fills those values and changes
 * Pnm_ppm struct to point to new array
 * Returns: (Pnm_ppm) modified Pnm_ppm struct
*/
Pnm_ppm modify_array(Pnm_ppm original)
{
    A2Methods_UArray2 modifiedA2 = trim_array(original);
    A2Methods_mapfun *map = original->methods->map_default;
    map(modifiedA2, apply_fill_modified, original);
    original->methods->free(&(original->pixels));
    original->pixels = modifiedA2;
    return original;
}
