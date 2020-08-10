/***********************************************************************
 *                              compress40.c
 * Assignment: Homework 4 for Comp 40, Fall 2019
 * Authors: Philip Miljanic (pmilja01) and Yves Chen (ychen22)
 * Date: October 24, 2019
 *
 * Summary: This program compresses and decompresses ppm images
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
#include <math.h>
#include "bitpack.h"
#include "ArrayHandling.h"
#include "Coeff_Calc.h"
#include "Check_Range.h"
#include "WordPack.h"


//helper function to print in Big Endian
void printBE(UArray_T array);


/*
 * Name: compress40
 * Parameters: (FILE*) input
 * Purpose: Compresses an image input into 32 bit words and prints the words to
 *          stdout
 * Returns: (void)
*/
void compress40 (FILE *input)
{
    /*Makes new pnm_ppm object*/
    A2Methods_T methods = uarray2_methods_blocked;
    Pnm_ppm RGB_ppm = Pnm_ppmread(input, methods);
    A2Methods_mapfun *map = RGB_ppm->methods->map_default;
    RGB_ppm = modify_array(RGB_ppm);

    /* Declares sequence of RGB structs */
    int height = RGB_ppm->methods->height(RGB_ppm->pixels);
    int width = RGB_ppm->methods->width(RGB_ppm->pixels);
    Seq_T RGB_seq = Seq_new(height*width);
    Seq_closure closure = calloc(1, sizeof(struct Seq_closure));
    assert(closure);
    closure->seq = RGB_seq;
    closure->denominator = RGB_ppm->denominator;

    /* Updates pixels arrays*/
    map(RGB_ppm->pixels, RGB_UArray_fill, closure);
    Pnm_ppmfree(&RGB_ppm);

    /* Prints file header and code words to stdout */
    printf("COMP40 Compressed image format 2\n%u %u", width, height);
    printf("\n");
    UArray_T wordArray = DCT_Calc(RGB_seq, height, width);
    printBE(wordArray);
    /* Frees remaining structs */
    UArray_free(&wordArray);
    free(closure);
    Seq_free(&RGB_seq);
}

/*
 * Name: printBE
 * Parameters: (UArray_T) array
 * Purpose: Prints 32 bit words stored in UArray in big endian order
 * Returns: (void)
*/
void printBE(UArray_T array)
{
    int length = UArray_length(array);
    for (int i = 0; i < length; i++) {
        uint32_t * elem = UArray_at(array, i);
        for (int j = 0; j < 4; j++) {
            putchar(Bitpack_getu(*elem, 8, 24 - j*8));
        }
    }
}


/*
 * Name: decompress40
 * Parameters: (FILE) *input
 * Purpose: Decompresses 32 bit code words and writes an image to stdout in
 *          binary
 * Returns: (void)
*/
void decompress40 (FILE *input)
{
    unsigned height, width;
    int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u", &width,
                      &height);
    assert(read == 2);
    int ch = getc(input);
    assert(ch == '\n');

    Seq_T values = Seq_new(100);


    for(unsigned i = 0; i < width*height/4; i++) {
        dct_values dct = unpack_word(input);
        /* fills sequence with cosine values */
        Seq_addhi(values, dct);
    }

    A2Methods_T methods = uarray2_methods_blocked;
    Pnm_ppm final = calloc(1,sizeof(struct Pnm_ppm));
    assert(final);
    final->methods = methods;
    final->pixels = final->methods->new_with_blocksize(width, height,
                    sizeof(struct Pnm_rgb), 2);
    final->width = width;
    final->height = height;
    final->denominator = 255;
    A2Methods_mapfun *map = final->methods->map_default;
    Seq_closure closure = calloc(1, sizeof(struct Seq_closure));
    assert(closure);
    closure->denominator = final->denominator;
    closure->count = 0;
    closure->seq = values;

    /* maps over 2x2block and calculates RGB values */
    map(final->pixels, decompress_dct, closure);
    Pnm_ppmwrite(stdout, final);

    /* freeing cdt structs in seq values */
    while(Seq_length(values) != 0) {
        dct_values curr = Seq_remlo(values);
        free(curr);
    }
    free(closure);
    Seq_free(&values);
    Pnm_ppmfree(&final);
}
