/*
 * PROJECT:    Sherry Platform
 * FILE:       SpBase.c
 * PURPOSE:    Implementation for the essential POSIX helper functions
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "SpBase.h"

#include <string.h>
#include <stdlib.h>

MO_EXTERN_C MO_POINTER MOAPI SpAllocateMemory(
    _Mo_In_ MO_UINTN Size)
{
    if (!Size)
    {
        return MO_NULL;
    }

    MO_POINTER Block = malloc(Size);
    if (!Block)
    {
        return MO_NULL;
    }

    memset(Block, 0, Size);
    return Block;
}

MO_EXTERN_C MO_VOID MOAPI SpFreeMemory(
    _Mo_In_Opt_ MO_POINTER Block)
{
    free(Block);
}
