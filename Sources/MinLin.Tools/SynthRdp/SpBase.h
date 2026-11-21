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

#endif // !SP_BASE
