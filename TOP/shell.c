#include "procInfo.c"

#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <bits/signum.h>



#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>



/*** costanti utili ***/
#define NUMCMD 6
#define LENCMD 128
#define LENPATH 128


/***** proc->state ****/
#define RUNNING 82
#define STOPPED 84
#define SLEEPING 83


/*** colori per shell ***/
#define RED     "\033[0;31m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[0;33m"
#define CYAN    "\033[0;36m"
#define WHITE   "\033[0;37m"



struct dirent **namelist;
int len_namelist;
char *color;




// //prendo solo i d_name con numeri (in quanto processi)
 int filterize(const struct dirent *dir) {
       return !fnmatch("[1-9]*", dir->d_name, 0);
 }





int dimensione = 0;
int *array;
ProcessInfo *proc;




char* cmd[NUMCMD]= {
    "interrupt",
    "kill",
    "stop",
    "resume",
    "wakeup",
    "exit"
};







int * array_stopped;
int * array_running;
int * array_sleeping;

int cont_run = 0, cont_sleep = 0, cont_stop = 0;

int free_block_run, free_block_sleep, free_block_stop;





/*************************************** PRINT TOP *********************************************************/

void print_top() {

    time_t rawtime;
    time(&rawtime);    
    struct tm *time = localtime(&rawtime);

    
    double uptime;
    get_uptime(&uptime);
    
    printf("\n%s**********************************************************************************************************************************************************************************************\n", WHITE);
    
    printf("\n\n\n\t\t\t\ttop - %.2d:%.2d:%.2d\n", 
            time->tm_hour, time->tm_min, time->tm_sec);
    

   
    MemoryInfo *memory = (MemoryInfo*)malloc(sizeof(MemoryInfo));
    get_memoryinfo(memory);
    
    //CICLO FOR SUI PID
    color = CYAN;
    printf("\n\n -------------------------------------------------------------- TABELLA PROCESSI ATTIVI ---------------------------------------------------------------------------------------------\n\n");
    printf("\t%s  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------",color);
    printf("\n\t%s |   PID \t USER     PR\t  NI  \t  VIRT \t\t     RES\t\tSHM  \t STATE      %%CPU \t    %%MEM  \t   TIME\t         COMMAND\t\t|\n",color);
    printf("\t%s ------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n",color);
    
    proc = (ProcessInfo*)malloc(sizeof(ProcessInfo));
    
    //prendo dalla /proc solo numeri (--> processi)
    int n = scandir("/proc", &namelist, filterize, alphasort);
    len_namelist = n;
    
    //setto array di visibilità
    array_running = (int*)malloc(len_namelist*sizeof(int));
    array_sleeping = (int*)malloc(len_namelist*sizeof(int));
    array_stopped = (int*)malloc(len_namelist*sizeof(int));

    int i = 0;

    cont_run = 0; 
    cont_sleep = 0;
    cont_stop = 0;
    while(n--) {  
        get_processinfo(proc, atoi(namelist[n]->d_name));

        int time = (proc->utime + proc->stime) / 100;
        //char *color;
        color = WHITE;
        if (proc->state == RUNNING) { 
            array_running[cont_run] = proc->pid;
            color = GREEN;
            cont_run++;
        }
        else if (proc->state == SLEEPING){ 
            array_sleeping[cont_sleep] = proc->pid;
            color = YELLOW;
            cont_sleep++;
        }
        else if (proc->state == STOPPED){ 
            color = RED;
            array_stopped[cont_stop] = proc->pid;
            cont_stop++;
        }                        
                         //pid      //pr      //ni  //virt    //res  //shr //%c  //%cpu   //%mem   //%time    //command
       printf("%s \t  %6d \t user \t %3ld \t %2ld \t %8lu \t %8ld \t %8ld \t %2c \t %6f \t %6f \t %3d:%d:%d \t %-5s\e[0m\n",
                color,
                proc->pid,
                proc->priority,
                proc->nice,
                proc->virt / (10),
                proc->res,
                proc->shared / (10),
                proc->state,
                (float)  ((proc->utime + proc->stime) / (uptime - (proc->startime / 100))),
                (float) ((proc->resident + proc->data) * 100) / (memory->memTotal),
                time/3600, (time/60)%60, time%60,
                proc->command); 
        i++;
    }

    printf("\t%s-----------------------------------------------------------------------------------------------------------------------------------------------------------------\n",YELLOW);
    color = WHITE;
    printf("%s\n\n---------------------------------\n numero di processi attivi: %d |\n",WHITE,i);
    printf("%s--------------------------------------------------------------------FINE TABELLA---------------------------------------------------------------------------------------------------\n\n",color);


    
    

    free_block_run = cont_run +1;
    free_block_sleep = cont_sleep +1;
    free_block_stop = cont_stop+1;


    free(proc);    
    free(memory);
}


