/***********************************************************************
 *                              ppmtrancs.c
 * Assignment: Homework 3 for Comp 40, Fall 2019
 * Authors: Andreas Moe (amoe01) and Yves Chen (ychen22)
 * Date: October 9, 2019
 *
 * Summary: This program rotates ppm images and 
 ***********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "cputiming.h"

/* Closure struct to be passed into map functions */
struct Closure {
        A2Methods_T methods;
        A2Methods_UArray2 array;
};

/* Function declarations */
void setImage(Pnm_ppm img, int width, int height, A2Methods_UArray2 array);
void rotate_0(Pnm_ppm img);
void rotate_90(int i, int j, A2Methods_UArray2 array, A2Methods_Object *elem, 
        void *cl);
void rotate_180(int i, int j, A2Methods_UArray2 array, A2Methods_Object *elem, 
        void *cl);
void rotate_270(int i, int j, A2Methods_UArray2 array, A2Methods_Object *elem, 
        void *cl);

#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (0)

static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] [filename]\n",
                        progname);
        exit(1);
}

/*
 * setImage
 *    Purpose: Updates a Pnm_ppm object with a different array and the 
 *             corresponding width and height.
 * Parameters: A Pnm_ppm object, width and height of the new array, and the new
 *             array itself.
 *    Returns: Nothing.
 *    Expects: Nothing.
 */
void setImage(Pnm_ppm img, int width, int height, A2Methods_UArray2 array)
{
        img->width = width;
        img->height = height;
        img->methods->free(&(img->pixels));
        img->pixels = array;
}

/*
 * rotate_0
 *    Purpose: Rotates an image 0 degrees. Meant to be used as an apply function
 *             with methods->map. 
 * Parameters: A Pnm_ppm object.
 *    Returns: Nothing.
 *    Expects: Nothing.
 */
void rotate_0(Pnm_ppm img)
{
        Pnm_ppmwrite(stdout, img);
}

/*
 * rotate_90
 *    Purpose: Rotates an image 90 degrees. Meant to be used as an apply 
 *             function with methods->map. 
 * Parameters: Two integers for the column and row, an A2Methods_UArray2 object,
 *             an A2Methods_Object pointer for each element and a void pointer
 *             closure argument.
 *    Returns: Nothing.
 *    Expects: Nothing.
 */
void rotate_90(int i, int j, A2Methods_UArray2 array, A2Methods_Object *elem, 
        void *cl)
{
        struct Closure *mcl = cl; 
        int newI = mcl->methods->height(array) - j - 1;
        int newJ = i;
        void *pixelptr = mcl->methods->at(mcl->array, newI, newJ);
        Pnm_rgb pixelNew = pixelptr;
        Pnm_rgb pixelOld = elem;
        *pixelNew = *pixelOld;
}

/*
 * rotate_180
 *    Purpose: Rotates an image 180 degrees. Meant to be used as an apply 
 *             function with methods->map. 
 * Parameters: Two integers for the column and row, an A2Methods_UArray2 object,
 *             an A2Methods_Object pointer for each element and a void pointer
 *             closure argument.
 *    Returns: Nothing.
 *    Expects: Nothing.
 */
void rotate_180(int i, int j, A2Methods_UArray2 array, A2Methods_Object *elem, 
        void *cl)
{
        struct Closure *mcl = cl;
        int newI = mcl->methods->width(array) - i - 1;
        int newJ = mcl->methods->height(array) - j - 1;
        void *pixelptr = mcl->methods->at(mcl->array, newI, newJ);
        Pnm_rgb pixelNew = pixelptr;
        Pnm_rgb pixelOld = elem;
        *pixelNew = *pixelOld;
}

/*
 * rotate_270
 *    Purpose: Rotates an image 270 degrees. Meant to be used as an apply 
 *             function with methods->map. 
 * Parameters: Two integers for the column and row, an A2Methods_UArray2 object,
 *             an A2Methods_Object pointer for each element and a void pointer
 *             closure argument.
 *    Returns: Nothing.
 *    Expects: Nothing.
 */
void rotate_270(int i, int j, A2Methods_UArray2 array, A2Methods_Object *elem, 
        void *cl)
{
        struct Closure *mcl = cl;
        int newI = j;
        int newJ = mcl->methods->width(array) - i - 1;
        void *pixelptr = mcl->methods->at(mcl->array, newI, newJ);
        Pnm_rgb pixelNew = pixelptr;
        Pnm_rgb pixelOld = elem;
        *pixelNew = *pixelOld;
}

