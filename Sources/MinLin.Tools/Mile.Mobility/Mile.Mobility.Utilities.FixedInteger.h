/*
 * PROJECT:    Mouri Internal Library Essentials
 * FILE:       Mile.Mobility.Utilities.FixedInteger.h
 * PURPOSE:    Definition for Mobility Fixed Width Integer Utilities
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MILE_MOBILITY_UTILITIES_FIXEDINTEGER
#define MILE_MOBILITY_UTILITIES_FIXEDINTEGER

#include "Mile.Mobility.Portable.Types.h"

/*
 * - All functions in this header file are C functions.
 * - All functions are prefixed with "MoMileFixedInteger".
 * - Only depends on Mile.Mobility.Portable.Types.h.
 */

/**
  * @brief Performs checked addition of two integer values with a fixed 8-bit
  *        width.
  * @param Result The pointer to the variable that receives the result of
  *               (Left + Right). Result must not be null. If the return value
  *               is MO_FALSE, the value pointed to by Result is not modified.
  * @param Signed Indicates whether the fixed 8-bit integer values are signed
  *               (MO_TRUE) or unsigned (MO_FALSE). If MO_FALSE, the valid range
  *               is 0 to (2 ^ 8) - 1. If MO_TRUE, Left and Right are
  *               interpreted as signed integer values represented as 8-bit
  *               unsigned integer values; a negative value -X is represented as
  *               the 8-bit unsigned integer value (2 ^ 8) - X, and the valid
  *               range is -(2 ^ 7) to (2 ^ 7) - 1. The result is returned using
  *               the same representation.
  * @param Left The first operand.
  * @param Right The second operand.
  * @return MO_TRUE if the addition succeeds and the result is representable in
  *         the range determined by the fixed 8-bit width and Signed. MO_FALSE
  *         if Result is null or the result is out of range.
  */
MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedAddition8(
    _Mo_Out_ PMO_UINT8 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT8 Left,
    _Mo_In_ MO_UINT8 Right);

/**
  * @brief Performs checked subtraction of two integer values with a fixed 8-bit
  *        width.
  * @param Result The pointer to the variable that receives the result of
  *               (Left - Right). Result must not be null. If the return value
  *               is MO_FALSE, the value pointed to by Result is not modified.
  * @param Signed Indicates whether the fixed 8-bit integer values are signed
  *               (MO_TRUE) or unsigned (MO_FALSE). If MO_FALSE, the valid range
  *               is 0 to (2 ^ 8) - 1. If MO_TRUE, Left and Right are
  *               interpreted as signed integer values represented as 8-bit
  *               unsigned integer values; a negative value -X is represented as
  *               the 8-bit unsigned integer value (2 ^ 8) - X, and the valid
  *               range is -(2 ^ 7) to (2 ^ 7) - 1. The result is returned using
  *               the same representation.
  * @param Left The first operand.
  * @param Right The second operand.
  * @return MO_TRUE if the subtraction succeeds and the result is representable
  *         in the range determined by the fixed 8-bit width and Signed.
  *         MO_FALSE if Result is null or the result is out of range.
  */
MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedSubtraction8(
    _Mo_Out_ PMO_UINT8 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT8 Left,
    _Mo_In_ MO_UINT8 Right);

/**
  * @brief Performs checked multiplication of two integer values with a fixed
  *        8-bit width.
  * @param Result The pointer to the variable that receives the result of
  *               (Left * Right). Result must not be null. If the return value
  *               is MO_FALSE, the value pointed to by Result is not modified.
  * @param Signed Indicates whether the fixed 8-bit integer values are signed
  *               (MO_TRUE) or unsigned (MO_FALSE). If MO_FALSE, the valid range
  *               is 0 to (2 ^ 8) - 1. If MO_TRUE, Left and Right are
  *               interpreted as signed integer values represented as 8-bit
  *               unsigned integer values; a negative value -X is represented as
  *               the 8-bit unsigned integer value (2 ^ 8) - X, and the valid
  *               range is -(2 ^ 7) to (2 ^ 7) - 1. The result is returned using
  *               the same representation.
  * @param Left The first operand.
  * @param Right The second operand.
  * @return MO_TRUE if the multiplication succeeds and the result is
  *         representable in the range determined by the fixed 8-bit width and
  *         Signed. MO_FALSE if Result is null or the result is out of range.
  */
MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedMultiplication8(
    _Mo_Out_ PMO_UINT8 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT8 Left,
    _Mo_In_ MO_UINT8 Right);

/**
  * @brief Performs checked addition of two integer values with a fixed 16-bit
  *        width.
  * @param Result The pointer to the variable that receives the result of
  *               (Left + Right). Result must not be null. If the return value
  *               is MO_FALSE, the value pointed to by Result is not modified.
  * @param Signed Indicates whether the fixed 16-bit integer values are signed
  *               (MO_TRUE) or unsigned (MO_FALSE). If MO_FALSE, the valid range
  *               is 0 to (2 ^ 16) - 1. If MO_TRUE, Left and Right are
  *               interpreted as signed integer values represented as 16-bit
  *               unsigned integer values; a negative value -X is represented as
  *               the 16-bit unsigned integer value (2 ^ 16) - X, and the valid
  *               range is -(2 ^ 15) to (2 ^ 15) - 1. The result is returned
  *               using the same representation.
  * @param Left The first operand.
  * @param Right The second operand.
  * @return MO_TRUE if the addition succeeds and the result is representable in
  *         the range determined by the fixed 16-bit width and Signed. MO_FALSE
  *         if Result is null or the result is out of range.
  */
MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedAddition16(
    _Mo_Out_ PMO_UINT16 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT16 Left,
    _Mo_In_ MO_UINT16 Right);

/**
  * @brief Performs checked subtraction of two integer values with a fixed
  *        16-bit width.
  * @param Result The pointer to the variable that receives the result of
  *               (Left - Right). Result must not be null. If the return value
  *               is MO_FALSE, the value pointed to by Result is not modified.
  * @param Signed Indicates whether the fixed 16-bit integer values are signed
  *               (MO_TRUE) or unsigned (MO_FALSE). If MO_FALSE, the valid range
  *               is 0 to (2 ^ 16) - 1. If MO_TRUE, Left and Right are
  *               interpreted as signed integer values represented as 16-bit
  *               unsigned integer values; a negative value -X is represented as
  *               the 16-bit unsigned integer value (2 ^ 16) - X, and the valid
  *               range is -(2 ^ 15) to (2 ^ 15) - 1. The result is returned
  *               using the same representation.
  * @param Left The first operand.
  * @param Right The second operand.
  * @return MO_TRUE if the subtraction succeeds and the result is representable
  *         in the range determined by the fixed 16-bit width and Signed.
  *         MO_FALSE if Result is null or the result is out of range.
  */
MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedSubtraction16(
    _Mo_Out_ PMO_UINT16 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT16 Left,
    _Mo_In_ MO_UINT16 Right);

/**
  * @brief Performs checked multiplication of two integer values with a fixed
  *        16-bit width.
  * @param Result The pointer to the variable that receives the result of
  *               (Left * Right). Result must not be null. If the return value
  *               is MO_FALSE, the value pointed to by Result is not modified.
  * @param Signed Indicates whether the fixed 16-bit integer values are signed
  *               (MO_TRUE) or unsigned (MO_FALSE). If MO_FALSE, the valid range
  *               is 0 to (2 ^ 16) - 1. If MO_TRUE, Left and Right are
  *               interpreted as signed integer values represented as 16-bit
  *               unsigned integer values; a negative value -X is represented as
  *               the 16-bit unsigned integer value (2 ^ 16) - X, and the valid
  *               range is -(2 ^ 15) to (2 ^ 15) - 1. The result is returned
  *               using the same representation.
  * @param Left The first operand.
  * @param Right The second operand.
  * @return MO_TRUE if the multiplication succeeds and the result is
  *         representable in the range determined by the fixed 16-bit width and
  *         Signed. MO_FALSE if Result is null or the result is out of range.
  */
MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedMultiplication16(
    _Mo_Out_ PMO_UINT16 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT16 Left,
    _Mo_In_ MO_UINT16 Right);

/**
  * @brief Performs checked addition of two integer values with a fixed 32-bit
  *        width.
  * @param Result The pointer to the variable that receives the result of
  *               (Left + Right). Result must not be null. If the return value
  *               is MO_FALSE, the value pointed to by Result is not modified.
  * @param Signed Indicates whether the fixed 32-bit integer values are signed
  *               (MO_TRUE) or unsigned (MO_FALSE). If MO_FALSE, the valid range
  *               is 0 to (2 ^ 32) - 1. If MO_TRUE, Left and Right are
  *               interpreted as signed integer values represented as 32-bit
  *               unsigned integer values; a negative value -X is represented as
  *               the 32-bit unsigned integer value (2 ^ 32) - X, and the valid
  *               range is -(2 ^ 31) to (2 ^ 31) - 1. The result is returned
  *               using the same representation.
  * @param Left The first operand.
  * @param Right The second operand.
  * @return MO_TRUE if the addition succeeds and the result is representable in
  *         the range determined by the fixed 32-bit width and Signed. MO_FALSE
  *         if Result is null or the result is out of range.
  */
MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedAddition32(
    _Mo_Out_ PMO_UINT32 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT32 Left,
    _Mo_In_ MO_UINT32 Right);

/**
  * @brief Performs checked subtraction of two integer values with a fixed
  *        32-bit width.
  * @param Result The pointer to the variable that receives the result of
  *               (Left - Right). Result must not be null. If the return value
  *               is MO_FALSE, the value pointed to by Result is not modified.
  * @param Signed Indicates whether the fixed 32-bit integer values are signed
  *               (MO_TRUE) or unsigned (MO_FALSE). If MO_FALSE, the valid range
  *               is 0 to (2 ^ 32) - 1. If MO_TRUE, Left and Right are
  *               interpreted as signed integer values represented as 32-bit
  *               unsigned integer values; a negative value -X is represented as
  *               the 32-bit unsigned integer value (2 ^ 32) - X, and the valid
  *               range is -(2 ^ 31) to (2 ^ 31) - 1. The result is returned
  *               using the same representation.
  * @param Left The first operand.
  * @param Right The second operand.
  * @return MO_TRUE if the subtraction succeeds and the result is representable
  *         in the range determined by the fixed 32-bit width and Signed.
  *         MO_FALSE if Result is null or the result is out of range.
  */
MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedSubtraction32(
    _Mo_Out_ PMO_UINT32 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT32 Left,
    _Mo_In_ MO_UINT32 Right);

/**
  * @brief Performs checked multiplication of two integer values with a fixed
  *        32-bit width.
  * @param Result The pointer to the variable that receives the result of
  *               (Left * Right). Result must not be null. If the return value
  *               is MO_FALSE, the value pointed to by Result is not modified.
  * @param Signed Indicates whether the fixed 32-bit integer values are signed
  *               (MO_TRUE) or unsigned (MO_FALSE). If MO_FALSE, the valid range
  *               is 0 to (2 ^ 32) - 1. If MO_TRUE, Left and Right are
  *               interpreted as signed integer values represented as 32-bit
  *               unsigned integer values; a negative value -X is represented as
  *               the 32-bit unsigned integer value (2 ^ 32) - X, and the valid
  *               range is -(2 ^ 31) to (2 ^ 31) - 1. The result is returned
  *               using the same representation.
  * @param Left The first operand.
  * @param Right The second operand.
  * @return MO_TRUE if the multiplication succeeds and the result is
  *         representable in the range determined by the fixed 32-bit width and
  *         Signed. MO_FALSE if Result is null or the result is out of range.
  */
MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedMultiplication32(
    _Mo_Out_ PMO_UINT32 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT32 Left,
    _Mo_In_ MO_UINT32 Right);

