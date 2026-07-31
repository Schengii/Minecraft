#ifndef __khrplatform_h_
#define __khrplatform_h_

/*
** Copyright (c) 2008-2018 The Khronos Group Inc.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and associated documentation files (the "Materials"),
** to deal in the Materials without restriction, including without limitation
** the rights to use, copy, modify, merge, publish, distribute, sublicense,
** and/or sell copies of the Materials, and to permit persons to whom the
** Materials are furnished to do so, subject to the following conditions:
*/

#if defined(_WIN32) && !defined(__SCITECH_SNAP__)
#   define KHRONOS_APICALL __declspec(dllimport)
#   define KHRONOS_APIENTRY __stdcall
#else
#   define KHRONOS_APICALL
#   define KHRONOS_APIENTRY
#endif

#define KHRONOS_APIATTRIBUTES

#include <stdint.h>
#include <stddef.h>
typedef intptr_t                khronos_intptr_t;
typedef uintptr_t               khronos_uintptr_t;
typedef ptrdiff_t               khronos_intptrdiff_t;
typedef size_t                  khronos_uintptrdiff_t;
typedef ptrdiff_t               khronos_sizei_t;
typedef int32_t                 khronos_int32_t;
typedef uint32_t                khronos_uint32_t;
typedef int64_t                 khronos_int64_t;
typedef uint64_t                khronos_uint64_t;
typedef int8_t                  khronos_int8_t;
typedef uint8_t                 khronos_uint8_t;
typedef int16_t                 khronos_int16_t;
typedef uint16_t                khronos_uint16_t;
typedef float                   khronos_float_t;
typedef double                  khronos_double_t;
typedef khronos_intptrdiff_t    khronos_ssize_t;
typedef khronos_uintptrdiff_t   khronos_usize_t;
typedef khronos_int64_t         khronos_utime_nanoseconds_t;
typedef khronos_int64_t         khronos_stime_nanoseconds_t;

#endif /* __khrplatform_h_ */
