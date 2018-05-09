#ifndef HEADER_GPIO_H

#define HEADER_GPIO_H
#include<sys/mman.h>
#include<sys/fcntl.h>
#include<unistd.h>
#include<sys/types.h>

#define BASE_IO_PREI 0x3F000000
#define BASE_GPIO    0x200000
#define SIZE         0xB0
#define MEMFILE      "/dev/gpiomem"

#define INPUT(g) ( *(gpio + (g)/10) & ~(7 << 3*((g)%10)) )
#define READ(g)  (( *(gpio + 13 + (g)/32) >> ((g)%32) ) & 1)

#define GPPUD           (*(gpio + 37))
#define xGPPUDCLK(g)    (*(gpio + 38 + (g)/32)) 
#define GPPUDCLK(g,s)   ( (s==1) ? ( xGPPUDCLK(g) |= (1 << (g)%32) ) : ( xGPPUDCLK(g) &= ~(1 << (g)%32) ) )

#define PULL_RESISTOR_DISABLE 0x00
#define PULL_RESISTOR_DOWN    0x01
#define PULL_RESISTOR_UP      0x02

extern int* gpio;

void* mappingIO(void);
int getPullResistor(int);

#endif
