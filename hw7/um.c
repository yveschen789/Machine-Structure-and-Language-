//WORKING VERSION USING C ARRAYS WITH CLEAN VALGRIND

/******************************************************************************
 *
 *     um.c
 *
 *     Assignment: Comp40 HW6 (UM)
 *     Authors:    Era Iyer (eiyer01) and Yves Chen (ychen22)
 *     Date:       12/02/2019
 *
 *     This file serves as the main for our um program. It is responsible
 *     for creating and allocating space for the overall memory structure
 *     as well as segment zero. It calls the appropriate functions to load
 *     the um file and execute each instruction. All memory is freed at the end
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

#define INIT_SIZE 10

/*struct containing c arrays for data storage*/
struct Skeleton{
    uint32_t **skeleton_seq;
    uint32_t *unmapped;
    int unmapped_length;
    int skeleton_length;
    int unmapped_index;
    int skel_index;

};

void execute(struct Skeleton *skeleton);
void loadAndStore(uint32_t **skeletonSeq, FILE *fp);

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

int main(int argc, char const *argv[]) {

    assert(argc == 2);
    FILE *fp = NULL;
    fp = fopen(argv[1], "r");
    assert(fp != NULL);

    struct stat st;
    stat(argv[1], &st);
    int size = st.st_size;

    /* allocating space for skeleton struct and all arrays within it */
    struct Skeleton *skeleton = malloc(sizeof(struct Skeleton));

    uint32_t *unmap = calloc((INIT_SIZE),sizeof(uint32_t));
    skeleton->unmapped = unmap;
    skeleton->unmapped_index = 0;
    skeleton->unmapped_length = INIT_SIZE;

    uint32_t **skeletonSeq = calloc((INIT_SIZE),sizeof(uint32_t *));
    skeleton->skeleton_seq = skeletonSeq;
    skeleton->skel_index = 1;
    skeleton->skeleton_length = INIT_SIZE;

    skeleton->skeleton_seq[0] = calloc((1+(size/4)),sizeof(uint32_t));
    loadAndStore(skeleton->skeleton_seq, fp);

    /* storing segment 0's size in the 0th element */  
    skeleton->skeleton_seq[0][0] = (uint32_t)(size/4);
    execute(skeleton);

    /* freeing memory allocated for stored data and closing file */
    fclose(fp);
    if(skeleton->unmapped != NULL){
        free(skeleton->unmapped);
    }

    int skel_len = skeleton->skeleton_length;
    for(int i = 0; i < skel_len; i++){
        uint32_t *seg = skeleton->skeleton_seq[i];
        if(seg != NULL){
            free(seg);
        }
    }
    free(skeleton->skeleton_seq);
    free(skeleton);

    return 0;
}

void loadAndStore(uint32_t **skeletonSeq, FILE *fp){

    assert(fp);
    uint32_t instruction = 0;
    uint32_t curr_byte = 0;

    int counter = 1;

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
      skeletonSeq[0][counter] = instruction;
      counter++;
      }

}

