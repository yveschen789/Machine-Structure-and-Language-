//WORKING VERSION UARRAY SKELETON, UARRAY UNMAPPED, UARRAY SEGEMENTS, STATIC INLINE OPERATIONS

/******************************************************************************
 *
 *     um.c
 *
 *     Assignment: Comp40 HW6 (UM)
 *     Authors:    Era Iyer (eiyer01) and Yves Chen (ychen22)
 *     Date:       11/19/2019
 *
 *     This file serves as the main for our um program. It is responsible
 *     for creating and allocating space for the overall memory structure
 *     as well as segment zero. It calls the appropriate functions to load
*      the um file and execute each instruction. All memory is freed at the end
 *
 *****************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <inttypes.h>

#include "assert.h"
#include "uarray.h"

#define INIT_SIZE 100

struct Skeleton{
    UArray_T skeleton_seq;
    UArray_T unmapped;
    int unmapped_index;
    int index;

};

//void execute(struct Skeleton *skeleton);
void loadAndStore(UArray_T skeletonSeq, FILE *fp);

bool Bitpack_fitsu(uint64_t n, unsigned width);
bool Bitpack_fitss( int64_t n, unsigned width);
uint64_t Bitpack_getu(uint64_t word, unsigned width,
                      unsigned lsb);
int64_t Bitpack_gets(uint64_t word, unsigned width,
                     unsigned lsb);
uint64_t Bitpack_newu(uint64_t word, unsigned width,
                      unsigned lsb, uint64_t value);
uint64_t Bitpack_news(uint64_t word, unsigned width,
                      unsigned lsb,  int64_t value);

extern Except_T Bitpack_Overflow;



void loadAndStore(UArray_T skeletonSeq, FILE *fp){

    assert(fp);
    assert(UArray_length(skeletonSeq)>0);
    uint32_t instruction = 0;
    uint32_t curr_byte = 0;
    UArray_T segmentZero = *((UArray_T *)UArray_at(skeletonSeq, 0));

    int counter = 0;

    while((int)curr_byte != EOF){

    /* pulling and creating valid instruction */
      for (unsigned long i = 0; i < 4; i++) {
              curr_byte = getc(fp);
              if (Bitpack_fitsu(curr_byte, 8)){
                instruction = Bitpack_newu(instruction, 8, 24 - (i * 8),
                                curr_byte);
              }else{
                  return;
              }
      }

      /* storing instruction in segmentZero */
      *((uint32_t *)UArray_at(segmentZero, counter)) = instruction;
      counter++;

      }

}



/*
 * Name: add
 * Parameters: uint32_t *a, uint32_t *b, uint32_t *c
 * Purpose: sets the value at register a equal to the value at register b plus
 *          the value at register c
 * Returns: nothing
*/
static inline uint32_t add(uint32_t b, uint32_t c){
    return (b + c) % 4294967296;
}

/*
 * Name: loadval
 * Parameters: uint32_t *reg, uint32_t value
 * Purpose: sets the value at the register given in the parameter to the value
 *          provided in the parameter
 * Returns: nothing
*/
// static inline uint32_t loadval(uint32_t value){
//     return value;
// }

/*
 * Name: multiply
 * Parameters uint32_t *a, uint32_t *b, uint32_t *c
 * Purpose: takes 3 registers and multiplies register b and c and stores it
 *          in memory where c points to after by modding 2^32
 * Returns: nothing
*/
static inline uint32_t multiply(uint32_t b, uint32_t c){
    return (b * c) % 4294967296;
}

/*
 * Name: divide
 * Parameters: uint32_t *a, uint32_t *b, uint32_t *c
 * Purpose: sets the value at register a equal to the value at register b
 *          divided by the value at register c
 * Returns: nothing
*/
static inline uint32_t divide(uint32_t b, uint32_t c){
    return (b / c);
}

/*
 * Name: bitwiseNand
 * Parameters: uint32_t *reg, uint32_t value
 * Purpose: gets the not of register b and register c and sets value at
 *          regsiter a equal to this
 * Returns: nothing
*/
static inline uint32_t bitwiseNand(uint32_t b, uint32_t c){
    return ~(b & c);
}

