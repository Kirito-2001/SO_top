#include "procInfo.h"
#include <errno.h>



/* https://stackoverflow.com/questions/39066998/what-are-the-meaning-of-values-at-proc-pid-stat 
    https://serverfault.com/questions/908459/what-is-the-unit-of-a-page-in-proc-pid-statm       */

 void get_memoryinfo(MemoryInfo *mem) {
     ssize_t read, len = 0;
     char *line;
     // check if mem have been initialized
     if(mem == NULL){ 
        perror("ERROR on get_memoryinfo: memory not initialized"); 
        exit(EXIT_FAILURE); 
        }
     // open meminfo file; check for errors in fopen
     FILE *fp = fopen("/proc/meminfo", "r");
     if(!fp){ 
        perror("ERROR on get_memoryinfo: fopen /proc/meminfo file"); 
        exit(EXIT_FAILURE); 
        } 
     // get values and side effect on mem
     while((read = getline(&line, (long unsigned int*) &len, fp)) != -1) {
         sscanf(line, "MemTotal: %ld kB", &mem->memTotal);
     }
     // close meminfo file */
     if(fclose(fp)){ 
        perror("ERROR on get_memoryinfo: fclose /proc/meminfo file"); 
        exit(EXIT_FAILURE); 
        }
 }



void get_uptime(double *uptime) {
    // open uptime file; check for errors in fopen 
    FILE *fp = fopen("/proc/uptime", "r");   
    if(!fp){ 
        perror("ERROR on get_uptime: fopen /proc/uptime file"); 
        exit(EXIT_FAILURE); 
        } 
    fscanf(fp, "%lf", uptime);
    // close file
    if(fclose(fp)){ 
        perror("ERROR on get_uptime: fclose /proc/uptime file"); 
        exit(EXIT_FAILURE); 
        }  
}




void get_processinfo(ProcessInfo *proc, int pid) {
    char filename[256];
    sprintf(filename, "/proc/%d/stat", pid);
    FILE *fp = fopen(filename, "r");
    if(!fp){ 
        perror("ERROR on get_processinfo: fopen /proc/[pid]/stat file"); 
        exit(EXIT_FAILURE); 
        }
                                                                                                                       
    fscanf(fp, "%d %s %c %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %lu %lu %lu %lu %ld %ld %*d %*d %lld %lu %ld ",
            &proc->pid, 
            proc->command, 
            &proc->state,
            &proc->utime,
            &proc->stime,
            &proc->cutime,
            &proc->cstime,
            &proc->priority,
            &proc->nice,            
            &proc->startime,
            &proc->virt,
            &proc->res); 
            
    if(fclose(fp)){
        perror("ERROR on get_processinfo: fclose /proc/[pid]/stat file"); 
        exit(EXIT_FAILURE); 
        }

    // write in filename the directory
    sprintf(filename,"/proc/%d/statm",pid);
    // open statm file
    fp = fopen(filename,"r");
    if (!fp){
        perror("ERROR on get_processinfo: fopen /proc/[pid]/statm file"); 
        exit(EXIT_FAILURE); 
    } 
    // get statm info
    fscanf(fp, "%*d %ld %ld %*d %*d %ld %*d",
        &proc->resident,
        &proc->shared,
        &proc->data);
    // close statm file    
    if(fclose(fp)){
        perror("ERROR on get_processinfo: fclose /proc/[pid]/statm file"); 
        exit(EXIT_FAILURE); 
        }
    
}

