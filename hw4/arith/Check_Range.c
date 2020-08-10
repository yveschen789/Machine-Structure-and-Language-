/***********************************************************************
 *                              Check_Range.c
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
#include <arith40.h>
#include "Check_Range.h"
/*
 * Name: check_range
 * Parameters: (float) y, r
 * Purpose: Checks if value y exceeds the bounds pos and neg r and returns
 *          y if within the range or otherwise the bound r or -r
 * Returns: (float) in bounds value
*/
float check_range(float y, float r)
{
    if (y > r) {
        y = r;
    }
    else if (y < -r) {
        y = -r;
    }
    return y;
}

/*
 * Name: check_urange
 * Parameters: (float) y, r
 * Purpose: Checks if value y exceeds the value r or is lower than 0 and
 *          returns y if within the range, otherwise the bound r or 0
 * Returns: (float) in bounds value
*/
float check_urange(float y, float r)
{
    if (y > r) {
        y = r;
    }
    else if (y < 0) {
        y = 0;
    }
    return y;
}