/**
  * @brief Performs checked addition of two integer values with a fixed 64-bit
  *        width.
  * @param Result The pointer to the variable that receives the result of
  *               (Left + Right). Result must not be null. If the return value
  *               is MO_FALSE, the value pointed to by Result is not modified.
  * @param Signed Indicates whether the fixed 64-bit integer values are signed
  *               (MO_TRUE) or unsigned (MO_FALSE). If MO_FALSE, the valid range
  *               is 0 to (2 ^ 64) - 1. If MO_TRUE, Left and Right are
  *               interpreted as signed integer values represented as 64-bit
  *               unsigned integer values; a negative value -X is represented as
  *               the 64-bit unsigned integer value (2 ^ 64) - X, and the valid
  *               range is -(2 ^ 63) to (2 ^ 63) - 1. The result is returned
  *               using the same representation.
  * @param Left The first operand.
  * @param Right The second operand.
  * @return MO_TRUE if the addition succeeds and the result is representable in
  *         the range determined by the fixed 64-bit width and Signed. MO_FALSE
  *         if Result is null or the result is out of range.
  */
MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedAddition64(
    _Mo_Out_ PMO_UINT64 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT64 Left,
    _Mo_In_ MO_UINT64 Right);

/**
  * @brief Performs checked subtraction of two integer values with a fixed
  *        64-bit width.
  * @param Result The pointer to the variable that receives the result of
  *               (Left - Right). Result must not be null. If the return value
  *               is MO_FALSE, the value pointed to by Result is not modified.
  * @param Signed Indicates whether the fixed 64-bit integer values are signed
  *               (MO_TRUE) or unsigned (MO_FALSE). If MO_FALSE, the valid range
  *               is 0 to (2 ^ 64) - 1. If MO_TRUE, Left and Right are
  *               interpreted as signed integer values represented as 64-bit
  *               unsigned integer values; a negative value -X is represented as
  *               the 64-bit unsigned integer value (2 ^ 64) - X, and the valid
  *               range is -(2 ^ 63) to (2 ^ 63) - 1. The result is returned
  *               using the same representation.
  * @param Left The first operand.
  * @param Right The second operand.
  * @return MO_TRUE if the subtraction succeeds and the result is representable
  *         in the range determined by the fixed 64-bit width and Signed.
  *         MO_FALSE if Result is null or the result is out of range.
  */
MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedSubtraction64(
    _Mo_Out_ PMO_UINT64 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT64 Left,
    _Mo_In_ MO_UINT64 Right);

/**
  * @brief Performs checked multiplication of two integer values with a fixed
  *        64-bit width.
  * @param Result The pointer to the variable that receives the result of
  *               (Left * Right). Result must not be null. If the return value
  *               is MO_FALSE, the value pointed to by Result is not modified.
  * @param Signed Indicates whether the fixed 64-bit integer values are signed
  *               (MO_TRUE) or unsigned (MO_FALSE). If MO_FALSE, the valid range
  *               is 0 to (2 ^ 64) - 1. If MO_TRUE, Left and Right are
  *               interpreted as signed integer values represented as 64-bit
  *               unsigned integer values; a negative value -X is represented as
  *               the 64-bit unsigned integer value (2 ^ 64) - X, and the valid
  *               range is -(2 ^ 63) to (2 ^ 63) - 1. The result is returned
  *               using the same representation.
  * @param Left The first operand.
  * @param Right The second operand.
  * @return MO_TRUE if the multiplication succeeds and the result is
  *         representable in the range determined by the fixed 64-bit width and
  *         Signed. MO_FALSE if Result is null or the result is out of range.
  */
MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedMultiplication64(
    _Mo_Out_ PMO_UINT64 Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINT64 Left,
    _Mo_In_ MO_UINT64 Right);

/**
  * @brief Performs checked addition of two integer values with a fixed native
  *        width.
  * @param Result The pointer to the variable that receives the result of
  *               (Left + Right). Result must not be null. If the return value
  *               is MO_FALSE, the value pointed to by Result is not modified.
  * @param Signed Indicates whether the fixed native-width integer values are
  *               signed (MO_TRUE) or unsigned (MO_FALSE). If MO_FALSE, the valid
  *               range is 0 to (2 ^ N) - 1, where N is the number of bits in
  *               MO_UINTN. If MO_TRUE, Left and Right are interpreted as signed
  *               integer values represented as native-width unsigned integer
  *               values; a negative value -X is represented as the native-width
  *               unsigned integer value (2 ^ N) - X, and the valid range is
  *               -(2 ^ (N - 1)) to (2 ^ (N - 1)) - 1. The result is returned
  *               using the same representation.
  * @param Left The first operand.
  * @param Right The second operand.
  * @return MO_TRUE if the addition succeeds and the result is representable in
  *         the range determined by the fixed native width and Signed. MO_FALSE
  *         if Result is null or the result is out of range.
  */
MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedAddition(
    _Mo_Out_ PMO_UINTN Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINTN Left,
    _Mo_In_ MO_UINTN Right);

/**
  * @brief Performs checked subtraction of two integer values with a fixed
  *        native width.
  * @param Result The pointer to the variable that receives the result of
  *               (Left - Right). Result must not be null. If the return value
  *               is MO_FALSE, the value pointed to by Result is not modified.
  * @param Signed Indicates whether the fixed native-width integer values are
  *               signed (MO_TRUE) or unsigned (MO_FALSE). If MO_FALSE, the valid
  *               range is 0 to (2 ^ N) - 1, where N is the number of bits in
  *               MO_UINTN. If MO_TRUE, Left and Right are interpreted as signed
  *               integer values represented as native-width unsigned integer
  *               values; a negative value -X is represented as the native-width
  *               unsigned integer value (2 ^ N) - X, and the valid range is
  *               -(2 ^ (N - 1)) to (2 ^ (N - 1)) - 1. The result is returned
  *               using the same representation.
  * @param Left The first operand.
  * @param Right The second operand.
  * @return MO_TRUE if the subtraction succeeds and the result is representable
  *         in the range determined by the fixed native width and Signed.
  *         MO_FALSE if Result is null or the result is out of range.
  */
MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedSubtraction(
    _Mo_Out_ PMO_UINTN Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINTN Left,
    _Mo_In_ MO_UINTN Right);

/**
  * @brief Performs checked multiplication of two integer values with a fixed
  *        native width.
  * @param Result The pointer to the variable that receives the result of
  *               (Left * Right). Result must not be null. If the return value
  *               is MO_FALSE, the value pointed to by Result is not modified.
  * @param Signed Indicates whether the fixed native-width integer values are
  *               signed (MO_TRUE) or unsigned (MO_FALSE). If MO_FALSE, the valid
  *               range is 0 to (2 ^ N) - 1, where N is the number of bits in
  *               MO_UINTN. If MO_TRUE, Left and Right are interpreted as signed
  *               integer values represented as native-width unsigned integer
  *               values; a negative value -X is represented as the native-width
  *               unsigned integer value (2 ^ N) - X, and the valid range is
  *               -(2 ^ (N - 1)) to (2 ^ (N - 1)) - 1. The result is returned
  *               using the same representation.
  * @param Left The first operand.
  * @param Right The second operand.
  * @return MO_TRUE if the multiplication succeeds and the result is
  *         representable in the range determined by the fixed native width and
  *         Signed. MO_FALSE if Result is null or the result is out of range.
  */
MO_EXTERN_C MO_BOOL MOAPI MoMileFixedIntegerCheckedMultiplication(
    _Mo_Out_ PMO_UINTN Result,
    _Mo_In_ MO_BOOL Signed,
    _Mo_In_ MO_UINTN Left,
    _Mo_In_ MO_UINTN Right);

/**
  * @brief Reads an integer value with a fixed 8-bit width from memory.
  * @param BaseAddress The address of at least 1 readable byte. The address must
  *                    be valid; this function does not validate it.
  * @return The integer value with a fixed 8-bit width read from BaseAddress.
  */
MO_EXTERN_C MO_UINT8 MOAPI MoMileFixedIntegerRead8(
    _Mo_In_ MO_CONSTANT_POINTER BaseAddress);

/**
  * @brief Writes an integer value with a fixed 8-bit width to memory.
  * @param BaseAddress The address of at least 1 writable byte. The address must
  *                    be valid; this function does not validate it.
  * @param Value The integer value with a fixed 8-bit width to write.
  */
