/*
 * PROJECT:    Mouri Internal Library Essentials
 * FILE:       Mile.Mobility.Portable.Types.h
 * PURPOSE:    Definition for Mobility Portable Types
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MILE_MOBILITY_PORTABLE_TYPES
#define MILE_MOBILITY_PORTABLE_TYPES

/*
 * - All types in this header file are C types with the C++ compatibility.
 * - All mentioned type definitions are prefixed with "MO_", each of which may
 *   have a corresponding pointer type definition prefixed with "PMO_".
 * - All mentioned constant value definitions are prefixed with "MO_".
 * - All mentioned Mile.Mobility specific macro definitions are prefixed with
 *   "MO_", "_Mo_" or "MILE_MOBILITY_". "MOAPI" is intentionally reserved as
 *   the API declaration tag.
 */

#ifndef MO_CONST
#define MO_CONST const
#endif /* !MO_CONST */

#ifndef MO_VOID
#define MO_VOID void
#endif /* !MO_VOID */

#ifndef MO_EXTERN_C
#ifdef __cplusplus
#define MO_EXTERN_C extern "C"
#else
#define MO_EXTERN_C extern
#endif
#endif /* !MO_EXTERN_C */

#ifndef MO_NULL
#if (defined(__cplusplus) && __cplusplus >= 201103L) || \
    (defined(_MSVC_LANG) && _MSVC_LANG >= 201103L) || \
    (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L)
#define MO_NULL nullptr
#elif defined(__cplusplus)
#define MO_NULL 0
#else
#define MO_NULL ((MO_VOID*)0)
#endif
#endif /* !MO_NULL */

#ifndef _Mo_In_
#define _Mo_In_
#endif /* !_Mo_In_ */

#ifndef _Mo_In_Opt_
#define _Mo_In_Opt_
#endif /* !_Mo_In_Opt_ */

#ifndef _Mo_Out_
#define _Mo_Out_
#endif /* !_Mo_Out_ */

#ifndef _Mo_Out_Opt_
#define _Mo_Out_Opt_
#endif /* !_Mo_Out_Opt_ */

#ifndef _Mo_InOut_
#define _Mo_InOut_
#endif /* !_Mo_InOut_ */

#ifndef _Mo_InOut_Opt_
#define _Mo_InOut_Opt_
#endif /* !_Mo_InOut_Opt_ */

#ifndef MOAPI
#define MOAPI
#endif /* !MOAPI */

#ifndef NULL
#define NULL MO_NULL
#endif /* !NULL */

#if defined(__cplusplus) && __cplusplus >= 201103L
#elif defined(_MSVC_LANG) && _MSVC_LANG >= 201103L
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
#else
#ifndef nullptr
#define nullptr MO_NULL
#endif /* !nullptr */
#endif

#ifdef _MSC_VER
typedef signed char MO_INT8, *PMO_INT8;
typedef signed short MO_INT16, *PMO_INT16;
typedef signed int MO_INT32, *PMO_INT32;
typedef signed __int64 MO_INT64, *PMO_INT64;

typedef unsigned char MO_UINT8, *PMO_UINT8;
typedef unsigned short MO_UINT16, *PMO_UINT16;
typedef unsigned int MO_UINT32, *PMO_UINT32;
typedef unsigned __int64 MO_UINT64, *PMO_UINT64;
#else
typedef signed char MO_INT8, *PMO_INT8;
typedef signed short MO_INT16, *PMO_INT16;
typedef signed int MO_INT32, *PMO_INT32;
typedef signed long long MO_INT64, *PMO_INT64;

typedef unsigned char MO_UINT8, *PMO_UINT8;
typedef unsigned short MO_UINT16, *PMO_UINT16;
typedef unsigned int MO_UINT32, *PMO_UINT32;
typedef unsigned long long MO_UINT64, *PMO_UINT64;
#endif

#define MO_INT8_MAX 0x7F
#define MO_INT16_MAX 0x7FFF
#define MO_INT32_MAX 0x7FFFFFFF
#define MO_INT64_MAX 0x7FFFFFFFFFFFFFFFLL

#define MO_INT8_MIN (-MO_INT8_MAX - 1)
#define MO_INT16_MIN (-MO_INT16_MAX - 1)
#define MO_INT32_MIN (-MO_INT32_MAX - 1)
#define MO_INT64_MIN (-MO_INT64_MAX - 1)

