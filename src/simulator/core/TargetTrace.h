/**
 * @file TargetTrace.h
 * @brief Declares the TargetTrace component used by the simulator core.
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

#include "TargetState.h"
#include "EncoderEvent.h"
#include "MidiEvent.h"

#include <array>
#include <bitset>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <cstdint>
#include <cstring>

namespace sim {

namespace stream {

    template<typename T>
    static void write(T data, std::ostream &stream) {
        stream.write(reinterpret_cast<const char *>(&data), sizeof(T));
    }

    template<typename T>
    static void read(T &data, std::istream &stream) {
        stream.read(reinterpret_cast<char *>(&data), sizeof(T));
    }

    template<typename T>
    static T read(std::istream &stream) {
        T data;
        stream.read(reinterpret_cast<char *>(&data), sizeof(T));
        return data;
    }

} // namespace stream

/**
 * @brief Provides state trace behavior for the desktop simulator.
 */
template<typename T>
class StateTrace {
public:
    typedef T Record;
    typedef std::pair<uint32_t, T> Item;

    /**
     * @brief Returns the items.
     *
     * @return Reference to the stored item collection.
     */
    const std::vector<Item> &items() const { return _items; }

    /**
     * @brief Executes write.
     *
     * @param[in] time Time used by the operation.
     * @param[in] state New state or logical level to apply.
     */
    void write(uint32_t time, const T &state) {
        if (_items.empty()) {
            _items.emplace_back(time, state);
        } else if (time == _items.back().first) {
            _items.back().second = state;
        } else if (state != _items.back().second) {
            _items.emplace_back(time, state);
        }
    }

    /**
     * @brief Writes stream.
     *
     * @param[in] stream Stream used by the operation.
     */
    void writeStream(std::ostream &stream) const {
        stream::write<uint32_t>(_items.size(), stream);
        for (const auto &item : _items) {
            stream::write(item, stream);
        }
    }

    /**
     * @brief Reads stream.
     *
     * @param[in] stream Stream used by the operation.
     */
    void readStream(std::istream &stream) {
        _items.resize(stream::read<uint32_t>(stream));
        for (auto &item : _items) {
            stream::read(item, stream);
        }
    }

private:
    std::vector<Item> _items; ///< Chronologically ordered timestamp/value records in this trace.
};

/**
 * @brief Provides event trace behavior for the desktop simulator.
 */
template<typename T>
class EventTrace {
public:
    typedef T Record;
    typedef std::pair<uint32_t, T> Item;

    /**
     * @brief Returns the items.
     *
     * @return Reference to the stored item collection.
     */
    const std::vector<Item> &items() const { return _items; }

    /**
     * @brief Executes write.
     *
     * @param[in] time Time used by the operation.
     * @param[in] event Event to process.
     */
    void write(uint32_t time, const T &event) {
        _items.emplace_back(time, event);
    }

    /**
     * @brief Writes stream.
     *
     * @param[in] stream Stream used by the operation.
     */
    void writeStream(std::ostream &stream) const {
        stream::write<uint32_t>(_items.size(), stream);
        for (const auto &item : _items) {
            stream::write(item, stream);
        }
    }

    /**
     * @brief Reads stream.
     *
     * @param[in] stream Stream used by the operation.
     */
    void readStream(std::istream &stream) {
        _items.resize(stream::read<uint32_t>(stream));
        for (auto &item : _items) {
            stream::read(item, stream);
        }
    }

private:
    std::vector<Item> _items; ///< Chronologically ordered timestamp/value records in this trace.
};

typedef StateTrace<ButtonState> ButtonTrace;
typedef StateTrace<AdcState> AdcTrace;
typedef StateTrace<DigitalInputState> DigitalInputTrace;
typedef StateTrace<LedState> LedTrace;
typedef StateTrace<GateOutputState> GateOutputTrace;
typedef StateTrace<DacState> DacTrace;
typedef StateTrace<DigitalOutputState> DigitalOutputTrace;
typedef StateTrace<LcdState> LcdTrace;

typedef EventTrace<EncoderEvent> EncoderTrace;
typedef EventTrace<MidiEvent> MidiTrace;

/**
 * @brief Provides target trace behavior for the desktop simulator.
 */
struct TargetTrace {
    // state traces
    ButtonTrace button; ///< Trace channel recording timestamped button activity.
    AdcTrace adc; ///< Trace channel recording timestamped adc activity.
    DigitalInputTrace digitalInput; ///< Trace channel recording timestamped digital input activity.
    LedTrace led; ///< Trace channel recording timestamped led activity.
    GateOutputTrace gateOutput; ///< Current digital gate output state.
    DacTrace dac; ///< Trace channel recording timestamped dac activity.
    DigitalOutputTrace digitalOutput; ///< Trace channel recording timestamped digital output activity.
    LcdTrace lcd; ///< Trace channel recording timestamped lcd activity.

    // event traces
    EncoderTrace encoder; ///< Trace channel recording timestamped encoder activity.
    MidiTrace midiInput; ///< Trace channel recording timestamped midi input activity.
    MidiTrace midiOutput; ///< Trace channel recording timestamped midi output activity.

    /**
     * @brief Writes stream.
     *
     * @param[in] stream Stream used by the operation.
     */
    void writeStream(std::ostream &stream) const;
    /**
     * @brief Reads stream.
     *
     * @param[in] stream Stream used by the operation.
     */
    void readStream(std::istream &stream);

    /**
     * @brief Saves to file.
     *
     * @param[in] filename Filename identifying the file or path used by the operation.
     */
    void saveToFile(const std::string &filename) const;
    /**
     * @brief Loads from file.
     *
     * @param[in] filename Filename identifying the file or path used by the operation.
     */
    void loadFromFile(const std::string &filename);

    /**
     * @brief Saves to text.
     *
     * @param[in] filename Filename identifying the file or path used by the operation.
     */
    void saveToText(const std::string &filename) const;
};

} // namespace sim
