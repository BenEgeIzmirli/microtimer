#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint64_t regs[5];

int main(int argc, char** argv) {
    if ( argc != 2 ) {
        printf("Nah\n");
        exit(-1);
    }
    int ct = 0;
    while ( argv[1][ct++] );
    ct--;

    bprintf(regs,4,"%llx %llx %llx %llx\n",argv[1]);

    printf("Success : %s\n",(char*)regs);
    exit(0);
}