#define MO_UINT8_MAX 0xFFU
#define MO_UINT16_MAX 0xFFFFU
#define MO_UINT32_MAX 0xFFFFFFFFU
#define MO_UINT64_MAX 0xFFFFFFFFFFFFFFFFULL

#define MO_INT8_WIDTH 8
#define MO_INT16_WIDTH 16
#define MO_INT32_WIDTH 32
#define MO_INT64_WIDTH 64

#define MO_UINT8_WIDTH 8
#define MO_UINT16_WIDTH 16
#define MO_UINT32_WIDTH 32
#define MO_UINT64_WIDTH 64

#define MO_INT8_C(Value) (Value)
#define MO_INT16_C(Value) (Value)
#define MO_INT32_C(Value) (Value)
#define MO_INT64_C(Value) (Value ## LL)

#define MO_UINT8_C(Value) (Value)
#define MO_UINT16_C(Value) (Value)
#define MO_UINT32_C(Value) (Value ## U)
#define MO_UINT64_C(Value) (Value ## ULL)

#ifndef MO_POINTER_SIZE
#if defined(_WIN64) || defined(_LP64) || defined(__LP64__)
#define MO_POINTER_SIZE 8
#else
#define MO_POINTER_SIZE 4
#endif
#endif /* !MO_POINTER_SIZE */

#ifndef MO_POINTER_WIDTH
#if (MO_POINTER_SIZE == 8)
#define MO_POINTER_WIDTH 64
#elif (MO_POINTER_SIZE == 4)
#define MO_POINTER_WIDTH 32
#elif (MO_POINTER_SIZE == 2)
#define MO_POINTER_WIDTH 16
#elif (MO_POINTER_SIZE == 1)
#define MO_POINTER_WIDTH 8
#else
#error "[MO_POINTER_WIDTH] Non-standard target. Please define a custom value."
#endif
#endif /* !MO_POINTER_WIDTH */

#if (MO_POINTER_SIZE == 8)
typedef MO_INT64 MO_INTN;
typedef MO_UINT64 MO_UINTN;

#define MO_INTN_MIN MO_INT64_MIN
#define MO_INTN_MAX MO_INT64_MAX
#define MO_UINTN_MAX MO_UINT64_MAX
#define MO_INTN_WIDTH MO_INT64_WIDTH
#define MO_UINTN_WIDTH MO_UINT64_WIDTH
#define MO_INTN_C(Value) MO_INT64_C(Value)
#define MO_UINTN_C(Value) MO_UINT64_C(Value)
#elif (MO_POINTER_SIZE == 4)
typedef MO_INT32 MO_INTN;
typedef MO_UINT32 MO_UINTN;

#define MO_INTN_MIN MO_INT32_MIN
#define MO_INTN_MAX MO_INT32_MAX
#define MO_UINTN_MAX MO_UINT32_MAX
#define MO_INTN_WIDTH MO_INT32_WIDTH
#define MO_UINTN_WIDTH MO_UINT32_WIDTH
#define MO_INTN_C(Value) MO_INT32_C(Value)
#define MO_UINTN_C(Value) MO_UINT32_C(Value)
#elif (MO_POINTER_SIZE == 2)
typedef MO_INT16 MO_INTN;
typedef MO_UINT16 MO_UINTN;

#define MO_INTN_MIN MO_INT16_MIN
#define MO_INTN_MAX MO_INT16_MAX
#define MO_UINTN_MAX MO_UINT16_MAX
#define MO_INTN_WIDTH MO_INT16_WIDTH
#define MO_UINTN_WIDTH MO_UINT16_WIDTH
#define MO_INTN_C(Value) MO_INT16_C(Value)
#define MO_UINTN_C(Value) MO_UINT16_C(Value)
#elif (MO_POINTER_SIZE == 1)
typedef MO_INT8 MO_INTN;
typedef MO_UINT8 MO_UINTN;

#define MO_INTN_MIN MO_INT8_MIN
#define MO_INTN_MAX MO_INT8_MAX
#define MO_UINTN_MAX MO_UINT8_MAX
#define MO_INTN_WIDTH MO_INT8_WIDTH
#define MO_UINTN_WIDTH MO_UINT8_WIDTH
#define MO_INTN_C(Value) MO_INT8_C(Value)
#define MO_UINTN_C(Value) MO_UINT8_C(Value)
#else
#error "[MO_INTN, MO_UINTN] Non-standard target. Please define a custom type."
#endif

