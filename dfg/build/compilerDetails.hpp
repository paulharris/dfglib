#pragma once

#include "../dfgDefs.hpp" // For DFG_STRINGIZE
#include "../preprocessor/compilerInfoMsvc.hpp"

#define DFG_COMPILER_NAME_SIMPLE_VC2010          "MSVC_2010"
#define DFG_COMPILER_NAME_SIMPLE_VC2012          "MSVC_2012"
#define DFG_COMPILER_NAME_SIMPLE_VC2013          "MSVC_2013"
#define DFG_COMPILER_NAME_SIMPLE_VC2015_RTM      "MSVC_2015_rtm"
#define DFG_COMPILER_NAME_SIMPLE_VC2015_UPDATE1  "MSVC_2015_u1"
#define DFG_COMPILER_NAME_SIMPLE_VC2015_UPDATE2  "MSVC_2015_u2"
#define DFG_COMPILER_NAME_SIMPLE_VC2015_UPDATE3  "MSVC_2015_u3"
#define DFG_COMPILER_NAME_SIMPLE_VC2017_RC       "MSVC_2017_RC"
#define DFG_COMPILER_NAME_SIMPLE_VC_UNKNOWN      "MSVC_UNKNOWN"
#define DFG_COMPILER_NAME_SIMPLE_UNKNOWN         "UNKNOWN_COMPILER"

#if DFG_MSVC_VER == DFG_MSVC_VER_2010
    #define DFG_COMPILER_NAME_SIMPLE DFG_COMPILER_NAME_SIMPLE_VC2010
#elif DFG_MSVC_VER == DFG_MSVC_VER_2012
    #define DFG_COMPILER_NAME_SIMPLE DFG_COMPILER_NAME_SIMPLE_VC2012
#elif DFG_MSVC_VER == DFG_MSVC_VER_2013
    #define DFG_COMPILER_NAME_SIMPLE DFG_COMPILER_NAME_SIMPLE_VC2013
#elif defined(_MSC_FULL_VER) && _MSC_FULL_VER == 190023026  // Compiler versions are from http://stackoverflow.com/questions/30760889/unknown-compiler-version-while-compiling-boost-with-msvc-14-0-vs-2015
    #define DFG_COMPILER_NAME_SIMPLE DFG_COMPILER_NAME_SIMPLE_VC2015_RTM
#elif defined(_MSC_FULL_VER) && _MSC_FULL_VER == 190023506
    #define DFG_COMPILER_NAME_SIMPLE DFG_COMPILER_NAME_SIMPLE_VC2015_UPDATE1
#elif defined(_MSC_FULL_VER) && _MSC_FULL_VER == 190023918
    #define DFG_COMPILER_NAME_SIMPLE DFG_COMPILER_NAME_SIMPLE_VC2015_UPDATE2
#elif defined(_MSC_FULL_VER) && _MSC_FULL_VER == 190024215 // Was 190024210 in the link above, changed to this based on version in AppVeyor build.
    #define DFG_COMPILER_NAME_SIMPLE DFG_COMPILER_NAME_SIMPLE_VC2015_UPDATE3
#elif DFG_MSVC_VER == DFG_MSVC_VER_2017_RC
    #define DFG_COMPILER_NAME_SIMPLE DFG_COMPILER_NAME_SIMPLE_VC2017_RC
#elif defined(_MSC_FULL_VER)
    #pragma message("Unrecognized MSVC compiler, _MSC_FULL_VER = " DFG_STRINGIZE(_MSC_FULL_VER))
    #define DFG_COMPILER_NAME_SIMPLE DFG_COMPILER_NAME_SIMPLE_VC_UNKNOWN
#elif defined(__MINGW32__)
    #define DFG_COMPILER_NAME_SIMPLE "MinGW_" DFG_STRINGIZE(__GNUC__) "." DFG_STRINGIZE(__GNUC_MINOR__) "." DFG_STRINGIZE(__GNUC_PATCHLEVEL__)
#else
    #define DFG_COMPILER_NAME_SIMPLE DFG_COMPILER_NAME_SIMPLE_UNKNOWN
    #pragma message("Note: Unrecognized compiler")
#endif

#ifdef _DEBUG
    #define DFG_BUILD_DEBUG_RELEASE_TYPE    "debug"
#else
    #define DFG_BUILD_DEBUG_RELEASE_TYPE    "release"
#endif
