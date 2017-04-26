#ifndef _LOGFILE_
#define _LOGFILE_

#ifdef __cplusplus
extern "C"{
#endif 

#include <stdio.h>
#include <pthread.h>
    

typedef struct 
{
    const char* fileName;
    FILE* file;
    pthread_mutex_t mutex;
    int signal;
} log;



log* log_init(const char* fileName,int signalID);
void log_free(log* log);
void log_open(log* log);
void log_close(log* log);

void log_write(log* log,const char *format, ...);

#ifdef __cplusplus
}
#endif


#endif // _LOGFILE_
