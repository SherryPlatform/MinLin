/*
 * PROJECT:    Mouri Internal Library Essentials
 * FILE:       Mile.Mobility.Utilities.FixedInteger.c
 * PURPOSE:    Implementation for Mobility Fixed Width Integer Utilities
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mile.Mobility.Utilities.FixedInteger.h"

/* Check the assumptions on the fixed width integer types. */

MO_C_STATIC_ASSERT(1 == sizeof(MO_UINT8));
MO_C_STATIC_ASSERT(2 == sizeof(MO_UINT16));
MO_C_STATIC_ASSERT(4 == sizeof(MO_UINT32));
MO_C_STATIC_ASSERT(8 == sizeof(MO_UINT64));
MO_C_STATIC_ASSERT(MO_POINTER_SIZE == sizeof(MO_UINTN));

/* Internal macro to compute the sign bit mask. */

#define MO_INTERNAL_SIGN_BIT_MASK(TYPE) \
    ((TYPE)(((TYPE)(1)) << (sizeof(TYPE) * 8 - 1)))

#define MO_INTERNAL_SIGN_BIT_MASK_8 MO_INTERNAL_SIGN_BIT_MASK(MO_UINT8)
#define MO_INTERNAL_SIGN_BIT_MASK_16 MO_INTERNAL_SIGN_BIT_MASK(MO_UINT16)
#define MO_INTERNAL_SIGN_BIT_MASK_32 MO_INTERNAL_SIGN_BIT_MASK(MO_UINT32)
#define MO_INTERNAL_SIGN_BIT_MASK_64 MO_INTERNAL_SIGN_BIT_MASK(MO_UINT64)

MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedAddition8(
    _Mo_Out_ PMO_UINT8 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT8 Left,
    _Mo_In_ MO_UINT8 Right)
{
#define VALUE_TYPE MO_UINT8
#define VALUE_SIGN_BIT_MASK MO_INTERNAL_SIGN_BIT_MASK_8

    if (!Result)
    {
        /* The result pointer must not be null. */
        return MO_FALSE;
    }

    {
        VALUE_TYPE Candidate = Left + Right;

        if (Signed)
        {
            VALUE_TYPE SignBitMask = VALUE_SIGN_BIT_MASK;
            if (0 == ((Left ^ Right) & SignBitMask) &&
                0 != ((Left ^ Candidate) & SignBitMask))
            {
                /* The signed addition overflowed. */
                return MO_FALSE;
            }
        }
        else
        {
            if (Candidate < Left)
            {
                /* The unsigned addition overflowed. */
                return MO_FALSE;
            }
        }

        /* Set the result only when the addition succeeds and the result is */
        /* representable in the range determined by the fixed width and */
        /* Signed. */
        *Result = Candidate;
    }

    return MO_TRUE;

#undef VALUE_TYPE
#undef VALUE_SIGN_BIT_MASK
}

MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedSubtraction8(
    _Mo_Out_ PMO_UINT8 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT8 Left,
    _Mo_In_ MO_UINT8 Right)
{
#define VALUE_TYPE MO_UINT8
#define VALUE_SIGN_BIT_MASK MO_INTERNAL_SIGN_BIT_MASK_8

    if (!Result)
    {
        /* The result pointer must not be null. */
        return MO_FALSE;
    }

    if (!Signed && Left < Right)
    {
        /* The unsigned subtraction underflowed. */
        return MO_FALSE;
    }

    {
        VALUE_TYPE Candidate = Left - Right;

        if (Signed)
        {
            VALUE_TYPE SignBitMask = VALUE_SIGN_BIT_MASK;
            if (0 != ((Left ^ Right) & SignBitMask) &&
                0 != ((Left ^ Candidate) & SignBitMask))
            {
                /* The signed subtraction overflowed. */
                return MO_FALSE;
            }
        }

        /* Set the result only when the subtraction succeeds and the result */
        /* is representable in the range determined by the fixed width and */
        /* Signed. */
        *Result = Candidate;
    }

    return MO_TRUE;

#undef VALUE_TYPE
#undef VALUE_SIGN_BIT_MASK
}

MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedMultiplication8(
    _Mo_Out_ PMO_UINT8 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT8 Left,
    _Mo_In_ MO_UINT8 Right)
{
#define VALUE_TYPE MO_UINT8
#define VALUE_SIGN_BIT_MASK MO_INTERNAL_SIGN_BIT_MASK_8

    if (!Result)
    {
        /* The result pointer must not be null. */
        return MO_FALSE;
    }

    {
        VALUE_TYPE LeftMagnitude = 0;
        VALUE_TYPE RightMagnitude = 0;
        VALUE_TYPE Limit = 0;
        MO_BOOL ResultNegative = MO_FALSE;

        if (Signed)
        {
            VALUE_TYPE SignBitMask = VALUE_SIGN_BIT_MASK;
            MO_BOOL LeftNegative = (Left & SignBitMask) ? MO_TRUE : MO_FALSE;
            MO_BOOL RightNegative = (Right & SignBitMask) ? MO_TRUE : MO_FALSE;
            VALUE_TYPE HalfRange = VALUE_SIGN_BIT_MASK;

            LeftMagnitude = LeftNegative
                ? ((VALUE_TYPE)(0) - Left)
                : Left;

            RightMagnitude = RightNegative
                ? ((VALUE_TYPE)(0) - Right)
                : Right;

            ResultNegative = (LeftNegative != RightNegative)
                ? MO_TRUE
                : MO_FALSE;

            Limit = ResultNegative ? HalfRange : HalfRange - 1;
        }
        else
        {
            LeftMagnitude = Left;
            RightMagnitude = Right;
            Limit = ((VALUE_TYPE)(((VALUE_TYPE)(0)) - ((VALUE_TYPE)(1))));
        }

        {
            VALUE_TYPE ResultMagnitude = 0;

            if (0 == LeftMagnitude || 0 == RightMagnitude)
            {
                ResultMagnitude = 0;
            }
            else
            {
                if (LeftMagnitude > Limit || RightMagnitude > Limit)
                {
                    /* The multiplication result is out of range. */
                    return MO_FALSE;
                }

                if (1 == LeftMagnitude)
                {
                    ResultMagnitude = RightMagnitude;
                }
                else if (1 == RightMagnitude)
                {
                    ResultMagnitude = LeftMagnitude;
                }
                else
                {
                    if (LeftMagnitude > Limit / RightMagnitude)
                    {
                        /* The multiplication result is out of range. */
                        return MO_FALSE;
                    }

                    ResultMagnitude = LeftMagnitude * RightMagnitude;
                }
            }

            /* Set the result only when the multiplication succeeds and the */
            /* result is representable in the range determined by the fixed */
            /* width and Signed. */
            *Result = (Signed && ResultNegative)
                ? (((VALUE_TYPE)(0)) - ResultMagnitude)
                : ResultMagnitude;
        }
    }

    return MO_TRUE;

#undef VALUE_TYPE
#undef VALUE_SIGN_BIT_MASK
}

MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedAddition16(
    _Mo_Out_ PMO_UINT16 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT16 Left,
    _Mo_In_ MO_UINT16 Right)
{
#define VALUE_TYPE MO_UINT16
#define VALUE_SIGN_BIT_MASK MO_INTERNAL_SIGN_BIT_MASK_16

    if (!Result)
    {
        /* The result pointer must not be null. */
        return MO_FALSE;
    }

    {
        VALUE_TYPE Candidate = Left + Right;

        if (Signed)
        {
            VALUE_TYPE SignBitMask = VALUE_SIGN_BIT_MASK;
            if (0 == ((Left ^ Right) & SignBitMask) &&
                0 != ((Left ^ Candidate) & SignBitMask))
            {
                /* The signed addition overflowed. */
                return MO_FALSE;
            }
        }
        else
        {
            if (Candidate < Left)
            {
                /* The unsigned addition overflowed. */
                return MO_FALSE;
            }
        }

        /* Set the result only when the addition succeeds and the result is */
        /* representable in the range determined by the fixed width and */
        /* Signed. */
        *Result = Candidate;
    }

    return MO_TRUE;

#undef VALUE_TYPE
#undef VALUE_SIGN_BIT_MASK
}

MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedSubtraction16(
    _Mo_Out_ PMO_UINT16 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT16 Left,
    _Mo_In_ MO_UINT16 Right)
{
#define VALUE_TYPE MO_UINT16
#define VALUE_SIGN_BIT_MASK MO_INTERNAL_SIGN_BIT_MASK_16

    if (!Result)
    {
        /* The result pointer must not be null. */
        return MO_FALSE;
    }

    if (!Signed && Left < Right)
    {
        /* The unsigned subtraction underflowed. */
        return MO_FALSE;
    }

    {
        VALUE_TYPE Candidate = Left - Right;

        if (Signed)
        {
            VALUE_TYPE SignBitMask = VALUE_SIGN_BIT_MASK;
            if (0 != ((Left ^ Right) & SignBitMask) &&
                0 != ((Left ^ Candidate) & SignBitMask))
            {
                /* The signed subtraction overflowed. */
                return MO_FALSE;
            }
        }

        /* Set the result only when the subtraction succeeds and the result */
        /* is representable in the range determined by the fixed width and */
        /* Signed. */
        *Result = Candidate;
    }

    return MO_TRUE;

#undef VALUE_TYPE
#undef VALUE_SIGN_BIT_MASK
}

MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedMultiplication16(
    _Mo_Out_ PMO_UINT16 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT16 Left,
    _Mo_In_ MO_UINT16 Right)
{
#define VALUE_TYPE MO_UINT16
#define VALUE_SIGN_BIT_MASK MO_INTERNAL_SIGN_BIT_MASK_16

    if (!Result)
    {
        /* The result pointer must not be null. */
        return MO_FALSE;
    }

    {
        VALUE_TYPE LeftMagnitude = 0;
        VALUE_TYPE RightMagnitude = 0;
        VALUE_TYPE Limit = 0;
        MO_BOOL ResultNegative = MO_FALSE;

        if (Signed)
        {
            VALUE_TYPE SignBitMask = VALUE_SIGN_BIT_MASK;
            MO_BOOL LeftNegative = (Left & SignBitMask) ? MO_TRUE : MO_FALSE;
            MO_BOOL RightNegative = (Right & SignBitMask) ? MO_TRUE : MO_FALSE;
            VALUE_TYPE HalfRange = VALUE_SIGN_BIT_MASK;

            LeftMagnitude = LeftNegative
                ? ((VALUE_TYPE)(0) - Left)
                : Left;

            RightMagnitude = RightNegative
                ? ((VALUE_TYPE)(0) - Right)
                : Right;

            ResultNegative = (LeftNegative != RightNegative)
                ? MO_TRUE
                : MO_FALSE;

            Limit = ResultNegative ? HalfRange : HalfRange - 1;
        }
        else
        {
            LeftMagnitude = Left;
            RightMagnitude = Right;
            Limit = ((VALUE_TYPE)(((VALUE_TYPE)(0)) - ((VALUE_TYPE)(1))));
        }

        {
            VALUE_TYPE ResultMagnitude = 0;

            if (0 == LeftMagnitude || 0 == RightMagnitude)
            {
                ResultMagnitude = 0;
            }
            else
            {
                if (LeftMagnitude > Limit || RightMagnitude > Limit)
                {
                    /* The multiplication result is out of range. */
                    return MO_FALSE;
                }

                if (1 == LeftMagnitude)
                {
                    ResultMagnitude = RightMagnitude;
                }
                else if (1 == RightMagnitude)
                {
                    ResultMagnitude = LeftMagnitude;
                }
                else
                {
                    if (LeftMagnitude > Limit / RightMagnitude)
                    {
                        /* The multiplication result is out of range. */
                        return MO_FALSE;
                    }

                    ResultMagnitude = LeftMagnitude * RightMagnitude;
                }
            }

            /* Set the result only when the multiplication succeeds and the */
            /* result is representable in the range determined by the fixed */
            /* width and Signed. */
            *Result = (Signed && ResultNegative)
                ? (((VALUE_TYPE)(0)) - ResultMagnitude)
                : ResultMagnitude;
        }
    }

    return MO_TRUE;

#undef VALUE_TYPE
#undef VALUE_SIGN_BIT_MASK
}

MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedAddition32(
    _Mo_Out_ PMO_UINT32 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT32 Left,
    _Mo_In_ MO_UINT32 Right)
{
#define VALUE_TYPE MO_UINT32
#define VALUE_SIGN_BIT_MASK MO_INTERNAL_SIGN_BIT_MASK_32

    if (!Result)
    {
        /* The result pointer must not be null. */
        return MO_FALSE;
    }

    {
        VALUE_TYPE Candidate = Left + Right;

        if (Signed)
        {
            VALUE_TYPE SignBitMask = VALUE_SIGN_BIT_MASK;
            if (0 == ((Left ^ Right) & SignBitMask) &&
                0 != ((Left ^ Candidate) & SignBitMask))
            {
                /* The signed addition overflowed. */
                return MO_FALSE;
            }
        }
        else
        {
            if (Candidate < Left)
            {
                /* The unsigned addition overflowed. */
                return MO_FALSE;
            }
        }

        /* Set the result only when the addition succeeds and the result is */
        /* representable in the range determined by the fixed width and */
        /* Signed. */
        *Result = Candidate;
    }

    return MO_TRUE;

#undef VALUE_TYPE
#undef VALUE_SIGN_BIT_MASK
}

MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedSubtraction32(
    _Mo_Out_ PMO_UINT32 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT32 Left,
    _Mo_In_ MO_UINT32 Right)
{
#define VALUE_TYPE MO_UINT32
#define VALUE_SIGN_BIT_MASK MO_INTERNAL_SIGN_BIT_MASK_32

    if (!Result)
    {
        /* The result pointer must not be null. */
        return MO_FALSE;
    }

    if (!Signed && Left < Right)
    {
        /* The unsigned subtraction underflowed. */
        return MO_FALSE;
    }

    {
        VALUE_TYPE Candidate = Left - Right;

        if (Signed)
        {
            VALUE_TYPE SignBitMask = VALUE_SIGN_BIT_MASK;
            if (0 != ((Left ^ Right) & SignBitMask) &&
                0 != ((Left ^ Candidate) & SignBitMask))
            {
                /* The signed subtraction overflowed. */
                return MO_FALSE;
            }
        }

        /* Set the result only when the subtraction succeeds and the result */
        /* is representable in the range determined by the fixed width and */
        /* Signed. */
        *Result = Candidate;
    }

    return MO_TRUE;

#undef VALUE_TYPE
#undef VALUE_SIGN_BIT_MASK
}

MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedMultiplication32(
    _Mo_Out_ PMO_UINT32 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT32 Left,
    _Mo_In_ MO_UINT32 Right)
{
#define VALUE_TYPE MO_UINT32
#define VALUE_SIGN_BIT_MASK MO_INTERNAL_SIGN_BIT_MASK_32

    if (!Result)
    {
        /* The result pointer must not be null. */
        return MO_FALSE;
    }

    {
        VALUE_TYPE LeftMagnitude = 0;
        VALUE_TYPE RightMagnitude = 0;
        VALUE_TYPE Limit = 0;
        MO_BOOL ResultNegative = MO_FALSE;

        if (Signed)
        {
            VALUE_TYPE SignBitMask = VALUE_SIGN_BIT_MASK;
            MO_BOOL LeftNegative = (Left & SignBitMask) ? MO_TRUE : MO_FALSE;
            MO_BOOL RightNegative = (Right & SignBitMask) ? MO_TRUE : MO_FALSE;
            VALUE_TYPE HalfRange = VALUE_SIGN_BIT_MASK;

            LeftMagnitude = LeftNegative
                ? ((VALUE_TYPE)(0) - Left)
                : Left;

            RightMagnitude = RightNegative
                ? ((VALUE_TYPE)(0) - Right)
                : Right;

            ResultNegative = (LeftNegative != RightNegative)
                ? MO_TRUE
                : MO_FALSE;

            Limit = ResultNegative ? HalfRange : HalfRange - 1;
        }
        else
        {
            LeftMagnitude = Left;
            RightMagnitude = Right;
            Limit = ((VALUE_TYPE)(((VALUE_TYPE)(0)) - ((VALUE_TYPE)(1))));
        }

        {
            VALUE_TYPE ResultMagnitude = 0;

            if (0 == LeftMagnitude || 0 == RightMagnitude)
            {
                ResultMagnitude = 0;
            }
            else
            {
                if (LeftMagnitude > Limit || RightMagnitude > Limit)
                {
                    /* The multiplication result is out of range. */
                    return MO_FALSE;
                }

                if (1 == LeftMagnitude)
                {
                    ResultMagnitude = RightMagnitude;
                }
                else if (1 == RightMagnitude)
                {
                    ResultMagnitude = LeftMagnitude;
                }
                else
                {
                    if (LeftMagnitude > Limit / RightMagnitude)
                    {
                        /* The multiplication result is out of range. */
                        return MO_FALSE;
                    }

                    ResultMagnitude = LeftMagnitude * RightMagnitude;
                }
            }

            /* Set the result only when the multiplication succeeds and the */
            /* result is representable in the range determined by the fixed */
            /* width and Signed. */
            *Result = (Signed && ResultNegative)
                ? (((VALUE_TYPE)(0)) - ResultMagnitude)
                : ResultMagnitude;
        }
    }

    return MO_TRUE;

#undef VALUE_TYPE
#undef VALUE_SIGN_BIT_MASK
}

MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedAddition64(
    _Mo_Out_ PMO_UINT64 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT64 Left,
    _Mo_In_ MO_UINT64 Right)
{
#define VALUE_TYPE MO_UINT64
#define VALUE_SIGN_BIT_MASK MO_INTERNAL_SIGN_BIT_MASK_64

    if (!Result)
    {
        /* The result pointer must not be null. */
        return MO_FALSE;
    }

    {
        VALUE_TYPE Candidate = Left + Right;

        if (Signed)
        {
            VALUE_TYPE SignBitMask = VALUE_SIGN_BIT_MASK;
            if (0 == ((Left ^ Right) & SignBitMask) &&
                0 != ((Left ^ Candidate) & SignBitMask))
            {
                /* The signed addition overflowed. */
                return MO_FALSE;
            }
        }
        else
        {
            if (Candidate < Left)
            {
                /* The unsigned addition overflowed. */
                return MO_FALSE;
            }
        }

        /* Set the result only when the addition succeeds and the result is */
        /* representable in the range determined by the fixed width and */
        /* Signed. */
        *Result = Candidate;
    }

    return MO_TRUE;

#undef VALUE_TYPE
#undef VALUE_SIGN_BIT_MASK
}

MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedSubtraction64(
    _Mo_Out_ PMO_UINT64 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT64 Left,
    _Mo_In_ MO_UINT64 Right)
{
#define VALUE_TYPE MO_UINT64
#define VALUE_SIGN_BIT_MASK MO_INTERNAL_SIGN_BIT_MASK_64

    if (!Result)
    {
        /* The result pointer must not be null. */
        return MO_FALSE;
    }

    if (!Signed && Left < Right)
    {
        /* The unsigned subtraction underflowed. */
        return MO_FALSE;
    }

    {
        VALUE_TYPE Candidate = Left - Right;

        if (Signed)
        {
            VALUE_TYPE SignBitMask = VALUE_SIGN_BIT_MASK;
            if (0 != ((Left ^ Right) & SignBitMask) &&
                0 != ((Left ^ Candidate) & SignBitMask))
            {
                /* The signed subtraction overflowed. */
                return MO_FALSE;
            }
        }

        /* Set the result only when the subtraction succeeds and the result */
        /* is representable in the range determined by the fixed width and */
        /* Signed. */
        *Result = Candidate;
    }

    return MO_TRUE;

#undef VALUE_TYPE
#undef VALUE_SIGN_BIT_MASK
}

MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedMultiplication64(
    _Mo_Out_ PMO_UINT64 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT64 Left,
    _Mo_In_ MO_UINT64 Right)
{
#define VALUE_TYPE MO_UINT64
#define VALUE_SIGN_BIT_MASK MO_INTERNAL_SIGN_BIT_MASK_64

    if (!Result)
    {
        /* The result pointer must not be null. */
        return MO_FALSE;
    }

    {
        VALUE_TYPE LeftMagnitude = 0;
        VALUE_TYPE RightMagnitude = 0;
        VALUE_TYPE Limit = 0;
        MO_BOOL ResultNegative = MO_FALSE;

        if (Signed)
        {
            VALUE_TYPE SignBitMask = VALUE_SIGN_BIT_MASK;
            MO_BOOL LeftNegative = (Left & SignBitMask) ? MO_TRUE : MO_FALSE;
            MO_BOOL RightNegative = (Right & SignBitMask) ? MO_TRUE : MO_FALSE;
            VALUE_TYPE HalfRange = VALUE_SIGN_BIT_MASK;

            LeftMagnitude = LeftNegative
                ? ((VALUE_TYPE)(0) - Left)
                : Left;

            RightMagnitude = RightNegative
                ? ((VALUE_TYPE)(0) - Right)
                : Right;

            ResultNegative = (LeftNegative != RightNegative)
                ? MO_TRUE
                : MO_FALSE;

            Limit = ResultNegative ? HalfRange : HalfRange - 1;
        }
        else
        {
            LeftMagnitude = Left;
            RightMagnitude = Right;
            Limit = ((VALUE_TYPE)(((VALUE_TYPE)(0)) - ((VALUE_TYPE)(1))));
        }

        {
            VALUE_TYPE ResultMagnitude = 0;

            if (0 == LeftMagnitude || 0 == RightMagnitude)
            {
                ResultMagnitude = 0;
            }
            else
            {
                if (LeftMagnitude > Limit || RightMagnitude > Limit)
                {
                    /* The multiplication result is out of range. */
                    return MO_FALSE;
                }

                if (1 == LeftMagnitude)
                {
                    ResultMagnitude = RightMagnitude;
                }
                else if (1 == RightMagnitude)
                {
                    ResultMagnitude = LeftMagnitude;
                }
                else
                {
                    if (LeftMagnitude > Limit / RightMagnitude)
                    {
                        /* The multiplication result is out of range. */
                        return MO_FALSE;
                    }

                    ResultMagnitude = LeftMagnitude * RightMagnitude;
                }
            }

            /* Set the result only when the multiplication succeeds and the */
            /* result is representable in the range determined by the fixed */
            /* width and Signed. */
            *Result = (Signed && ResultNegative)
                ? (((VALUE_TYPE)(0)) - ResultMagnitude)
                : ResultMagnitude;
        }
    }

    return MO_TRUE;

#undef VALUE_TYPE
#undef VALUE_SIGN_BIT_MASK
}

MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedAddition(
    _Mo_Out_ PMO_UINTN Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINTN Left,
    _Mo_In_ MO_UINTN Right)
{
#if (MO_POINTER_SIZE == 8)
    return MoMileFixedIntegerCheckedAddition64(
        (PMO_UINT64)Result,
        Signed,
        (MO_UINT64)Left,
        (MO_UINT64)Right);
#elif (MO_POINTER_SIZE == 4)
    return MoMileFixedIntegerCheckedAddition32(
        (PMO_UINT32)Result,
        Signed,
        (MO_UINT32)Left,
        (MO_UINT32)Right);
#elif (MO_POINTER_SIZE == 2)
    return MoMileFixedIntegerCheckedAddition16(
        (PMO_UINT16)Result,
        Signed,
        (MO_UINT16)Left,
        (MO_UINT16)Right);
#elif (MO_POINTER_SIZE == 1)
    return MoMileFixedIntegerCheckedAddition8(
        (PMO_UINT8)Result,
        Signed,
        (MO_UINT8)Left,
        (MO_UINT8)Right);
#else
#error "[Mile.Mobility.Utilities.FixedInteger] Unsupported platform."
#endif
}

MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedSubtraction(
    _Mo_Out_ PMO_UINTN Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINTN Left,
    _Mo_In_ MO_UINTN Right)
{
#if (MO_POINTER_SIZE == 8)
    return MoMileFixedIntegerCheckedSubtraction64(
        (PMO_UINT64)Result,
        Signed,
        (MO_UINT64)Left,
        (MO_UINT64)Right);
#elif (MO_POINTER_SIZE == 4)
    return MoMileFixedIntegerCheckedSubtraction32(
        (PMO_UINT32)Result,
        Signed,
        (MO_UINT32)Left,
        (MO_UINT32)Right);
#elif (MO_POINTER_SIZE == 2)
    return MoMileFixedIntegerCheckedSubtraction16(
        (PMO_UINT16)Result,
        Signed,
        (MO_UINT16)Left,
        (MO_UINT16)Right);
#elif (MO_POINTER_SIZE == 1)
    return MoMileFixedIntegerCheckedSubtraction8(
        (PMO_UINT8)Result,
        Signed,
        (MO_UINT8)Left,
        (MO_UINT8)Right);
#else
#error "[Mile.Mobility.Utilities.FixedInteger] Unsupported platform."
#endif
}

MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedMultiplication(
    _Mo_Out_ PMO_UINTN Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINTN Left,
    _Mo_In_ MO_UINTN Right)
{
#if (MO_POINTER_SIZE == 8)
    return MoMileFixedIntegerCheckedMultiplication64(
        (PMO_UINT64)Result,
        Signed,
        (MO_UINT64)Left,
        (MO_UINT64)Right);
#elif (MO_POINTER_SIZE == 4)
    return MoMileFixedIntegerCheckedMultiplication32(
        (PMO_UINT32)Result,
        Signed,
        (MO_UINT32)Left,
        (MO_UINT32)Right);
#elif (MO_POINTER_SIZE == 2)
    return MoMileFixedIntegerCheckedMultiplication16(
        (PMO_UINT16)Result,
        Signed,
        (MO_UINT16)Left,
        (MO_UINT16)Right);
#elif (MO_POINTER_SIZE == 1)
    return MoMileFixedIntegerCheckedMultiplication8(
        (PMO_UINT8)Result,
        Signed,
        (MO_UINT8)Left,
        (MO_UINT8)Right);
#else
#error "[Mile.Mobility.Utilities.FixedInteger] Unsupported platform."
#endif
}

MO_EXTERN_C MO_UINT8 MOAPI MoMileFixedIntegerRead8(
    _Mo_In_ MO_CONSTANT_POINTER BaseAddress)
{
    MO_CONST MO_UINT8* Base = ((MO_CONST MO_UINT8*)(BaseAddress));
    return Base[0];
}

MO_EXTERN_C MO_VOID MOAPI MoMileFixedIntegerWrite8(
    _Mo_Out_ MO_POINTER BaseAddress,
    _Mo_In_ MO_UINT8 Value)
{
    MO_UINT8* Base = ((MO_UINT8*)(BaseAddress));
    Base[0] = Value;
}

MO_EXTERN_C MO_UINT16 MOAPI MoMileFixedIntegerReadBigEndian16(
    _Mo_In_ MO_CONSTANT_POINTER BaseAddress)
{
    MO_CONST MO_UINT8* Base = ((MO_CONST MO_UINT8*)(BaseAddress));

    return
        (((MO_UINT16)(Base[0])) << 8) |
        (((MO_UINT16)(Base[1])) << 0);
}

MO_EXTERN_C MO_VOID MOAPI MoMileFixedIntegerWriteBigEndian16(
    _Mo_Out_ MO_POINTER BaseAddress,
    _Mo_In_ MO_UINT16 Value)
{
    MO_UINT8* Base = ((MO_UINT8*)(BaseAddress));

    Base[0] = ((MO_UINT8)(Value >> 8));
    Base[1] = ((MO_UINT8)(Value >> 0));
}