MO_EXTERN_C MO_VOID MOAPI MoMileFixedIntegerWrite8(
    _Mo_Out_ MO_POINTER BaseAddress,
    _Mo_In_ MO_UINT8 Value);

/**
  * @brief Reads a big-endian integer value with a fixed 16-bit width from
  *        memory.
  * @param BaseAddress The address of at least 2 readable bytes. The address must
  *                    be valid; this function does not validate it.
  * @return The big-endian integer value with a fixed 16-bit width read from
  *         BaseAddress.
  */
MO_EXTERN_C MO_UINT16 MOAPI MoMileFixedIntegerReadBigEndian16(
    _Mo_In_ MO_CONSTANT_POINTER BaseAddress);

/**
  * @brief Writes a big-endian integer value with a fixed 16-bit width to memory.
  * @param BaseAddress The address of at least 2 writable bytes. The address must
  *                    be valid; this function does not validate it.
  * @param Value The integer value with a fixed 16-bit width to write in
  *              big-endian byte order.
  */
MO_EXTERN_C MO_VOID MOAPI MoMileFixedIntegerWriteBigEndian16(
    _Mo_Out_ MO_POINTER BaseAddress,
    _Mo_In_ MO_UINT16 Value);

/**
  * @brief Reads a little-endian integer value with a fixed 16-bit width from
  *        memory.
  * @param BaseAddress The address of at least 2 readable bytes. The address must
  *                    be valid; this function does not validate it.
  * @return The little-endian integer value with a fixed 16-bit width read from
  *         BaseAddress.
  */
MO_EXTERN_C MO_UINT16 MOAPI MoMileFixedIntegerReadLittleEndian16(
    _Mo_In_ MO_CONSTANT_POINTER BaseAddress);

/**
  * @brief Writes a little-endian integer value with a fixed 16-bit width to
  *        memory.
  * @param BaseAddress The address of at least 2 writable bytes. The address must
  *                    be valid; this function does not validate it.
  * @param Value The integer value with a fixed 16-bit width to write in
  *              little-endian byte order.
  */
MO_EXTERN_C MO_VOID MOAPI MoMileFixedIntegerWriteLittleEndian16(
    _Mo_Out_ MO_POINTER BaseAddress,
    _Mo_In_ MO_UINT16 Value);

/**
  * @brief Reads a big-endian integer value with a fixed 32-bit width from
  *        memory.
  * @param BaseAddress The address of at least 4 readable bytes. The address must
  *                    be valid; this function does not validate it.
  * @return The big-endian integer value with a fixed 32-bit width read from
  *         BaseAddress.
  */
MO_EXTERN_C MO_UINT32 MOAPI MoMileFixedIntegerReadBigEndian32(
    _Mo_In_ MO_CONSTANT_POINTER BaseAddress);

/**
  * @brief Writes a big-endian integer value with a fixed 32-bit width to memory.
  * @param BaseAddress The address of at least 4 writable bytes. The address must
  *                    be valid; this function does not validate it.
  * @param Value The integer value with a fixed 32-bit width to write in
  *              big-endian byte order.
  */
MO_EXTERN_C MO_VOID MOAPI MoMileFixedIntegerWriteBigEndian32(
    _Mo_Out_ MO_POINTER BaseAddress,
    _Mo_In_ MO_UINT32 Value);

/**
  * @brief Reads a little-endian integer value with a fixed 32-bit width from
  *        memory.
  * @param BaseAddress The address of at least 4 readable bytes. The address must
  *                    be valid; this function does not validate it.
  * @return The little-endian integer value with a fixed 32-bit width read from
  *         BaseAddress.
  */
MO_EXTERN_C MO_UINT32 MOAPI MoMileFixedIntegerReadLittleEndian32(
    _Mo_In_ MO_CONSTANT_POINTER BaseAddress);

/**
  * @brief Writes a little-endian integer value with a fixed 32-bit width to
  *        memory.
  * @param BaseAddress The address of at least 4 writable bytes. The address must
  *                    be valid; this function does not validate it.
  * @param Value The integer value with a fixed 32-bit width to write in
  *              little-endian byte order.
  */