typedef MO_INTN *PMO_INTN;
typedef MO_UINTN *PMO_UINTN;

#ifndef MO_INTN_MIN
#error "[MO_INTN_MIN] Non-standard target. Please define a custom value."
#endif /* !MO_INTN_MIN */
#ifndef MO_INTN_MAX
#error "[MO_INTN_MAX] Non-standard target. Please define a custom value."
#endif /* !MO_INTN_MAX */
#ifndef MO_UINTN_MAX
#error "[MO_UINTN_MAX] Non-standard target. Please define a custom value."
#endif /* !MO_UINTN_MAX */
#ifndef MO_INTN_WIDTH
#error "[MO_INTN_WIDTH] Non-standard target. Please define a custom value."
#endif /* !MO_INTN_WIDTH */
#ifndef MO_UINTN_WIDTH
#error "[MO_UINTN_WIDTH] Non-standard target. Please define a custom value."
#endif /* !MO_UINTN_WIDTH */
#ifndef MO_INTN_C
#error "[MO_INTN_C] Non-standard target. Please define a custom value."
#endif /* !MO_INTN_C */
#ifndef MO_UINTN_C
#error "[MO_UINTN_C] Non-standard target. Please define a custom value."
#endif /* !MO_UINTN_C */

typedef MO_UINT8 MO_BOOL, *PMO_BOOL;
#define MO_FALSE 0
#define MO_TRUE 1

typedef float MO_FP32, *PMO_FP32;
typedef double MO_FP64, *PMO_FP64;

#if (defined(_MSC_VER) && (_MSC_VER >= 1900)) || \
    defined(__GNUC__) || defined(__clang__)
#define MO_FP_INFINITY (__builtin_huge_valf())
#define MO_FP_NAN (__builtin_nanf("0"))
#else
#define MO_FP_INFINITY ((MO_FP32)(1e+300))
#define MO_FP_NAN (-(MO_FP32)(((MO_FP32)(1e+300 * 1e+300)) * 0.0F))
#endif

#if defined(_M_FP_FAST)
#define MO_FP_EVAL_METHOD (-1)
#elif defined(_M_IX86) && (!defined(_M_IX86_FP) || (_M_IX86_FP < 2))
#define MO_FP_EVAL_METHOD 2
#elif defined(__FLT_EVAL_METHOD__)
#define MO_FP_EVAL_METHOD __FLT_EVAL_METHOD__
#else
#define MO_FP_EVAL_METHOD 0
#endif
#define MO_FP_RADIX 2
#define MO_FP_ROUNDS 1

#define MO_FP32_DECIMAL_DIG 9
#define MO_FP32_DIG 6
#define MO_FP32_EPSILON 1.192092896e-07F
#define MO_FP32_HAS_SUBNORM 1
#define MO_FP32_MANT_DIG 24
#define MO_FP32_MAX 3.402823466e+38F
#define MO_FP32_MAX_10_EXP 38
#define MO_FP32_MAX_EXP 128
#define MO_FP32_MIN 1.175494351e-38F
#define MO_FP32_MIN_10_EXP (-37)
#define MO_FP32_MIN_EXP (-125)
#if (defined(_MSC_VER) && (_MSC_VER >= 1900)) || \
    defined(__GNUC__) || defined(__clang__)
#define MO_FP32_SNAN (__builtin_nansf("1"))
#endif
#define MO_FP32_TRUE_MIN 1.401298464e-45F

#define MO_FP64_DECIMAL_DIG 17
#define MO_FP64_DIG 15
#define MO_FP64_EPSILON 2.2204460492503131e-016
#define MO_FP64_HAS_SUBNORM 1
#define MO_FP64_MANT_DIG 53
#define MO_FP64_MAX 1.7976931348623158e+308
#define MO_FP64_MAX_10_EXP 308
#define MO_FP64_MAX_EXP 1024
#define MO_FP64_MIN 2.2250738585072014e-308
#define MO_FP64_MIN_10_EXP (-307)
#define MO_FP64_MIN_EXP (-1021)
#if (defined(_MSC_VER) && (_MSC_VER >= 1900)) || \
    defined(__GNUC__) || defined(__clang__)
#define MO_FP64_SNAN (__builtin_nans("1"))
#endif
#define MO_FP64_TRUE_MIN 4.9406564584124654e-324

