#pragma once

// languageFeatureInfo.hpp
// Purpose: Define constants that can be used to do conditional compilation based on availability of different
// language features.

#include "../preprocessor/compilerInfoMsvc.hpp"

#define DFG_LANGFEAT_MOVABLE_STREAMS	1

#if defined(_MSC_VER) && (_MSC_VER < DFG_MSVC_VER_2013)
    #define DFG_LANGFEAT_HAS_ISNAN	0
#else
    #define DFG_LANGFEAT_HAS_ISNAN	1
#endif

#if defined(_MSC_VER) && (_MSC_VER >= DFG_MSVC_VER_2010)
    #define DFG_LANGFEAT_TYPETRAITS_11  1
#else
    #define DFG_LANGFEAT_TYPETRAITS_11  0
#endif

