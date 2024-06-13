#ifndef LOG_H
#define LOG_H


#ifdef __cplusplus
/* C++ includes */
#include <stdint.h>
#else
/* C includes */
#include <inttypes.h>
#include <stdbool.h>
#endif

/*
 * structure used for tracking execution stop reasons
 *
 * If compiled with a C compiler, make sure that the C99 dialect or later is used.
 * (-std=c99 with a GNU C compiler)
 */
typedef enum {
    ENTER_IO,
    QUANTUM_EXPIRED,
    COMPLETED,
} ExecutionStopReasonType;

/**
 * @brief 
 * 
 * @param procID 
 * @param cpuExecutedTime 
 * @param ioExecutedTime 
 * @param totalElapsedTime 
 * @param stopReason 
 */
void log_cpuburst_execution (unsigned int procID,
                             unsigned int cpuExecutedTime,
                             unsigned int ioExecutedTime,
                             unsigned int totalElapsedTime,
                             ExecutionStopReasonType stopReason);

/**
 * @brief 
 * 
 * @param bursts - 1D array 
 */
void log_process_bursts (unsigned int bursts[], size_t numOfBursts);

/**
 * @brief 
 * 
 * @param procID 
 * @param completionTime 
 * @param totalWaitTime 
 */
void log_process_completion (unsigned int procID,
                             unsigned int completionTime,
        // wait time is the time spent in the ready queue
        // wait time = completionTime - total cpu bursts - total io bursts
                             unsigned int totalWaitTime);

#endif