/*
 * Name: conditional_move
 * Parameters: uint32_t *a, uint32_t *b, uint32_t *c
 * Purpose: checks if the memory pointed to by pointer c contains 0, if not
            it puts the value pointed to by b in a.
 * Returns: nothing
*/
static inline uint32_t conditional_move(uint32_t a, uint32_t b, uint32_t c){
    if(c != 0){
        return b;
    }
    return a;
}

/*
 * Name: input
 * Parameters: uint32_t *reg
 * Purpose: reads in from stdin and sets value at reg equal to value read
 * Returns: nothing
*/
static inline uint32_t input(){
    int c = fgetc(stdin);
    if(c <= 255 && c >=0)
        return (uint32_t)c;
    if(c == EOF)
        return ~0;
    return ~0;
}


/*
 * Name: output
 * Parameters: uint32_t *reg
 * Purpose: outputs value at register given to stdout
 * Returns: nothing
*/
static inline void output(uint32_t reg){
    if((int)reg <= 255 && (int)reg >=0)
        fputc((char)reg, stdout);
}

/*
 * Name: load_program
 * Parameters: Seq_T skeletonSeq, uint32_t *b
 * Purpose: duplicates segment at register b and replaces segment 0 with this
 * Returns: nothing
*/
// void load_program(Seq_T skeletonSeq, uint32_t *b){
static inline void load_program(UArray_T skeletonSeq, uint32_t b){
    if(b != 0){
        UArray_T segement_b = *((UArray_T *)UArray_at(skeletonSeq, b));

        int blength = UArray_length(segement_b);

        UArray_T newSeq = UArray_new(blength, sizeof(uint32_t));
        for(int i = 0; i < blength; i ++){
            void *ins = UArray_at(segement_b, i);
            uint32_t *instruction = ins;
            *((uint32_t *)UArray_at(newSeq, i)) = *instruction;
        }
        *((UArray_T *)UArray_at(skeletonSeq, 0)) = newSeq;
    }
}

/*
 * Name: map_segment
 * Parameters: uint32_t *b, uint32_t *c, struct Skeleton *skeleton
 * Purpose: creates a new segement within skeleton segment that is the size of
 *          the value at register c. Sets value at register b equal to the
 *          index at which the new segement was mapped to
 * Returns: nothing
*/
static inline uint32_t map_segment(uint32_t c, struct Skeleton *skeleton){
    UArray_T skeletonSeq = skeleton->skeleton_seq;
    UArray_T newSeg = UArray_new(c, sizeof(uint32_t));
    int newlength = c;
     
    for(int i = 0 ; i < newlength; i++)
        *((uint32_t *)UArray_at(newSeg, i)) = 0;
     if (skeleton->unmapped_index == 0){
        *((UArray_T *)UArray_at(skeletonSeq, skeleton->index)) = newSeg;
        
        skeleton->index +=1;
        int skel_length = UArray_length(skeleton->skeleton_seq);
        if(skeleton->index >= skel_length){
             UArray_resize(skeleton->skeleton_seq, skel_length*2);
        }
        return skeleton->index-1;
    }
    else{
        uint32_t indice = *((uint32_t *)UArray_at(skeleton->unmapped, skeleton->unmapped_index-1));
        skeleton->unmapped_index-=1;

        int unmapped_length = UArray_length(skeleton->unmapped);
        if(skeleton->unmapped_index < unmapped_length/2){
            UArray_resize(skeleton->unmapped, unmapped_length/2);
        }
        *((UArray_T *)UArray_at(skeletonSeq, indice)) = newSeg;
        return indice;
    }
}

/*
 * Name: unmap_segment
 * Parameters: uint32_t *c, struct Skeleton *skeleton
 * Purpose: appends uint32_t value at register c to the unmapped sequence
 *          and frees value at register c in the skeleton sequene
 * Returns: nothing
*/
static inline void unmap_segment(uint32_t c, struct Skeleton *skeleton){
    
    *((uint32_t *)UArray_at(skeleton->unmapped, skeleton->unmapped_index)) = c;
    skeleton->unmapped_index += 1;
    int unmapped_length = UArray_length(skeleton->unmapped);
    if(skeleton->unmapped_index >= unmapped_length){
        UArray_resize(skeleton->unmapped, unmapped_length*2);
    }
    UArray_T skeletonSeq = skeleton->skeleton_seq;
    UArray_T getSeg = *((UArray_T *)UArray_at(skeletonSeq, c));
    UArray_free(&getSeg);
    *((UArray_T *)UArray_at(skeletonSeq, c)) = NULL;
}

