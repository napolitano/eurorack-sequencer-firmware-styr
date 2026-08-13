/**
 * @file Song.h
 * @brief Declares the Song component used by the sequencer model.
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

#include "Config.h"

#include "Serialize.h"

#include "core/math/Math.h"

#include <array>

#include <cstdint>

/**
 * @brief Stores and manipulates song model data.
 */
class Song {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    /**
     * @brief Stores and manipulates slot model data.
     */
    class Slot {
    public:
        /**
         * @brief Returns pattern.
         *
         * @param[in] trackIndex Zero-based track index.
         *
         * @return Current/selected pattern index.
         */
        int pattern(int trackIndex) const {
            return (_patterns >> (trackIndex << 2)) & 0xf;
        }

        /**
         * @brief Returns mute.
         *
         * @param[in] trackIndex Zero-based track index.
         *
         * @return `true` if mute; otherwise `false`.
         */
        bool mute(int trackIndex) const {
            return (_mutes >> trackIndex) & 0x1;
        }

        /**
         * @brief Returns the repeats.
         *
         * @return Configured song-step repeat count.
         */
        int repeats() const { return _repeats; }

        /**
         * @brief Clears stored runtime/container state.
         */
        void clear();

        /**
         * @brief Writes the supplied value/data to the destination.
         *
         * @param[in,out] writer Writer that receives serialized output data.
         */
        void write(VersionedSerializedWriter &writer) const;
        /**
         * @brief Reads a value/data from the source.
         *
         * @param[in] reader Reader that supplies serialized input data.
         */
        void read(VersionedSerializedReader &reader);

    private:
        /**
         * @brief Fills patterns.
         *
         * @param[in] pattern Pattern index or pattern data addressed by the operation.
         *
         * @return Result of fillPatterns().
         */
        static uint32_t fillPatterns(int pattern) {
            uint32_t patterns = pattern & 0xf;
            patterns |= patterns << 4;
            patterns |= patterns << 8;
            patterns |= patterns << 16;
            return patterns;
        }

        /**
         * @brief Sets the pattern.
         *
         * @param[in] trackIndex Zero-based track index.
         * @param[in] pattern New zero-based pattern index to store or apply.
         */
        void setPattern(int trackIndex, int pattern) {
            pattern = clamp(pattern, 0, CONFIG_PATTERN_COUNT - 1);
            uint32_t patterns = _patterns & ~(0xf << (trackIndex << 2));
            patterns |= (pattern & 0xf) << (trackIndex << 2);
            _patterns = patterns;
        }

        /**
         * @brief Sets the pattern.
         *
         * @param[in] pattern New zero-based pattern index to store or apply.
         */
        void setPattern(int pattern) {
            _patterns = fillPatterns(pattern);
        }

        /**
         * @brief Sets the mute.
         *
         * @param[in] trackIndex Zero-based track index.
         * @param[in] mute `true` to enable/set mute; `false` to disable/clear it.
         */
        void setMute(int trackIndex, bool mute) {
            uint8_t bit = 0x1 << trackIndex;
            _mutes = (_mutes & ~bit) | (mute ? bit : 0);
        }

        /**
         * @brief Toggles mute.
         *
         * @param[in] trackIndex Zero-based track index.
         */
        void toggleMute(int trackIndex) {
            setMute(trackIndex, !mute(trackIndex));
        }

        /**
         * @brief Sets the repeats.
         *
         * @param[in] repeats Repeats consumed by `setRepeats()`.
         */
        void setRepeats(int repeats) {
            _repeats = clamp(repeats, 1, 128);
        }

        uint32_t _patterns; ///< Packed pattern assignments for this song slot.
        uint8_t _mutes; ///< Packed track-mute flags for this song slot.
        uint8_t _repeats; ///< Repeat count encoded for this song slot.

        friend class Song;
    };

    //----------------------------------------
    // Properties
    //----------------------------------------

    // slots

    /**
     * @brief Returns slot.
     *
     * @param[in] slot Zero-based project/storage slot index.
     *
     * @return Reference to the slot.
     */
    const Slot &slot(int slot) const { return _slots[slot]; }
          /**
           * @brief Returns slot.
           *
           * @param[in] slot Zero-based project/storage slot index.
           *
           * @return Reference to the slot.
           */
          Slot &slot(int slot)       { return _slots[slot]; }

