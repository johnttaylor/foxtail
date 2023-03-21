#ifndef COLONY_CONFIG_H_
#define COLONY_CONFIG_H_

// Enable Trace
#define USE_CPL_SYSTEM_TRACE

// Tweak BETTER_ENUM
#define USE_CPL_TYPE_BETTERENUM_MIN_FOOTPRINT
#define USE_CPL_TYPE_BETTERENUM_EXTEND_128_32

// Increase shell buffers 
#define OPTION_CPL_TSHELL_PROCESSOR_INPUT_SIZE      (1024*32)
#define OPTION_CPL_TSHELL_PROCESSOR_OUTPUT_SIZE     (1024*32)

// Enable console hack - so the polled TShell work'ish
#define USE_CPL_IO_STDIO_WIN32_STDIN_CONSOLE_HACK

#endif
