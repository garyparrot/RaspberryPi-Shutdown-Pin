#include<sys/mman.h>
#include<sys/fcntl.h>
#include<stdio.h>
#include<sys/reboot.h>
#include<unistd.h>
#include<strings.h>
#include<errno.h>

#define BASE_IO_PREI 0x3F000000
#define BASE_GPIO    0x200000
#define SIZE         0xB0
#define MEMFILE      ("/dev/gpiomem")
#define S_INPUT      (21)
#define SLEEP_PERIOD (5)
#define ALTER_REPEAT (2)

#define INPUT(g) ( *(gpio + (g)/10) & ~(7 << 3*((g)%10)) )
#define READ(g) (( *(gpio + (0x34 >> 2) + (g)/32) >> (((g)%32))) & 1)

#define GPPUD ( *(gpio + (0x94 >> 2)) )
#define xGPPUDCLK(g) ( *(gpio + (0x98 >> 2) + (g)/32) ) 
#define GPPUDCLK(g,s) ( (s==1) ? ( xGPPUDCLK(g) |= (1 << (g)%32) ) : ( xGPPUDCLK(g) &= ~(1 << (g)%32) ) )

#define PULL_RESISTOR_DISABLE 0x00
#define PULL_RESISTOR_DOWN 0x01
#define PULL_RESISTOR_UP 0x02

int falarm = 0;             //Determine false alarm is enabled.
int alter = ALTER_REPEAT;   //A timeout for shutdown raspberry pi.

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

int main(int args, char** argv){
    errno = 0;
    setbuffer(stdin,NULL,0);
    setbuffer(stdout,NULL,0);

    for(int i = 1;i < args;i++){
        if(strcasecmp(argv[i],"--false-alarm") == 0)
            falarm = 1;
        else if(strcasecmp(argv[i],"--alter") == 0)
            ;      //Not implemented               
        else{
            printf("Unknow flags %s\n",argv[i]);
            return -2;
        }
    }
    
    int* gpio = (int*)mappingIO();
    if(gpio == NULL){
        printf("Can't mapping process, please check for permission, error code = %d. \n",errno);
        return -1;
    }

    //setup port
    INPUT(S_INPUT);                 //treat S_INPUT as input port
    GPPUD = PULL_RESISTOR_DOWN;     //enable pull resistor
    GPPUDCLK(S_INPUT,1);            //enable pull resistor in port S_INPUT

    //Detecting S_INPUT voltage level
    int alter = ALTER_REPEAT;
    while(1){

        if(READ(S_INPUT) && alter > 0)  
            alter-=1;
        else           
            alter = ALTER_REPEAT;
        
        if(0 >= alter){
            
            sync();
            
            if(falarm){     
                printf("False alarm!\n");
            }else if( -1 == reboot(RB_POWER_OFF) ){
                printf("Can't reboot, errno code = %d\n",errno);
                return -3;
            }               
        }

        sleep(SLEEP_PERIOD);
    }
}
