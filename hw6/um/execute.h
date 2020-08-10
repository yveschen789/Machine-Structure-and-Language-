/******************************************************************************
 *
 *     execute.h
 *
 *     Assignment: Comp40 HW6 (UM)
 *     Authors:    Era Iyer (eiyer01) and Yves Chen (ychen22)
 *     Date:       11/19/2019
 *
 *     header file for .c that handles reading instructions and executing the
 *     operations with a function call.
 *
 *****************************************************************************/
#ifndef EXECUTE_INCLUDED
#define EXECUTE_INCLUDED
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "seq.h"
#include "bitpack.h"
#include "seq.h"
#include "skeleton.h"
#include "operations.h"

/*
 * Name: execute
 * Parameters: struct Skeleton *skeleton
 * Purpose: handles reading and executing instructions from segmentZero
 *          at specified index until halt command, running an Operations
 *          function for each bitunpacked instruction
 * Returns: nothing
*/
void execute(struct Skeleton *skeleton);

#endif
