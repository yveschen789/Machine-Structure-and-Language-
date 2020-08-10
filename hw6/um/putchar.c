#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char const *argv[]) {
    FILE *fp = NULL;

    fp = fopen(argv[1], "w");

    putc(212	,fp);   //putting 'r' 4 times
    putc(0	,fp);
    putc(0	,fp);
    putc(2 ,fp);


    putc(192	,fp);   //putting 'r' 4 times
    putc(0	,fp);
    putc(0	,fp);
    putc(18 ,fp);

    // putc(212	,fp);   //putting 'r' 4 times
    // putc(0	,fp);
    // putc(0	,fp);
    // putc(2 ,fp);
    //
    // putc(48	,fp);   //putting 'r' 4 times
    // putc(0	,fp);
    // putc(0	,fp);
    // putc(234 ,fp);
    //
    // putc(112,fp);   //putting 'r' 4 times
    // putc(0	,fp);
    // putc(0	,fp);
    // putc(0 ,fp);

    fclose(fp);

    return 0;
}