/*******************************************************************************************************************************/










/*********************************** funzioni utili riutilizzabili ******************************************************/

void print_command(){
    printf("%s\n\n\n\n\n\n\n\n --------> hai a disposizione i seguenti comandi per operare tra processi:\n\n\t",WHITE);
    for(int j = 0; j<NUMCMD;j++){
        if(j==0){
            printf("%d) %s: per interrompere un processo\n\n\t",j+1,cmd[j]);
        }
        else if(j==1){
            printf("%d) %s: per killare un processo\n\n\t",j+1,cmd[j]);
        }
        else if(j==2){
            printf("%d) %s: per stoppare un processo attivo\n\n\t",j+1,cmd[j]);
        }
        else if(j==3){
            printf("%d) %s: per riprendere un processo stoppato\n\n\t",j+1,cmd[j]);
        }
        else if(j==4){
            printf("%d) %s: per svegliare un processo dormiente\n\n\t",j+1,cmd[j]);
        }
        else{
            printf("%d) %s (or CTL+C): per uscire dalla top\n\n\t",j+1,cmd[j]);
        }
    }
    
}









//3 funzioni che stampano le liste dei tre tipi di processi di interesse

void print_proc_running(int len){
    printf("\n\n%s********************************************************************************************************************",CYAN);
    printf("\n%si pid dei processi running sono:  %s", WHITE,GREEN);
    for(int z = 0; z<len;z++){
        if(z%10 == 0 && z!= 0){
            printf("\n\t\t\t\t  | %s%d |",GREEN,array_running[z]);
        }
        else{ 
            printf("| %s%d |",GREEN,array_running[z]);
        }
    }
    free(array_running);

}

void print_proc_sleep(int len){
    printf("\n\n%si pid dei processi sleeping sono: %s",WHITE,YELLOW);
    for(int z = 0; z<len;z++){
        if(z%10 == 0 && z!= 0){
            printf("\n\t\t\t\t  | %s%d |", YELLOW,array_sleeping[z]);

        }        
        else{printf("| %s%d |", YELLOW,array_sleeping[z]);}
    }
    free(array_sleeping);
}

void print_proc_stop(int len){
    printf("\n\n%si pid dei processi stopped sono:  %s",WHITE,RED);
    for(int z = 0; z<len;z++){
        if(z%10 == 0 && z!= 0){
            printf("\n\t\t\t\t  | %s%d |",RED, array_stopped[z]);

        }
        else{printf("| %s%d |",RED, array_stopped[z]);}
    }
    printf("\n%s*************************************************************************************************************************\n", CYAN);
    free(array_stopped);

}


/*************************************************************************************************************************/















