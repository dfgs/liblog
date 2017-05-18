#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <stdarg.h>

LogFile* logDictionary[128] = {0}; 

static int nowToString(char* buffer,int size)
{
	time_t now;
	struct tm *tmp;
	//05/12/2017 07:10:14
	
	memset(buffer,0,size);
	now=time(NULL);
	tmp = localtime(&now);
	return strftime(buffer, size, "%m/%d/%Y %T",tmp);
}


static int nowToFormattedString(char* buffer,int size,const char* format)
{
	time_t now;
	struct tm *tmp;
	//05/12/2017 07:10:14
	
	memset(buffer,0,size);
	now=time(NULL);
	tmp = localtime(&now);
	return strftime(buffer, size, format,tmp);
}


static int log_newFile(LogFile* log)
{
    int result;
	char newFileName[1024];
	char now[20];
	
	if (log->file!=NULL) 
	{
		fclose(log->file);
		log->file=NULL;
		if (log->renameFile!=0)
		{
			nowToFormattedString(now,20,"%Y%m%d%M%H%S");
			strcpy(newFileName,log->fileName);
			strcat(newFileName,"-");
			strcat(newFileName,now);
			rename(log->fileName,newFileName);		
		}
	}
	
	log->file = fopen(log->fileName, "a");
	if (log->file == NULL)
	{
		perror("Failed to create log file");
		return -1;
	}
	else
	{
		result=setvbuf(log->file, NULL, _IOLBF, 0);
		if (result!=0)
		{
			perror("Failed to set log file buffer");
		return -1;
		}
	}
	return 0;
}

static void sig_handler(int signo)
{
	LogFile* log;
	
	log=logDictionary[signo];
	if (log==NULL) return;
	
	pthread_mutex_lock(&log->mutex);
	log_newFile(log);
	pthread_mutex_unlock(&log->mutex);
	
}







LogFile* log_open(const char* fileName,int signalID,int renameFile)
{
	LogFile* logFile;

	if (signalID>=128)
	{
        fprintf(stderr,"Invalid signal ID");
		return NULL;
	}
	
	if (logDictionary[signalID]!=NULL)
	{
		fprintf(stderr,"Signal ID already registered");
		return NULL;
	}
		
    logFile=malloc(sizeof(LogFile));
	logDictionary[signalID]=logFile;
		
	logFile->signal=signalID;
	logFile->renameFile=renameFile;
	logFile->fileName=fileName;
	logFile->file=NULL;
	if (log_newFile(logFile)!=0)
	{
		log_close(logFile);
		return NULL;
	}
		
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

void log_close(LogFile* log)
{
	logDictionary[log->signal]=NULL;
	if (log->file!=NULL) fclose(log->file);
	pthread_mutex_destroy(&log->mutex);
	signal(log->signal,SIG_DFL);
    free(log);
}




void log_write(LogFile* log,const char *format, ...)
{
	char now[128];
	va_list ap;

	if (log->file==NULL) return;
	
	pthread_mutex_lock(&log->mutex);
	
	nowToString(now,128);
	fprintf(log->file,"%s|",now);
	
	va_start(ap, format);
	vfprintf(log->file, format, ap);
	va_end(ap);
	
	fprintf(log->file,"\n");
	fflush(log->file);
 
	pthread_mutex_unlock(&log->mutex);

}