int main(int argc, char *argv[]) 
{
        CPUTime_T timer = NULL;
        char *time_file_name = NULL;
        FILE *time_file = NULL;
        double total_time = 0;
        FILE *file = NULL;

        Pnm_ppm Pnm_image = NULL;

        struct Closure *mcl = malloc(sizeof(struct Closure));
        /* Array that represents the transformed image */
        A2Methods_UArray2 writeUarray = NULL; 
        int   rotation       = 0;
        int   i;


        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        assert(map);

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major, 
                                    "row-major");
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major, 
                                    "column-major");
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                } else if (strcmp(argv[i], "-rotate") == 0) {
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr, 
                                        "Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {    /* Not a number */
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-time") == 0) {
                        time_file_name = argv[++i];
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else {
                        break;
                }
        }
        
        file = fopen(argv[argc - 1], "r");
        if (file == NULL) {
                file = stdin;
        }
        Pnm_image = Pnm_ppmread(file, methods);

        /* Create CPUTime_T object only if time file is specified. */
        if (time_file_name != NULL) {
                timer = CPUTime_New();
        }

        if (rotation == 0) {
                if (timer != NULL) {
                        CPUTime_Start(timer);
                        rotate_0(Pnm_image);
                        total_time = CPUTime_Stop(timer);
                }
                else {
                        rotate_0(Pnm_image);
                }
        }
        if (rotation == 90) {
                /* Initialize writeUarray with correct dimensions */
                writeUarray = methods->new(Pnm_image->height, 
                        Pnm_image->width, methods->size(Pnm_image->pixels));
                /* Put in struct Closure to be passed to map */
                mcl->methods = methods;
                mcl->array= writeUarray;

                if (timer != NULL) {
                        CPUTime_Start(timer);
                        map(Pnm_image->pixels, rotate_90, mcl);
                        total_time = CPUTime_Stop(timer);
                }
                else {
                        map(Pnm_image->pixels, rotate_90, mcl);
                }

                setImage(Pnm_image, Pnm_image->height, Pnm_image->width, 
                        mcl->array);
                Pnm_ppmwrite(stdout, Pnm_image);
        }
        if (rotation == 180) {
                /* Initialize writeUarray with correct dimensions */
                writeUarray = methods->new(Pnm_image->
                        width, Pnm_image->height, methods->size
                                (Pnm_image->pixels));
                mcl->methods = methods;
                mcl->array= writeUarray;
                /* Put in struct Closure to be passed to map */
                if (timer != NULL) {
                        CPUTime_Start(timer);
                        map(Pnm_image->pixels, rotate_180, mcl);
                        total_time = CPUTime_Stop(timer);
                }
                else {
                        map(Pnm_image->pixels, rotate_180, mcl);
                }

                setImage(Pnm_image, Pnm_image->width, Pnm_image->height, 
                        mcl->array);
                Pnm_ppmwrite(stdout, Pnm_image);
        }
        if (rotation == 270) {
                /* Initialize writeUarray with correct dimensions */
                writeUarray = methods->new(Pnm_image->height, 
                        Pnm_image->width, methods->size(Pnm_image->pixels));
                mcl->methods = methods;
                mcl->array= writeUarray;
                /* Put in struct Closure to be passed to map */
                if (timer != NULL) {
                        CPUTime_Start(timer);
                        map(Pnm_image->pixels, rotate_270, mcl);
                        total_time = CPUTime_Stop(timer);
                }
                else {
                        map(Pnm_image->pixels, rotate_270, mcl);
                }

                setImage(Pnm_image, Pnm_image->height, Pnm_image->width, 
                        mcl->array);
                Pnm_ppmwrite(stdout, Pnm_image);
        }
        /* If program was run with time option */
        if (total_time != 0) {
                fprintf(time_file, "Img Size: %d x %d\n", 
                        methods->width(Pnm_image->pixels), 
                                methods->height(Pnm_image->pixels));
                fprintf(time_file, "Rotation: %d\n", rotation);
                fprintf(time_file, "Total Time: %f\n", total_time);
                double avg_time = total_time / 
                        (methods->width(Pnm_image->pixels) * 
                                methods->height(Pnm_image->pixels));
                fprintf(time_file, "Average Time: %f\n", avg_time);
                fclose(time_file);
                CPUTime_Free(&timer);

        }
        Pnm_ppmfree(&Pnm_image);
        free(mcl);
        return 0;
}