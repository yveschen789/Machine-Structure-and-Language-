/******************************************************************************
 *
 *     execute.c
 *
 *     Assignment: Comp40 HW6 (UM)
 *     Authors:    Era Iyer (eiyer01) and Yves Chen (ychen22)
 *     Date:       11/19/2019
 *
 *     Handles reading instructions and executing the operations with a
 *     function call.
 *
 *****************************************************************************/

#include "assert.h"
#include "execute.h"


/*
 * Name: execute
 * Parameters: struct Skeleton *skeleton
 * Purpose: handles reading and executing instructions from segmentZero
 *          at specified index until halt command, running an Operations
 *          function for each bitunpacked instruction
 * Returns: nothing
*/
void execute(struct Skeleton *skeleton){

    Seq_T skeletonSeq = skeleton->skeleton_seq;
    uint32_t register_arr[8] = {0};
    uint32_t program_counter = 0;
    uint32_t value;

    void *s = Seq_get(skeletonSeq, 0);
    Seq_T segmentZero = (Seq_T)s;
    uint32_t regA, regB, regC;
    bool halt_check = false;

    while (halt_check != true){
        /* bit unpacking the instruction */
        assert(program_counter < (uint32_t)Seq_length(segmentZero));

        /* pulls 32 bit instruction from sequence at a counted index */
        uint32_t instruction = (uintptr_t)Seq_get(segmentZero,
                                program_counter);

        uint32_t currOp = (uint32_t)Bitpack_getu(instruction, 4, 28);

        if (currOp != 13){
        regA = (uint32_t)Bitpack_getu(instruction, 3, 6);
        regB = (uint32_t)Bitpack_getu(instruction, 3, 3);
        regC = (uint32_t)Bitpack_getu(instruction, 3, 0);
        }

        /* calling correct function in operations depending on opcode */
        switch(currOp)
        {
            case 0:
                conditional_move(&register_arr[regA], &register_arr[regB],
                                 &register_arr[regC]);
                break;
            case 1:
                segmented_load(&register_arr[regA], &register_arr[regB],
                             &register_arr[regC], skeletonSeq);
                break;
            case 2:
                segmented_store(&register_arr[regA], &register_arr[regB],
                            &register_arr[regC], skeletonSeq);
                break;
            case 3:
                add(&register_arr[regA], &register_arr[regB],
                    &register_arr[regC]);
                break;
            case 4:
                multiply(&register_arr[regA], &register_arr[regB],
                         &register_arr[regC]);
                break;
            case 5:
                divide(&register_arr[regA], &register_arr[regB],
                       &register_arr[regC]);
                break;
            case 6:
                bitwiseNand(&register_arr[regA], &register_arr[regB],
                            &register_arr[regC]);
                break;
            case 7:
                halt_check = halt();
                break;
            case 8:
                map_segment(&register_arr[regB], &register_arr[regC],
                            skeleton);
                break;
            case 9:
                unmap_segment(&register_arr[regC], skeleton);
                break;
            case 10:
                output(&register_arr[regC]);
                break;
            case 11:
                input(&register_arr[regC]);
                break;
            case 12:
                load_program(skeletonSeq, &register_arr[regB]);
                /* changes segmentzero sequence that loop pulls from */
                void *s = Seq_get(skeletonSeq, 0);
                segmentZero = (Seq_T)s;
                /* updates the counter for segmentZero */
                program_counter = register_arr[regC];
                break;
            case 13:
                /* special case bitunpacking */
                regA = (uint32_t)Bitpack_getu(instruction, 3, 25);
                value = (uint32_t)Bitpack_getu(instruction, 25, 0);
                loadval(&register_arr[regA], value);
                break;

       }
      if (currOp != 12)
        program_counter++;
   }
}