MO_EXTERN_C MO_UINT16 MOAPI MoMileFixedIntegerReadLittleEndian16(
    _Mo_In_ MO_CONSTANT_POINTER BaseAddress)
{
    MO_CONST MO_UINT8* Base = ((MO_CONST MO_UINT8*)(BaseAddress));

    return
        (((MO_UINT16)(Base[0])) << 0) |
        (((MO_UINT16)(Base[1])) << 8);
}

MO_EXTERN_C MO_VOID MOAPI MoMileFixedIntegerWriteLittleEndian16(
    _Mo_Out_ MO_POINTER BaseAddress,
    _Mo_In_ MO_UINT16 Value)
{
    MO_UINT8* Base = ((MO_UINT8*)(BaseAddress));

    Base[0] = ((MO_UINT8)(Value >> 0));
    Base[1] = ((MO_UINT8)(Value >> 8));
}

MO_EXTERN_C MO_UINT32 MOAPI MoMileFixedIntegerReadBigEndian32(
    _Mo_In_ MO_CONSTANT_POINTER BaseAddress)
{
    MO_CONST MO_UINT8* Base = ((MO_CONST MO_UINT8*)(BaseAddress));

    return
        (((MO_UINT32)(Base[0])) << 24) |
        (((MO_UINT32)(Base[1])) << 16) |
        (((MO_UINT32)(Base[2])) << 8) |
        (((MO_UINT32)(Base[3])) << 0);
}

MO_EXTERN_C MO_VOID MOAPI MoMileFixedIntegerWriteBigEndian32(
    _Mo_Out_ MO_POINTER BaseAddress,
    _Mo_In_ MO_UINT32 Value)
{
    MO_UINT8* Base = ((MO_UINT8*)(BaseAddress));

    Base[0] = ((MO_UINT8)(Value >> 24));
    Base[1] = ((MO_UINT8)(Value >> 16));
    Base[2] = ((MO_UINT8)(Value >> 8));
    Base[3] = ((MO_UINT8)(Value >> 0));
}

MO_EXTERN_C MO_UINT32 MOAPI MoMileFixedIntegerReadLittleEndian32(
    _Mo_In_ MO_CONSTANT_POINTER BaseAddress)
{
    MO_CONST MO_UINT8* Base = ((MO_CONST MO_UINT8*)(BaseAddress));

    return
        (((MO_UINT32)(Base[0])) << 0) |
        (((MO_UINT32)(Base[1])) << 8) |
        (((MO_UINT32)(Base[2])) << 16) |
        (((MO_UINT32)(Base[3])) << 24);
}

MO_EXTERN_C MO_VOID MOAPI MoMileFixedIntegerWriteLittleEndian32(
    _Mo_Out_ MO_POINTER BaseAddress,
    _Mo_In_ MO_UINT32 Value)
{
    MO_UINT8* Base = ((MO_UINT8*)(BaseAddress));

    Base[0] = ((MO_UINT8)(Value >> 0));
    Base[1] = ((MO_UINT8)(Value >> 8));
    Base[2] = ((MO_UINT8)(Value >> 16));
    Base[3] = ((MO_UINT8)(Value >> 24));
}

MO_EXTERN_C MO_UINT64 MOAPI MoMileFixedIntegerReadBigEndian64(
    _Mo_In_ MO_CONSTANT_POINTER BaseAddress)
{
    MO_CONST MO_UINT8* Base = ((MO_CONST MO_UINT8*)(BaseAddress));

    return
        (((MO_UINT64)(Base[0])) << 56) |
        (((MO_UINT64)(Base[1])) << 48) |
        (((MO_UINT64)(Base[2])) << 40) |
        (((MO_UINT64)(Base[3])) << 32) |
        (((MO_UINT64)(Base[4])) << 24) |
        (((MO_UINT64)(Base[5])) << 16) |
        (((MO_UINT64)(Base[6])) << 8) |
        (((MO_UINT64)(Base[7])) << 0);
}

MO_EXTERN_C MO_VOID MOAPI MoMileFixedIntegerWriteBigEndian64(
    _Mo_Out_ MO_POINTER BaseAddress,
    _Mo_In_ MO_UINT64 Value)
{
    MO_UINT8* Base = ((MO_UINT8*)(BaseAddress));

    Base[0] = ((MO_UINT8)(Value >> 56));
    Base[1] = ((MO_UINT8)(Value >> 48));
    Base[2] = ((MO_UINT8)(Value >> 40));
    Base[3] = ((MO_UINT8)(Value >> 32));
    Base[4] = ((MO_UINT8)(Value >> 24));
    Base[5] = ((MO_UINT8)(Value >> 16));
    Base[6] = ((MO_UINT8)(Value >> 8));
    Base[7] = ((MO_UINT8)(Value >> 0));
}

MO_EXTERN_C MO_UINT64 MOAPI MoMileFixedIntegerReadLittleEndian64(
    _Mo_In_ MO_CONSTANT_POINTER BaseAddress)
{
    MO_CONST MO_UINT8* Base = ((MO_CONST MO_UINT8*)(BaseAddress));

    return
        (((MO_UINT64)(Base[0])) << 0) |
        (((MO_UINT64)(Base[1])) << 8) |
        (((MO_UINT64)(Base[2])) << 16) |
        (((MO_UINT64)(Base[3])) << 24) |
        (((MO_UINT64)(Base[4])) << 32) |
        (((MO_UINT64)(Base[5])) << 40) |
        (((MO_UINT64)(Base[6])) << 48) |
        (((MO_UINT64)(Base[7])) << 56);
}