MO_EXTERN_C MO_VOID MOAPI MoMileFixedIntegerWriteLittleEndian32(
    _Mo_Out_ MO_POINTER BaseAddress,
    _Mo_In_ MO_UINT32 Value);

/**
  * @brief Reads a big-endian integer value with a fixed 64-bit width from
  *        memory.
  * @param BaseAddress The address of at least 8 readable bytes. The address must
  *                    be valid; this function does not validate it.
  * @return The big-endian integer value with a fixed 64-bit width read from
  *         BaseAddress.
  */
MO_EXTERN_C MO_UINT64 MOAPI MoMileFixedIntegerReadBigEndian64(
    _Mo_In_ MO_CONSTANT_POINTER BaseAddress);

/**
  * @brief Writes a big-endian integer value with a fixed 64-bit width to memory.
  * @param BaseAddress The address of at least 8 writable bytes. The address must
  *                    be valid; this function does not validate it.
  * @param Value The integer value with a fixed 64-bit width to write in
  *              big-endian byte order.
  */
MO_EXTERN_C MO_VOID MOAPI MoMileFixedIntegerWriteBigEndian64(
    _Mo_Out_ MO_POINTER BaseAddress,
    _Mo_In_ MO_UINT64 Value);

/**
  * @brief Reads a little-endian integer value with a fixed 64-bit width from
  *        memory.
  * @param BaseAddress The address of at least 8 readable bytes. The address must
  *                    be valid; this function does not validate it.
  * @return The little-endian integer value with a fixed 64-bit width read from
  *         BaseAddress.
  */
MO_EXTERN_C MO_UINT64 MOAPI MoMileFixedIntegerReadLittleEndian64(
    _Mo_In_ MO_CONSTANT_POINTER BaseAddress);

/**
  * @brief Writes a little-endian integer value with a fixed 64-bit width to
  *        memory.
  * @param BaseAddress The address of at least 8 writable bytes. The address must
  *                    be valid; this function does not validate it.
  * @param Value The integer value with a fixed 64-bit width to write in
  *              little-endian byte order.
  */
MO_EXTERN_C MO_VOID MOAPI MoMileFixedIntegerWriteLittleEndian64(
    _Mo_Out_ MO_POINTER BaseAddress,
    _Mo_In_ MO_UINT64 Value);

/**
  * @brief Reads a big-endian integer value with a fixed native width from
  *        memory.
  * @param BaseAddress The address of a readable native-width byte sequence. The
  *                    address must be valid; this function does not validate it.
  * @return The big-endian integer value with a fixed native width read from
  *         BaseAddress.
  */
MO_EXTERN_C MO_UINTN MOAPI MoMileFixedIntegerReadBigEndian(
    _Mo_In_ MO_CONSTANT_POINTER BaseAddress);

/**
  * @brief Writes a big-endian integer value with a fixed native width to memory.
  * @param BaseAddress The address of a writable native-width byte sequence. The
  *                    address must be valid; this function does not validate it.
  * @param Value The integer value with a fixed native width to write in
  *              big-endian byte order.
  */
MO_EXTERN_C MO_VOID MOAPI MoMileFixedIntegerWriteBigEndian(
    _Mo_Out_ MO_POINTER BaseAddress,
    _Mo_In_ MO_UINTN Value);

/**
  * @brief Reads a little-endian integer value with a fixed native width from
  *        memory.
  * @param BaseAddress The address of a readable native-width byte sequence. The
  *                    address must be valid; this function does not validate it.
  * @return The little-endian integer value with a fixed native width read from
  *         BaseAddress.
  */
MO_EXTERN_C MO_UINTN MOAPI MoMileFixedIntegerReadLittleEndian(
    _Mo_In_ MO_CONSTANT_POINTER BaseAddress);

/**
  * @brief Writes a little-endian integer value with a fixed native width to
  *        memory.
  * @param BaseAddress The address of a writable native-width byte sequence. The
  *                    address must be valid; this function does not validate it.
  * @param Value The integer value with a fixed native width to write in
  *              little-endian byte order.
  */
