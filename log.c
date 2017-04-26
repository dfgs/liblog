#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>

log* logDictionary[128]; 

static void sig_handler(int signo)
{
	log* log;
	
	log=logDictionary[signo];
	if (log==NULL) return;
	
	//log_write(log,  __func__,"NA",DEBUG,"Received signal %i",signo);
	pthread_mutex_lock(&log->mutex);
	log_close(log);
	log_open(log);
	pthread_mutex_unlock(&log->mutex);
	
}

static void nowToString(char* Buffer)
{
	time_t now;
	struct tm *tmp;
	
	now=time(NULL);
	tmp = localtime(&now);
	strftime(Buffer, 128, "%m/%d/%Y %T",tmp);
}

static log* log_createLog()
{
    log* logFile;

    logFile=malloc(sizeof(log));

    return logFile;
}




log* log_init(const char* fileName,int signalID)
{
	log* logFile;

	if (signalID>=128)
	{
        perror("Invalid signal ID");
		return NULL;
	}
	
	if (logDictionary[signalID]!=NULL)
	{
		perror("Signal ID already registered");
		return NULL;
	}
	
	
    logFile=log_createLog();
	logDictionary[signalID]=logFile;
	
	
	logFile->signal=signalID;
	logFile->fileName=fileName;
	
	logFile->file = fopen(fileName, "w");
    if (logFile->file==NULL)
    {
        perror("Cannot open log file");
		log_free(logFile);
        return NULL;
    }
	
	if (pthread_mutex_init(&logFile->mutex,NULL)!=0)
	{
        perror("Cannot initialize mutex");
		log_free(logFile);
        return NULL;
	}

 	if (signal(signalID, sig_handler) == SIG_ERR)
	{
		perror("Failed to register signal");
		log_free(logFile);
		exit(EXIT_FAILURE);
	}
	
	

}

void log_free(log* log)
{
	logDictionary[log->signal]=NULL;
	if (log->file!=NULL) fclose(log->file);
	pthread_mutex_destroy(&log->mutex);
	signal(log->signal,SIG_DFL);
    free(log);
}

void log_open(log* log)
{
    int result;
 
	log->file = fopen(log->fileName, "a");
	if (log->file == NULL)
	{
		perror("Failed to create log file");
	}
 	result=setvbuf(log->file, NULL, _IOLBF, 0);
    if (result!=0)
    {
 		perror("Failed to set log file buffer");
	}
}
void log_close(log* log)
{
	fclose(log->file);
    //renameLog();
}


void log_write(log* log,const char *format, ...)
{
	char buffer[128];
	va_list ap;

	pthread_mutex_lock(&log->mutex);
	
	nowToString(buffer);
	fprintf(log->file,"%s|",buffer);

	va_start(ap, format);
	vfprintf(log->file, format, ap);
	va_end(ap);
	
	fprintf(log->file,"\n");
	fflush(log->file);
 
	pthread_mutex_unlock(&log->mutex);

}

