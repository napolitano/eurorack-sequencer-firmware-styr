/**
 * @file MidiMessage.h
 * @brief Declares the MidiMessage component used by the shared MIDI support.
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
#include <array>

#include <cstdint>
#include <cstring>

/**
 * @brief Encodes, decodes and owns references to MIDI channel, system and realtime messages.
 */
class MidiMessage {
public:
    using PayloadID = uint8_t;
    /**
     * @brief Invalid payload constant used by this component.
     */
    static constexpr PayloadID InvalidPayload = 0; ///< Sentinel payload identifier representing no allocated MIDI SysEx payload.

    // Channel (voice) messages

    /**
     * @brief Enumerates the supported channel message values.
     */
    enum ChannelMessage {
        NoteOff         = 0x80, ///< Selects the note off channel message.
        NoteOn          = 0x90, ///< Selects the note on channel message.
        KeyPressure     = 0xa0, ///< Selects the key pressure channel message.
        ControlChange   = 0xb0, ///< Selects the control change channel message.
        ProgramChange   = 0xc0, ///< Selects the program change channel message.
        ChannelPressure = 0xd0, ///< Selects the channel pressure channel message.
        PitchBend       = 0xe0, ///< Selects the pitch bend channel message.
    };

    /**
     * @brief Returns channel message length.
     *
     * @param[in] message Message to process.
     *
     * @return Number of MIDI data bytes in the channel message.
     */
    static int channelMessageLength(ChannelMessage message) {
        switch (message) {
        case NoteOff:
        case NoteOn:
        case KeyPressure:
        case ControlChange:
        case PitchBend:
            return 2;
        case ChannelPressure:
        case ProgramChange:
            return 1;
        default:
            return 0;
        }
    }
    /**
     * @brief Reports whether channel message.
     *
     * @param[in] status MIDI status byte or status value to classify.
     *
     * @return `true` if channel message; otherwise `false`.
     */
    static bool isChannelMessage(uint8_t status) { return (status & 0xf0) >= 0x80 && (status & 0xf0) < 0xf0; }

    template<uint8_t ChannelMessage>
    /**
     * @brief Reports whether channel message.
     *
     * @param[in] status MIDI status byte or status value to classify.
     *
     * @return `true` if channel message; otherwise `false`.
     */
    static bool isChannelMessage(uint8_t status) { return (status & 0xf0) == ChannelMessage; }

    /**
     * @brief Returns channel message.
     *
     * @param[in] status MIDI status byte or status value to classify.
     *
     * @return Decoded MIDI channel-message type.
     */
    static ChannelMessage channelMessage(uint8_t status) { return ChannelMessage(status & 0xf0); }

    // System (common) messages

    /**
     * @brief Enumerates the supported system message values.
     */
    enum SystemMessage {
        SystemExclusive = 0xf0, ///< Selects the system exclusive system message.
        TimeCode        = 0xf1, ///< Selects the time code system message.
        SongPosition    = 0xf2, ///< Selects the song position system message.
        SongSelect      = 0xf3, ///< Selects the song select system message.
        TuneRequest     = 0xf6, ///< Selects the tune request system message.
        EndOfExclusive  = 0xf7, ///< Selects the end of exclusive system message.
    };

    /**
     * @brief Returns system message length.
     *
     * @param[in] message Message to process.
     *
     * @return Number of MIDI data bytes in the system-common message.
     */
    static int systemMessageLength(SystemMessage message) {
        switch (message) {
        case TimeCode:
        case SongSelect:
            return 1;
        case SongPosition:
            return 2;
        default:
            return 0;
        }
    }

    /**
     * @brief Reports whether system message.
     *
     * @param[in] status MIDI status byte or status value to classify.
     *
     * @return `true` if system message; otherwise `false`.
     */
    static bool isSystemMessage(uint8_t status) { return (status & 0xf8) == 0xf0; }

    template<uint8_t SystemMessage>
    /**
     * @brief Reports whether system message.
     *
     * @param[in] status MIDI status byte or status value to classify.
     *
     * @return `true` if system message; otherwise `false`.
     */
    static bool isSystemMessage(uint8_t status) { return status == SystemMessage; }

    /**
     * @brief Returns system message.
     *
     * @param[in] status MIDI status byte or status value to classify.
     *
     * @return Decoded MIDI system-common message type.
     */
    static SystemMessage systemMessage(uint8_t status) { return SystemMessage(status); }

    // (System) realtime messages

