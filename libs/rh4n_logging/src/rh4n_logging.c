#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "rh4n_logging.h"

#define MAX_PREFIX_LEN 2024
#define DATETIME_LEN 24

const char *rh4nLoggingGetLevelStr(int);

RH4nLogrule *rh4nLoggingCreateRule(const char *library, const char *program, const int level, const char *logpath) {
    RH4nLogrule *a_logLevel = NULL;

    if(!(a_logLevel = calloc(1, sizeof(RH4nLogrule)))) {
        fprintf(stderr, "Could not create logging rule. calloc returned NULL\n");
        fflush(stderr);
        return(NULL);
    }

    a_logLevel->level = level;

    strncpy(a_logLevel->nat_library, library, sizeof(a_logLevel->nat_library)-1);
    strncpy(a_logLevel->nat_program, program, sizeof(a_logLevel->nat_program)-1);
    strncpy(a_logLevel->logpath, logpath, sizeof(a_logLevel->logpath)-1);
    a_logLevel->outputfile = NULL;

    return(a_logLevel);
}

RH4nLogrule *rh4nLoggingCreateStreamingRule(const char *library, const char *program, const int level, const char *logpath) {
    RH4nLogrule *tmp = NULL;
    time_t rawtime;
    struct tm *timeinfo;
    char *logfile = NULL;

    if((tmp = rh4nLoggingCreateRule(library, program, level, logpath)) == NULL) {
        return(NULL);
    }

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    if(strlen(logpath) == 0) {
        tmp->outputfile = stdout;
    } else {
        logfile = rh4nLoggingCreateLogfilepath(library, program, logpath, timeinfo);

        if((tmp->outputfile = fopen(logfile, "a")) == NULL) {
            fprintf(stderr, "Could not open logfile [%s]: %s\n", logfile, strerror(errno));
            fflush(stderr);
            return(NULL);
        }
    }

    return(tmp);
}

void rh4n_del_log_rule(RH4nLogrule *a_rule) {
    if(a_rule->outputfile) fclose(a_rule->outputfile);
    free(a_rule);
}

void rh4n_log(RH4nLogrule *rule, int level, 
    const char *file, const char *func, long line,
    const char *format, ...) {

    if(rule == NULL)  { return; }
    if(level < rule->level) { return; } 

    char logprefix[MAX_PREFIX_LEN+1],
         datetime_buff[DATETIME_LEN+1],
         *endformat = NULL,
         *filepath = NULL;
    const char *level_str = NULL;
    int formatlen = 0;
    time_t rawtime;
    struct tm *timeinfo;
    struct timeval millis;
    va_list args;
    pid_t pid = 0;

    FILE *output = NULL;

    memset(logprefix, 0x00, MAX_PREFIX_LEN);

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    gettimeofday(&millis, NULL);

    strftime(datetime_buff, sizeof(datetime_buff), "%d.%m.%Y - %H:%M:%S", timeinfo);

    if(!(level_str = rh4nLoggingGetLevelStr(level))) 
        return;

    pid = getpid();

    if(rule->level == RH4N_DEVELOP) {
        snprintf(logprefix, MAX_PREFIX_LEN, "%s.%06d [%-8s->%-8s@%d] %-7s (%s@%s:%ld) - ", datetime_buff, millis.tv_usec, 
            rule->nat_library, rule->nat_program, pid, level_str, func, file, line);
    } else {
        snprintf(logprefix, MAX_PREFIX_LEN, "%s.%06d [%-8s->%-8s@%d] %-7s - ", datetime_buff, millis.tv_usec, 
            rule->nat_library, rule->nat_program, pid, level_str);
    }

    if(!rule->outputfile)
        filepath = rh4nLoggingCreateLogfilepath(rule->nat_library, rule->nat_program, rule->logpath, timeinfo);
   
    formatlen = snprintf(NULL, 0, "%s%s\n", logprefix, format);
    if((endformat = calloc(sizeof(char), formatlen+1)) == NULL) {
        fprintf(stderr, "Logging: calloc returned NULL!!\n");
        fflush(stderr);
        return;
    }
    snprintf(endformat, formatlen+1, "%s%s\n", logprefix, format);

    if(!rule->outputfile) {
        if((output = fopen(filepath, "a")) == NULL) {
            fprintf(stderr, "Could not open logfile [%s]. Error: [%s]\n", filepath, strerror(errno));
            return;
        }
        va_start(args, format);
        vfprintf(output, endformat, args);
        fclose(output);
        va_end(args);
    } else {
        va_start(args, format);
        vfprintf(rule->outputfile, endformat, args);
        fflush(rule->outputfile);
        va_end(args);
    }

    free(endformat);
    return;
}

const char *rh4nLoggingGetLevelStr(int level) {
    static char *level_strs[] = {
        "DEVELOP",
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR",
        "FATAL"
   };

    if(level < 0 || level > (sizeof(level_strs)/sizeof(char*))) 
        return(NULL);

    return(level_strs[level]);
}

int rh4nLoggingConvertStrtoInt(const char* cloglevel) {
    if(strcmp(cloglevel, "DEVELOP") == 0) 
        return(RH4N_DEVELOP);
    if(strcmp(cloglevel, "DEBUG") == 0) 
        return(RH4N_DEBUG);
    if(strcmp(cloglevel, "INFO") == 0) 
        return(RH4N_INFO);
    if(strcmp(cloglevel, "WARN") == 0) 
        return(RH4N_WARN);
    if(strcmp(cloglevel, "WARNING") == 0) 
        return(RH4N_WARN);
    if(strcmp(cloglevel, "ERROR") == 0) 
        return(RH4N_ERROR);
    if(strcmp(cloglevel, "FATAL") == 0) 
        return(RH4N_FATAL);
    return(-1);
}

char *rh4nLoggingCreateLogfilepath(const char* library, const char* program, const char *logpath, struct tm* time) {
    static char logfilepath[3000];
    char datebuff[11];

    strftime(datebuff, sizeof(datebuff), "%d.%m.%Y", time);

    sprintf(logfilepath, "%s/rh4n_%s_%s_%s.log", logpath, library, program, datebuff);
    return(logfilepath);
}
