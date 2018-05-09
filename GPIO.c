#include "GPIO.h"

void* mappingIO(){
    int fd = open(MEMFILE, O_RDWR | O_SYNC);
    
    void* result = mmap(
         NULL,                          //Addr
         SIZE,                          //length
         PROT_READ | PROT_WRITE,        //prot
         MAP_SHARED,                    //flags
         fd,                            //file dscripter
         BASE_IO_PREI + BASE_GPIO       //offset
    );
    if(fd >= 0)     close(fd);
    
    return (result == MAP_FAILED) ? NULL : result;
}

int getPullResistor(int pin){
    if( 0<= pin && pin <= 8)    return PULL_RESISTOR_UP;
    if(34<= pin && pin <=36)    return PULL_RESISTOR_UP;
    if(46<= pin && pin <=53)    return PULL_RESISTOR_UP;
    return PULL_RESISTOR_DOWN;
}