/*
 * Name: segmented_load
 * Parameters: uint32_t *a, uint32_t *b, uint32_t *c, Seq_T skeletonSeq
 * Purpose: sets value at register a equal to the value at register c within
 *          the sequence at register b
 * Returns: nothing
*/
static inline uint32_t segmented_load(uint32_t b, uint32_t c, UArray_T skeletonSeq){
    UArray_T seg = *((UArray_T *)UArray_at(skeletonSeq, b));
    assert(seg);
    uint32_t length = (uint32_t)UArray_length(seg);

    if (c <= length){
        void *ins = UArray_at(seg, c);
        uint32_t *instruction = ins;
        return *instruction;
    }
    return EXIT_FAILURE;
}

/*
 * Name: segmented_store
 * Parameters: uint32_t *a, uint32_t *b, uint32_t *c, Seq_T skeleton
 * Purpose: gets value at register a equal to the value at register c within
 *          the sequence at register b
 * Returns: nothing
*/
static inline void segmented_store(uint32_t a, uint32_t b, uint32_t c, UArray_T skeletonSeq){
    //printf("value is %d\n", a);
    UArray_T seg = *((UArray_T *)UArray_at(skeletonSeq, a));
    assert(seg);
    uint32_t length = (uint32_t)UArray_length(seg);

    if (b <= length)
        *((uint32_t *)UArray_at(seg, b)) = c;
}

// void execute(struct Skeleton *skeleton){


// }

int main(int argc, char const *argv[]) {

    assert(argc == 2);
    FILE *fp = NULL;
    fp = fopen(argv[1], "r");
    assert(fp != NULL);
    
    struct stat st;
    stat(argv[1], &st);
    int size = st.st_size;

    /* allocating space for skeleton struct and all sequences within it */
    struct Skeleton *skeleton = malloc(sizeof(struct Skeleton));

    UArray_T unmap = UArray_new(INIT_SIZE, sizeof(uint32_t));
    skeleton->unmapped = unmap;
    skeleton->unmapped_index = 0;

    UArray_T skeletonSeq = UArray_new(INIT_SIZE, sizeof(UArray_T));
    skeleton->skeleton_seq = skeletonSeq;
    skeleton->index = 1;

    UArray_T segmentZero = UArray_new(size/4, sizeof(uint32_t));
    *((UArray_T *)UArray_at(skeleton->skeleton_seq, 0)) = segmentZero;

     /* calling appropriate modules for loading and executing instructions */
     loadAndStore(skeleton->skeleton_seq, fp);
     //execute(skeleton);
    
    //UArray_T skeletonSeq = skeleton->skeleton_seq;
    uint32_t register_arr[8] = {0};
    uint32_t program_counter = 0;
    uint32_t value;

    //UArray_T segmentZero = *((UArray_T *)UArray_at(skeletonSeq, 0));
    uint32_t regA, regB, regC;
    bool halt_check = false;
    uint32_t segZero = (uint32_t)UArray_length(segmentZero);

    uint32_t opmask = 4026531840;
    uint32_t Amask = 448;
    uint32_t Bmask = 56;
    uint32_t Cmask = 7;
    uint32_t LAmask = 234881024;
    uint32_t Vmask = 33554431;



    while (halt_check != true){
        /* bit unpacking the instruction */
        assert(program_counter < segZero);

        /* pulls 32 bit instruction from sequence at a counted index */

        void *ins = UArray_at(segmentZero, program_counter);
        uint32_t *instruct = ins;
        uint32_t instruction = *instruct;

        uint32_t currOp = instruction & opmask;
        currOp = currOp >> 28;

        if (currOp != 13){
            regA = instruction & Amask;
            regA = regA >> 6;
            regB = instruction & Bmask;
            regB = regB >> 3;
            regC = instruction & Cmask;
        }

        /* calling correct function in operations depending on opcode */
        switch(currOp)
        {
            case 0:
                register_arr[regA] = conditional_move(register_arr[regA], 
                                register_arr[regB], register_arr[regC]);
                break;
            case 1:
                register_arr[regA] = segmented_load(register_arr[regB],
                             register_arr[regC], skeletonSeq);
                break;
            case 2:
                segmented_store(register_arr[regA], register_arr[regB],
                            register_arr[regC], skeletonSeq);
                break;
            case 3:
                register_arr[regA] = add(register_arr[regB],
                    register_arr[regC]);
                break;
            case 4:
                register_arr[regA] = multiply(register_arr[regB],
                         register_arr[regC]);
                break;
            case 5:
                register_arr[regA] = divide(register_arr[regB],
                       register_arr[regC]);
                break;
            case 6:
                register_arr[regA] = bitwiseNand(register_arr[regB],
                            register_arr[regC]);
                break;
            case 7:
                halt_check = true;
                break;
            case 8:
                register_arr[regB] = map_segment(register_arr[regC],
                            skeleton);
                break;
            case 9:
                unmap_segment(register_arr[regC], skeleton);
                break;
            case 10:
                output(register_arr[regC]);
                break;
            case 11:
                register_arr[regC] = input();
                break;
            case 12:
                load_program(skeletonSeq, register_arr[regB]);
                segmentZero = *((UArray_T *)UArray_at(skeletonSeq, 0));
                /* updates the counter for segmentZero */
                program_counter = register_arr[regC];
                segZero = (uint32_t)UArray_length(segmentZero);
                break;
            case 13:
                /* special case bitunpacking */
                regA = LAmask & instruction;
                regA = regA >> 25;
                value = Vmask & instruction;
                register_arr[regA] = value;
                break;

       }
      if (currOp != 12)
        program_counter++;
   }
     /* freeing memory allocated for sequences and closing files */
     fclose(fp);

     UArray_free(&skeleton->unmapped);

    int skel_len = UArray_length(skeleton->skeleton_seq);
      for(int i = 0; i < skel_len; i++){
        UArray_T seg = *((UArray_T *)UArray_at(skeleton->skeleton_seq, i));
         if(seg != NULL){
            UArray_free(&seg);
         }

     }
    UArray_free(&skeleton->skeleton_seq);
    free(skeleton);

    return 0;
}

