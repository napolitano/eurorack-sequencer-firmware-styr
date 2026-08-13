/**
 * @file TargetState.h
 * @brief Declares the TargetState component used by the simulator core.
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

#include "TargetConfig.h"
#include "FrameBuffer.h"

#include <bitset>
#include <array>

#include <cstdint>

namespace sim {

/**
 * @brief Stores button state used while the component is running.
 */
struct ButtonState {
    /**
     * @brief Number of  entries supported by the implementation.
     */
    static constexpr int Count = TargetConfig::Rows * TargetConfig::ColsButton; ///< Number of  entries supported by the implementation.

    std::bitset<Count> state; ///< Pressed/released state bit for each simulated front-panel button.

    /**
     * @brief Constructs a ButtonState instance.
     */
    ButtonState() : state(false) {}

    /**
     * @brief Stores the supplied value.
     *
     * @param[in] index Zero-based set index.
     * @param[in] pressed Whether pressed is enabled for this operation.
     */
    void set(int index, bool pressed) {
        if (index >= 0 && index < Count) {
            state.set(index, pressed);
        }
    }

    /**
     * @brief Compares this value with another instance for value equality.
     *
     * @param[in] other Same-type value to compare with this instance.
     *
     * @return `true` when the represented state is equal; otherwise `false`.
     */
    bool operator==(const ButtonState &other) const { return state == other.state; }
    /**
     * @brief Compares this value with another instance for value inequality.
     *
     * @param[in] other Same-type value to compare with this instance.
     *
     * @return `true` when the represented state differs; otherwise `false`.
     */
    bool operator!=(const ButtonState &other) const { return state != other.state; }
};

/**
 * @brief Stores adc state used while the component is running.
 */
struct AdcState {
    /**
     * @brief Number of  entries supported by the implementation.
     */
    static constexpr int Count = TargetConfig::AdcChannels; ///< Number of  entries supported by the implementation.

    /**
     * @brief Fixed-capacity storage for state.
     */
    std::array<uint16_t, Count> state; ///< Raw ADC sample for each simulated analog-input channel.

    /**
     * @brief Constructs a AdcState instance.
     */
    AdcState() { state.fill(0); }

    /**
     * @brief Stores the supplied value.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] value Logical/state value to store or drive.
     */
    void set(int channel, uint16_t value) {
        if (channel >= 0 && channel < Count) {
            state[channel] = value;
        }
    }

    /**
     * @brief Compares this value with another instance for value equality.
     *
     * @param[in] other Same-type value to compare with this instance.
     *
     * @return `true` when the represented state is equal; otherwise `false`.
     */
    bool operator==(const AdcState &other) const { return state == other.state; }
    /**
     * @brief Compares this value with another instance for value inequality.
     *
     * @param[in] other Same-type value to compare with this instance.
     *
     * @return `true` when the represented state differs; otherwise `false`.
     */
    bool operator!=(const AdcState &other) const { return state != other.state; }
};

/**
 * @brief Stores digital input state used while the component is running.
 */
struct DigitalInputState {
    /**
     * @brief Number of  entries supported by the implementation.
     */
    static constexpr int Count = TargetConfig::DigitalInputs; ///< Number of  entries supported by the implementation.

    std::bitset<Count> state; ///< Logic level for each simulated digital input.

    /**
     * @brief Constructs a DigitalInputState instance.
     */
    DigitalInputState() : state(false) {}

    /**
     * @brief Stores the supplied value.
     *
     * @param[in] pin Hardware/platform pin identifier.
     * @param[in] high Whether high is enabled for this operation.
     */
    void set(int pin, bool high) {
        if (pin >= 0 && pin < Count) {
            state.set(pin, high);
        }
    }