MO_EXTERN_C MO_VOID MOAPI MoMileFixedIntegerWriteLittleEndian64(
    _Mo_Out_ MO_POINTER BaseAddress,
    _Mo_In_ MO_UINT64 Value)
{
    MO_UINT8* Base = ((MO_UINT8*)(BaseAddress));

    Base[0] = ((MO_UINT8)(Value >> 0));
    Base[1] = ((MO_UINT8)(Value >> 8));
    Base[2] = ((MO_UINT8)(Value >> 16));
    Base[3] = ((MO_UINT8)(Value >> 24));
    Base[4] = ((MO_UINT8)(Value >> 32));
    Base[5] = ((MO_UINT8)(Value >> 40));
    Base[6] = ((MO_UINT8)(Value >> 48));
    Base[7] = ((MO_UINT8)(Value >> 56));
}

MO_EXTERN_C MO_UINTN MOAPI MoMileFixedIntegerReadBigEndian(
    _Mo_In_ MO_CONSTANT_POINTER BaseAddress)
{
#if (MO_POINTER_SIZE == 8)
    return (MO_UINTN)MoMileFixedIntegerReadBigEndian64(BaseAddress);
#elif (MO_POINTER_SIZE == 4)
    return (MO_UINTN)MoMileFixedIntegerReadBigEndian32(BaseAddress);
#elif (MO_POINTER_SIZE == 2)
    return (MO_UINTN)MoMileFixedIntegerReadBigEndian16(BaseAddress);
#elif (MO_POINTER_SIZE == 1)
    return (MO_UINTN)MoMileFixedIntegerRead8(BaseAddress);
#else
#error "[Mile.Mobility.Utilities.FixedInteger] Unsupported platform."
#endif
}

MO_EXTERN_C MO_VOID MOAPI MoMileFixedIntegerWriteBigEndian(
    _Mo_Out_ MO_POINTER BaseAddress,
    _Mo_In_ MO_UINTN Value)
{
#if (MO_POINTER_SIZE == 8)
    MoMileFixedIntegerWriteBigEndian64(BaseAddress, (MO_UINT64)Value);
#elif (MO_POINTER_SIZE == 4)
    MoMileFixedIntegerWriteBigEndian32(BaseAddress, (MO_UINT32)Value);
#elif (MO_POINTER_SIZE == 2)
    MoMileFixedIntegerWriteBigEndian16(BaseAddress, (MO_UINT16)Value);
#elif (MO_POINTER_SIZE == 1)
    MoMileFixedIntegerWrite8(BaseAddress, (MO_UINT8)Value);
#else
#error "[Mile.Mobility.Utilities.FixedInteger] Unsupported platform."
#endif
}

MO_EXTERN_C MO_UINTN MOAPI MoMileFixedIntegerReadLittleEndian(
    _Mo_In_ MO_CONSTANT_POINTER BaseAddress)
{
#if (MO_POINTER_SIZE == 8)
    return (MO_UINTN)MoMileFixedIntegerReadLittleEndian64(BaseAddress);
#elif (MO_POINTER_SIZE == 4)
    return (MO_UINTN)MoMileFixedIntegerReadLittleEndian32(BaseAddress);
#elif (MO_POINTER_SIZE == 2)
    return (MO_UINTN)MoMileFixedIntegerReadLittleEndian16(BaseAddress);
#elif (MO_POINTER_SIZE == 1)
    return (MO_UINTN)MoMileFixedIntegerRead8(BaseAddress);
#else
#error "[Mile.Mobility.Utilities.FixedInteger] Unsupported platform."
#endif
}

MO_EXTERN_C MO_VOID MOAPI MoMileFixedIntegerWriteLittleEndian(
    _Mo_Out_ MO_POINTER BaseAddress,
    _Mo_In_ MO_UINTN Value)
{
#if (MO_POINTER_SIZE == 8)
    MoMileFixedIntegerWriteLittleEndian64(BaseAddress, (MO_UINT64)Value);
#elif (MO_POINTER_SIZE == 4)
    MoMileFixedIntegerWriteLittleEndian32(BaseAddress, (MO_UINT32)Value);
#elif (MO_POINTER_SIZE == 2)
    MoMileFixedIntegerWriteLittleEndian16(BaseAddress, (MO_UINT16)Value);
#elif (MO_POINTER_SIZE == 1)
    MoMileFixedIntegerWrite8(BaseAddress, (MO_UINT8)Value);
#else
#error "[Mile.Mobility.Utilities.FixedInteger] Unsupported platform."
#endif
}

MO_EXTERN_C MO_INT8 MOAPI MoMileFixedIntegerSequenceCompare8(
    _Mo_In_ MO_UINT8 Left,
    _Mo_In_ MO_UINT8 Right)
{
#define VALUE_TYPE MO_UINT8
#define SIGNED_VALUE_TYPE MO_INT8
#define VALUE_SIGN_BIT_MASK MO_INTERNAL_SIGN_BIT_MASK_8
#define SIGNED_VALUE_MAXIMUM MO_INT8_MAX
#define SIGNED_VALUE_MINIMUM MO_INT8_MIN

    VALUE_TYPE Difference = Left - Right;

    if (Difference <= ((VALUE_TYPE)(SIGNED_VALUE_MAXIMUM)))
    {
        return ((SIGNED_VALUE_TYPE)(Difference));
    }

    if (Difference == VALUE_SIGN_BIT_MASK)
    {
        return ((SIGNED_VALUE_TYPE)(SIGNED_VALUE_MINIMUM));
    }

    {
        VALUE_TYPE Magnitude = ((VALUE_TYPE)(((VALUE_TYPE)(0)) - Difference));
        return ((SIGNED_VALUE_TYPE)(-((SIGNED_VALUE_TYPE)(Magnitude))));
    }

#undef VALUE_TYPE
#undef SIGNED_VALUE_TYPE
#undef VALUE_SIGN_BIT_MASK
#undef SIGNED_VALUE_MAXIMUM
#undef SIGNED_VALUE_MINIMUM
}

