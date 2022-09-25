#ifndef _LOGGER_H_
#define _LOGGER_H_

#define LM_NONE     0
#define LM_DEADLY   1
#define LM_ERROR    2
#define LM_INFO     3
#define LM_DEBUG    4

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_DEADLY(x) Log(LM_DEADLY, x)
#define LOG_ERROR(x) Log(LM_ERROR, x)
#define LOG_INFO(x) Log(LM_INFO, x)
#define LOG_DEBUG(x) Log(LM_DEBUG, x)

typedef void(*LogCallback)(const char* msg, int level);
void SetLogCallback(LogCallback cb);
void Log(int _level, const char* msg, ...);

#ifdef __cplusplus
}
#endif

#endif // !_LOGGER_H_
