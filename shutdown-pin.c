#include <stdio.h>
#include <strings.h>
#include <errno.h>
#include <sys/reboot.h>
#include <sys/types.h>
#include "GPIO.h"

#define S_INPUT      21
#define SLEEP_PERIOD 1
#define ALTER_REPEAT 10

int* gpio = NULL;                   
int falarm = 0;                     //False alarm
int persist = 1;                    //Persist GPIO mode

int isRoot() { return getuid() == 0; }

int programFlags(int args,char** argv){
    for(int i = 1;i < args;i++){
        if(strcasecmp(argv[i],"--false-alarm") == 0)
            falarm = 1;
        else if(strcasecmp(argv[i],"--no-persist") == 0)
            persist = 0;
        else{
            printf("Unknow flags %s\n",argv[i]);
            return 0;
        }
    }
    return 1;
}

int getTrigger(int pin){
    switch(getPullResistor(pin)){
        case PULL_RESISTOR_UP:      return 0;
        case PULL_RESISTOR_DOWN:    return 1;
        default:                    return -1;
    }
}

void setGPIO(int pin){
    INPUT(pin);                 //treat S_INPUT as input port
    GPPUD = getTrigger(pin);    //enable pull resistor
    GPPUDCLK(pin,1);            //enable pull resistor in port S_INPUT
}

int main(int args, char** argv){
    setbuffer(stdin ,NULL,0);
    setbuffer(stdout,NULL,0);
    errno = 0;

    if(isRoot() == 0){
        printf("In order to shutdown device, you must to run this program with root permission. ");
        return -1;
    }
    if(programFlags(args,argv) == 0){ 
        printf("Please remove incorrect flags.");
        return -2;
    }
    if((gpio = (int*)mappingIO()) == NULL){
        printf("Can not mapping process, please check for permission. errno code=%d",errno);
        return -3;
    }
    setGPIO(S_INPUT);
 
    int alter = ALTER_REPEAT;
    while(1){

        if(persist)
            setGPIO(S_INPUT);
        
        int trigger_on = (READ(S_INPUT) == getTrigger(S_INPUT));
        alter = (trigger_on) ? (alter-1) : (ALTER_REPEAT);

        if(alter <= 0){
            if(falarm)
                printf("False alarm!\n");
            else
                sync(),reboot(RB_POWER_OFF);
        }

        sleep(SLEEP_PERIOD);
    }
}