    /**
     * @brief Enumerates the supported real time message values.
     */
    enum RealTimeMessage {
        Tick            = 0xf8, ///< Selects the tick real time message.
        Start           = 0xfa, ///< Selects the start real time message.
        Continue        = 0xfb, ///< Selects the continue real time message.
        Stop            = 0xfc, ///< Selects the stop real time message.
        ActiveSensing   = 0xfe, ///< Selects the active sensing real time message.
        Reset           = 0xff, ///< Selects the reset real time message.
    };

    /**
     * @brief Reports whether real time message.
     *
     * @param[in] status MIDI status byte or status value to classify.
     *
     * @return `true` if real time message; otherwise `false`.
     */
    static bool isRealTimeMessage(uint8_t status) { return (status & 0xf8) == 0xf8; }

    template<uint8_t RealTimeMessage>
    /**
     * @brief Reports whether real time message.
     *
     * @param[in] status MIDI status byte or status value to classify.
     *
     * @return `true` if real time message; otherwise `false`.
     */
    static bool isRealTimeMessage(uint8_t status) { return status == RealTimeMessage; }

    /**
     * @brief Returns real time message.
     *
     * @param[in] status MIDI status byte or status value to classify.
     *
     * @return Decoded MIDI realtime-message type.
     */
    static RealTimeMessage realTimeMessage(uint8_t status) { return RealTimeMessage(status); }

    /**
     * @brief Reports whether clock message.
     *
     * @param[in] status MIDI status byte or status value to classify.
     *
     * @return `true` if clock message; otherwise `false`.
     */
    static bool isClockMessage(uint8_t status) {
        return isRealTimeMessage(status) && (
            status == Tick ||
            status == Start ||
            status == Continue ||
            status == Stop
        );
    }

    // Message data

    /**
     * @brief Returns the status.
     *
     * @return Raw MIDI status byte.
     */
    inline uint8_t status() const { return _raw[0]; }
    /**
     * @brief Returns the data0.
     *
     * @return First MIDI data byte.
     */
    inline uint8_t data0() const { return _raw[1]; }
    /**
     * @brief Returns the data1.
     *
     * @return Second MIDI data byte.
     */
    inline uint8_t data1() const { return _raw[2]; }

    /**
     * @brief Returns the raw.
     *
     * @return Pointer to the raw; `nullptr` when no value is available.
     */
    inline const uint8_t *raw() const { return _raw; }
    /**
     * @brief Returns the length.
     *
     * @return Number of length represented by this object.
     */
    inline uint8_t length() const { return _length & 0x3; }

    // Channel messages

    /**
     * @brief Reports whether channel message.
     *
     * @return `true` if channel message; otherwise `false`.
     */
    bool isChannelMessage() const {
        return isChannelMessage(status());
    }

    /**
     * @brief Returns the channel message.
     *
     * @return Decoded MIDI channel-message type.
     */
    ChannelMessage channelMessage() const {
        return MidiMessage::channelMessage(status());
    }

    /**
     * @brief Returns the channel.
     *
     * @return MIDI channel in the internal 0..15 representation.
     */
    uint8_t channel() const {
        return status() & 0xf;
    }

    // NoteOff

    /**
     * @brief Reports whether note off.
     *
     * @return `true` if note off; otherwise `false`.
     */
    bool isNoteOff() const {
        return isChannelMessage<NoteOff>(status());
    }

    // NoteOn

    /**
     * @brief Reports whether note on.
     *
     * @return `true` if note on; otherwise `false`.
     */
    bool isNoteOn() const {
        return isChannelMessage<NoteOn>(status());
    }

    /**
     * @brief Returns the note.
     *
     * @return Note value encoded by the message or model object.
     */
    uint8_t note() const {
        return data0();
    }

    /**
     * @brief Returns the velocity.
     *
     * @return MIDI velocity in the 0..127 range.
     */
    uint8_t velocity() const {
        return data1();
    }

    // KeyPressure

    /**
     * @brief Reports whether key pressure.
     *
     * @return `true` if key pressure; otherwise `false`.
     */
    bool isKeyPressure() const {
        return isChannelMessage<KeyPressure>(status());
    }

    /**
     * @brief Returns the key pressure.
     *
     * @return MIDI polyphonic key-pressure value in the 0..127 range.
     */
    uint8_t keyPressure() const {
        return data1();
    }

    // ControlChange

    /**
     * @brief Reports whether control change.
     *
     * @return `true` if control change; otherwise `false`.
     */
    bool isControlChange() const {
        return isChannelMessage<ControlChange>(status());
    }