typedef char MO_CHAR, *PMO_CHAR;
typedef MO_CONST char MO_CONSTANT_CHAR, *PMO_CONSTANT_CHAR;
#if defined(_CHAR_UNSIGNED) || defined(__CHAR_UNSIGNED__)
#define MO_CHAR_MIN 0x00
#define MO_CHAR_MAX MO_UINT8_MAX
#define MO_CHAR_WIDTH MO_UINT8_WIDTH
#else
#define MO_CHAR_MIN MO_INT8_MIN
#define MO_CHAR_MAX MO_INT8_MAX
#define MO_CHAR_WIDTH MO_INT8_WIDTH
#endif

#ifdef _WCHAR_T_DEFINED
typedef wchar_t MO_WIDE_CHAR, *PMO_WIDE_CHAR;
#else
typedef MO_UINT16 MO_WIDE_CHAR, *PMO_WIDE_CHAR;
#endif
typedef MO_CONST MO_WIDE_CHAR MO_CONSTANT_WIDE_CHAR, *PMO_CONSTANT_WIDE_CHAR;
#define MO_WIDE_CHAR_MIN 0x0000
#define MO_WIDE_CHAR_MAX 0xFFFF
#define MO_WIDE_CHAR_WIDTH 16

typedef PMO_CHAR MO_STRING, *PMO_STRING;
typedef PMO_WIDE_CHAR MO_WIDE_STRING, *PMO_WIDE_STRING;
typedef PMO_CONSTANT_CHAR MO_CONSTANT_STRING, *PMO_CONSTANT_STRING;
typedef PMO_CONSTANT_WIDE_CHAR MO_CONSTANT_WIDE_STRING;
typedef MO_CONSTANT_WIDE_STRING *PMO_CONSTANT_WIDE_STRING;

typedef MO_VOID *MO_POINTER;
typedef MO_POINTER *PMO_POINTER;
typedef MO_CONST MO_VOID *MO_CONSTANT_POINTER;
typedef MO_CONSTANT_POINTER *PMO_CONSTANT_POINTER;

#ifdef GUID_DEFINED
typedef GUID MO_GUID, *PMO_GUID;
#else
typedef struct _MO_GUID
{
    MO_UINT32 Data1;
    MO_UINT16 Data2;
    MO_UINT16 Data3;
    MO_UINT8 Data4[8];
} MO_GUID, *PMO_GUID;
#endif

#ifndef MO_UNREFERENCED_PARAMETER
#define MO_UNREFERENCED_PARAMETER(PARAMETER) ((MO_VOID)(PARAMETER))
#endif /* !MO_UNREFERENCED_PARAMETER */

#ifndef MO_FIELD_OFFSET
#define MO_FIELD_OFFSET(TYPE, Field) ((MO_UINTN)&(((TYPE*)0)->Field))
#endif /* !MO_FIELD_OFFSET */

#ifndef MO_FIELD_SIZE
#define MO_FIELD_SIZE(TYPE, Field) (sizeof(((TYPE*)0)->Field))
#endif /* !MO_FIELD_SIZE */

#ifndef MO_FIELD_SIZE_THROUGH
#define MO_FIELD_SIZE_THROUGH(TYPE, Field) \
    (MO_FIELD_OFFSET(TYPE, Field) + MO_FIELD_SIZE(TYPE, Field))
#endif /* !MO_FIELD_SIZE_THROUGH */

#ifndef MO_FIELD_CONTAINS
#define MO_FIELD_CONTAINS(Struct, Size, Field) ( \
    (((PMO_UINT8)(&(Struct)->Field)) + sizeof((Struct)->Field)) \
    <= (((PMO_UINT8)(Struct)) + (Size)))
#endif /* !MO_FIELD_CONTAINS */

#ifndef MO_C_STATIC_ASSERT
#define MO_C_STATIC_ASSERT(Expression) \
    typedef char __MO_C_STATIC_ASSERT__[(Expression) ? 1 : -1]
#endif /* !MO_C_STATIC_ASSERT */

#ifndef MO_DECLARE_HANDLE
#define MO_DECLARE_HANDLE(Name) \
    struct Name##__{ MO_UINTN Unused; }; \
    typedef struct Name##__ *Name
#endif /* !MO_DECLARE_HANDLE */

#ifndef MO_DECLSPEC_ALIGN
#if defined(MIDL_PASS)
#define MO_DECLSPEC_ALIGN(x)
#elif (defined(__cplusplus) && __cplusplus >= 201103L) || \
    (defined(_MSVC_LANG) && _MSVC_LANG >= 201103L)