MO_EXTERN_C MO_VOID MOAPI MoMileFixedIntegerWriteLittleEndian(
    _Mo_Out_ MO_POINTER BaseAddress,
    _Mo_In_ MO_UINTN Value);

/**
  * @brief Compares two sequence values with a fixed 8-bit width.
  * @param Left The left sequence value.
  * @param Right The right sequence value.
  * @return The signed 8-bit wrapping distance from Right to Left, computed as
  *         (Left - Right) modulo (2 ^ 8). A positive value means Left is after
  *         Right, a negative value means Left is before Right, and zero means
  *         they are equal, provided that the actual distance is less than
  *         (2 ^ 7). The exact half-range distance is ambiguous and returns
  *         -(2 ^ 7).
  */
MO_EXTERN_C MO_INT8 MOAPI MoMileFixedIntegerSequenceCompare8(
    _Mo_In_ MO_UINT8 Left,
    _Mo_In_ MO_UINT8 Right);

/**
  * @brief Compares two sequence values with a fixed 16-bit width.
  * @param Left The left sequence value.
  * @param Right The right sequence value.
  * @return The signed 16-bit wrapping distance from Right to Left, computed as
  *         (Left - Right) modulo (2 ^ 16). A positive value means Left is after
  *         Right, a negative value means Left is before Right, and zero means
  *         they are equal, provided that the actual distance is less than
  *         (2 ^ 15). The exact half-range distance is ambiguous and returns
  *         -(2 ^ 15).
  */
MO_EXTERN_C MO_INT16 MOAPI MoMileFixedIntegerSequenceCompare16(
    _Mo_In_ MO_UINT16 Left,
    _Mo_In_ MO_UINT16 Right);

/**
  * @brief Compares two sequence values with a fixed 32-bit width.
  * @param Left The left sequence value.
  * @param Right The right sequence value.
  * @return The signed 32-bit wrapping distance from Right to Left, computed as
  *         (Left - Right) modulo (2 ^ 32). A positive value means Left is after
  *         Right, a negative value means Left is before Right, and zero means
  *         they are equal, provided that the actual distance is less than
  *         (2 ^ 31). The exact half-range distance is ambiguous and returns
  *         -(2 ^ 31).
  */
MO_EXTERN_C MO_INT32 MOAPI MoMileFixedIntegerSequenceCompare32(
    _Mo_In_ MO_UINT32 Left,
    _Mo_In_ MO_UINT32 Right);

/**
  * @brief Compares two sequence values with a fixed 64-bit width.
  * @param Left The left sequence value.
  * @param Right The right sequence value.
  * @return The signed 64-bit wrapping distance from Right to Left, computed as
  *         (Left - Right) modulo (2 ^ 64). A positive value means Left is after
  *         Right, a negative value means Left is before Right, and zero means
  *         they are equal, provided that the actual distance is less than
  *         (2 ^ 63). The exact half-range distance is ambiguous and returns
  *         -(2 ^ 63).
  */
MO_EXTERN_C MO_INT64 MOAPI MoMileFixedIntegerSequenceCompare64(
    _Mo_In_ MO_UINT64 Left,
    _Mo_In_ MO_UINT64 Right);

/**
  * @brief Compares two sequence values with a fixed native width.
  * @param Left The left sequence value.
  * @param Right The right sequence value.
  * @return The signed native-width wrapping distance from Right to Left,
  *         computed as (Left - Right) modulo (2 ^ N), where N is the number of
  *         bits in MO_UINTN. A positive value means Left is after Right, a
  *         negative value means Left is before Right, and zero means they are
  *         equal, provided that the actual distance is less than (2 ^ (N - 1)).
  *         The exact half-range distance is ambiguous and returns
  *         -(2 ^ (N - 1)).
  */
MO_EXTERN_C MO_INTN MOAPI MoMileFixedIntegerSequenceCompare(
    _Mo_In_ MO_UINTN Left,
    _Mo_In_ MO_UINTN Right);

#endif // !MILE_MOBILITY_UTILITIES_FIXEDINTEGER
