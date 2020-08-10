/*
 * umlab.c
 *
 * The functions defined in this lab should be linked against umlabwrite.c
 * to produce a unit test writing program. Any additional functions and unit
 * tests written for the lab go here.
 */

#include <stdint.h>
#include <stdio.h>

#include <assert.h>
#include <seq.h>
#include "bitpack.h"


typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc);
Um_instruction loadval(unsigned ra, unsigned val);


/* Wrapper functions for each of the instructions */

static inline Um_instruction halt(void)
{
        return three_register(HALT, 0, 0, 0);
}

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

static inline Um_instruction add(Um_register a, Um_register b, Um_register c)
{
        return three_register(ADD, a, b, c);
}

static inline Um_instruction sload(Um_register a, Um_register b, Um_register c)
{
        return three_register(SLOAD, a, b, c);
}
static inline Um_instruction sstore(Um_register a, Um_register b, Um_register c)
{
        return three_register(SSTORE, a, b, c);
}

static inline Um_instruction mul(Um_register a, Um_register b, Um_register c)
{
        return three_register(MUL, a, b, c);
}

static inline Um_instruction div(Um_register a, Um_register b, Um_register c)
{
        return three_register(DIV, a, b, c);
}

static inline Um_instruction mov(Um_register a, Um_register b, Um_register c)
{
        return three_register(CMOV, a, b, c);
}

static inline Um_instruction nand(Um_register a, Um_register b, Um_register c)
{
        return three_register(NAND, a, b, c);
}

static inline Um_instruction map(Um_register b, Um_register c)
{
        return three_register(ACTIVATE, 0, b, c);
}

static inline Um_instruction unmap( Um_register c)
{
        return three_register(INACTIVATE, 0, 0, c);
}

static inline Um_instruction output(Um_register c){
        return three_register(OUT, 0, 0, c);
}

static inline Um_instruction input(Um_register c){
        return three_register(IN, 0, 0, c);
}

/* Functions for working with streams */

