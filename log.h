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
	int renameFile;
} LogFile;



LogFile* log_open(const char* fileName,int signalID,int renameFile);
void log_close(LogFile* log);

void log_write(LogFile* log,const char *format, ...);

#ifdef __cplusplus
}
#endif


#endif // _LOGFILE_