/********************************************  MAIN  *********************************************************************/
int cont = 0;//per far ripartire tutto dall'else dopo aver inserito comandi (e quindi non stampare all'infinito la top)
int print=1;
int main(int argc, char**argv) {
    while(1){
        if(argc == 1 && cont == 0){
            while(1) {
                if(print) {
                    print = 0;
                    print_top();
                    print_proc_running(cont_run);
                    print_proc_sleep(cont_sleep);
                    print_proc_stop(cont_stop);

                } 
                cont++;
                break;
            }
        }
        else if(argc >1){
              printf("invalid syntax, u don't need parameters. just write './shell'\n"); 
              break;
        }
        else if(cont != 0){
            print_command();
            
            //break;

            char user_cmd[LENCMD]={0};

            char* path=malloc(LENPATH);
            strcpy(path, "TOP/:"); //path

            
            printf("\n%s ",path);

            while(1){

                fflush(stdin);
                fgets(user_cmd, LENCMD, stdin);

                //remove new line
                if ((strlen(user_cmd) > 0) && (user_cmd[strlen (user_cmd) - 1] == '\n')){
                    user_cmd[strlen (user_cmd) - 1] = '\0';
                }

                char* CMD;
                char* ARG;

                CMD=strtok(user_cmd," ");
                ARG=strtok(NULL," ");
                
                
              



                //VARI COMANDI INSERITI
                
                //printf("\n%s ",path);

                if(CMD){
                    int k = 0,pid = 0,found =0;

                    //CASO EXIT
                    if(strcmp(CMD,"exit") == 0){
                        cont = 0;
                        printf("\n%s**********************************************************************************************************************************************************************************************\n", WHITE);
                        break; //esco dal while
                    }
    






                     //CASO INTERRUPT
                     else if (strcmp(CMD,"interrupt") == 0){
                        
                        //found =0;
                        if(!ARG){
                            printf("sintax error, digit: interrupt <pid_proc>");
                            printf("\n%s ",path);

                        }
                        else{
                            pid = atoi(ARG);
                             while(1){
                                
                                    while(k<len_namelist) {
                                        if (pid == atoi(namelist[k]->d_name)){
                                            found = 1;
                                            get_processinfo(proc, atoi(namelist[k]->d_name));
                                            break;
                                        }
                                        k++;
                                    }
                                    if(proc->state==RUNNING && pid == proc->pid){
                                       
                                        if(found == 0){
                                            printf("\nsintax error, pid non presente tra i pid dei processi attivi\n");
                                            printf("\n%s ",path);
                                            break;
                                        }
                                        else{

                                            if(kill((pid_t)atoi(namelist[k]->d_name),SIGINT)){
                                                //non faccio nulla perche CTRL+C
                                            }
                                            else{
                                                printf("\nerrore nel ritorno valore del pid: ret_value =%d .\n",kill((pid_t)atoi(namelist[k]->d_name),SIGINT)); 
                                                printf("\npid: %d not %s\n", atoi(ARG), strsignal(SIGINT));    
                                                printf("\n%s ",path);
                                                break;   
                                            }
                                            
                                        }
                                    }
                                
                                    else{
                                        printf("\nERROR: you have selected a NOT RUNNING process, try again\n");
                                        printf("\n%s ",path);
                                        break;
                                    }
                                

                            }   
                        }
                        //cont = 0;
                        
                    }








                    //CASO KILL
                    else if (strcmp(CMD,"kill") == 0){
                        
                        //found =0;
                        if(!ARG){
                            printf("sintax error, digit: kill <pid_proc>");
                            printf("\n%s ",path);

                        }
                        else{
                            pid = atoi(ARG);
                             while(1){
                                //check if pid is correct
                                    while(k<len_namelist) {
                                        if (pid == atoi(namelist[k]->d_name)){
                                            found = 1;
                                            get_processinfo(proc, atoi(namelist[k]->d_name));
                                        
                                            break;
                                        }
                                        k++;
                                    }
                                    if(proc->state==RUNNING && pid == proc->pid){
                                        if(found == 0){
                                            printf("\nsintax error, pid non presente tra i pid dei processi attivi\n");
                                            printf("\n%s ",path);
                                            break;
                                        }
                                        else{
                    
                                            if(kill((pid_t)atoi(namelist[k]->d_name),SIGKILL)){
                                                printf("\nho ucciso correttamente il processo con pid: %d\n\n", pid);
                                                //ristampo la lista dei processi
                                                sleep(3);
                                                print_top();
                                                print_proc_running(cont_run);
                                                print_proc_sleep(cont_sleep);
                                                print_proc_stop(cont_stop);
                                                print_command();
                                                printf("\n%s ",path);
                                                break;
                                                
                                            }
                                            else{
                                                printf("\nerrore nel ritorno valore del pid: ret_value =%d .\n",kill((pid_t)atoi(namelist[k]->d_name),SIGKILL)); 
                                                printf("\npid: %d not %s\n", atoi(ARG), strsignal(SIGKILL));    
                                                printf("\n%s ",path);
                                                break;   
                                            }
                                            
                                        }
                                    }
                                
                                    else{
                                        printf("\nERROR: you have selected a NOT RUNNING process, try again\n");
                                        printf("\n%s ",path);
                                        break;
                                    }
                                

                            }   
                        }
                        //cont = 0;
                        
                    }



                    
                    //CASO STOP
                    else if (strcmp(CMD,"stop") == 0){
                        
                        //found =0;
                        if(!ARG){
                            printf("sintax error, digit: stop <pid_proc>");
                            printf("\n%s ",path);

                        }
                        else{
                            pid = atoi(ARG);

                             while(1){
                                //check if pid is correct

                                    while(k<len_namelist) {
                                        if (pid == atoi(namelist[k]->d_name)){
                                            found = 1;
                                            get_processinfo(proc, atoi(namelist[k]->d_name));
                                            
                                            break;
                                        }
                                        k++;
                                    }
                                     
                                    if(proc->state==RUNNING && pid == proc->pid){
                                        
                                        if(found == 0){
                                            printf("\nsintax error, pid non presente tra i pid dei processi attivi\n");
                                            printf("\n%s ",path);
                                            break;
                                        }
                                        else{

                    
                                            if(kill((pid_t)atoi(namelist[k]->d_name),SIGSTOP)){
                                                printf("\nsto stoppando correttamente il processo con pid: %d", pid);
                                                printf("\n ... attendi ...\n");
                                                //ristampo la lista dei processi
                                                sleep(3);
                                                print_top();
                                                print_proc_running(cont_run);
                                                print_proc_sleep(cont_sleep);
                                                print_proc_stop(cont_stop);
                                                print_command();
                                                printf("\n%s ",path);
                                                break;
                                                
                                            }
                                            else{
                                                printf("\nerrore nel ritorno valore del pid: ret_value =%d .\n",kill((pid_t)atoi(namelist[k]->d_name),SIGSTOP)); 
                                                printf("\npid: %d not %s\n", atoi(ARG), strsignal(SIGCONT));    
                                                printf("\n%s ",path);
                                                break;   
                                            }
                                            
                                        }
                                    }
                                
                                    else{
                                        printf("\n\npid del processo  proc->pid :%d",proc->pid);
                                        printf("\tstato: %d",proc->state);
                                        printf("\nERROR: you have selected a NOT ACTIVE process, try again\n");
                                        printf("\n%s ",path);
                                        break;
                                    }
                                

                            }   
                        }
                        //cont = 0;
                        
                    }




                    //CASO RESUME
                    else if (strcmp(CMD,"resume") == 0){
                        
                        //found =0;
                        if(!ARG){
                            printf("sintax error, digit: resume <pid_proc>");
                            printf("\n%s ",path);

                        }
                        else{
                            pid = atoi(ARG);

                             while(1){
                                //check if pid is correct

                                    while(k<len_namelist) {
                                        if (pid == atoi(namelist[k]->d_name)){
                                            found = 1;
                                            get_processinfo(proc, atoi(namelist[k]->d_name));
                                            
                                            break;
                                        }
                                        k++;
                                    }
                                    if(proc->state==STOPPED && pid == proc->pid){
                                        
                                        if(found == 0){
                                            printf("\nsintax error, pid non presente tra i pid dei processi attivi\n");
                                            printf("\n%s ",path);
                                            break;
                                        }
                                        else{

                    
                                            if(kill((pid_t)atoi(namelist[k]->d_name),SIGCONT)){
                                                printf("\nsto riesumando correttamente il processo con pid: %d", pid);
                                                printf("\n ... attendi ...\n");
                                                //ristampo la lista dei processi
                                                sleep(3);
                                                print_top();
                                                print_proc_running(cont_run);
                                                print_proc_sleep(cont_sleep);
                                                print_proc_stop(cont_stop);
                                                print_command();
                                                printf("\n%s ",path);
                                                break;
                                                
                                            }
                                            else{
                                                printf("\nerrore nel ritorno valore del pid: ret_value =%d .\n",kill((pid_t)atoi(namelist[k]->d_name),SIGCONT)); 
                                                printf("\npid: %d not %s\n", atoi(ARG), strsignal(SIGCONT));    
                                                printf("\n%s ",path);
                                                break;   
                                            }
                                            
                                        }
                                    }
                                
                                    else{
                                        printf("\nERROR: you have selected a NOT STOPPED process, try again\n");
                                        printf("\n%s ",path);
                                        break;
                                    }
                                

                            }   
                        }
                        //cont = 0;
                        
                    }


                    



                    //CASO WAKEUP
                    else if (strcmp(CMD,"wakeup") == 0){
                        
                        //found =0;
                        if(!ARG){
                            printf("sintax error, digit: wakeup <pid_proc>");
                            printf("\n%s ",path);

                        }
                        else{
                            pid = atoi(ARG);
                             while(1){
                                //check if pid is correct

                                //if(proc->state==STOPPED){
                                    while(k<len_namelist) {
                                        if (pid == atoi(namelist[k]->d_name)){
                                            found = 1;
                                            get_processinfo(proc, atoi(namelist[k]->d_name));
                                            // printf("\n\npid del processo  proc->pid :%d",proc->pid);
                                            // printf("\tatoi(ARG): %d",atoi(ARG));
                                            // printf("\tpid: %d",pid);
                                            break;
                                        }
                                        k++;
                                    }
                                    if(proc->state==SLEEPING && pid == proc->pid){
                                        
                                        if(found == 0){
                                            printf("\nsintax error, pid non presente tra i pid dei processi attivi\n");
                                            printf("\n%s ",path);
                                            break;
                                        }
                                        else{

                                            if(kill((pid_t)atoi(namelist[k]->d_name),SIGUSR1)){
                                                printf("\nho svegliato correttamente il processo con pid: %d\n\n", pid);
                                                //ristampo la lista dei processi
                                                sleep(3);
                                                print_top();
                                                print_proc_running(cont_run);
                                                print_proc_sleep(cont_sleep);
                                                print_proc_stop(cont_stop);
                                                print_command();
                                                printf("\n%s ",path);
                                                break;
                                                
                                            }
                                            else{
                                                printf("\nerrore nel ritorno valore del pid: ret_value =%d .\n",kill((pid_t)atoi(namelist[k]->d_name),SIGUSR1)); 
                                                printf("\npid: %d not %s\n", atoi(ARG), strsignal(SIGUSR1));    
                                                printf("\n%s ",path);
                                                break;   
                                            }
                                            
                                        }
                                    }
                                
                                    else{
                                        printf("\nERROR: you have selected a NOT SLEEPING process, try again\n");
                                        printf("\n%s ",path);
                                        break;
                                    }
                                

                            }   
                        }
                        //cont = 0;
                        
                    }

        







                    //CASI NON COMPRESI
                    else{
                        printf("\t\tERROR: nessun comando presente.");
                        printf("\n\t\tristampo i processi in esecuzione:\n\n");
                        sleep(3);
                        print_top();
                        print_proc_running(cont_run);
                        print_proc_sleep(cont_sleep);
                        print_proc_stop(cont_stop);
                        printf("%s\n\n\n\n\n\n\n\n --------> hai a disposizione i seguenti comandi per operare tra processi:\n\n\t",WHITE);
                        for(int j = 0; j<NUMCMD;j++){
                            if(j==0){
                                printf("%d) %s: per interrompere un processo\n\n\t",j+1,cmd[j]);
                            }
                            else if(j==1){
                                printf("%d) %s: per killare un processo\n\n\t",j+1,cmd[j]);
                            }
                            else if(j==2){
                                printf("%d) %s: per stoppare un processo attivo\n\n\t",j+1,cmd[j]);
                            }
                            else if(j==3){
                                printf("%d) %s: per riprendere un processo stoppato\n\n\t",j+1,cmd[j]);
                            }
                            else if(j==4){
                                printf("%d) %s: per svegliare un processo dormiente\n\n\t",j+1,cmd[j]);
                            }
                            else{
                                printf("%d) %s (or CTL+C): per uscire dalla top\n\n\t",j+1,cmd[j]);
                            }
                        }
                        printf("\n%s ",path);
                    }
            
                }

            }
            break;
            free(path);
        

            
        }
        
        
  
    }
    return 0;
}