static inline void emit(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

extern void Um_write_sequence(FILE *output, Seq_T stream);


/* Unit tests for the UM */

void emit_halt_test(Seq_T stream)
{
        emit(stream, halt());
}

void emit_verbose_halt_test(Seq_T stream)
{

        emit(stream, loadval(r1, 'B'));
        emit(stream, output(r1));
        emit(stream, loadval(r1, 'a'));
        emit(stream, output(r1));
        emit(stream, loadval(r1, 'd'));
        emit(stream, output(r1));
        emit(stream, loadval(r1, '!'));
        emit(stream, output(r1));
        emit(stream, loadval(r1, '\n'));
        emit(stream, output(r1));
        emit(stream, halt());

}

void emit_add_test(Seq_T stream){

        emit(stream, loadval(r1, 0));
        emit(stream, loadval(r2, 48));
        emit(stream, loadval(r3, 2));
        emit(stream, add(r1, r2, r3));   //should output 4264
        emit(stream, output(r1));

        emit(stream, halt());

}

void emit_mul_test(Seq_T stream){

        emit(stream, loadval(r1, 50));  //r1 = 2
        emit(stream, loadval(r2, 1));   //r2 = 1
        emit(stream, loadval(r3, 73));
        emit(stream, mul(r3, r1, r2));  //r3 = 2
        emit(stream, output(r3));   //should output 2

        emit(stream, halt());
}

void emit_div_test(Seq_T stream){

        emit(stream, loadval(r1, 0));
        emit(stream, loadval(r2, 28));
        emit(stream, loadval(r3, 7));
        emit(stream, div(r1, r2, r3));
        emit(stream, loadval(r4, 48));
        emit(stream, add(r5, r1, r4));
        emit(stream, output(r5)); //should output 4

        emit(stream, halt());

}

void emit_mov_test(Seq_T stream){

        emit(stream, loadval(r1, 11111));
        emit(stream, loadval(r2, 8642));
        emit(stream, loadval(r3, 1));
        emit(stream, mov(r1, r2, r3));
        emit(stream, output(r1)); //should be 8642

        emit(stream, halt());

}

void emit_bnand_test(Seq_T stream){

        emit(stream, loadval(r1, 72));
        emit(stream, loadval(r2, 12));  //12 in binary is  000001100
        emit(stream, loadval(r3, 309)); //309 in binary is 100110101
        emit(stream, nand(r1, r2, r3));
        emit(stream, output(r1)); //should be 4294967291

        emit(stream, halt());

}

void emit_test_arith(Seq_T stream){
       emit(stream, loadval(r1, 5));
       emit(stream, loadval(r2, 11));
       emit(stream, loadval(r3, 9));
       emit(stream, loadval(r4, 48));
       emit(stream, mov(r1, r2, r3));
       emit(stream, add(r5, r1, r4));
       emit(stream, output(r5));  //r1 becomes 11
       emit(stream, nand(r1, r2, r3));
       emit(stream, add(r5, r1, r4));
       emit(stream, loadval(r7, '\n'));
       emit(stream, output(r7));
       emit(stream, output(r5)); //r1 becomes 4294967286
       emit(stream, div(r1, r2, r3));
       emit(stream, mul(r1, r2, r3));
       emit(stream, output(r7));
       //emit(stream, add(r5, r1, r4));
       //emit(stream, output(r5)); //r1 becomes 11*9 = 99

        emit(stream, halt());

}

void emit_in_out_test(Seq_T stream){
    emit(stream, input(r1));  //input 48924
    emit(stream, loadval(r2, 48));
    emit(stream, add(r3, r2, r1));
    emit(stream, output(r3)); //r2 should output 992
    emit(stream, halt());
}

void emit_map_unmap_test(Seq_T stream){
        emit(stream, loadval(r3, 55));
        emit(stream, loadval(r4, 51));
        emit(stream, map(r2, r3));  //creating a sequence of size 17 into r2
        //emit(stream, output(r2));
        emit(stream, map(r1, r4));  //creating a sequence of size 3 into r2
        emit(stream, unmap(r2));   //unmapping r2
        emit(stream, map(r3, r4));
        emit(stream, loadval(r4, 48));
        emit(stream, add(r5, r3, r4));
        emit(stream, output(r5)); //should be 1 the first time map_test was
        //called

        emit(stream, halt());

}


void emit_map_test(Seq_T stream){
        emit(stream, loadval(r3, 55));
        emit(stream, map(r2, r3));  //mapping sequence of size 17 at index 1
        //of skeleton
        //emit(stream, output(r2)); //r2 should be 1
        //emit(stream, unmap(r2));
        emit(stream, input(r4)); //r4 input is 342
        emit(stream, loadval(r4, 34));
        emit(stream, loadval(r5, 0)); //setting r5 to 0
        emit(stream, sstore(r2, r5, r4)); //accessing r2 [index 1] index within
        //skeleton segment, within r2 segment, getting r5 index [index 0],
        //placing r4, 342
        // => skeleton_seq[[r2][r5]]=r4
        emit(stream, loadval(r5, 1)); //setting r5 to 1
        //emit(stream, input(r4));   //r4 input should be is 52
        emit(stream, loadval(r4, 52));
        emit(stream, sstore(r2, r5, r4)); //setting skeleton_seq[[r2][r5]]=r4
        //=> 52

        emit(stream, loadval(r5, 0)); //setting r5 to 0
        emit(stream, sload(r2, r5, r6)); //r6 has the values stored at [0]
        //emit(stream, output(r6));

        emit(stream, loadval(r5, 1));
        emit(stream, sload(r2, r5, r6)); //r6 has the values stored at [1]
        emit(stream, output(r2));

        emit(stream, halt());

}

void emit_unmap_test(Seq_T stream){
        emit(stream, loadval(r3, 1));
        emit(stream, unmap(r3));
        emit(stream, map(r2, r4));
        emit(stream, output(r2));
        emit(stream, map(r2, r1));
        emit(stream, output(r2));


        emit(stream, halt());

}

void emit_print_six_test(Seq_T stream){


        emit(stream, loadval(r1,48));
        emit(stream, loadval(r2,6));
        emit(stream, add(r3, r1, r2));
        emit(stream, output(r3));
        emit(stream, halt());

}

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc){


        assert(op < 13);

        uint64_t instruct = 0;
        instruct = Bitpack_newu(instruct, 4, 28, op );
        instruct = Bitpack_newu(instruct, 3, 6, ra );
        instruct = Bitpack_newu(instruct, 3, 3, rb );
        instruct = Bitpack_newu(instruct, 3, 0, rc );

        return (Um_instruction)instruct;
}

void Um_write_sequence(FILE *output, Seq_T stream){



        int sLength= Seq_length(stream);

        for (int i = 0; i < sLength; i++){
                uint32_t instruction = (uintptr_t)Seq_remlo(stream);
                fputc(Bitpack_getu(instruction, 8, 24), output);
                fputc(Bitpack_getu(instruction, 8, 16), output);
                fputc(Bitpack_getu(instruction, 8, 8), output);
                fputc(Bitpack_getu(instruction, 8, 0), output);



        }

}

Um_instruction loadval(unsigned ra, unsigned val){

        uint64_t instruct = 0;
        instruct = Bitpack_newu(instruct, 4, 28, 13);
        instruct = Bitpack_newu(instruct, 3, 25, ra );
        instruct = Bitpack_newu(instruct, 25, 0, val );

        return (Um_instruction)instruct;
}
