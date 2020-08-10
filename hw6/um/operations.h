/******************************************************************************
 *
 *     operations.h
 *
 *     Assignment: Comp40 HW6 (UM)
 *     Authors:    Era Iyer (eiyer01) and Yves Chen (ychen22)
 *     Date:       11/19/2019
 *
 *     This file contains the interface for the operations module. This
 *     includes all of the function declarations for the 13 possible
 *     instructions
 *
 *****************************************************************************/

#ifndef OPERATIONS_INCLUDED
#define OPERATIONS_INCLUDED
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitpack.h"
#include "seq.h"
#include "skeleton.h"

/*
 * Name: halt
 * Parameters: nothing
 * Purpose: returns true to execute and sets local boolean to true and execute
 *          exits while loop
 * Returns: true boolean value
*/
bool halt();

/*
 * Name: add
 * Parameters: uint32_t *a, uint32_t *b, uint32_t *c
 * Purpose: sets the value at register a equal to the value at register b plus
 *          the value at register c
 * Returns: nothing
*/
void add(uint32_t *a, uint32_t *b, uint32_t *c);

/*
 * Name: loadval
 * Parameters: uint32_t *reg, uint32_t value
 * Purpose: sets the value at the register given in the parameter to the value
 *          provided in the parameter
 * Returns: nothing
*/
void loadval(uint32_t *reg, uint32_t value);

/*
 * Name: multiply
 * Parameters uint32_t *a, uint32_t *b, uint32_t *c
 * Purpose: takes 3 registers and multiplies register b and c and stores it
 *          in memory where c points to after by modding 2^32
 * Returns: nothing
*/
void multiply(uint32_t *a, uint32_t *b, uint32_t *c);

/*
 * Name: divide
 * Parameters: uint32_t *a, uint32_t *b, uint32_t *c
 * Purpose: sets the value at register a equal to the value at register b
 *          divided by the value at register c
 * Returns: nothing
*/
void divide(uint32_t *a, uint32_t *b, uint32_t *c);

/*
 * Name: bitwiseNand
 * Parameters: uint32_t *reg, uint32_t value
 * Purpose: gets the not of register b and register c and sets value at
 *          regsiter a equal to this
 * Returns: nothing
*/
void bitwiseNand(uint32_t *a, uint32_t *b, uint32_t *c);

/*
 * Name: conditional_move
 * Parameters: uint32_t *a, uint32_t *b, uint32_t *c
 * Purpose: checks if the memory pointed to by pointer c contains 0, if not
            it puts the value pointed to by b in a.
 * Returns: nothing
*/
void conditional_move(uint32_t *a, uint32_t *b, uint32_t *c);

/*
 * Name: input
 * Parameters: uint32_t *reg
 * Purpose: reads in from stdin and sets value at reg equal to value read
 * Returns: nothing
*/
void input(uint32_t *reg);

/*
 * Name: output
 * Parameters: uint32_t *reg
 * Purpose: outputs value at register given to stdout
 * Returns: nothing
*/
void output(uint32_t *reg);

/*
 * Name: load_program
 * Parameters: Seq_T skeletonSeq, uint32_t *b
 * Purpose: duplicates segment at register b and replaces segment 0 with this
 * Returns: nothing
*/
void load_program(Seq_T skeletonSeq, uint32_t *reg_b);

/*
 * Name: map_segment
 * Parameters: uint32_t *b, uint32_t *c, struct Skeleton *skeleton
 * Purpose: creates a new segement within skeleton segment that is the size of
 *          the value at register c. Sets value at register b equal to the
 *          index at which the new segement was mapped to
 * Returns: nothing
*/
void map_segment(uint32_t *b, uint32_t *c, struct Skeleton *skeleton);

/*
 * Name: unmap_segment
 * Parameters: uint32_t *c, struct Skeleton *skeleton
 * Purpose: appends uint32_t value at register c to the unmapped sequence
 *          and frees value at register c in the skeleton sequene
 * Returns: nothing
*/
void unmap_segment(uint32_t *c, struct Skeleton *skeleton);

/*
 * Name: segmented_load
 * Parameters: uint32_t *a, uint32_t *b, uint32_t *c, Seq_T skeletonSeq
 * Purpose: sets value at register a equal to the value at register c within
 *          the sequence at register b
 * Returns: nothing
*/
void segmented_load(uint32_t *a, uint32_t *b, uint32_t *c, Seq_T skeletonSeq);

/*
 * Name: segmented_store
 * Parameters: uint32_t *a, uint32_t *b, uint32_t *c, Seq_T skeleton
 * Purpose: gets value at register a equal to the value at register c within
 *          the sequence at register b
 * Returns: nothing
*/
void segmented_store(uint32_t *a, uint32_t *b, uint32_t *c, Seq_T skeletonSeq);


#endif