MO_EXTERN_C MO_INT16 MOAPI MoMileFixedIntegerSequenceCompare16(
    _Mo_In_ MO_UINT16 Left,
    _Mo_In_ MO_UINT16 Right)
{
#define VALUE_TYPE MO_UINT16
#define SIGNED_VALUE_TYPE MO_INT16
#define VALUE_SIGN_BIT_MASK MO_INTERNAL_SIGN_BIT_MASK_16
#define SIGNED_VALUE_MAXIMUM MO_INT16_MAX
#define SIGNED_VALUE_MINIMUM MO_INT16_MIN

    VALUE_TYPE Difference = Left - Right;

    if (Difference <= ((VALUE_TYPE)(SIGNED_VALUE_MAXIMUM)))
    {
        return ((SIGNED_VALUE_TYPE)(Difference));
    }

    if (Difference == VALUE_SIGN_BIT_MASK)
    {
        return ((SIGNED_VALUE_TYPE)(SIGNED_VALUE_MINIMUM));
    }

    {
        VALUE_TYPE Magnitude = ((VALUE_TYPE)(((VALUE_TYPE)(0)) - Difference));
        return ((SIGNED_VALUE_TYPE)(-((SIGNED_VALUE_TYPE)(Magnitude))));
    }

#undef VALUE_TYPE
#undef SIGNED_VALUE_TYPE
#undef VALUE_SIGN_BIT_MASK
#undef SIGNED_VALUE_MAXIMUM
#undef SIGNED_VALUE_MINIMUM
}

MO_EXTERN_C MO_INT32 MOAPI MoMileFixedIntegerSequenceCompare32(
    _Mo_In_ MO_UINT32 Left,
    _Mo_In_ MO_UINT32 Right)
{
#define VALUE_TYPE MO_UINT32
#define SIGNED_VALUE_TYPE MO_INT32
#define VALUE_SIGN_BIT_MASK MO_INTERNAL_SIGN_BIT_MASK_32
#define SIGNED_VALUE_MAXIMUM MO_INT32_MAX
#define SIGNED_VALUE_MINIMUM MO_INT32_MIN

    VALUE_TYPE Difference = Left - Right;

    if (Difference <= ((VALUE_TYPE)(SIGNED_VALUE_MAXIMUM)))
    {
        return ((SIGNED_VALUE_TYPE)(Difference));
    }

    if (Difference == VALUE_SIGN_BIT_MASK)
    {
        return ((SIGNED_VALUE_TYPE)(SIGNED_VALUE_MINIMUM));
    }

    {
        VALUE_TYPE Magnitude = ((VALUE_TYPE)(((VALUE_TYPE)(0)) - Difference));
        return ((SIGNED_VALUE_TYPE)(-((SIGNED_VALUE_TYPE)(Magnitude))));
    }

#undef VALUE_TYPE
#undef SIGNED_VALUE_TYPE
#undef VALUE_SIGN_BIT_MASK
#undef SIGNED_VALUE_MAXIMUM
#undef SIGNED_VALUE_MINIMUM
}

MO_EXTERN_C MO_INT64 MOAPI MoMileFixedIntegerSequenceCompare64(
    _Mo_In_ MO_UINT64 Left,
    _Mo_In_ MO_UINT64 Right)
{
#define VALUE_TYPE MO_UINT64
#define SIGNED_VALUE_TYPE MO_INT64
#define VALUE_SIGN_BIT_MASK MO_INTERNAL_SIGN_BIT_MASK_64
#define SIGNED_VALUE_MAXIMUM MO_INT64_MAX
#define SIGNED_VALUE_MINIMUM MO_INT64_MIN

    VALUE_TYPE Difference = Left - Right;

    if (Difference <= ((VALUE_TYPE)(SIGNED_VALUE_MAXIMUM)))
    {
        return ((SIGNED_VALUE_TYPE)(Difference));
    }

    if (Difference == VALUE_SIGN_BIT_MASK)
    {
        return ((SIGNED_VALUE_TYPE)(SIGNED_VALUE_MINIMUM));
    }

    {
        VALUE_TYPE Magnitude = ((VALUE_TYPE)(((VALUE_TYPE)(0)) - Difference));
        return ((SIGNED_VALUE_TYPE)(-((SIGNED_VALUE_TYPE)(Magnitude))));
    }

#undef VALUE_TYPE
#undef SIGNED_VALUE_TYPE
#undef VALUE_SIGN_BIT_MASK
#undef SIGNED_VALUE_MAXIMUM
#undef SIGNED_VALUE_MINIMUM
}

MO_EXTERN_C MO_INTN MOAPI MoMileFixedIntegerSequenceCompare(
    _Mo_In_ MO_UINTN Left,
    _Mo_In_ MO_UINTN Right)
{
#if (MO_POINTER_SIZE == 8)
    return ((MO_INTN)(MoMileFixedIntegerSequenceCompare64(
        ((MO_UINT64)(Left)),
        ((MO_UINT64)(Right)))));
#elif (MO_POINTER_SIZE == 4)
    return ((MO_INTN)(MoMileFixedIntegerSequenceCompare32(
        ((MO_UINT32)(Left)),
        ((MO_UINT32)(Right)))));
#elif (MO_POINTER_SIZE == 2)
    return ((MO_INTN)(MoMileFixedIntegerSequenceCompare16(
        ((MO_UINT16)(Left)),
        ((MO_UINT16)(Right)))));
#elif (MO_POINTER_SIZE == 1)
    return ((MO_INTN)(MoMileFixedIntegerSequenceCompare8(
        ((MO_UINT8)(Left)),
        ((MO_UINT8)(Right)))));
#else
#error "[Mile.Mobility.Utilities.FixedInteger] Unsupported platform."
#endif
}
