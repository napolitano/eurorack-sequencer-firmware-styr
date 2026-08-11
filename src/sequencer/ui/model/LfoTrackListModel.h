/**
 * @file LfoTrackListModel.h
 * @brief Declares the LfoTrackListModel component used by the sequencer UI.
 *
 * @author Axel Napolitano — Styr implementation and modernization
 * @copyright 2026 Axel Napolitano
 *
 * @par License
 * MIT; see LICENSES/MIT.txt.
 *
 * SPDX-FileCopyrightText: 2026 Axel Napolitano
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include "Config.h"

#include "RoutableListModel.h"

#include "model/LfoTrack.h"

class LfoTrackListModel : public RoutableListModel {
public:
    void setTrack(LfoTrack &track) {
        _track = &track;
    }

    virtual int rows() const override {
        return Last;
    }

    virtual int columns() const override {
        return 2;
    }

    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (column == 0) {
            formatName(Item(row), str);
        } else if (column == 1) {
            formatValue(Item(row), str);
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
        if (column == 1) {
            editValue(Item(row), value, shift);
        }
    }

    virtual Routing::Target routingTarget(int row) const override {
        switch (Item(row)) {
        case Waveform: return Routing::Target::LfoShape;
        case Hi:       return Routing::Target::LfoHi;
        case Low:      return Routing::Target::LfoLow;
        case Speed:    return Routing::Target::LfoSpeed;
        case Mode:     return Routing::Target::LfoMode;
        case Clip:     return Routing::Target::LfoClip;
        case PulseWidth: return Routing::Target::LfoPulseWidth;
        default: return Routing::Target::None;
        }
    }

private:
    enum Item {
        Waveform,
        Hi,
        Low,
        Mode,
        Speed,
        Clip,
        PulseWidth,
        Last
    };

    static const char *itemName(Item item) {
        switch (item) {
        case Waveform:   return "Shape";
        case Hi:         return "Hi";
        case Low:        return "Low";
        case Mode:       return "Mode";
        case Speed:      return "Speed";
        case Clip:       return "Clip";
        case PulseWidth: return "PW";
        case Last:       break;
        }
        return nullptr;
    }

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    void formatValue(Item item, StringBuilder &str) const {
        if (!_track) { str("-"); return; }
        switch (item) {
        case Waveform:   _track->printWaveform(str); break;
        case Hi:         _track->printHi(str); break;
        case Low:        _track->printLow(str); break;
        case Mode:       _track->printMode(str); break;
        case Speed:      _track->printSpeed(str); break;
        case Clip:       _track->printClip(str); break;
        case PulseWidth: _track->printPulseWidth(str); break;
        case Last: break;
        }
    }

    void editValue(Item item, int value, bool shift) {
        if (!_track) return;
        switch (item) {
        case Waveform:   _track->editWaveform(value, shift); break;
        case Hi:         _track->editHi(value, shift); break;
        case Low:        _track->editLow(value, shift); break;
        case Mode:       _track->editMode(value, shift); break;
        case Speed:      _track->editSpeed(value, shift); break;
        case Clip:       _track->editClip(value, shift); break;
        case PulseWidth: _track->editPulseWidth(value, shift); break;
        case Last: break;
        }
    }

    LfoTrack *_track = nullptr;
};