    /**
     * @brief Compares this value with another instance for value equality.
     *
     * @param[in] other Same-type value to compare with this instance.
     *
     * @return `true` when the represented state is equal; otherwise `false`.
     */
    bool operator==(const DigitalInputState &other) const { return state == other.state; }
    /**
     * @brief Compares this value with another instance for value inequality.
     *
     * @param[in] other Same-type value to compare with this instance.
     *
     * @return `true` when the represented state differs; otherwise `false`.
     */
    bool operator!=(const DigitalInputState &other) const { return state != other.state; }
};

/**
 * @brief Stores led state used while the component is running.
 */
struct LedState {
    /**
     * @brief Number of  entries supported by the implementation.
     */
    static constexpr int Count = TargetConfig::Rows * TargetConfig::ColsLed; ///< Number of  entries supported by the implementation.

    /**
     * @brief Pointer to state; `nullptr` denotes that no object/resource is assigned.
     */
    std::bitset<Count * 2> state; ///< Pointer to state; `nullptr` denotes that no object/resource is assigned.

    /**
     * @brief Constructs a LedState instance.
     */
    LedState() : state(false) {}

    /**
     * @brief Stores the supplied value.
     *
     * @param[in] index Zero-based set index.
     * @param[in] red Whether red is enabled for this operation.
     * @param[in] green Whether green is enabled for this operation.
     */
    void set(int index, bool red, bool green) {
        if (index >= 0 && index < Count) {
            state.set(index * 2, red);
            state.set(index * 2 + 1, green);
        }
    }

    /**
     * @brief Compares this value with another instance for value equality.
     *
     * @param[in] other Same-type value to compare with this instance.
     *
     * @return `true` when the represented state is equal; otherwise `false`.
     */
    bool operator==(const LedState &other) const { return state == other.state; }
    /**
     * @brief Compares this value with another instance for value inequality.
     *
     * @param[in] other Same-type value to compare with this instance.
     *
     * @return `true` when the represented state differs; otherwise `false`.
     */
    bool operator!=(const LedState &other) const { return state != other.state; }
};

/**
 * @brief Stores gate output state used while the component is running.
 */
struct GateOutputState {
    /**
     * @brief Number of  entries supported by the implementation.
     */
    static constexpr int Count = TargetConfig::GateChannels; ///< Number of  entries supported by the implementation.

    std::bitset<Count> state; ///< Logic level for each simulated gate output.

    /**
     * @brief Constructs a GateOutputState instance.
     */
    GateOutputState() : state(false) {}

    /**
     * @brief Stores the supplied value.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] high Whether high is enabled for this operation.
     */
    void set(int channel, bool high) {
        if (channel >= 0 && channel < Count) {
            state.set(channel, high);
        }
    }

    /**
     * @brief Compares this value with another instance for value equality.
     *
     * @param[in] other Same-type value to compare with this instance.
     *
     * @return `true` when the represented state is equal; otherwise `false`.
     */
    bool operator==(const GateOutputState &other) const { return state == other.state; }
    /**
     * @brief Compares this value with another instance for value inequality.
     *
     * @param[in] other Same-type value to compare with this instance.
     *
     * @return `true` when the represented state differs; otherwise `false`.
     */
    bool operator!=(const GateOutputState &other) const { return state != other.state; }
};

/**
 * @brief Stores dac state used while the component is running.
 */
struct DacState {
    /**
     * @brief Number of  entries supported by the implementation.
     */
    static constexpr int Count = TargetConfig::DacChannels; ///< Number of  entries supported by the implementation.

    /**
     * @brief Fixed-capacity storage for state.
     */
    std::array<uint16_t, Count> state; ///< Raw DAC code for each simulated analog-output channel.

    /**
     * @brief Constructs a DacState instance.
     */
    DacState() { state.fill(0); }

    /**
     * @brief Stores the supplied value.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] value Logical/state value to store or drive.
     */
    void set(int channel, uint16_t value) {
        if (channel >= 0 && channel < Count) {
            state[channel] = value;
        }
    }

