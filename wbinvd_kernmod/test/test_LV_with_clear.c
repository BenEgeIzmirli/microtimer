#include <fcntl.h>      /* open */ 
#include <unistd.h>     /* exit */
#include <sys/ioctl.h>  /* ioctl */
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

FILE* file_desc;

int MAJOR_NUM;

/* Get the message of the device driver */
#define IOCTL_GET_MSG _IOR(MAJOR_NUM, 1, char *)
 /* This IOCTL is used for output, to get the message 
  * of the device driver. However, we still need the 
  * buffer to place the message in to be input, 
  * as it is allocated by the process.
  */

#define CLEAR_CACHE \
file_desc = fopen("/dev/wbinvd","rb"); \
if ( file_desc == NULL ) { \
    printf("Can't find device /dev/wbinvd\n"); \
    exit(-1); \
} \
fclose(file_desc); \
asm volatile("MFENCE");

#define L3_SIZE 1024*1024*6U

#define LINE_SIZE 64

static char DATA[L3_SIZE];

int main() {
    char ret=0;
    for ( int i=0 ; i < LINE_SIZE ; i++ ) {
        CLEAR_CACHE
        for ( int i=0 ; i < L3_SIZE ; i += LINE_SIZE ) {
            ret ^= DATA[i];
        }
    }
    //CLEAR_CACHE
    return ret;
}