void execute(struct Skeleton *skeleton){

    uint32_t register_arr[8] = {0};
    uint32_t program_counter = 0;
    uint32_t value;

    uint32_t regA, regB, regC;
    bool halt_check = false;
    uint32_t segZero = skeleton->skeleton_seq[0][0];

    uint32_t opmask = 4026531840;
    uint32_t Amask = 448;
    uint32_t Bmask = 56;
    uint32_t Cmask = 7;
    uint32_t LAmask = 234881024;
    uint32_t Vmask = 33554431;


    while (halt_check != true){
        
        /* bit unpacking the instruction */
        assert(program_counter < segZero);
        uint32_t instruction = skeleton->skeleton_seq[0][program_counter+1];
        uint32_t currOp = instruction & opmask;
        currOp = currOp >> 28;
         
        if (currOp != 13){
          regA = instruction & Amask;
          regA = regA >> 6;
          regB = instruction & Bmask;
          regB = regB >> 3;
          regC = instruction & Cmask;
        }

       /* conditional move */
        if(currOp == 0){
          if(register_arr[regC] != 0){
            register_arr[regA]= register_arr[regB];
          }
        }
       /* segmented load */
        else if(currOp == 1){
          uint32_t *seg = skeleton->skeleton_seq[register_arr[regB]];
          assert(seg);
          uint32_t length = seg[0];

          if (register_arr[regC] <= length){
            register_arr[regA] = seg[register_arr[regC]+1];
          }
        }
        /* segmented store */
        else if(currOp == 2){
          uint32_t *seg = skeleton->skeleton_seq[register_arr[regA]];
          assert(seg);

          if (register_arr[regB] <= seg[0])
            seg[1+register_arr[regB]] = register_arr[regC];
        }
        /* add */
        else if(currOp == 3){
          register_arr[regA] = (register_arr[regB] + register_arr[regC])
                                % 4294967296;
        }
        /* multiply */
        else if(currOp == 4){
          register_arr[regA] = (register_arr[regB] * register_arr[regC]) 
                                % 4294967296;
        }
        /* divide */
        else if(currOp == 5){
          register_arr[regA] = (register_arr[regB] / register_arr[regC]);
        }
        /* bitwise NAND */
        else if(currOp == 6){
          register_arr[regA] = ~(register_arr[regB] & register_arr[regC]);
        }
        /* halt */
        else if(currOp == 7){
          halt_check = true;

        }
        /* map */
        else if(currOp == 8){
            uint32_t *newSeg = calloc((register_arr[regC] + 1), 
                               sizeof(uint32_t));
            newSeg[0] = register_arr[regC];
            for(int i = 1 ; i < (int)register_arr[regC]; i++)
                newSeg[i] = 0;

             if (skeleton->unmapped_index == 0){
                skeleton->skeleton_seq[skeleton->skel_index] = newSeg;
                register_arr[regB] = skeleton->skel_index;
                skeleton->skel_index +=1;
                if(skeleton->skel_index >= skeleton->skeleton_length){
                    skeleton->skeleton_seq = realloc(skeleton->skeleton_seq, 
                                             sizeof(uint32_t *) *
                                             (skeleton->skeleton_length*2));
                    int len = skeleton->skeleton_length;
                    for(int i = len; i < len * 2; i ++){
                        skeleton->skeleton_seq[i] = NULL;
                    }
                    skeleton->skeleton_length = len * 2;
                }

            }
            else{
                uint32_t ind = skeleton->unmapped[skeleton->unmapped_index-1];
                skeleton->unmapped_index-=1;
                skeleton->skeleton_seq[ind] = newSeg;
                register_arr[regB] = ind;
            }
        }
        /* unmap */
        else if(currOp == 9){
            skeleton->unmapped[skeleton->unmapped_index] = register_arr[regC];
            skeleton->unmapped_index += 1;
            if(skeleton->unmapped_index >= skeleton->unmapped_length){
                skeleton->unmapped = realloc(skeleton->unmapped, skeleton->unmapped_length * 2 * sizeof(uint32_t));
                skeleton->unmapped_length = skeleton->unmapped_length * 2;
            }
            uint32_t *getSeg = skeleton->skeleton_seq[register_arr[regC]];
            free(getSeg);
            skeleton->skeleton_seq[register_arr[regC]] = NULL;
        }
        /* output */ 
        else if(currOp == 10){
            if((int)register_arr[regC] <= 255 && (int)register_arr[regC] >=0)
                fputc((char)register_arr[regC], stdout);
        }
        /* input */
        else if(currOp == 11){
            int c = fgetc(stdin);
            if(c <= 255 && c >=0)
                register_arr[regC] = (uint32_t)c;
            if(c == EOF)
                register_arr[regC] = ~0;

        }
        /* load program */
        else if(currOp == 12){
            if(register_arr[regB] != 0){
                uint32_t *segement_b = skeleton->skeleton_seq[register_arr[regB]];
                int blength = segement_b[0];
                uint32_t *newSeq = malloc((1+blength)*sizeof(uint32_t));
                memcpy(newSeq, segement_b,(1+blength)*sizeof(uint32_t));
                free(skeleton->skeleton_seq[0]);
                skeleton->skeleton_seq[0] = newSeq;
            }
            program_counter = register_arr[regC];
            segZero = skeleton->skeleton_seq[0][0];
        }
        /* load value */
        else if(currOp == 13){
            regA = LAmask & instruction;
            regA = regA >> 25;
            value = Vmask & instruction;
            register_arr[regA] = value;
        }
      if (currOp != 12)
        program_counter++;
    }
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

