/**
 * @file Event.h
 * @brief Declares the Event component used by the sequencer UI.
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

#include "Key.h"

#include "engine/MidiPort.h"

#include "core/midi/MidiMessage.h"

/**
 * @brief Implements event operations and maintains the associated runtime state.
 */
class Event {
public:
    /**
     * @brief Enumerates the supported type values.
     */
    enum Type {
        KeyUp, ///< Selects the key up type.
        KeyDown, ///< Selects the key down type.
        KeyPress, ///< Selects the key press type.
        Encoder, ///< Selects the encoder type.
        Midi, ///< Selects the midi type.
    };

    /**
     * @brief Constructs a Event instance.
     *
     * @param[in] type Type discriminator selecting the variant or behavior handled by this API.
     */
    Event(Type type) :
        /**
         * @brief Returns the type.
         */
        _type(type)
    {}

    /**
     * @brief Returns the type.
     *
     * @return Event type identifier.
     */
    Type type() const { return _type; }

    /**
     * @brief Returns the consumed.
     *
     * @return `true` if consumed; otherwise `false`.
     */
    bool consumed() const { return _consumed; }
    /**
     * @brief Consumes the current buffered event/value.
     */
    void consume() { _consumed = true; }

    template<typename T>
    /**
     * @brief Returns the as.
     *
     * @return Reference to the as.
     */
    inline T &as() {
        return *static_cast<T *>(this);
    }

private:
    Type _type; ///< Selected type that determines how `Event` interprets or renders its data.
    /**
     * @brief Whether consumed is true in the current state.
     */
    bool _consumed = false; ///< True after an event handler has consumed the event and propagation should stop.
};

/**
 * @brief Implements key event operations and maintains the associated runtime state.
 */
class KeyEvent : public Event {
public:
    /**
     * @brief Constructs a KeyEvent instance.
     *
     * @param[in] type Type discriminator selecting the variant or behavior handled by this API.
     * @param[in] key Key identifier associated with the input event.
     */
    KeyEvent(Type type, const Key &key) :
        /**
         * @brief Returns the event.
         */
        Event(type),
        /**
         * @brief Returns the key.
         */
        _key(key)
    {}

    /**
     * @brief Returns the key.
     *
     * @return Reference to the key.
     */
    const Key &key() const { return _key; }

private:
    Key _key; ///< Front-panel key associated with this UI key event.
};

/**
 * @brief Implements key press event operations and maintains the associated runtime state.
 */
class KeyPressEvent : public KeyEvent {
public:
    /**
     * @brief Constructs a KeyPressEvent instance.
     *
     * @param[in] type Type discriminator selecting the variant or behavior handled by this API.
     * @param[in] key Key identifier associated with the input event.
     * @param[in] count Number of items covered by the operation.
     */
    KeyPressEvent(Type type, const Key &key, int count) :
        /**
         * @brief Returns the key event.
         */
        KeyEvent(type, key),
        /**
         * @brief Returns the count.
         */
        _count(count)
    {}

    /**
     * @brief Returns the count.
     *
     * @return Number of count represented by this object.
     */
    int count() const { return _count; }

private:
    int _count; ///< Repeat/count value carried by this UI event.
};

/**
 * @brief Implements encoder event operations and maintains the associated runtime state.
 */
class EncoderEvent : public Event {
public:
    /**
     * @brief Constructs a EncoderEvent instance.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] pressed Whether pressed is enabled for this operation.
     */
    EncoderEvent(int value, bool pressed) :
        /**
         * @brief Performs the event operation for this EncoderEvent.
         *
         * @param[in] Encoder Encoder supplied to the operation.
         */
        Event(Event::Encoder),
        /**
         * @brief Returns the value.
         */
        _value(value),
        /**
         * @brief Returns the pressed.
         */
        _pressed(pressed)
    {}

    /**
     * @brief Returns the value.
     *
     * @return Value carried by this event or model field.
     */
    int value() const { return _value; }
    /**
     * @brief Returns the pressed.
     *
     * @return `true` if pressed; otherwise `false`.
     */
    bool pressed() const { return _pressed; }

private:
    /**
     * @brief UI value representing value.
     */
    int _value; ///< Integer payload carried by the UI event; interpretation depends on the event type.
    /**
     * @brief Whether pressed is true in the current state.
     */
    bool _pressed; ///< Current pressed/down state of the associated key or control.
};

/**
 * @brief Implements midi event operations and maintains the associated runtime state.
 */
class MidiEvent : public Event {
public:
    /**
     * @brief Constructs a MidiEvent instance.
     *
     * @param[in] port MIDI or logical port selected for the operation.
     * @param[in] message Message to process.
     */
    MidiEvent(MidiPort port, const MidiMessage &message) :
        /**
         * @brief Performs the event operation for this MidiEvent.
         *
         * @param[in] Midi Midi supplied to the operation.
         */
        Event(Event::Midi),
        /**
         * @brief Returns the port.
         */
        _port(port),
        /**
         * @brief Returns the message.
         */
        _message(message)
    {}

    /**
     * @brief Returns the port.
     *
     * @return Configured MIDI port.
     */
    MidiPort port() const { return _port; }

    /**
     * @brief Returns the message.
     *
     * @return Reference to the message.
     */
    const MidiMessage &message() const { return _message; }

private:
    MidiPort _port; ///< MIDI port associated with this state.
    MidiMessage _message; ///< MIDI message stored/queued by this state.
};
