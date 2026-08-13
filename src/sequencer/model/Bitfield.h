/**
 * @file Bitfield.h
 * @brief Declares the Bitfield component used by the sequencer model.
 *
 * @author Simon Kallweit — original PER|FORMER implementation lineage
 * @author Axel Napolitano — Styr modifications, integration and modernization
 * @copyright 2017-2018 Simon Kallweit
 * @copyright 2026 Axel Napolitano
 *
 * @par License
 * MIT; see LICENSES/MIT.txt.
 *
 * SPDX-FileCopyrightText: 2017-2018 Simon Kallweit
 * SPDX-FileCopyrightText: 2026 Axel Napolitano
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <algorithm>
#include <type_traits>

#include <cstdint>
#include <cstddef>

/**
 * @brief Stores and manipulates unsigned value model data.
 */
template<int Bits_>
/**
 * @brief Stores and manipulates unsigned value model data.
 */
struct UnsignedValue {
    /**
     * @brief Number of bits used by the encoded value.
     */
    static constexpr int Bits = Bits_; ///< Number of bits used by the encoded value.
    /**
     * @brief Minimum representable value.
     */
    static constexpr int Min = 0; ///< Minimum representable value.
    static constexpr int Max = (1 << Bits) - 1; ///< Largest unsigned value representable by the configured bit width. ///< Largest unsigned value representable by the configured bit width.
    static constexpr int Range = (1 << Bits); ///< Number of raw bit patterns available in the signed packed representation. ///< Number of raw bit patterns available in the signed packed representation.

    /**
     * @brief Returns the bits.
     *
     * @return Number of bits used by the packed value.
     */
    static int bits() { return Bits; }
    /**
     * @brief Returns the min.
     *
     * @return Configured minimum value.
     */
    static int min() { return Min; }
    /**
     * @brief Returns the max.
     *
     * @return Configured maximum value.
     */
    static int max() { return Max; }
    /**
     * @brief Returns the range.
     *
     * @return Configured numeric/source range.
     */
    static int range() { return Range; }

    /**
     * @brief Returns clamp.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     *
     * @return Input value clamped to the supported range.
     */
    static int clamp(int value) {
        return std::max(min(), std::min(max(), value));
    }
};

/**
 * @brief Stores and manipulates signed value model data.
 */
template<int Bits_>
/**
 * @brief Stores and manipulates signed value model data.
 */
struct SignedValue {
    /**
     * @brief Number of bits used by the encoded value.
     */
    static constexpr int Bits = Bits_; ///< Number of bits used by the encoded value.
    static constexpr int Min = -(1 << (Bits - 1)) + 1; ///< Smallest signed value accepted by the packed representation; the most-negative two's-complement code is intentionally excluded. ///< Smallest signed value accepted by the packed representation; the most-negative two's-complement code is intentionally excluded.
    static constexpr int Max = (1 << (Bits - 1)) - 1; ///< Largest signed value accepted by the packed representation. ///< Largest signed value accepted by the packed representation.
    static constexpr int Range = (1 << Bits); ///< Number of raw bit patterns available in the signed packed representation. ///< Number of raw bit patterns available in the signed packed representation.

    /**
     * @brief Returns the bits.
     *
     * @return Number of bits used by the packed value.
     */
    static int bits() { return Bits; }
    /**
     * @brief Returns the min.
     *
     * @return Configured minimum value.
     */
    static int min() { return Min; }
    /**
     * @brief Returns the max.
     *
     * @return Configured maximum value.
     */
    static int max() { return Max; }
    /**
     * @brief Returns the range.
     *
     * @return Configured numeric/source range.
     */
    static int range() { return Range; }

    /**
     * @brief Returns clamp.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     *
     * @return Input value clamped to the supported range.
     */
    static int clamp(int value) {
        return std::max(min(), std::min(max(), value));
    }
};

// the following is based on https://blog.codef00.com/2014/12/06/portable-bitfields-using-c11/

/**
 * @brief Stores and manipulates bit field model data.
 */
template<typename T, size_t Index, size_t Bits = 1>
/**
 * @brief Stores and manipulates bit field model data.
 */
class BitField {
private:
    /**
     * @brief Enumerates the supported enumeration values.
     */
    enum {
        Mask = (1u << Bits) - 1u ///< Selects the mask option.
    };

public:
    template <class T2>
    /**
     * @brief Stores a value in this packed bit-field while preserving neighboring bits.
     *
     * @param[in] value Value to mask to this field's width and write into the backing integer.
     *
     * @return Reference to this bit-field proxy after the update.
     */
    BitField &operator=(T2 value) {
        value_ = (value_ & ~(Mask << Index)) | ((value & Mask) << Index);
        return *this;
    }

    /**
     * @brief Extracts this packed field as its underlying integer type.
     *
     * @return Field value shifted to bit zero and masked to the configured width.
     */
    operator T() const             { return (value_ >> Index) & Mask; }
    /**
     * @brief Tests whether this packed field contains a nonzero value.
     *
     * @return `true` when any bit in this field is set; otherwise `false`.
     */
    explicit operator bool() const { return value_ & (Mask << Index); }
    /**
     * @brief Implements operator++ for this type.
     *
     * @return Reference to the operator++.
     */
    BitField &operator++()         { return *this = *this + 1; }
    /**
     * @brief Implements operator++ for this type.
     *
     * @note Includes an unnamed `int` input parameter as declared by the inherited/interface signature.
     *
     * @return Reference to this packed value after incrementing it.
     */
    T operator++(int)              { T r = *this; ++*this; return r; }
    /**
     * @brief Implements operator-- for this type.
     *
     * @return Reference to the operator--.
     */
    BitField &operator--()         { return *this = *this - 1; }
    /**
     * @brief Implements operator-- for this type.
     *
     * @note Includes an unnamed `int` input parameter as declared by the inherited/interface signature.
     *
     * @return Reference to this packed value after decrementing it.
     */
    T operator--(int)              { T r = *this; --*this; return r; }

private:
    T value_; ///< Packed integer storage containing this bitfield value.
};


/**
 * @brief Stores and manipulates bit field model data.
 */
template<typename T, size_t Index>
/**
 * @brief Stores and manipulates bit field model data.
 */
class BitField<T, Index, 1> {
private:
    /**
     * @brief Enumerates the supported enumeration values.
     */
    enum {
        Bits = 1, ///< Selects the bits option.
        Mask = 0x01 ///< Selects the mask option.
    };

public:
    /**
     * @brief Stores a Boolean value in this one-bit field while preserving neighboring bits.
     *
     * @param[in] value Boolean value to encode in the backing integer.
     *
     * @return Reference to this bit-field proxy after the update.
     */
    BitField &operator=(bool value) {
        value_ = (value_ & ~(Mask << Index)) | (value << Index);
        return *this;
    }

    /**
     * @brief Reads the Boolean value stored in this one-bit field.
     *
     * @return `true` when the field bit is set; otherwise `false`.
     */
    explicit operator bool() const { return value_ & (Mask << Index); }

private:
    T value_; ///< Packed integer storage containing this bitfield value.
};