    /**
     * @brief Returns the control number.
     *
     * @return MIDI Control Change controller number in the 0..127 range.
     */
    uint8_t controlNumber() const {
        return data0();
    }

    /**
     * @brief Returns the control value.
     *
     * @return MIDI Control Change value in the 0..127 range.
     */
    uint8_t controlValue() const {
        return data1();
    }

    // ProgramChange

    /**
     * @brief Reports whether program change.
     *
     * @return `true` if program change; otherwise `false`.
     */
    bool isProgramChange() const {
        return isChannelMessage<ProgramChange>(status());
    }

    /**
     * @brief Returns the program number.
     *
     * @return MIDI Program Change number in the 0..127 range.
     */
    uint8_t programNumber() const {
        return data0();
    }

    // ChannelPressure

    /**
     * @brief Reports whether channel pressure.
     *
     * @return `true` if channel pressure; otherwise `false`.
     */
    bool isChannelPressure() const {
        return isChannelMessage<ChannelPressure>(status());
    }

    /**
     * @brief Returns the channel pressure.
     *
     * @return MIDI channel-pressure value in the 0..127 range.
     */
    uint8_t channelPressure() const {
        return data0();
    }

    // PitchBend

    /**
     * @brief Reports whether pitch bend.
     *
     * @return `true` if pitch bend; otherwise `false`.
     */
    bool isPitchBend() const {
        return isChannelMessage<PitchBend>(status());
    }

    /**
     * @brief Returns the pitch bend.
     *
     * @return Decoded MIDI pitch-bend value.
     */
    int pitchBend() const {
        return (int(data1()) << 7 | int(data0())) - int(0x2000);
    }

    // System messages

    /**
     * @brief Reports whether system message.
     *
     * @return `true` if system message; otherwise `false`.
     */
    bool isSystemMessage() const {
        return MidiMessage::isSystemMessage(status());
    }

    /**
     * @brief Returns the system message.
     *
     * @return Decoded MIDI system-common message type.
     */
    SystemMessage systemMessage() const {
        return MidiMessage::systemMessage(status());
    }

    /**
     * @brief Reports whether system exclusive.
     *
     * @return `true` if system exclusive; otherwise `false`.
     */
    bool isSystemExclusive() const { return isSystemMessage<SystemExclusive>(status()); }
    /**
     * @brief Reports whether time code.
     *
     * @return `true` if time code; otherwise `false`.
     */
    bool isTimeCode() const { return isSystemMessage<TimeCode>(status()); }
    /**
     * @brief Reports whether song position.
     *
     * @return `true` if song position; otherwise `false`.
     */
    bool isSongPosition() const { return isSystemMessage<SongPosition>(status()); }
    /**
     * @brief Reports whether song select.
     *
     * @return `true` if song select; otherwise `false`.
     */
    bool isSongSelect() const { return isSystemMessage<SongSelect>(status()); }
    /**
     * @brief Reports whether tune request.
     *
     * @return `true` if tune request; otherwise `false`.
     */
    bool isTuneRequest() const { return isSystemMessage<TuneRequest>(status()); }

    /**
     * @brief Returns the song position.
     *
     * @return MIDI Song Position Pointer value.
     */
    int songPosition() const {
        return (int(data1()) << 7 | int(data0()));
    }

    /**
     * @brief Returns the song number.
     *
     * @return MIDI song-selection number.
     */
    uint8_t songNumber() const {
        return data0();
    }

    // Real-time messages

    /**
     * @brief Reports whether real time message.
     *
     * @return `true` if real time message; otherwise `false`.
     */
    bool isRealTimeMessage() const {
        return MidiMessage::isRealTimeMessage(status());
    }

    /**
     * @brief Returns the real time message.
     *
     * @return Decoded MIDI real-time message type.
     */
    RealTimeMessage realTimeMessage() const {
        return MidiMessage::realTimeMessage(status());
    }

    /**
     * @brief Reports whether clock message.
     *
     * @return `true` if clock message; otherwise `false`.
     */
    bool isClockMessage() const {
        return MidiMessage::isClockMessage(status());
    }

