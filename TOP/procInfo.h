#include <stdio.h>
#include <stdlib.h>



 typedef struct MemoryInfo {
     unsigned long memTotal;
 } MemoryInfo;


/* https://stackoverflow.com/questions/39066998/what-are-the-meaning-of-values-at-proc-pid-stat 
    https://serverfault.com/questions/908459/what-is-the-unit-of-a-page-in-proc-pid-statm       */
typedef struct ProcessInfo {
    unsigned pid;
    char command[20];
    char state;
    unsigned long utime;
    unsigned long stime;
    unsigned long cutime;
    unsigned long cstime;
    long long startime;
    long int priority;
    long int nice;
    unsigned long virt;
    long int res;
    //
    long int resident;
    long int shared;
    long int data;
} ProcessInfo;

void get_memoryinfo(MemoryInfo *mem);
void get_uptime(double *uptime);
void get_processinfo(ProcessInfo *proc, int pid);

/*
            (14) utime  %lu
                    Amount of time that this process has been scheduled
                    in user mode, measured in clock ticks (divide by
                    sysconf(_SC_CLK_TCK)).  This includes guest time,
                    guest_time (time spent running a virtual CPU, see
                    below), so that applications that are not aware of
                    the guest time field do not lose that time from
                    their calculations.

            (15) stime  %lu
                    Amount of time that this process has been scheduled
                    in kernel mode, measured in clock ticks (divide by
                    sysconf(_SC_CLK_TCK)).

            (16) cutime  %ld
                    Amount of time that this process's waited-for chil‐
                    dren have been scheduled in user mode, measured in
                    clock ticks (divide by sysconf(_SC_CLK_TCK)).  (See
                    also times(2).)  This includes guest time,
                    cguest_time (time spent running a virtual CPU, see
                    below).

            (17) cstime  %ld
                    Amount of time that this process's waited-for chil‐
                    dren have been scheduled in kernel mode, measured in
                    clock ticks (divide by sysconf(_SC_CLK_TCK)).

            (18) priority  %ld
                    (Explanation for Linux 2.6) For processes running a
                    real-time scheduling policy (policy below; see
                    sched_setscheduler(2)), this is the negated schedul‐
                    ing priority, minus one; that is, a number in the
                    range -2 to -100, corresponding to real-time priori‐
                    ties 1 to 99.  For processes running under a non-
                    real-time scheduling policy, this is the raw nice
                    value (setpriority(2)) as represented in the kernel.
                    The kernel stores nice values as numbers in the
                    range 0 (high) to 39 (low), corresponding to the
                    user-visible nice range of -20 to 19.

                    Before Linux 2.6, this was a scaled value based on
                    the scheduler weighting given to this process.

            (19) nice  %ld
                    The nice value (see setpriority(2)), a value in the
                    range 19 (low priority) to -20 (high priority).

            (22) starttime  %llu
                    The time the process started after system boot.  In
                    kernels before Linux 2.6, this value was expressed
                    in jiffies.  Since Linux 2.6, the value is expressed
                    in clock ticks (divide by sysconf(_SC_CLK_TCK)).

                    The format for this field was %lu before Linux 2.6.


            resident   (2) resident set size
                        (same as VmRSS in /proc/[pid]/status)
            
            share      (3) shared pages (i.e., backed by a file)

            data       (6) data + stack



*/