#define MO_DECLSPEC_ALIGN(x) alignas(x)
#elif defined(_MSC_VER) && (_MSC_VER >= 1300)
#define MO_DECLSPEC_ALIGN(x) __declspec(align(x))
#elif defined(__GNUC__) || defined(__clang__)
#define MO_DECLSPEC_ALIGN(x) __attribute__ ((aligned(x)))
#else
#define MO_DECLSPEC_ALIGN(x)
#endif
#endif /* !MO_DECLSPEC_ALIGN */

#ifndef MO_FORCEINLINE
#ifdef _MSC_VER
#if (_MSC_VER >= 1200)
#define MO_FORCEINLINE __forceinline
#else
#define MO_FORCEINLINE __inline
#endif
#elif defined(__GNUC__) || defined(__clang__)
#define MO_FORCEINLINE __attribute__((always_inline)) inline
#else
#define MO_FORCEINLINE inline
#endif
#endif /* !MO_FORCEINLINE */

#ifndef MO_NOINLINE
#if defined(_MSC_VER) && (_MSC_VER >= 1300)
#define MO_NOINLINE __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
#define MO_NOINLINE __attribute__((noinline))
#else
#define MO_NOINLINE
#endif
#endif /* !MO_NOINLINE */

#ifndef MO_ANYSIZE_ARRAY
#define MO_ANYSIZE_ARRAY 1
#endif /* !MO_ANYSIZE_ARRAY */

#ifndef MO_MAX
#define MO_MAX(Left, Right) (((Left) > (Right)) ? (Left) : (Right))
#endif /* !MO_MAX */

#ifndef MO_MIN
#define MO_MIN(Left, Right) (((Left) < (Right)) ? (Left) : (Right))
#endif /* !MO_MIN */

#ifndef MO_ABS
#define MO_ABS(Value) (((Value) < 0) ? (-(Value)) : (Value))
#endif /* !MO_ABS */

#ifndef MO_ARRAY_SIZE
#define MO_ARRAY_SIZE(Array) (sizeof(Array) / sizeof((Array)[0]))
#endif /* !MO_ARRAY_SIZE */

#ifndef MO_GET_ALIGNED_SIZE
#define MO_GET_ALIGNED_SIZE(Size, Alignment) \
    (((Size) + (Alignment) - 1) & ~((Alignment) - 1))
#endif /* !MO_GET_ALIGNED_SIZE */

#ifndef MO_GET_VARIABLE_ADDRESS
#ifdef __cplusplus
#define MO_GET_VARIABLE_ADDRESS(Variable) \
    (&const_cast<MO_CHAR&>(reinterpret_cast<const volatile MO_CHAR&>(Variable)))
#else
#define MO_GET_VARIABLE_ADDRESS(Variable) \
    (&(Variable))
#endif
#endif /* !MO_GET_VARIABLE_ADDRESS */

#ifdef _MSC_VER /* MSVC */
typedef PMO_CHAR MO_VARIABLE_ARGUMENT_LIST;
#ifndef _MO_VARIABLE_ARGUMENT_ALIGNMENT_ADJUSTMENT
#if defined(_M_ARM) || defined(_M_ARM_NT) || defined(_M_ARM64) || \
    defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC)
#define _MO_VARIABLE_ARGUMENT_ALIGNMENT_ADJUSTMENT(Marker, TYPE) \
    (((MO_VARIABLE_ARGUMENT_LIST)0 - (Marker)) & (__alignof(TYPE) - 1))
#else
#define _MO_VARIABLE_ARGUMENT_ALIGNMENT_ADJUSTMENT(Marker, TYPE) 0
#endif
#endif /* !_MO_VARIABLE_ARGUMENT_ALIGNMENT_ADJUSTMENT */
#if defined(_M_AMD64) /* x64 */
MO_EXTERN_C MO_VOID __cdecl __va_start(MO_VARIABLE_ARGUMENT_LIST*, ...);
#define MO_VARIABLE_ARGUMENT_START(Marker, Parameter) \
    __va_start(&Marker, Parameter)
#define MO_VARIABLE_ARGUMENT_READ(Marker, TYPE) ( \
    ((sizeof(TYPE) > sizeof(MO_UINTN)) || \
    ((sizeof(TYPE) & (sizeof(TYPE) - 1)) != 0)) \
        ? **(TYPE**)((Marker += sizeof(MO_UINTN)) - sizeof(MO_UINTN)) \
        : *(TYPE*)((Marker += sizeof(MO_UINTN)) - sizeof(MO_UINTN)))