    /**
     * @brief Reports whether tick.
     *
     * @return `true` if tick; otherwise `false`.
     */
    bool isTick() const { return isRealTimeMessage<Tick>(status()); }
    /**
     * @brief Reports whether start.
     *
     * @return `true` if start; otherwise `false`.
     */
    bool isStart() const { return isRealTimeMessage<Start>(status()); }
    /**
     * @brief Reports whether continue.
     *
     * @return `true` if continue; otherwise `false`.
     */
    bool isContinue() const { return isRealTimeMessage<Continue>(status()); }
    /**
     * @brief Reports whether stop.
     *
     * @return `true` if stop; otherwise `false`.
     */
    bool isStop() const { return isRealTimeMessage<Stop>(status()); }
    /**
     * @brief Reports whether active sensing.
     *
     * @return `true` if active sensing; otherwise `false`.
     */
    bool isActiveSensing() const { return isRealTimeMessage<ActiveSensing>(status()); }
    /**
     * @brief Reports whether reset.
     *
     * @return `true` if reset; otherwise `false`.
     */
    bool isReset() const { return isRealTimeMessage<Reset>(status()); }

    // Payload

    /**
     * @brief Reports whether the MidiMessage has payload.
     *
     * @return `true` if payload; otherwise `false`.
     */
    bool hasPayload() const {
        return payloadID() != InvalidPayload;
    }

    /**
     * @brief Clears payload.
     */
    void clearPayload() {
        auto id = payloadID();
        if (id != InvalidPayload) {
            decPayloadRefCount(id);
            setPayloadID(InvalidPayload);
        }
    }

    /**
     * @brief Sets the payload.
     *
     * @param[in] data Input data to read, decode, copy, or process.
     * @param[in] size Number of bytes or elements covered by the operation.
     */
    void setPayload(const uint8_t *data, size_t size) {
        clearPayload();
        auto id = allocatePayload(size);
        if (id != InvalidPayload) {
            setPayloadID(id);
            std::memcpy(payloadData(id), data, size);
        }
    }

    /**
     * @brief Returns the payload data.
     *
     * @return Pointer to the payload data; `nullptr` when no value is available.
     */
    const uint8_t *payloadData() const {
        return payloadData(payloadID());
    }

    /**
     * @brief Returns the payload length.
     *
     * @return Length of the referenced MIDI payload in bytes.
     */
    size_t payloadLength() const {
        return payloadLength(payloadID());
    }

    /**
     * @brief Sets the payload id.
     *
     * @param[in] id Identifier of the addressed object/resource.
     */
    void setPayloadID(PayloadID id) {
        _length = (_length & 0x3) | (id << 2);
    }

    /**
     * @brief Returns the payload id.
     *
     * @return Identifier of the associated MIDI payload.
     */
    PayloadID payloadID() const {
        return _length >> 2;
    }

    // Utilities

    /**
     * @brief Normalizes note-on-with-zero-velocity messages to note-off semantics.
     */
    void fixFakeNoteOff() {
        if (isNoteOn() && velocity() == 0) {
            _raw[0] = NoteOff | (_raw[0] & 0xf);
        }
    }

    // Constructor

    /**
     * @brief Constructs a MidiMessage instance.
     */
    MidiMessage() = default;
    /**
     * @brief Constructs a MidiMessage instance.
     *
     * @param[in] other Other object/value used for comparison or combination.
     */
    MidiMessage(const MidiMessage &other) {
        _raw[0] = other._raw[0];
        _raw[1] = other._raw[1];
        _raw[2] = other._raw[2];
        _length = other._length;

        if (hasPayload()) {
            incPayloadRefCount(payloadID());
        }
    }
    /**
     * @brief Constructs a MidiMessage instance.
     *
     * @param[in] status MIDI status byte or status value to classify.
     */
    MidiMessage(uint8_t status) :
        _raw { status }, _length(1)
    /**
     * @brief Constructs a MidiMessage instance.
     *
     * @param[in] status MIDI status byte or status value to classify.
     * @param[in] data0 First MIDI/data payload byte.
     */
    {}
    /**
     * @brief Constructs a MidiMessage instance.
     *
     * @param[in] status MIDI status byte or status value to classify.
     * @param[in] data0 Data0 supplied to the operation.
     */
    MidiMessage(uint8_t status, uint8_t data0) :
        _raw { status, data0 }, _length(2)
    /**
     * @brief Constructs a MidiMessage instance.
     *
     * @param[in] status MIDI status byte or status value to classify.
     * @param[in] data0 First MIDI/data payload byte.
     * @param[in] data1 First MIDI data byte for the selected message type.
     */
    {}
    /**
     * @brief Constructs a MidiMessage instance.
     *
     * @param[in] status MIDI status byte or status value to classify.
     * @param[in] data0 Data0 supplied to the operation.
     * @param[in] data1 Data1 supplied to the operation.
     */
    MidiMessage(uint8_t status, uint8_t data0, uint8_t data1) :
        _raw { status, data0, data1 }, _length(3)
    /**
     * @brief Constructs a MidiMessage instance.
     *
     * @param[in] raw Raw encoded MIDI message value.
     * @param[in] length Number of bytes or elements covered by the operation.
     */
    {}
    /**
     * @brief Constructs a MidiMessage instance.
     *
     * @param[in] raw Raw supplied to the operation.
     * @param[in] length Number of bytes or elements covered by the operation.
     */
    MidiMessage(const uint8_t *raw, size_t length) {
        _raw[0] = length > 0 ? raw[0] : 0;
        _raw[1] = length > 1 ? raw[1] : 0;
        _raw[2] = length > 2 ? raw[2] : 0;
        _length = std::min(size_t(2), length);
    }

