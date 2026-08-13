/**
 * @file MidiConfig.h
 * @brief Declares the MidiConfig component used by the sequencer model.
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

#include "Types.h"
#include "Bitfield.h"
#include "ModelUtils.h"
#include "Serialize.h"

#include "core/utils/StringBuilder.h"
#include "core/math/Math.h"

/**
 * @brief Stores and edits midi config configuration.
 */
template<bool IsSource>
/**
 * @brief Stores and edits midi config configuration.
 */
class MidiConfig {
public:
    /**
     * @brief Applies a UI edit delta to the currently addressed value.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void edit(int value, bool shift) {
        if (shift) {
            editPort(value, false);
        } else {
            setIndexed(indexed() + value);
        }
    }

    /**
     * @brief Writes the object representation to the supplied output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void print(StringBuilder &str) const {
        printPort(str);
        str(" (");
        printChannel(str);
        str(")");
    }

    // port

    /**
     * @brief Returns the port.
     *
     * @return Configured MIDI port.
     */
    Types::MidiPort port() const { return _port; }

    /**
     * @brief Sets the port.
     *
     * @param[in] port MIDI or logical port selected for the operation.
     */
    void setPort(Types::MidiPort port) {
        _port = ModelUtils::clampedEnum(port);
    }

    /**
     * @brief Adjusts the port from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editPort(int value, bool shift) {
        setPort(ModelUtils::adjustedEnum(port(), value));
    }

    /**
     * @brief Formats the port into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printPort(StringBuilder &str) const {
        str(Types::midiPortName(port()));
    }

    // channel

    /**
     * @brief Returns the channel.
     *
     * @return MIDI channel in the internal 0..15 representation.
     */
    int channel() const {
        return _channel;
    }

    /**
     * @brief Sets the channel.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     */
    void setChannel(int channel) {
        _channel = clamp(channel, IsSource ? -1 : 0, 15);
    }

    /**
     * @brief Adjusts the channel from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editChannel(int value, bool shift) {
        setChannel(channel() + value);
    }

    /**
     * @brief Formats the channel into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printChannel(StringBuilder &str) const {
        Types::printMidiChannel(str, channel());
    }

    /**
     * @brief Reports whether omni.
     *
     * @return `true` if omni; otherwise `false`.
     */
    bool isOmni() const {
        return _channel == -1;
    }

    /**
     * @brief Reports whether first.
     *
     * @return `true` if first; otherwise `false`.
     */
    bool isFirst() const {
        return _port == Types::MidiPort(0) && _channel == (IsSource ? -1 : 0);
    }

    /**
     * @brief Constructs a MidiConfig instance.
     */
    MidiConfig() {
        clear();
    }

    /**
     * @brief Clears stored runtime/container state.
     */
    void clear() {
        setPort(Types::MidiPort::Midi);
        setChannel(IsSource ? -1 : 0);
    }

    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in,out] writer Writer that receives serialized output data.
     */
    void write(VersionedSerializedWriter &writer) const {
        writer.write(_port);
        writer.write(_channel);
    }

    /**
     * @brief Reads a value/data from the source.
     *
     * @param[in] reader Reader that supplies serialized input data.
     */
    void read(VersionedSerializedReader &reader) {
        reader.read(_port);
        reader.read(_channel);
    }

    /**
     * @brief Compares this value with another instance for value equality.
     *
     * @param[in] other Same-type value to compare with this instance.
     *
     * @return `true` when the represented state is equal; otherwise `false`.
     */
    bool operator==(const MidiConfig<IsSource> &other) const {
        return _port == other._port && _channel == other._channel;
    }

    /**
     * @brief Compares this value with another instance for value inequality.
     *
     * @param[in] other Same-type value to compare with this instance.
     *
     * @return `true` when the represented state differs; otherwise `false`.
     */
    bool operator!=(const MidiConfig<IsSource> &other) const {
        return !(*this == other);
    }

private:
    /**
     * @brief Returns the indexed.
     *
     * @return Zero-based indexed.
     */
    int indexed() const {
        return int(port()) * (IsSource ? 17 : 16) + channel() + (IsSource ? 1 : 0);
    }

    /**
     * @brief Sets the indexed.
     *
     * @param[in] index Zero-based indexed index.
     */
    void setIndexed(int index) {
        index = clamp(index, 0, int(Types::MidiPort::Last) * (IsSource ? 17 : 16) - 1);
        setPort(Types::MidiPort(index / (IsSource ? 17 : 16)));
        setChannel(index % (IsSource ? 17 : 16) - (IsSource ? 1 : 0));
    }

    Types::MidiPort _port; ///< MIDI transport port selected by this source/target configuration.
    int8_t _channel; ///< Internal MIDI channel index; source configurations may use `-1` for all channels.
};

typedef MidiConfig<true> MidiSourceConfig;
typedef MidiConfig<false> MidiTargetConfig;