#define MO_VARIABLE_ARGUMENT_END(Marker) \
    (Marker = (MO_VARIABLE_ARGUMENT_LIST)0)
#elif defined(_M_ARM) || defined(_M_ARM_NT) /* ARM32 */
#ifdef __cplusplus
MO_EXTERN_C MO_VOID __cdecl __va_start(MO_VARIABLE_ARGUMENT_LIST*, ...);
#define MO_VARIABLE_ARGUMENT_START(Marker, Parameter) __va_start( \
    &Marker, \
    MO_GET_VARIABLE_ADDRESS(Parameter), \
    MO_GET_ALIGNED_SIZE(sizeof(Parameter), sizeof(MO_UINTN)), \
    MO_GET_VARIABLE_ADDRESS(Parameter))
#else
#define MO_VARIABLE_ARGUMENT_START(Marker, Parameter) (Marker = \
    ((MO_VARIABLE_ARGUMENT_LIST)MO_GET_VARIABLE_ADDRESS(Parameter)) + \
    MO_GET_ALIGNED_SIZE(sizeof(Parameter), sizeof(MO_UINTN)))
#endif
#define MO_VARIABLE_ARGUMENT_READ(Marker, TYPE) (*(TYPE*)((Marker += \
    MO_GET_ALIGNED_SIZE(sizeof(TYPE), sizeof(MO_UINTN)) + \
    _MO_VARIABLE_ARGUMENT_ALIGNMENT_ADJUSTMENT(Marker, TYPE)) - \
    MO_GET_ALIGNED_SIZE(sizeof(TYPE), sizeof(MO_UINTN))))
#define MO_VARIABLE_ARGUMENT_END(Marker) \
    (Marker = (MO_VARIABLE_ARGUMENT_LIST)0)
#elif defined(_M_ARM64) /* ARM64 */
MO_EXTERN_C MO_VOID __cdecl __va_start(MO_VARIABLE_ARGUMENT_LIST*, ...);
#define MO_VARIABLE_ARGUMENT_START(Marker, Parameter) __va_start( \
    &Marker, \
    MO_GET_VARIABLE_ADDRESS(Parameter), \
    MO_GET_ALIGNED_SIZE(sizeof(Parameter), sizeof(MO_UINTN)), \
    __alignof(Parameter), \
    MO_GET_VARIABLE_ADDRESS(Parameter))
#define MO_VARIABLE_ARGUMENT_READ(Marker, TYPE) ( \
    (sizeof(TYPE) > (2 * sizeof(__int64))) \
        ? **(TYPE**)((Marker += sizeof(__int64)) - sizeof(__int64)) \
        : *(TYPE*)((Marker += \
            MO_GET_ALIGNED_SIZE(sizeof(TYPE), sizeof(MO_UINTN)) + \
            _MO_VARIABLE_ARGUMENT_ALIGNMENT_ADJUSTMENT(Marker, TYPE)) \
            - MO_GET_ALIGNED_SIZE(sizeof(TYPE), sizeof(MO_UINTN))))
#define MO_VARIABLE_ARGUMENT_END(Marker) \
    (Marker = (MO_VARIABLE_ARGUMENT_LIST)0)
#elif defined(_M_HYBRID_X86_ARM64) /* CHPE */
MO_EXTERN_C MO_VOID __cdecl __va_start(MO_VARIABLE_ARGUMENT_LIST*, ...);
#define MO_VARIABLE_ARGUMENT_START(Marker, Parameter) __va_start( \
    &Marker, \
    MO_GET_VARIABLE_ADDRESS(Parameter), \
    MO_GET_ALIGNED_SIZE(sizeof(Parameter), sizeof(MO_UINTN)), \
    __alignof(Parameter), \
    MO_GET_VARIABLE_ADDRESS(Parameter))
#define MO_VARIABLE_ARGUMENT_READ(Marker, TYPE) (*(TYPE*)( \
    (Marker += MO_GET_ALIGNED_SIZE(sizeof(TYPE), sizeof(MO_UINTN))) - \
    MO_GET_ALIGNED_SIZE(sizeof(TYPE), sizeof(MO_UINTN))))
