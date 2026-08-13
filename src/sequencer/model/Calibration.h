/**
 * @file Calibration.h
 * @brief Declares the Calibration component used by the sequencer model.
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

#include "core/utils/StringBuilder.h"
#include "core/math/Math.h"

#include <array>

/**
 * @brief Stores and manipulates calibration model data.
 */
class Calibration {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    /**
     * @brief Stores and manipulates cv output model data.
     */
    class CvOutput {
    public:
        /**
         * @brief Minimum supported voltage.
         */
        static constexpr int MinVoltage = -5; ///< Minimum supported voltage.
        /**
         * @brief Maximum supported voltage.
         */
        static constexpr int MaxVoltage = 5; ///< Maximum supported voltage.
        /**
         * @brief Items per volt constant used by this component.
         */
        static constexpr int ItemsPerVolt = 1; ///< Number of calibration points stored for each volt of CV output range.
        static constexpr int ItemCount = (MaxVoltage - MinVoltage) * ItemsPerVolt + 1; ///< Number of stored calibration points spanning the inclusive voltage range. ///< Number of stored calibration points spanning the inclusive voltage range.

        typedef std::array<uint16_t, ItemCount> ItemArray;

        // items

        /**
         * @brief Returns item to volts.
         *
         * @param[in] index Zero-based item to volts index.
         *
         * @return Voltage, in volts, corresponding to the item/value.
         */
        static float itemToVolts(int index) {
            return float(index) / ItemsPerVolt + MinVoltage;
        }

        /**
         * @brief Returns the display name of the addressed item.
         *
         * @param[out] str String builder that receives the formatted representation.
         * @param[in] index Zero-based item name index.
         */
        static void itemName(StringBuilder &str, int index) {
            str(TXT_MODEL_ITEM_TO_VOLTS_NAME, itemToVolts(index));
        }

        /**
         * @brief Returns the items.
         *
         * @return Reference to the stored item collection.
         */
        const ItemArray &items() const { return _items; }
              /**
               * @brief Returns the items.
               *
               * @return Reference to the stored item collection.
               */
              ItemArray &items()       { return _items; }

        /**
         * @brief Returns item.
         *
         * @param[in] index Zero-based item index.
         *
         * @return Result of item().
         */
        int item(int index) const {
            return _items[index] & 0x7fff;
        }

        /**
         * @brief Sets the item.
         *
         * @param[in] index Zero-based item index.
         * @param[in] value New item to store or apply.
         * @param[in] doUpdate Whether do update is enabled for this operation.
         */
        void setItem(int index, int value, bool doUpdate = true) {
            _items[index] = (_items[index] & 0x8000) | clamp(value, 0, 0x7fff);
            if (doUpdate) {
                update();
            }
        }

        /**
         * @brief Adjusts the item from a UI edit delta.
         *
         * @param[in] index Zero-based item index.
         * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
         * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
         */
        void editItem(int index, int value, int shift) {
            // inverted to improve usability
            setItem(index, item(index) - value * (shift ? 50 : 1));
        }

        /**
         * @brief Formats the item into the supplied string builder/output.
         *
         * @param[in] index Zero-based item index.
         * @param[out] str String builder that receives the formatted representation.
         */
        void printItem(int index, StringBuilder &str) const {
            // inverted to improve usability
            if (userDefined(index)) {
                str(TXT_MODEL_ITEM_VALUE, 0x7fff - item(index));
            } else {
                str(TXT_MODEL_ITEM_VALUE_AUTO, 0x7fff - item(index));
            }
        }

        /**
         * @brief Returns user defined.
         *
         * @param[in] index Zero-based user defined index.
         *
         * @return `true` if user defined; otherwise `false`.
         */
        bool userDefined(int index) const {
            return _items[index] & 0x8000;
        }

        /**
         * @brief Sets the user defined.
         *
         * @param[in] index Zero-based user defined index.
         * @param[in] value New user defined to store or apply.
         */
        void setUserDefined(int index, bool value) {
            _items[index] = (_items[index] & 0x7fff) | (value ? 0x8000 : 0);
            update();
        }

        /**
         * @brief Returns default item value.
         *
         * @param[in] index Zero-based default item index.
         *
         * @return Result of defaultItemValue().
         */
        int defaultItemValue(int index) const {
            // In ideal DAC/OpAmp configuration we get:
            // 0     ->  5.17V
            // 32768 -> -5.25V
            static constexpr float volts0 = 5.17f; ///< Ideal uncalibrated output voltage corresponding to DAC code 0. ///< Ideal uncalibrated output voltage corresponding to DAC code 0.
            static constexpr float volts1 = -5.25f; ///< Ideal uncalibrated output voltage corresponding to DAC code 32768. ///< Ideal uncalibrated output voltage corresponding to DAC code 32768.

            float volts = itemToVolts(index);

            return clamp(int((volts - volts0) / (volts1 - volts0) * 32768), 0, 0x7fff);
        }

        /**
         * @brief Returns volts to value.
         *
         * @param[in] volts Voltage value in volts.
         *
         * @return Model value corresponding to the supplied voltage.
         */
        uint16_t voltsToValue(float volts) const {
            volts = clamp(volts, float(MinVoltage), float(MaxVoltage));
            float fIndex = (volts - MinVoltage) * ItemsPerVolt;
            int index = std::floor(fIndex);
            if (index < ItemCount - 1) {
                float t = fIndex - index;
                return lerp(t, item(index), item(index + 1));
            } else {
                return item(ItemCount - 1);
            }
        }

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
         * @brief Updates the CvOutput for the current service cycle.
         */
        void update();

        /**
         * @brief Calibration table values indexed across the supported output-voltage range.
         */
        ItemArray _items; ///< Stored calibration/scale item values indexed by the model-defined item domain.
    };

    typedef std::array<CvOutput, CONFIG_CV_OUTPUT_CHANNELS> CvOutputArray;

    //----------------------------------------
    // cvOutputs
    //----------------------------------------

    /**
     * @brief Returns the cv outputs.
     *
     * @return Reference to the cv outputs.
     */
    const CvOutputArray &cvOutputs() const { return _cvOutputs; }
          /**
           * @brief Returns the cv outputs.
           *
           * @return Reference to the cv outputs.
           */
          CvOutputArray &cvOutputs()       { return _cvOutputs; }

    /**
     * @brief Returns the current CV value for a track output channel.
     *
     * @param[in] index Zero-based CV-output channel index.
     *
     * @return Reference to the cv output.
     */
    const CvOutput &cvOutput(int index) const { return _cvOutputs[index]; }
          /**
           * @brief Returns the current CV value for a track output channel.
           *
           * @param[in] index Zero-based CV-output channel index.
           *
           * @return Reference to the cv output.
           */
          CvOutput &cvOutput(int index)       { return _cvOutputs[index]; }

    //----------------------------------------
    // Methods
    //----------------------------------------

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
    CvOutputArray _cvOutputs; ///< Owned fixed-size collection of cv outputs.
};
