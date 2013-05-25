#ifndef DEBUG_H_
#define DEBUG_H_

#include <cassert>
#include <stdio.h>
#include <cstdarg>


///@file common/debug.h Debugging facilities @ingroup common
///@defgroup debug Debugging facilities
///@ingroup common
///@{

/// Prints a message
void message(const char* msg);
/// Prints a message (printf style)
void message_va(const char* msg, ...);

/// Prints an error message and stops (printf style)
#define ERROR(msg, ...) do { \
    fprintf(stderr, "error in function %s at %s:%d\n", __FUNCTION__, __FILE__, __LINE__); \
    fprintf(stderr, msg , ## __VA_ARGS__ ); fprintf(stderr, "\n" ); \
    fflush(stderr); assert(false); \
    } while(false)

/// Prints an error message and stops if a condition does not hold (printf style)
#define ERROR_IF_NOT(cond, msg, ...) do { \
    if(cond) break; \
    fprintf(stderr, "error (assertion failed) in function %s at %s:%d\n", __FUNCTION__, __FILE__, __LINE__); \
    fprintf(stderr, "condition: %s\n", #cond ); \
    fprintf(stderr, msg , ## __VA_ARGS__ ); fprintf(stderr, "\n" ); \
    fflush(stderr); assert(false); \
    } while(false)

/// Prints a warning message (printf style)
#define WARNING(msg, ...) do { \
    fprintf(stderr, "warning in function %s at %s:%d\n", __FUNCTION__, __FILE__, __LINE__); \
    fprintf(stderr, msg , ## __VA_ARGS__ ); fprintf(stderr, "\n" ); \
    fflush(stderr); \
    } while(false)

/// Prints a warning message if a condition does not hold (printf style)
#define WARNING_IF_NOT(cond, msg, ...) do { \
    if(cond) break; \
    fprintf(stderr, "warning (assertion failed) in function %s at %s:%d\n", __FUNCTION__, __FILE__, __LINE__); \
    fprintf(stderr, "condition: %s\n", #cond ); \
    fprintf(stderr, msg , ## __VA_ARGS__ ); fprintf(stderr, "\n" ); \
    fflush(stderr); \
    } while(false)

/// Error signaling unimplemented features
#define NOT_IMPLEMENTED_ERROR() do { \
    fprintf(stderr, "error (not implemented) in function %s at %s:%d\n", __FUNCTION__, __FILE__, __LINE__); \
    fflush(stderr); assert(false); \
    } while(false)

/// Error signaling unimplemented features
#define PUT_YOUR_CODE_HERE(msg, ...) do { \
    fprintf(stderr, "warning (code missing) in function %s at %s:%d\n", __FUNCTION__, __FILE__, __LINE__); \
    fprintf(stderr, msg , ## __VA_ARGS__ ); fprintf(stderr, "\n" ); \
    fflush(stderr); \
    } while(false)


/// Break into the debugger
void debug_break();

///@}

#endif

