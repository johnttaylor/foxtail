#ifndef COLONY_CONFIG_H_
#define COLONY_CONFIG_H_

// Turn off STDIO - using a Cpl::Io UART stream in place of
#define USE_BSP_NO_STDIO

// Enable Trace
#define USE_CPL_SYSTEM_TRACE

// Tweak BETTER_ENUM
#define USE_CPL_TYPE_BETTERENUM_MIN_FOOTPRINT
#define USE_CPL_TYPE_BETTERENUM_EXTEND_128_32

// Large TShell input
#define OPTION_CPL_TSHELL_PROCESSOR_INPUT_SIZE      (8*1024)

#endif
