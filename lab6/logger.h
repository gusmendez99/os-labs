#include <stdio.h>

#ifndef THREAD_COUNT
#define THREAD_COUNT 4
#endif

#ifndef ITER_COUNT
#define ITER_COUNT 16
#endif
#define gettid() syscall(SYS_gettid)

void *resource_manager(void* custom_logger);

struct logger {
    FILE* info;
};