Except_T Bitpack_Overflow = { "Overflow packing bits" };

static inline uint64_t shl(uint64_t word, unsigned bits)
{
        assert(bits <= 64);
        if (bits == 64)
                return 0;
        else
                return word << bits;
}

/*
 * shift R logical
 */
static inline uint64_t shr(uint64_t word, unsigned bits)
{
        assert(bits <= 64);
        if (bits == 64)
                return 0;
        else
                return word >> bits;
}

/*
 * shift R arith
 */
static inline int64_t sra(uint64_t word, unsigned bits)
{
        assert(bits <= 64);
        if (bits == 64)
                bits = 63; /* will get all copies of sign bit,
                            * which is correct for 64
                            */
  /* Warning: following uses non-portable >> on
     signed value...see K&R 2nd edition page 206. */
        return ((int64_t) word) >> bits;
}

/****************************************************************/
bool Bitpack_fitss( int64_t n, unsigned width)
{
        if (width >= 64)
                return true;
        int64_t narrow = sra(shl(n, 64 - width),
                             64 - width);
        return narrow == n;
}

bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        if (width >= 64)
                return true;
        /* thanks to Jai Karve and John Bryan  */
        /* clever shortcut instead of 2 shifts */
        return shr(n, width) == 0;
}

/****************************************************************/

int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        if (width == 0) return 0;    /* avoid capturing unknown sign bit    */

        unsigned hi = lsb + width; /* one beyond the most significant bit */
        assert(hi <= 64);
        return sra(shl(word, 64 - hi),
                   64 - width);
}

uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        unsigned hi = lsb + width; /* one beyond the most significant bit */
        assert(hi <= 64);
        /* different type of right shift */
        return shr(shl(word, 64 - hi),
                   64 - width);
}

/****************************************************************/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
                      uint64_t value)
{
        unsigned hi = lsb + width; /* one beyond the most significant bit */
        assert(hi <= 64);
        if (!Bitpack_fitsu(value, width))
                RAISE(Bitpack_Overflow);
        return shl(shr(word, hi), hi)                 /* high part */
                | shr(shl(word, 64 - lsb), 64 - lsb)  /* low part  */
                | (value << lsb);                     /* new part  */
}

uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,
                      int64_t value)
{
        if (!Bitpack_fitss(value, width))
                RAISE(Bitpack_Overflow);
        /* thanks to Michael Sackman and Gilad Gray */
        return Bitpack_newu(word, width, lsb, Bitpack_getu(value, width, 0));
}

