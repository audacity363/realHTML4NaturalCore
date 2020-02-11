#ifndef RH4NLIBLOGGING
#define RH4NLIBLOGGING

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

typedef enum {
    RH4N_DEVELOP = 0,
    RH4N_DEBUG = 1,
    RH4N_INFO = 2,
    RH4N_WARN = 3,
    RH4N_ERROR = 4,
    RH4N_FATAL = 5
} rh4n_log_level;

typedef struct rh4n_log_rule_s {
    int level;
    char nat_library[9];
    char nat_program[9];
    char logpath[2048];
    FILE *outputfile;
} RH4nLogrule;


RH4nLogrule *rh4nLoggingCreateRule(const char*, const char*, const int, const char*);
RH4nLogrule *rh4nLoggingCreateStreamingRule(const char*, const char*, const int, const char*);
const char *rh4nLoggingGetLevelStr(int level);
void rh4n_del_log_rule(RH4nLogrule *a_rule);
int rh4nLoggingConvertStrtoInt(const char*);
char *rh4nLoggingCreateLogfilepath(const char*, const char*, const char*, struct tm*);
void rh4n_log(RH4nLogrule *rule, int level, 
    const char *file, const char *func, long line,
    const char *format, ...);


#define rh4n_log_develop(rule, ...) rh4n_log(rule, RH4N_DEVELOP, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define rh4n_log_debug(rule, ...) rh4n_log(rule, RH4N_DEBUG, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define rh4n_log_info(rule, ...) rh4n_log(rule, RH4N_INFO, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define rh4n_log_warn(rule, ...) rh4n_log(rule, RH4N_WARN, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define rh4n_log_error(rule, ...) rh4n_log(rule, RH4N_ERROR, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define rh4n_log_fatal(rule, ...) rh4n_log(rule, RH4N_FATAL, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
