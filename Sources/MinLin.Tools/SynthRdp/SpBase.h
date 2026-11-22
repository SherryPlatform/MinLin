/*
 * PROJECT:    Sherry Platform
 * FILE:       SpBase.h
 * PURPOSE:    Definition for the essential POSIX helper functions
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef SP_BASE
#define SP_BASE

#include <Mile.Mobility.Portable.Types.h>

/**
 * @brief Allocates a block of memory. The allocated memory will be initialized
 *        to zero. The allocated memory is not movable.
 * @param Size The number of bytes to be allocated. If Size is zero, the
 *             function returns nullptr.
 * @return If the function succeeds, the return value is a pointer to the
 *         allocated memory block. If the function fails, the return value is
 *         nullptr.
 */
MO_EXTERN_C MO_POINTER MOAPI SpAllocateMemory(
    _Mo_In_ MO_UINTN Size);

/**
 * @brief Frees a memory block allocated by SpAllocateMemory.
 * @param Block A pointer to the memory block to be freed. This pointer is
 *        returned by SpAllocateMemory. If this pointer is nullptr, this
 *        function does nothing.
 */
MO_EXTERN_C MO_VOID MOAPI SpFreeMemory(
    _Mo_In_Opt_ MO_POINTER Block);

/**
 * @brief Write formatted data to a onebyte or multibyte string, suggested
 *        encoding with UTF-8.
 * @param Format Format-control string.
 * @param ArgList Pointer to list of optional arguments to be formatted.
 * @return A formatted string to be freed with SpFreeMemory, or MO_NULL on
 *         failure or an empty result.
*/
MO_STRING SpVFormatString(
    _Mo_In_ MO_CHAR MO_CONST* MO_CONST Format,
    _Mo_In_ MO_VARIABLE_ARGUMENT_LIST ArgList);

/**
 * @brief Write formatted data to a onebyte or multibyte string, suggested
 *        encoding with UTF-8.
 * @param Format Format-control string.
 * @param ... Optional arguments to be formatted.
 * @return A formatted string to be freed with SpFreeMemory, or MO_NULL on
 *         failure or an empty result.
*/
MO_STRING SpFormatString(
    _Mo_In_ MO_CHAR MO_CONST* MO_CONST Format,
    ...);

/**
 * @brief Converts from the onebyte or multibyte string, suggested encoding with
 *        UTF-8, to the Int64 integer.
 * @param Source The onebyte or multibyte string.
 * @param Radix The number base to use.
 * @return The converted Int64 integer value.
 * @remark For more information, see strtoll.
 */
MO_INT64 SpToInt64(
    _Mo_In_ MO_CONSTANT_STRING Source,
    _Mo_In_ MO_UINT8 Radix);

/**
 * @brief Converts from the onebyte or multibyte string, suggested encoding with
 *        UTF-8, to the UInt64 integer.
 * @param Source The onebyte or multibyte string.
 * @param Radix The number base to use.
 * @return The converted UInt64 integer value.
 * @remark For more information, see strtoull.
 */
MO_UINT64 SpToUInt64(
    _Mo_In_ MO_CONSTANT_STRING Source,
    _Mo_In_ MO_UINT8 Radix);

#endif // !SP_BASE