    /**
     * @brief Returns the slot count.
     *
     * @return Number of slot entries represented by the object.
     */
    int slotCount() const { return _slotCount; }
    /**
     * @brief Reports whether full.
     *
     * @return `true` if full; otherwise `false`.
     */
    bool isFull() const { return _slotCount >= _slots.size(); }
    /**
     * @brief Reports whether active slot.
     *
     * @param[in] slotIndex Zero-based slot index.
     *
     * @return `true` if active slot; otherwise `false`.
     */
    bool isActiveSlot(int slotIndex) const { return slotIndex >= 0 && slotIndex < _slotCount; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    /**
     * @brief Queues the requested pattern for chained playback.
     *
     * @param[in] pattern Pattern index or pattern data addressed by the operation.
     */
    void chainPattern(int pattern);
    /**
     * @brief Inserts slot.
     *
     * @param[in] slotIndex Zero-based slot index.
     */
    void insertSlot(int slotIndex);
    /**
     * @brief Removes slot.
     *
     * @param[in] slotIndex Zero-based slot index.
     */
    void removeSlot(int slotIndex);
    /**
     * @brief Duplicates slot.
     *
     * @param[in] slotIndex Zero-based slot index.
     */
    void duplicateSlot(int slotIndex);
    /**
     * @brief Swaps the contents/metadata of two storage slots.
     *
     * @param[in] fromIndex Zero-based from index.
     * @param[in] toIndex Zero-based to index.
     */
    void swapSlot(int fromIndex, int toIndex);

    /**
     * @brief Sets the pattern.
     *
     * @param[in] slotIndex Zero-based slot index.
     * @param[in] pattern New zero-based pattern index to store or apply.
     */
    void setPattern(int slotIndex, int pattern);
    /**
     * @brief Sets the pattern.
     *
     * @param[in] slotIndex Zero-based slot index.
     * @param[in] trackIndex Zero-based track index.
     * @param[in] pattern New zero-based pattern index to store or apply.
     */
    void setPattern(int slotIndex, int trackIndex, int pattern);
    /**
     * @brief Adjusts the pattern from a UI edit delta.
     *
     * @param[in] slotIndex Zero-based slot index.
     * @param[in] trackIndex Zero-based track index.
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     */
    void editPattern(int slotIndex, int trackIndex, int value);
    /**
     * @brief Sets the mute.
     *
     * @param[in] slotIndex Zero-based slot index.
     * @param[in] trackIndex Zero-based track index.
     * @param[in] mute `true` to enable/set mute; `false` to disable/clear it.
     */
    void setMute(int slotIndex, int trackIndex, bool mute);
    /**
     * @brief Toggles mute.
     *
     * @param[in] slotIndex Zero-based slot index.
     * @param[in] trackIndex Zero-based track index.
     */
    void toggleMute(int slotIndex, int trackIndex);
    /**
     * @brief Sets the repeats.
     *
     * @param[in] slotIndex Zero-based slot index.
     * @param[in] repeats Repeats consumed by `setRepeats()`.
     */
    void setRepeats(int slotIndex, int repeats);
    /**
     * @brief Adjusts the repeats from a UI edit delta.
     *
     * @param[in] slotIndex Zero-based slot index.
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     */
    void editRepeats(int slotIndex, int value);

    /**
     * @brief Returns track has mutes.
     *
     * @param[in] trackIndex Zero-based track index.
     *
     * @return `true` if track has mutes; otherwise `false`.
     */
    bool trackHasMutes(int trackIndex) const;

    /**
     * @brief Clears stored runtime/container state.
     */
    void clear();

    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in,out] writer Writer that receives serialized output data.
     */
    void write(VersionedSerializedWriter &writer) const;
    /**
     * @brief Reads a value/data from the source.
     *
     * @param[in] reader Reader that supplies serialized input data.
     */
    void read(VersionedSerializedReader &reader);

private:
    /**
     * @brief Fixed-capacity storage for slots.
     */
    std::array<Slot, CONFIG_SONG_SLOT_COUNT> _slots; ///< Fixed song-slot array indexed by the sequencer song position.
    /**
     * @brief Number of slot items currently tracked or supported.
     */
    uint8_t _slotCount; ///< Number of slot items currently tracked or supported.
};
