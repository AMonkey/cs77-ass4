#include "debug.h"

#include <cassert>
#include <stdio.h>
#include <cstdarg>

///@file common/debug.cpp Debugging facilities @ingroup common

void message(const char* msg) {
	fprintf(stdout, "%s\n", msg);
    fflush(stderr);
}

void message_va(const char* msg, ...) {
    va_list args;
    va_start (args, msg);
    vfprintf(stdout, msg, args);
    va_end (args);
    fprintf(stdout, "\n");
    fflush(stdout);
}

void debug_break() {
    message("break: press any key to continue.");
    getchar();
}