    /**
     * @brief Destroys the MidiMessage instance.
     */
    ~MidiMessage() {
        if (hasPayload()) {
            decPayloadRefCount(payloadID());
        }
    }

    /**
     * @brief Replaces this message with another message while maintaining shared-payload reference counts.
     *
     * @param[in] other MIDI message to copy.
     *
     * @return Reference to this message after assignment.
     */
    /**
     * @brief Replaces this message with another message while maintaining shared-payload reference counts.
     *
     * @param[in] other MIDI message to copy.
     *
     * @return Reference to this message after assignment.
     */
    MidiMessage& operator=(const MidiMessage& other)
    {
        if (hasPayload()) {
            decPayloadRefCount(payloadID());
        }

        _raw[0] = other._raw[0];
        _raw[1] = other._raw[1];
        _raw[2] = other._raw[2];
        _length = other._length;

        if (hasPayload()) {
            incPayloadRefCount(payloadID());
        }

        return *this;
    }

    // Factory

    /**
     * @brief Returns make note off.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] note Note value supplied to the operation.
     * @param[in] velocity Linear velocity vector in game-world units per second.
     *
     * @return Result of makeNoteOff().
     */
    static MidiMessage makeNoteOff(uint8_t channel, uint8_t note, uint8_t velocity = 0) {
        return MidiMessage(NoteOff | channel, note, velocity);
    }

    /**
     * @brief Returns make note on.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] note Note value supplied to the operation.
     * @param[in] velocity Linear velocity vector in game-world units per second.
     *
     * @return Result of makeNoteOn().
     */
    static MidiMessage makeNoteOn(uint8_t channel, uint8_t note, uint8_t velocity = 127) {
        return MidiMessage(NoteOn | channel, note, velocity);
    }

    /**
     * @brief Returns make key pressure.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] note Note value supplied to the operation.
     * @param[in] pressure MIDI/channel/key pressure value.
     *
     * @return Result of makeKeyPressure().
     */
    static MidiMessage makeKeyPressure(uint8_t channel, uint8_t note, uint8_t pressure) {
        return MidiMessage(KeyPressure | channel, note, pressure);
    }

    /**
     * @brief Returns make control change.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] controlNumber MIDI Control Change controller number in the 0..127 range.
     * @param[in] controlValue MIDI Control Change value in the 0..127 range.
     *
     * @return Result of makeControlChange().
     */
    static MidiMessage makeControlChange(uint8_t channel, uint8_t controlNumber, uint8_t controlValue) {
        return MidiMessage(ControlChange | channel, controlNumber, controlValue);
    }

    /**
     * @brief Returns make program change.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] programNumber MIDI Program Change program number in the 0..127 range.
     *
     * @return Result of makeProgramChange().
     */
    static MidiMessage makeProgramChange(uint8_t channel, uint8_t programNumber) {
        return MidiMessage(ProgramChange | channel, programNumber);
    }

    /**
     * @brief Returns make channel pressure.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] pressure MIDI/channel/key pressure value.
     *
     * @return Result of makeChannelPressure().
     */
    static MidiMessage makeChannelPressure(uint8_t channel, uint8_t pressure) {
        return MidiMessage(ChannelPressure | channel, pressure);
    }

    /**
     * @brief Returns make pitch bend.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] pitchBend MIDI pitch-bend value in the representation used by MidiMessage.
     *
     * @return Result of makePitchBend().
     */
    static MidiMessage makePitchBend(uint8_t channel, int pitchBend) {
        pitchBend = std::min(0, std::max(0x3fff, pitchBend + 0x2000));
        return MidiMessage(PitchBend | channel, pitchBend & 0x7f, (pitchBend >> 7) & 0x7f);
    }

