#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <sys/mman.h>

FILE* file_desc;
uint64_t t0, t1, num_cycles;
uint64_t start_cycles_low,start_cycles_high;
uint64_t end_cycles_low,end_cycles_high;

#define CLEAR_CACHE \
file_desc = fopen("/dev/wbinvd","rb"); \
if ( file_desc == NULL ) { \
    printf("Can't find device /dev/wbinvd\n"); \
    exit(-1); \
} \
fclose(file_desc); \
asm volatile("MFENCE");

#define START_TIMER \
asm volatile("MFENCE"); \
asm volatile("CPUID"); \
asm volatile("RDTSC":"=a"(start_cycles_low),"=d"(start_cycles_high)); \

#define END_TIMER \
asm volatile("RDTSCP":"=a"(end_cycles_low),"=d"(end_cycles_high)); \
asm volatile("CPUID"); \
asm volatile("MFENCE"); \
t0 = ((uint64_t) start_cycles_high << 32) | start_cycles_low; \
t1 = ((uint64_t) end_cycles_high << 32) | end_cycles_low; \
num_cycles = (uint64_t)(((int64_t)(t1-t0))/4);

int main(int argc, char** argv) {
    CLEAR_CACHE;
    START_TIMER;
    END_TIMER;
//    for ( int i=0 ; i < 1000 ; i++ ) {
//        asm volatile("pause");
//    }
  
    if ( argc < 1 ) {
        printf("Usage: a.out verbosity num_instructions num_runs rax_value [reg=value]\n"
               "   verbosity: 0=silent, 1=cycles only, 2=registers+cycles, 3=registers+more cycles\n"
               "   rax_value: the hexadecimal value to put in rax\n"
               "   reg=value: reg is any MASM register name, value is any hex value (not yet supported)\n");
        exit(1);
    }

//    int verbosity_temp = strtol(argv[1],NULL,0);
//    uint64_t rax_value_temp = strtoull(argv[2],NULL,16);
    int verbosity = strtol(argv[1],NULL,0);
    uint64_t rax_value = strtoull(argv[2],NULL,16);
//
//    CLEAR_CACHE;
//
//    int verbosity = verbosity_temp;
//    int rax_value = rax_value_temp;
//    END_TIMER;
//    printf("%"PRIi64"\n",num_cycles);
    
    uint64_t rax, rbx, rcx, rdx;

    if ( verbosity > 1 ) {
        asm volatile("cpuid" :"=a"(rax),"=b"(rbx), "=c"(rcx), "=d"(rdx) :"a"(rax_value));
        printf("rax=%"PRIx64"\nrbx=%"PRIx64"\nrcx=%"PRIx64"\nrdx=%"PRIx64"\n",rax,rbx,rcx,rdx);
    }

    uint64_t overhead,this_time,ret;

    for ( int i=0 ; i < 10 ; i++ ) {

        START_TIMER;
        asm volatile(
            #include "primer.asm.c"
        );
        END_TIMER;

        overhead = num_cycles;
        if ( verbosity > 1 ) {
            printf("overhead %"PRIi64"\n",overhead);
        }
        if ( overhead == 0 ) {
            printf("overhead %"PRIi64"\n",overhead);
        }

        START_TIMER;
        asm volatile(
            #include "primer.asm.c"
        );
        asm volatile(
            #include "timethis.asm.c"
        );
        END_TIMER;
    
        this_time = num_cycles-overhead;
        if ( verbosity > 2 ) {
            printf("%"PRIu64"\n", this_time);
        }
        if ( !ret ) {
            ret = this_time;
        } else {
            if ( this_time < ret ) {
                ret = this_time;
            }
        }
    }

    if ( verbosity > 0 ) {
        printf("%16"PRIx64" : %"PRIi64"\n",rax_value, ret);
    }
}









