#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "Adelson_RT.h"

#define TIME_SLICE 2 // Preemptive time quantum

// --- 4. THE DISPATCHER LOOP ---
void logEvent(uint32_t pid, int priority, int remaining, const char* action) {
    printf("[PID: %04d | Prio: %d | Time Left: %2d] -> %s\n", pid, priority, remaining, action);
}
int main() {
    printf("=== ADELSON-RT KERNEL BOOT SEQUENCE ===\n\n");
    Node* ReadyQueue = NULL;

    // Load initial processes
    ReadyQueue = insertProcess(ReadyQueue, createProcess(1001, 2, 5)); // Mid priority
    ReadyQueue = insertProcess(ReadyQueue, createProcess(1002, 0, 3)); // HIGH priority
    ReadyQueue = insertProcess(ReadyQueue, createProcess(1003, 5, 8)); // Low priority
    ReadyQueue = insertProcess(ReadyQueue, createProcess(1004, 2, 4)); // Mid priority (Tie-breaker test)

    printf("Bootstrapping processes... Beginning Dispatcher Loop.\n");
    printf("----------------------------------------------------\n");

    int system_clock = 0;

    // The CPU Cycle
    while (ReadyQueue != NULL) {
        process_metadata_t* current_process = NULL;
        
        // Context Switch: Pull highest priority task
        ReadyQueue = extractMinProcess(ReadyQueue, &current_process);
        current_process->state = INPROGRESS;
        
        logEvent(current_process->processid, current_process->priority, 
                 current_process->time_remaining, "DISPATCHED (Running)");

        // Execution Phase 
        int run_time = (current_process->time_remaining < TIME_SLICE) ? 
                        current_process->time_remaining : TIME_SLICE;
        
        usleep(run_time * 250000); // 0.25 sec per tick so you can watch it run
        system_clock += run_time;
        current_process->time_remaining -= run_time;

        // Evaluation Phase
        if (current_process->time_remaining <= 0) {
            current_process->state = COMPLETED;
            logEvent(current_process->processid, current_process->priority, 
                     0, "TERMINATED (Task Complete)\n");
            free(current_process); 
        } else {
            current_process->state = STALLED;
            logEvent(current_process->processid, current_process->priority, 
                     current_process->time_remaining, "PREEMPTED (Re-entering Queue)\n");
            
            ReadyQueue = insertProcess(ReadyQueue, current_process);
        }
    }

    printf("----------------------------------------------------\n");
    printf("SYSTEM HALTED. Total Clock Ticks: %d. Ready Queue Empty.\n", system_clock);
    return 0;
}