#pragma once

#define ODD_NULL_PASTE_IMPL(a, b) a##b
#define ODD_NULL_PASTE(a, b) ODD_NULL_PASTE_IMPL(a, b)

#ifndef ODD_DISTRIBUTION_BUILD
    #define ODD_NULL_PREFIX(className) ODD_NULL_PASTE(Null, className)
#else
    #define ODD_NULL_PREFIX(className) ODD_NULL_PASTE(RHI, className)
#endif