    /**
     * @brief Returns make system exclusive.
     *
     * @param[in] data Input data to read, decode, copy, or process.
     * @param[in] length Number of bytes or elements covered by the operation.
     *
     * @return Result of makeSystemExclusive().
     */
    static MidiMessage makeSystemExclusive(const uint8_t *data, size_t length) {
        MidiMessage message(SystemExclusive);
        message.setPayload(data, length);
        return message;
    }

    /**
     * @brief Writes diagnostic state to the configured debug output.
     *
     * @param[in] msg Raw MIDI message byte to process.
     */
    static void dump(const MidiMessage &msg);

    /**
     * @brief Sets the payload pool.
     *
     * @param[in] data Input data to read, decode, copy, or process.
     * @param[in] length Number of bytes or elements covered by the operation.
     */
    static void setPayloadPool(uint8_t *data, size_t length);

private:
    /**
     * @brief Returns allocate payload.
     *
     * @param[in] length Number of bytes or elements covered by the operation.
     *
     * @return Result of allocatePayload().
     */
    static PayloadID allocatePayload(size_t length);
    /**
     * @brief Increments the reference count of a shared MIDI payload.
     *
     * @param[in] id Identifier of the addressed object/resource.
     */
    static void incPayloadRefCount(PayloadID id);
    /**
     * @brief Decrements the reference count and releases the payload when it reaches zero.
     *
     * @param[in] id Identifier of the addressed object/resource.
     */
    static void decPayloadRefCount(PayloadID id);
    /**
     * @brief Returns payload data.
     *
     * @param[in] id Identifier of the addressed object/resource.
     *
     * @return Pointer to the payload data; `nullptr` when no value is available.
     */
    static uint8_t *payloadData(PayloadID id);
    /**
     * @brief Returns payload length.
     *
     * @param[in] id Identifier of the addressed object/resource.
     *
     * @return Length of the referenced MIDI payload in bytes.
     */
    static size_t payloadLength(PayloadID id);

    /**
     * @brief Stores the fields required to represent payload pool.
     */
    struct PayloadPool {
        /**
         * @brief Pointer to data; `nullptr` denotes that no object/resource is assigned.
         */
        uint8_t *data = nullptr; ///< Pointer to data; `nullptr` denotes that no object/resource is assigned.
        /**
         * @brief Number of valid encoded bytes/elements.
         */
        size_t length = 0; ///< Number of valid encoded bytes/elements.

        /**
         * @brief Stores the fields required to represent slot.
         */
        struct Slot {
            /**
             * @brief Pointer to data; `nullptr` denotes that no object/resource is assigned.
             */
            uint8_t *data = nullptr; ///< Pointer to data; `nullptr` denotes that no object/resource is assigned.
            /**
             * @brief Number of valid encoded bytes/elements.
             */
            uint8_t length = 0; ///< Number of valid encoded bytes/elements.
            /**
             * @brief Number of ref items currently tracked or supported.
             */
            uint8_t refCount = 0; ///< Number of ref items currently tracked or supported.
        };

        /**
         * @brief Number of slot entries supported by the implementation.
         */
        static constexpr size_t SlotCount = 4; ///< Number of slot entries supported by the implementation.
        /**
         * @brief Fixed-capacity storage for slots.
         */
        std::array<Slot, SlotCount> slots; ///< Fixed payload-pool slots used to share long MIDI message payloads without heap allocation.

        /**
         * @brief Returns the valid.
         *
         * @return `true` if valid; otherwise `false`.
         */
        bool valid() const { return data != nullptr; }
        /**
         * @brief Returns get slot.
         *
         * @param[in] id Identifier of the addressed object/resource.
         *
         * @return Pointer to the get slot; `nullptr` when no value is available.
         */
        Slot *getSlot(PayloadID id) {
            if (valid() && id != InvalidPayload) {
                size_t slotIndex = id - 1;
                if (slotIndex < SlotCount) {
                    return &slots[slotIndex];
                }
            }
            return nullptr;
        }
    };

    static PayloadPool _payloadPool; ///< Static object pool that owns variable-length System Exclusive payload blocks.

    /**
     * @brief Raw encoded bytes that make up the value/message.
     */
    uint8_t _raw[3]; ///< Raw encoded bytes that make up the value/message.
    /**
     * @brief Number of valid encoded bytes/elements.
     */
    uint8_t _length = 0; ///< Number of valid encoded bytes/elements.
};