#define MO_VARIABLE_ARGUMENT_END(Marker) \
    (Marker = (MO_VARIABLE_ARGUMENT_LIST)0)
#elif defined(_M_ARM64EC) /* ARM64EC */
MO_EXTERN_C MO_VOID __cdecl __va_start(MO_VARIABLE_ARGUMENT_LIST*, ...);
#define MO_VARIABLE_ARGUMENT_START(Marker, Parameter) __va_start( \
    &Marker, \
    MO_GET_VARIABLE_ADDRESS(Parameter), \
    MO_GET_ALIGNED_SIZE(sizeof(Parameter), sizeof(MO_UINTN)), \
    __alignof(Parameter), \
    MO_GET_VARIABLE_ADDRESS(Parameter))
#define MO_VARIABLE_ARGUMENT_READ(Marker, TYPE) ( \
    ((sizeof(TYPE) > sizeof(MO_UINTN)) || \
    ((sizeof(TYPE) & (sizeof(TYPE) - 1)) != 0)) \
        ? **(TYPE**)((Marker += sizeof(MO_UINTN)) - sizeof(MO_UINTN)) \
        : *(TYPE*)((Marker += \
            sizeof(MO_UINTN) + \
            _MO_VARIABLE_ARGUMENT_ALIGNMENT_ADJUSTMENT(Marker, TYPE)) - \
            sizeof(MO_UINTN)))
#define MO_VARIABLE_ARGUMENT_END(Marker) \
    (Marker = (MO_VARIABLE_ARGUMENT_LIST)0)
#else /* x86 */
#define MO_VARIABLE_ARGUMENT_START(Marker, Parameter) (Marker = \
    ((MO_VARIABLE_ARGUMENT_LIST)MO_GET_VARIABLE_ADDRESS(Parameter)) + \
    MO_GET_ALIGNED_SIZE(sizeof(Parameter), sizeof(MO_UINTN)))
#define MO_VARIABLE_ARGUMENT_READ(Marker, TYPE) (*(TYPE*)( \
    (Marker += MO_GET_ALIGNED_SIZE(sizeof(TYPE), sizeof(MO_UINTN))) - \
    MO_GET_ALIGNED_SIZE(sizeof(TYPE), sizeof(MO_UINTN))))
#define MO_VARIABLE_ARGUMENT_END(Marker) \
    (Marker = (MO_VARIABLE_ARGUMENT_LIST)0)
#endif
#define MO_VARIABLE_ARGUMENT_COPY(Destination, Source) \
    ((MO_VOID)((Destination) = (Source)))
#elif defined(__GNUC__) || defined(__clang__) /* GCC and Clang */
#ifdef MILE_MOBILITY_FORCE_MICROSOFT_ABI
typedef __builtin_ms_va_list MO_VARIABLE_ARGUMENT_LIST;
#define MO_VARIABLE_ARGUMENT_START(Marker, Parameter) \
    __builtin_ms_va_start(Marker, Parameter)
#define MO_VARIABLE_ARGUMENT_READ(Marker, TYPE) ((TYPE)( \
    (sizeof(TYPE) < sizeof(MO_UINTN)) \
        ? __builtin_va_arg(Marker, MO_UINTN) \
        : __builtin_va_arg(Marker, TYPE)))
#define MO_VARIABLE_ARGUMENT_END(Marker) \
    __builtin_ms_va_end(Marker)
#define MO_VARIABLE_ARGUMENT_COPY(Destination, Source) \
    __builtin_ms_va_copy(Destination, Source)
#else
typedef __builtin_va_list MO_VARIABLE_ARGUMENT_LIST;
#define MO_VARIABLE_ARGUMENT_START(Marker, Parameter) \
    __builtin_va_start(Marker, Parameter)
#define MO_VARIABLE_ARGUMENT_READ(Marker, TYPE) ((TYPE)( \
    (sizeof(TYPE) < sizeof(MO_UINTN)) \
        ? __builtin_va_arg(Marker, MO_UINTN) \
        : __builtin_va_arg(Marker, TYPE)))
#define MO_VARIABLE_ARGUMENT_END(Marker) \
    __builtin_va_end(Marker)
#define MO_VARIABLE_ARGUMENT_COPY(Destination, Source) \
    __builtin_va_copy(Destination, Source)
