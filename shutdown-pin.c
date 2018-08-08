#include <stdio.h>
#include <strings.h>
#include <errno.h>
#include <unistd.h>
#include <sys/reboot.h>
#include <sys/types.h>
#include <time.h>
#include "GPIO.h"

extern void __attribute__ ((__noreturn__)) exit(int);

#define LOG_FILE     "/var/log/shutdown-pin"
#define DEFAULT_PIN  21
#define SLEEP_PERIOD 1
#define ALTER_REPEAT 10

volatile int* gpio = NULL;                   
int nodaemon = 0;                   //Don't run this program as daemon
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

void __attribute__ ((__noreturn__)) help(){
    char* help_this_poor_guy = 
        "Shutdown pin - A hardware way to shut your pie down :(\n"
        "This is especially useful when you are blocking outside your pie, and you don't want to remove the power(Might harm your pie)\n"
        "\n[Flags]\n"
        "--false-alarm : Output a string to log instead of shut your pie down when triggered.\n"
        "--no-persist : Avoid other badass program change your GPIO setting.\n"
        "--quiet : Close standard output,mute this program\n"
        "--no-daemon : Don't run program like a daemon\n"
        "--pin <number> : Specify the pin number as shutdown trigger\n"
        "--help, -h : Show this messages\n"
        "\n[Log]\n"
        "The Location of Log File => "LOG_FILE"\n"
        "\n[Notice]\n"
        "This programn need the permission to halt the device, please give this little program permission :D\n"
        "\n"
        "Made by Garyparrot :))))\n";
    printf("%s",help_this_poor_guy);
    exit(0);
}

int programFlags(int args,char** argv){
    for(int i = 1;i < args;i++){
        if(strcasecmp(argv[i],"--false-alarm") == 0)
            falarm = 1;
        else if(strcasecmp(argv[i],"--no-persist") == 0)
            persist = 0;
        else if(strcasecmp(argv[i],"--quiet") == 0)
            fclose(stdout);
        else if(strcasecmp(argv[i],"--no-daemon") == 0)
            nodaemon = 1;
        else if(strcasecmp(argv[i],"--pin") == 0 && args > ++i){
            if((spin = atoi(argv[i])) == -1 ){
                printf("Unknow specified pin %s\n",argv[i]);
                return 0;
            }
        }else if(strcasecmp(argv[i],"--help") == 0 || strcasecmp(argv[i],"-h") == 0){
            help();
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

void gtime(char* buffer,size_t size){
    time_t cur_time;
    struct tm * time_info;
    
    time(&cur_time);
    time_info = localtime(&cur_time);
    strftime(buffer, size, "%H:%M:%S", time_info);
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

    if(programFlags(args,argv) == 0){ 
        printf("Please remove incorrect flags or fix it.\n");
        return -2;
    }
    if(isRoot() == 0){
        printf("In order to shutdown device, you must to run this program with root permission. \n");
        return -1;
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
    if(nodaemon == 0 && daemon(0,1) == -1){
        printf("Failed to become daemon, errno code = %d\n",errno);
        return -5;
    }
    if(freopen(LOG_FILE,"a+",stdout) == NULL){
        printf("Failed to open log file %s, errno code = %d\n",LOG_FILE,errno);
        return -6;
    }
    setGPIO(spin);
    
    int alter = ALTER_REPEAT;
    while(1){

        if(persist)
            setGPIO(spin);
        
        int trigger_on = (READ(spin) == getTrigger(spin));
        alter = (trigger_on) ? (alter-1) : (ALTER_REPEAT);

        if(alter == 0){
            char str[256];
            gtime(str,sizeof(str));
            printf("[%s]%s\n",str, (!falarm) ? "Shutdown" : "False alarm!");
            fflush(stdout);

            if(!falarm)
                sync(),reboot(RB_POWER_OFF);
        }

        sleep(SLEEP_PERIOD);
    }
}

