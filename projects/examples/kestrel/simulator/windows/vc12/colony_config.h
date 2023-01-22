#ifndef COLONY_CONFIG_H_
#define COLONY_CONFIG_H_

#define USE_CPL_SYSTEM_TRACE

#define OPTION_CPL_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD 10  // 10ms -->Windoze really can't go any 'faster' than this

// Force DOS newline
#define USE_CPL_IO_NEW_LINE_STANDARD   CPL_IO_NEW_LINE_NATIVE

// Set native newline for C++11 platform
#ifdef BUILD_VARIANT_CPP11
#define CPL_IO_NEW_LINE_NATIVE_MAP              "\015\012"
#endif

// Tweak BETTER_ENUM
#define USE_CPL_TYPE_BETTERENUM_MIN_FOOTPRINT
#define USE_CPL_TYPE_BETTERENUM_EXTEND_128_32

// Large TShell input
#define OPTION_CPL_TSHELL_PROCESSOR_INPUT_SIZE      (8*1024)

#endif