    /**
     * @brief Compares this value with another instance for value equality.
     *
     * @param[in] other Same-type value to compare with this instance.
     *
     * @return `true` when the represented state is equal; otherwise `false`.
     */
    bool operator==(const DacState &other) const { return state == other.state; }
    /**
     * @brief Compares this value with another instance for value inequality.
     *
     * @param[in] other Same-type value to compare with this instance.
     *
     * @return `true` when the represented state differs; otherwise `false`.
     */
    bool operator!=(const DacState &other) const { return state != other.state; }
};

/**
 * @brief Stores digital output state used while the component is running.
 */
struct DigitalOutputState {
    /**
     * @brief Number of  entries supported by the implementation.
     */
    static constexpr int Count = TargetConfig::DigitalOutputs; ///< Number of  entries supported by the implementation.

    std::bitset<Count> state; ///< Logic level for each simulated digital output.

    /**
     * @brief Constructs a DigitalOutputState instance.
     */
    DigitalOutputState() : state(false) {}

    /**
     * @brief Stores the supplied value.
     *
     * @param[in] pin Hardware/platform pin identifier.
     * @param[in] high Whether high is enabled for this operation.
     */
    void set(int pin, bool high) {
        if (pin >= 0 && pin < Count) {
            state.set(pin, high);
        }
    }

    /**
     * @brief Compares this value with another instance for value equality.
     *
     * @param[in] other Same-type value to compare with this instance.
     *
     * @return `true` when the represented state is equal; otherwise `false`.
     */
    bool operator==(const DigitalOutputState &other) const { return state == other.state; }
    /**
     * @brief Compares this value with another instance for value inequality.
     *
     * @param[in] other Same-type value to compare with this instance.
     *
     * @return `true` when the represented state differs; otherwise `false`.
     */
    bool operator!=(const DigitalOutputState &other) const { return state != other.state; }
};

/**
 * @brief Stores lcd state used while the component is running.
 */
struct LcdState {
    FrameBuffer state; ///< Snapshot of simulated state values captured for comparison, tracing, or rendering.

    /**
     * @brief Constructs a LcdState instance.
     */
    LcdState() {
        state.fill(0);
    }

    /**
     * @brief Stores the supplied value.
     *
     * @param[in] frameBuffer Framebuffer that receives drawing operations.
     */
    void set(const FrameBuffer &frameBuffer) {
        state = frameBuffer;
    }

    /**
     * @brief Compares this value with another instance for value equality.
     *
     * @param[in] other Same-type value to compare with this instance.
     *
     * @return `true` when the represented state is equal; otherwise `false`.
     */
    bool operator==(const LcdState &other) const { return state == other.state; }
    /**
     * @brief Compares this value with another instance for value inequality.
     *
     * @param[in] other Same-type value to compare with this instance.
     *
     * @return `true` when the represented state differs; otherwise `false`.
     */
    bool operator!=(const LcdState &other) const { return state != other.state; }
};

/**
 * @brief Stores target state used while the component is running.
 */
struct TargetState {
    ButtonState button; ///< Snapshot of simulated button values captured for comparison, tracing, or rendering.
    AdcState adc; ///< Snapshot of simulated adc values captured for comparison, tracing, or rendering.
    DigitalInputState digitalInput; ///< Snapshot of simulated digital input values captured for comparison, tracing, or rendering.
    LedState led; ///< Snapshot of simulated led values captured for comparison, tracing, or rendering.
    /**
     * @brief Current digital gate output state.
     */
    GateOutputState gateOutput; ///< Current digital gate output state.
    DacState dac; ///< Snapshot of simulated dac values captured for comparison, tracing, or rendering.
    DigitalOutputState digitalOutput; ///< Snapshot of simulated digital output values captured for comparison, tracing, or rendering.
    LcdState lcd; ///< Snapshot of simulated lcd values captured for comparison, tracing, or rendering.
};

} // namespace sim
