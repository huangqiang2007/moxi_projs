#ifndef __DBG_H_
#define __DBG_H_

#include "stm32f0xx.h"

#if 1
enum {
	DBG_ERROR = 0,
	DBG_INFO,
	DBG_WARN,
	DBG_DEBUG
};

static int8_t g_dbg_level = DBG_DEBUG;

#define pr_info(dbg_level, x, ...) \
	do { \
		if (dbg_level <= g_dbg_level)	\
			printf(x, ##__VA_ARGS__);	\
	} while(0);

#define pr_warn(x, ...) \
	do { \
		printf("Warn: "x, ##__VA_ARGS__);	\
	} while(0);

#define pr_err(x, ...) \
	do { \
		printf("Err: "x, ##__VA_ARGS__);	\
	} while(0);

#else
#define pr_info(x, ...) NULL
#endif

#endif

