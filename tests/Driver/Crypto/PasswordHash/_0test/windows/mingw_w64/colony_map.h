// Driver
#include "Driver/DIO/Simulated/mappings_.h"

// Cpl::System mappings
#ifdef BUILD_VARIANT_CPP11
#include "Cpl/System/Cpp11/_win32/mappings_.h"

#else
#include "Cpl/System/Win32/mappings_.h"
#endif

// strapi mapping
#include "Cpl/Text/_mappings/_mingw/strapi.h"


