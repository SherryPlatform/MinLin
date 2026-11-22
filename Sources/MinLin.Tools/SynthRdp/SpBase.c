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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

MO_STRING SpVFormatString(
    _Mo_In_ MO_CHAR MO_CONST* MO_CONST Format,
    _Mo_In_ MO_VARIABLE_ARGUMENT_LIST ArgList)
{
    int Length = 0;

    // Get the length of the format result.
    {
        MO_VARIABLE_ARGUMENT_LIST CurrentArgList;
        MO_VARIABLE_ARGUMENT_COPY(CurrentArgList, ArgList);
        Length = vsnprintf(MO_NULL, 0, Format, CurrentArgList);
        MO_VARIABLE_ARGUMENT_END(CurrentArgList);
    }
    if (Length > 0)
    {
        // Allocate for the format result.
        MO_STRING Buffer = (MO_STRING)(SpAllocateMemory(
            (MO_UINTN)(Length) + 1));
        if (Buffer)
        {
            // Format the string.
            MO_VARIABLE_ARGUMENT_LIST CurrentArgList;
            MO_VARIABLE_ARGUMENT_COPY(CurrentArgList, ArgList);
            Length = vsnprintf(
                Buffer,
                (size_t)(Length) + 1,
                Format,
                CurrentArgList);
            MO_VARIABLE_ARGUMENT_END(CurrentArgList);
        }
        if (Length > 0)
        {
            // If succeed, return result.
            return Buffer;
        }
        else
        {
            SpFreeMemory(Buffer);
        }
    }

    // Return MO_NULL on failure or an empty result.
    return MO_NULL;
}

MO_STRING SpFormatString(
    _Mo_In_ MO_CHAR MO_CONST* MO_CONST Format,
    ...)
{
    MO_VARIABLE_ARGUMENT_LIST ArgList;
    MO_VARIABLE_ARGUMENT_START(ArgList, Format);
    MO_STRING Result = SpVFormatString(Format, ArgList);
    MO_VARIABLE_ARGUMENT_END(ArgList);
    return Result;
}

MO_INT64 SpToInt64(
    _Mo_In_ MO_CONSTANT_STRING Source,
    _Mo_In_ MO_UINT8 Radix)
{
    return strtoll(Source, MO_NULL, Radix);
}

MO_UINT64 SpToUInt64(
    _Mo_In_ MO_CONSTANT_STRING Source,
    _Mo_In_ MO_UINT8 Radix)
{
    return strtoull(Source, MO_NULL, Radix);
}
