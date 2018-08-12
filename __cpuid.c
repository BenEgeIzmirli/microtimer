#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <sys/mman.h>
FILE* file_desc;

#define asm asm volatile

#define CLEAR_CACHE \
file_desc = fopen("/dev/wbinvd","rb"); \
if ( file_desc == NULL ) { \
    printf("Can't find device /dev/wbinvd\n"); \
    exit(-1); \
} \
fclose(file_desc); \
asm("MFENCE");

#define START_TIMER \
asm("MFENCE"); \
asm("CPUID"); \
asm("RDTSC":"=a"(start_cycles_low),"=d"(start_cycles_high)); \

#define END_TIMER \
asm("RDTSCP":"=a"(end_cycles_low),"=d"(end_cycles_high)); \
asm("CPUID"); \
asm("MFENCE"); \
t0 = ((uint64_t) start_cycles_high << 32) | start_cycles_low; \
t1 = ((uint64_t) end_cycles_high << 32) | end_cycles_low; \
num_cycles = (uint64_t)(((int64_t)(t1-t0))/4);

uint64_t t0, t1, num_cycles;
uint64_t start_cycles_low,start_cycles_high;
uint64_t end_cycles_low,end_cycles_high;


const char* primer = 
"        asm(\"xor %rax,%rax\");\n\
        asm(\"xor %rbx,%rbx\");\n\
        asm(\"xor %rcx,%rcx\");\n\
        asm(\"xor %rdx,%rdx\");\n\
";

int main(int argc, char** argv) {
    CLEAR_CACHE;
    START_TIMER; 
//    if ( argc < 2 ) {
//        printf("Usage: a.out verbosity num_instructions num_runs rax_value [reg=value]\n"
//               "   verbosity: 0=silent, 1=cycles only, 2=registers+cycles, 3=registers+more cycles\n"
//               "   rax_value: the hexadecimal value to put in rax\n"
//               "   reg=value: reg is any MASM register name, value is any hex value (not yet supported)\n");
//        exit(1);
//    }
//
//    int verbosity = strtol(argv[1],NULL,0);
//    int num_instructions = strtol(argv[2],NULL,0);
//    int num_runs = strtol(argv[3],NULL,0);
//    uint64_t rax_value = strtoull(argv[2],NULL,16);
    int verbosity = 3;
    int num_instructions = 1;
    int num_runs = 10;
    uint64_t rax_value = 0x20;
    
    END_TIMER;
    printf("%"PRIi64"\n",num_cycles);

    uint64_t rax, rbx, rcx, rdx;


    if ( verbosity > 1 ) {
        asm("cpuid" :"=a"(rax),"=b"(rbx), "=c"(rcx), "=d"(rdx) :"a"(rax_value));
        printf("rax=%"PRIx64"\nrbx=%"PRIx64"\nrcx=%"PRIx64"\nrdx=%"PRIx64"\n",rax,rbx,rcx,rdx);
    }

    int nr,ni;
    uint64_t overhead,this_time,ret;
    uint64_t ret_sum,ret_sq_sum;
    time_t trash;
    uint64_t i;
    for ( nr=num_runs ; nr ; nr-- ) {
        //CLEAR_CACHE;
        //asm("syscall"::"a"(0x0d),"b"(&trash));
        START_TIMER;
//        asm("");
        asm("xor %rax,%rax");
        asm("xor %rbx,%rbx");
        asm("xor %rcx,%rcx");
        asm("xor %rdx,%rdx");
        END_TIMER;
        overhead = num_cycles;
        if ( verbosity > 1 ) {
            printf("overhead %"PRIi64"\n",overhead);
        }
        if ( overhead == 0 ) {
            printf("overhead %"PRIi64"\n",overhead);
        }
        //CLEAR_CACHE;
        START_TIMER;
//        asm("");
        asm("xor %rax,%rax");
        asm("xor %rbx,%rbx");
        asm("xor %rcx,%rcx");
        asm("xor %rdx,%rdx");
        asm("cpuid" : : "a"(rax_value) );
        //asm("rdmsr" : : "c"(rax_value) );
        END_TIMER;

        // This crashes the computer :)
//        asm("xor %rax,%rax");
//        asm("xor %rbx,%rbx");
//        asm("xor %rcx,%rcx");
//        asm("xor %rdx,%rdx");
//        asm(".byte 0x0f");
//        asm(".byte 0x18");
//        asm(".byte 0x90");

        this_time = num_cycles-overhead;

//        if ( nr == num_runs ) {
//            continue;
//        }

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









