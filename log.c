#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <stdarg.h>

log* logDictionary[128] = {0}; 


static void log_newFile(log* log)
{
    int result;
 
	if (log->file!=NULL) 
	{
		fclose(log->file);
		log->file=NULL;
	}
	
	log->file = fopen(log->fileName, "a");
	if (log->file == NULL)
	{
		perror("Failed to create log file");
	}
	else
	{
		result=setvbuf(log->file, NULL, _IOLBF, 0);
		if (result!=0)
		{
			perror("Failed to set log file buffer");
		}
	}
}

static void sig_handler(int signo)
{
	log* log;
	
	log=logDictionary[signo];
	if (log==NULL) return;
	
	pthread_mutex_lock(&log->mutex);
	log_newFile(log);
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


log* log_open(const char* fileName,int signalID)
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
	
	
    logFile=malloc(sizeof(log));
	logDictionary[signalID]=logFile;
	
	
	logFile->signal=signalID;
	logFile->fileName=fileName;
	log_newFile(logFile);
	
	
	if (pthread_mutex_init(&logFile->mutex,NULL)!=0)
	{
        perror("Cannot initialize mutex");
		log_close(logFile);
        return NULL;
	}

 	if (signal(signalID, sig_handler) == SIG_ERR)
	{
		perror("Failed to register signal");
		log_close(logFile);
		return NULL;
	}
	
	
	return logFile;
}

void log_close(log* log)
{
	logDictionary[log->signal]=NULL;
	if (log->file!=NULL) fclose(log->file);
	pthread_mutex_destroy(&log->mutex);
	signal(log->signal,SIG_DFL);
    free(log);
}




void log_write(log* log,const char *format, ...)
{
	char buffer[128];
	va_list ap;

	if (log->file==NULL) return;
	
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

