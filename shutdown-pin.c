#include <stdio.h>
#include <strings.h>
#include <errno.h>
#include <sys/reboot.h>
#include <sys/types.h>
#include "GPIO.h"

#define DEFAULT_PIN  21
#define SLEEP_PERIOD 1
#define ALTER_REPEAT 10

int* gpio = NULL;                   
int falarm = 0;                     //False alarm
int persist = 1;                    //Persist GPIO mode
int spin = DEFAULT_PIN;             //specified pin

int isRoot() { return getuid() == 0; }

int atoi(char* num){
    int val = 0,i = 0;
    while(num[i] != '\0'){
        if(!('0' <= num[i] && num[i] <= '9'))  return -1;
        if(!(0 <= val && val <= 53))           return -1;
        val = val*10 + (num[i] - '0');
        i++;
    }
    return val;
}

int programFlags(int args,char** argv){
    for(int i = 1;i < args;i++){
        if(strcasecmp(argv[i],"--false-alarm") == 0)
            falarm = 1;
        else if(strcasecmp(argv[i],"--no-persist") == 0)
            persist = 0;
        else if(strcasecmp(argv[i],"--quiet") == 0)
            fclose(stdout);
        else if(strcasecmp(argv[i],"--pin") == 0 && args > ++i){
            if((spin = atoi(argv[i])) == -1 ){
                printf("Unknow specified pin %s\n",argv[i]);
                return 0;
            }
        }else{
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
    INPUT(pin);                 //treat spin as input port
    GPPUD = getTrigger(pin);    //enable pull resistor
    GPPUDCLK(pin,1);            //enable pull resistor in port spin
}

int main(int args, char** argv){
    setbuffer(stdin ,NULL,0);
    setbuffer(stdout,NULL,0);
    errno = 0;

    if(isRoot() == 0){
        printf("In order to shutdown device, you must to run this program with root permission. \n");
        return -1;
    }
    if(programFlags(args,argv) == 0){ 
        printf("Please remove incorrect flags or fix it.\n");
        return -2;
    }
    if((gpio = (int*)mappingIO()) == NULL){
        printf("Can not mapping process, please check for permission. errno code=%d\n",errno);
        return -3;
    }
    
    printf("using pin %d, give %s volt to pin %d, and wait for %d second to trigger shutdown\n",
            spin,
            (getTrigger(spin)) ? "HIGH" : "LOW",
            spin,
            SLEEP_PERIOD * ALTER_REPEAT
    );
    
    setGPIO(spin);
 
    int alter = ALTER_REPEAT;
    while(1){

        if(persist)
            setGPIO(spin);
        
        int trigger_on = (READ(spin) == getTrigger(spin));
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
