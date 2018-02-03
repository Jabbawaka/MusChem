#include "GLogger/GLogger.h"

#include <stdio.h>
#include <stdarg.h>


#ifdef __cplusplus
extern "C"
{
#endif


FILE *p_logFile;
FILE *p_errFile;


void GLog_init()
{
    p_logFile = fopen("logs/fullLog.txt", "w");
    p_errFile = fopen("logs/errLog.txt", "w");
}

void  GLog_printMsg
   (char const  *funcName,
    char const  *fileName,
    int          line,
    int          logType,
    char const  *printMsg,
    ...)
{
    /* ---- DECLARATION ---- */
    /* Variable list of arguments */
    va_list args;

    /* Prefix buffer */
    char prefixBuffer[4096];

    /* Message buffer */
    char msgBuffer[4096];

    /* ---- LOG PREFIXES ---- */
    switch(logType)
    {
        case GLOG_TYPE_NORMAL:
            sprintf(prefixBuffer, "[---] %s: ",
                funcName);
            break;
        case GLOG_TYPE_LOAD:
            sprintf(prefixBuffer, "[LDF] %s: ",
                funcName);
            break;
        case GLOG_TYPE_WARNING:
            sprintf(prefixBuffer, "[WRN] %s: ",
                funcName);
            break;
        case GLOG_TYPE_ERROR:
            sprintf(prefixBuffer, "[ERR] %s(%s-%d): ",
                funcName, fileName, line);
            break;
        default:
            sprintf(prefixBuffer, "[WRN] BAD LOGGING TYPE: ");
    }

    /* ---- PROCESS ACTUAL MESSAGE AND LOG IT ---- */
    /* Process variable arguments */
    va_start(args, printMsg);
    vsprintf(msgBuffer, printMsg, args);
    va_end(args);

    /* Log final message and flush the buffer */
    printf("%s%s\n", prefixBuffer, msgBuffer);
    fflush(stdout);

    if(p_logFile != NULL)
    {
        fprintf(p_logFile, "%s%s\n", prefixBuffer, msgBuffer);
    }
    if(logType == GLOG_TYPE_WARNING || logType == GLOG_TYPE_ERROR)
    {
        if(p_errFile != NULL)
        {
            fprintf(p_errFile, "%s%s\n", prefixBuffer, msgBuffer);
        }
    }
}

void GLog_close()
{
    if(p_logFile != NULL)
    {
        fclose(p_logFile);
    }
    if(p_errFile != NULL)
    {
        fclose(p_errFile);
    }
}

#ifdef __cplusplus
}
#endif