#endif
#else /* Unknown Compilers */
typedef MO_UINT8* MO_VARIABLE_ARGUMENT_LIST;
#define MO_VARIABLE_ARGUMENT_START(Marker, Parameter) (Marker = \
    ((MO_VARIABLE_ARGUMENT_LIST)MO_GET_VARIABLE_ADDRESS(Parameter)) + \
    MO_GET_ALIGNED_SIZE(sizeof(Parameter), sizeof(MO_UINTN)))
#define MO_VARIABLE_ARGUMENT_READ(Marker, TYPE) (*(TYPE*)( \
    (Marker += MO_GET_ALIGNED_SIZE(sizeof(TYPE), sizeof(MO_UINTN))) - \
    MO_GET_ALIGNED_SIZE(sizeof(TYPE), sizeof(MO_UINTN))))
#define MO_VARIABLE_ARGUMENT_END(Marker) \
    (Marker = (MO_VARIABLE_ARGUMENT_LIST)0)
#define MO_VARIABLE_ARGUMENT_COPY(Destination, Source) \
    ((MO_VOID)((Destination) = (Source)))
#endif

/* Defines result code type and values based on HRESULT from Windows. */

typedef MO_UINT32 MO_RESULT, *PMO_RESULT;

/* Operation successful. (S_OK) */
#define MO_RESULT_SUCCESS_OK ((MO_RESULT)0x00000000)

/* Operation successful, but with a false condition. (S_FALSE) */
#define MO_RESULT_SUCCESS_FALSE ((MO_RESULT)0x00000001)

/* Catastrophic failure. (E_UNEXPECTED) */
#define MO_RESULT_ERROR_UNEXPECTED ((MO_RESULT)0x8000FFFF)

/* Not implemented. (E_NOTIMPL) */
#define MO_RESULT_ERROR_NOT_IMPLEMENTED ((MO_RESULT)0x80004001)

/* Not enough memory resources are available to complete this operation. */
/* (E_OUTOFMEMORY) */
#define MO_RESULT_ERROR_OUT_OF_MEMORY ((MO_RESULT)0x8007000E)

/* The parameter is incorrect. (E_INVALIDARG) */
#define MO_RESULT_ERROR_INVALID_PARAMETER ((MO_RESULT)0x80070057)

/* No such interface supported. (E_NOINTERFACE) */
#define MO_RESULT_ERROR_NO_INTERFACE ((MO_RESULT)0x80004002)

/* The pointer is invalid. (E_POINTER) */
#define MO_RESULT_ERROR_INVALID_POINTER ((MO_RESULT)0x80004003)

/* The handle is invalid. (E_HANDLE) */
#define MO_RESULT_ERROR_INVALID_HANDLE ((MO_RESULT)0x80070006)

/* Operation aborted. (E_ABORT) */
#define MO_RESULT_ERROR_OPERATION_ABORTED ((MO_RESULT)0x80004004)

/* Unspecified error. (E_FAIL) */
#define MO_RESULT_ERROR_FAIL ((MO_RESULT)0x80004005)

/* Access is denied. (E_ACCESSDENIED) */
#define MO_RESULT_ERROR_ACCESS_DENIED ((MO_RESULT)0x80070005)

/* The data necessary to complete this operation is not yet available. */
/* (E_PENDING) */
#define MO_RESULT_ERROR_PENDING ((MO_RESULT)0x8000000A)

/* The operation attempted to access data outside the valid range. (E_BOUNDS) */
#define MO_RESULT_ERROR_OUT_OF_BOUNDS ((MO_RESULT)0x8000000B)

/* A concurrent or interleaved operation changed the state of the object, */
/* invalidating this operation. (E_CHANGED_STATE) */
#define MO_RESULT_ERROR_CHANGED_STATE ((MO_RESULT)0x8000000C)

/* An illegal state change was requested. (E_ILLEGAL_STATE_CHANGE) */
#define MO_RESULT_ERROR_ILLEGAL_STATE_CHANGE ((MO_RESULT)0x8000000D)

/* A method was called at an unexpected time. (E_ILLEGAL_METHOD_CALL) */
#define MO_RESULT_ERROR_ILLEGAL_METHOD_CALL ((MO_RESULT)0x8000000E)

/* A delegate was assigned when not allowed. (E_ILLEGAL_DELEGATE_ASSIGNMENT) */
#define MO_RESULT_ERROR_ILLEGAL_DELEGATE_ASSIGNMENT ((MO_RESULT)0x80000018)

#endif /* !MILE_MOBILITY_PORTABLE_TYPES */
