#ifndef H_GENERIC_LOGGER_H
#define H_GENERIC_LOGGER_H


#ifdef __cplusplus
extern "C"
{
#endif


#include <stdio.h>


#define GLOG_TYPE_NORMAL (0)
#define GLOG_TYPE_LOAD (1)
#define GLOG_TYPE_WARNING (2)
#define GLOG_TYPE_ERROR (3)


#define GLOG_MSG(printMsg, ...) (GLog_printMsg(__FUNCTION__,__FILE__,__LINE__, GLOG_TYPE_NORMAL,printMsg, ##__VA_ARGS__))
#define GLOG_LOAD(printMsg, ...) (GLog_printMsg(__FUNCTION__,__FILE__,__LINE__, GLOG_TYPE_LOAD,printMsg, ##__VA_ARGS__))
#define GLOG_WRN(printMsg, ...) (GLog_printMsg(__FUNCTION__,__FILE__,__LINE__, GLOG_TYPE_WARNING,printMsg, ##__VA_ARGS__))
#define GLOG_ERR(printMsg, ...) (GLog_printMsg(__FUNCTION__,__FILE__,__LINE__, GLOG_TYPE_ERROR,printMsg, ##__VA_ARGS__))


extern void GLog_init();

extern void GLog_printMsg
   (char const  *funcName,
    char const  *fileName,
    int          line,
    int          logType,
    char const  *printMsg,
    ...);

extern void GLog_close();

#ifdef __cplusplus
}
#endif


#endif /* H_GENERIC_LOGGER_H */
