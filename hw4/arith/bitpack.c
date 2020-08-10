/***********************************************************************
 *                              bitpack.c
 * Assignment: Homework 4 for Comp 40, Fall 2019
 * Authors: Philip Miljanic (pmilja01) and Yves Chen (ychen22)
 * Date: October 24, 2019
 *
 * Summary: Function suite that handles bitwise manipulations
 ***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <arith40.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include "except.h"
Except_T Bitpack_Overflow = { "Overflow packing bits" };

/*
 * Name: right_shiftu
 * Parameters: (uint64_t) value, shift
 * Purpose: Shifts bits of value to the right by shift
 * Returns: (uint64_t) newly shifted value
*/
uint64_t right_shiftu(uint64_t value, uint64_t shift)
{
    /* Handles the special case where shift is the size of the value */
    if (shift == sizeof(value)*8) {
        return value;
    }
    return (value / (uint64_t)pow(2,shift));
}

/*
 * Name: left_shiftu
 * Parameters: (uint64_t) value, shift
 * Purpose: Shifts bits of value to the left by shift
 * Returns: (uint64_t) newly shifted value
*/
uint64_t left_shiftu(uint64_t value, uint64_t shift)
{
    /* Handles the special case where shift is the size of the value */
    if (shift == sizeof(value)*8) {
        return value;
    }
    return (value * (uint64_t)pow(2,shift));
}

/*
 * Name: right_shifts
 * Parameters: (int64_t) value, shift
 * Purpose: Shifts bits of value to the right by shift
 * Returns: (int64_t) newly shifted value
*/
int64_t right_shifts(int64_t value, uint64_t shift)
{
    bool negative = false;
    /* checks if value is negative */
    if(value < 0) {
        value = value * -1;
        negative = true;
    }
    uint64_t temp = right_shiftu((uint64_t)value, shift);
    value = (int64_t) temp;
    /* If value was negative, it is converted back */
    if(negative) {
        value = value * -1;
    }
    return value;
}

/*
 * Name: left_shifts
 * Parameters: (int64_t) value, shift
 * Purpose: Shifts bits of value to the lefts by shift
 * Returns: (int64_t) newly shifted value
*/
int64_t left_shifts(int64_t value, uint64_t shift)
{
    bool negative = false;
    /* checks if value is negative */
    if(value < 0) {
        value = value*-1;
        negative = true;
    }
    uint64_t temp = left_shiftu((uint64_t)value, shift);
    value = (int64_t) temp;
    /* If value was negative, it is converted back */
    if(negative) {
        value = value * -1;
    }
    return value;
}

/*
 * Name: Bitpack_fitsu
 * Parameters: (uint64_t) n, (unsigned) width
 * Purpose: Checks if value can be represented in bit width size
 * Returns: (bool) whether or not n can be stored in width bits
*/
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
    if(n <= pow(2, width)) {
        return true;
    }
    else{
        return false;
    }
}

/*
 * Name: Bitpack_fitss
 * Parameters: (int64_t) n, (unsigned) width
 * Purpose: Checks if value can be represented in bit width size
 * Returns: (bool) whether or not n can be stored in width bits
*/
bool Bitpack_fitss(int64_t n, unsigned width)
{
    if (n <= 0) {
        n = n * (-1);
    }
    return Bitpack_fitsu((uint64_t) n, width - 1);
}

/*
 * Name: Bitpack_newu
 * Parameters: (uint64_t) word, value (unsigned) width, lsb
 * Purpose: Packs value in word starting lsb of size width
 * Returns: (uint64_t) newly packed word
*/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
                     uint64_t value)
{
    assert(width <= sizeof(word)*8);
    assert((width+lsb) <= sizeof(word)*8);
    /* Clears field using mask */
    uint64_t mask = ~0;
    mask = left_shiftu(mask,sizeof(word)*8 - width);
    mask = right_shiftu(mask, sizeof(word)*8 - width - lsb);
    mask = ~mask;
    word = word & mask;
    /* Adjusts value bit positioning */
    if (!Bitpack_fitsu(value, width)) {
        RAISE(Bitpack_Overflow);
    }
    value = left_shiftu(value, lsb);
    return (word | value);
}

/*
 * Name: Bitpack_news
 * Parameters: (uint64_t) word, (unsigned) width, lsb, (int64_t) value
 * Purpose: Packs value in word starting lsb of size width
 * Returns: (uint64_t) newly packed word
*/
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,
                      int64_t value)
{
    uint64_t Uvalue = (uint64_t)value;
    Uvalue = left_shiftu(Uvalue, sizeof(word)*8 - width);
    Uvalue = right_shiftu(Uvalue, sizeof(word)*8 - width);
    return Bitpack_newu(word, width, lsb, Uvalue);
}

/*
 * Name: Bitpack_getu
 * Parameters: (uint64_t) word, width, lsb
 * Purpose: Extracts bit field from word at lsb of size width
 * Returns: (uint64_t) bit field extracted
*/
uint64_t Bitpack_getu(uint64_t word, uint64_t width, uint64_t lsb)
{
    assert(width <= sizeof(word)*8);
    assert((width+lsb) <= sizeof(word)*8);
    uint64_t mask = ~0;
    mask = left_shiftu(mask, (sizeof(word)*8) - width);
    mask = right_shiftu(mask, (sizeof(word)*8) - width - lsb);
    uint64_t temp = mask & word;
    return right_shiftu(temp, lsb);
}

/*
 * Name: Bitpack_gets
 * Parameters: (int64_t) word, (uint64_t) width, lsb
 * Purpose: Extracts bit field from word at lsb of size width
 * Returns: (int64_t) bit field extracted
*/
int64_t Bitpack_gets(int64_t word, uint64_t width, uint64_t lsb)
{
    uint64_t temp_u = Bitpack_getu((uint64_t)word, width, lsb);
    uint64_t flag_neg = ~0;
    flag_neg = left_shiftu(flag_neg, sizeof(word)*8 - 1);
    /*Creates a flag to see if the most significant bit extracted is 1*/
    uint64_t flag_sign = right_shiftu(flag_neg, sizeof(word)*8 - width);
    uint64_t compare = flag_sign & temp_u;
    /*Checks if negative*/
    if (compare == flag_sign) {
        uint64_t convert_to_neg = ~0;
        convert_to_neg = left_shiftu(convert_to_neg, width);
        temp_u = convert_to_neg | temp_u;
    }
    return (int64_t)temp_u;
}
