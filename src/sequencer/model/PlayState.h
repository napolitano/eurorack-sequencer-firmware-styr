/**
 * @file PlayState.h
 * @brief Declares the PlayState component used by the sequencer model.
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
#include "ModelUtils.h"
#include "Routing.h"

#include <array>

#include <cstdint>

class Project;

/**
 * @brief Stores play state used while the component is running.
 */
class PlayState {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    /**
     * @brief Enumerates the supported execute type values.
     */
    enum ExecuteType {
        Immediate, ///< Selects the immediate execute type.
        Synced, ///< Selects the synced execute type.
        Latched, ///< Selects the latched execute type.
    };

    /**
     * @brief Stores track state used while the component is running.
     */
    class TrackState {
    public:
        //----------------------------------------
        // Properties
        //----------------------------------------

        // fillAmount

        /**
         * @brief Fills amount.
         *
         * @return Current fill amount used by playback decisions.
         */
        int fillAmount() const { return _fillAmount; }
        /**
         * @brief Sets the fill amount.
         *
         * @param[in] fillAmount Fill amount used by the probabilistic fill logic.
         */
        void setFillAmount(int fillAmount) {
            _fillAmount = clamp(fillAmount, 0, 100);
        }

        /**
         * @brief Adjusts the fill amount from a UI edit delta.
         *
         * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
         * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
         */
        void editFillAmount(int value, bool shift) {
            setFillAmount(ModelUtils::adjustedByStep(fillAmount(), value, 10, shift));
        }

        /**
         * @brief Formats the fill amount into the supplied string builder/output.
         *
         * @param[out] str String builder that receives the formatted representation.
         */
        void printFillAmount(StringBuilder &str) const {
            str(TXT_MODEL_FILL_AMOUNT, fillAmount());
        }

        //----------------------------------------
        // State
        //----------------------------------------

        /**
         * @brief Returns the mute.
         *
         * @return `true` if mute; otherwise `false`.
         */
        bool mute() const { return _state & Mute; }
        /**
         * @brief Returns the requested mute.
         *
         * @return `true` if requested mute; otherwise `false`.
         */
        bool requestedMute() const { return _state & RequestedMute; }
        /**
         * @brief Reports whether the TrackState has mute request.
         *
         * @return `true` if mute request; otherwise `false`.
         */
        bool hasMuteRequest() const { return hasRequests(State::MuteRequests); }

        /**
         * @brief Fills the target region or buffer using the current/supplied value.
         *
         * @return `true` if fill; otherwise `false`.
         */
        bool fill() const { return _state & Fill; }

        //----------------------------------------
        // Pattern
        //----------------------------------------

        /**
         * @brief Returns the pattern.
         *
         * @return Current pattern index.
         */
        int pattern() const { return _pattern; }
        /**
         * @brief Returns the requested pattern.
         *
         * @return Pattern index requested for the next eligible pattern change.
         */
        int requestedPattern() const { return _requestedPattern; }
        /**
         * @brief Reports whether the TrackState has pattern request.
         *
         * @return `true` if pattern request; otherwise `false`.
         */
        bool hasPatternRequest() const { return hasRequests(State::PatternRequests); }

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
        /**
         * @brief Enumerates the supported state values.
         */
        enum State {
            Mute                    = 1<<0, ///< Selects the mute state.
            RequestedMute           = 1<<1, ///< Selects the requested mute state.
            Fill                    = 1<<2, ///< Selects the fill state.
            FillHold                = 1<<3, ///< Selects the fill hold state.

            ImmediateMuteRequest    = 1<<4, ///< Selects the immediate mute request state.
            SyncedMuteRequest       = 1<<5, ///< Selects the synced mute request state.
            LatchedMuteRequest      = 1<<6, ///< Selects the latched mute request state.

            ImmediatePatternRequest = 1<<7, ///< Selects the immediate pattern request state.
            SyncedPatternRequest    = 1<<8, ///< Selects the synced pattern request state.
            LatchedPatternRequest   = 1<<9, ///< Selects the latched pattern request state.

            MuteRequests = ImmediateMuteRequest | SyncedMuteRequest | LatchedMuteRequest, ///< Selects the mute requests state.
            PatternRequests = ImmediatePatternRequest | SyncedPatternRequest | LatchedPatternRequest, ///< Selects the pattern requests state.
            ImmediateRequests = ImmediateMuteRequest | ImmediatePatternRequest, ///< Selects the immediate requests state.
            SyncedRequests = SyncedMuteRequest | SyncedPatternRequest, ///< Selects the synced requests state.
            LatchedRequests = LatchedMuteRequest | LatchedPatternRequest ///< Selects the latched requests state.
        };

        /**
         * @brief Returns mute request from execute type.
         *
         * @param[in] type Type discriminator selecting the variant or behavior handled by this API.
         *
         * @return Result of muteRequestFromExecuteType().
         */
        static State muteRequestFromExecuteType(ExecuteType type) {
            return State(int(ImmediateMuteRequest) << int(type));
        }

        /**
         * @brief Returns pattern request from execute type.
         *
         * @param[in] type Type discriminator selecting the variant or behavior handled by this API.
         *
         * @return Result of patternRequestFromExecuteType().
         */
        static State patternRequestFromExecuteType(ExecuteType type) {
            return State(int(ImmediatePatternRequest) << int(type));
        }

        /**
         * @brief Sets the requests.
         *
         * @param[in] requests Bit mask or collection of pending requests.
         */
        void setRequests(int requests) {
            _state |= uint16_t(requests);
        }

        /**
         * @brief Clears requests.
         *
         * @param[in] requests Bit mask or collection of pending requests.
         */
        void clearRequests(int requests) {
            _state &= ~uint16_t(requests);
        }

        /**
         * @brief Reports whether the TrackState has requests.
         *
         * @param[in] requests Bit mask or collection of pending requests.
         *
         * @return `true` if requests; otherwise `false`.
         */
        bool hasRequests(int requests) const {
            return _state & uint16_t(requests);
        }

        /**
         * @brief Sets the mute.
         *
         * @param[in] mute `true` to enable/set mute; `false` to disable/clear it.
         */
        void setMute(bool mute) {
            if (mute) {
                _state |= Mute;
            } else {
                _state &= ~Mute;
            }
        }

        /**
         * @brief Sets the requested mute.
         *
         * @param[in] mute Whether mute is enabled for this operation.
         */
        void setRequestedMute(bool mute) {
            if (mute) {
                _state |= RequestedMute;
            } else {
                _state &= ~RequestedMute;
            }
        }

        /**
         * @brief Sets the fill.
         *
         * @param[in] fill `true` to enable/set fill; `false` to disable/clear it.
         * @param[in] hold Whether hold is enabled for this operation.
         */
        void setFill(bool fill, bool hold) {
            if (fill) {
                _state |= Fill;
                if (hold) {
                    _state |= FillHold;
                } else {
                    _state &= ~FillHold;
                }
            } else {
                if (!(_state & FillHold)) {
                    _state &= ~Fill;
                }
            }
        }

        /**
         * @brief Sets the pattern.
         *
         * @param[in] pattern New zero-based pattern index to store or apply.
         */
        void setPattern(int pattern) {
            _pattern = pattern;
        }

        /**
         * @brief Sets the requested pattern.
         *
         * @param[in] pattern Pattern index or pattern data addressed by the operation.
         */
        void setRequestedPattern(int pattern) {
            _requestedPattern = pattern;
        }

        uint16_t _state; ///< Packed playback/request flags for the enclosing runtime state object.
        uint8_t _pattern; ///< Currently active zero-based pattern index for this track.
        uint8_t _requestedPattern; ///< Pending zero-based pattern index to commit at the selected execution boundary.
        uint8_t _fillAmount; ///< Current fill amount used when evaluating probabilistic fill behavior.

        friend class PlayState;
        friend class Engine;
    };

    /**
     * @brief Stores song state used while the component is running.
     */
    class SongState {
    public:
        /**
         * @brief Returns the playing.
         *
         * @return `true` if playing; otherwise `false`.
         */
        bool playing() const { return _state & Playing; }
        /**
         * @brief Reports whether the SongState has play requests.
         *
         * @return `true` if play requests; otherwise `false`.
         */
        bool hasPlayRequests() const { return hasRequests(PlayRequests); }
        /**
         * @brief Returns the current slot.
         *
         * @return Current project storage slot.
         */
        int currentSlot() const { return _currentSlot; }
        /**
         * @brief Returns the current repeat.
         *
         * @return Current song-step repeat counter.
         */
        int currentRepeat() const { return _currentRepeat; }
        /**
         * @brief Returns the requested slot.
         *
         * @return Project slot requested for the next eligible load/change.
         */
        int requestedSlot() const { return _requestedSlot; }

        /**
         * @brief Clears stored runtime/container state.
         */
        void clear();

    private:
        /**
         * @brief Enumerates the supported state values.
         */
        enum State {
            Playing                 = 1<<0, ///< Selects the playing state.

            ImmediatePlayRequest    = 1<<1, ///< Selects the immediate play request state.
            SyncedPlayRequest       = 1<<2, ///< Selects the synced play request state.
            LatchedPlayRequest      = 1<<3, ///< Selects the latched play request state.

            ImmediateStopRequest    = 1<<4, ///< Selects the immediate stop request state.
            SyncedStopRequest       = 1<<5, ///< Selects the synced stop request state.
            LatchedStopRequest      = 1<<6, ///< Selects the latched stop request state.

            PlayRequests = ImmediatePlayRequest | SyncedPlayRequest | LatchedPlayRequest, ///< Selects the play requests state.
            StopRequests = ImmediateStopRequest | SyncedStopRequest | LatchedStopRequest, ///< Selects the stop requests state.
            ImmediateRequests = ImmediatePlayRequest | ImmediateStopRequest, ///< Selects the immediate requests state.
            SyncedRequests = SyncedPlayRequest | SyncedStopRequest, ///< Selects the synced requests state.
            LatchedRequests = LatchedPlayRequest | LatchedStopRequest ///< Selects the latched requests state.
        };

        /**
         * @brief Returns play request from execute type.
         *
         * @param[in] type Type discriminator selecting the variant or behavior handled by this API.
         *
         * @return Result of playRequestFromExecuteType().
         */
        static State playRequestFromExecuteType(ExecuteType type) {
            return State(1<<(1 + type));
        }

        /**
         * @brief Stops request from execute type.
         *
         * @param[in] type Type discriminator selecting the variant or behavior handled by this API.
         *
         * @return Result of stopRequestFromExecuteType().
         */
        static State stopRequestFromExecuteType(ExecuteType type) {
            return State(1<<(4 + type));
        }

        /**
         * @brief Sets the requests.
         *
         * @param[in] requests Bit mask or collection of pending requests.
         */
        void setRequests(int requests) {
            _state |= uint8_t(requests);
        }

        /**
         * @brief Clears requests.
         *
         * @param[in] requests Bit mask or collection of pending requests.
         */
        void clearRequests(int requests) {
            _state &= ~uint8_t(requests);
        }

        /**
         * @brief Reports whether the SongState has requests.
         *
         * @param[in] requests Bit mask or collection of pending requests.
         *
         * @return `true` if requests; otherwise `false`.
         */
        bool hasRequests(int requests) const {
            return _state & uint8_t(requests);
        }

        /**
         * @brief Sets the playing.
         *
         * @param[in] playing `true` to enable/set playing; `false` to disable/clear it.
         */
        void setPlaying(bool playing) {
            if (playing) {
                _state |= Playing;
            } else {
                _state &= ~Playing;
            }
        }

        /**
         * @brief Sets the requested slot.
         *
         * @param[in] slot Zero-based project/storage slot index.
         */
        void setRequestedSlot(int slot) {
            _requestedSlot = slot;
        }

        /**
         * @brief Sets the current slot.
         *
         * @param[in] slot Zero-based project/storage slot index.
         */
        void setCurrentSlot(int slot) {
            _currentSlot = slot;
        }

        /**
         * @brief Sets the current repeat.
         *
         * @param[in] slot Zero-based project/storage slot index.
         */
        void setCurrentRepeat(int slot) {
            _currentRepeat = slot;
        }

        uint8_t _state; ///< Packed playback/request flags for the enclosing runtime state object.
        uint8_t _requestedSlot; ///< Song slot requested for the next eligible transition.
        /**
         * @brief Current slot.
         */
        uint8_t _currentSlot; ///< Current slot.
        /**
         * @brief Current repeat.
         */
        uint8_t _currentRepeat; ///< Current repeat.

        friend class PlayState;
        friend class Engine;
    };

    //----------------------------------------
    // Properties
    //----------------------------------------

    // track states

    /**
     * @brief Returns track state.
     *
     * @param[in] track Track model operated on by this component.
     *
     * @return Reference to the track state.
     */
    const TrackState &trackState(int track) const { return _trackStates[track]; }
          /**
           * @brief Returns track state.
           *
           * @param[in] track Track model operated on by this component.
           *
           * @return Reference to the track state.
           */
          TrackState &trackState(int track)       { return _trackStates[track]; }

    // song state

    /**
     * @brief Returns the song state.
     *
     * @return Reference to the song state.
     */
    const SongState &songState() const { return _songState; }
          /**
           * @brief Returns the song state.
           *
           * @return Reference to the song state.
           */
          SongState &songState()       { return _songState; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    /**
     * @brief Constructs a PlayState instance.
     *
     * @param[in] project Project model read or modified by the operation.
     */
    PlayState(Project &project);

    // mutes

    /**
     * @brief Mutes the addressed track.
     *
     * @param[in] track Track model operated on by this component.
     * @param[in] executeType Execution/commit timing mode used for the request.
     */
    void muteTrack(int track, ExecuteType executeType = Immediate);
    /**
     * @brief Unmutes the addressed track.
     *
     * @param[in] track Track model operated on by this component.
     * @param[in] executeType Execution/commit timing mode used for the request.
     */
    void unmuteTrack(int track, ExecuteType executeType = Immediate);
    /**
     * @brief Toggles mute track.
     *
     * @param[in] track Track model operated on by this component.
     * @param[in] executeType Execution/commit timing mode used for the request.
     */
    void toggleMuteTrack(int track, ExecuteType executeType = Immediate);

    /**
     * @brief Mutes all tracks.
     *
     * @param[in] executeType Execution/commit timing mode used for the request.
     */
    void muteAll(ExecuteType executeType = Immediate);
    /**
     * @brief Unmutes all tracks.
     *
     * @param[in] executeType Execution/commit timing mode used for the request.
     */
    void unmuteAll(ExecuteType executeType = Immediate);

    // solos

    /**
     * @brief Mutes other tracks so the addressed track plays alone.
     *
     * @param[in] track Track model operated on by this component.
     * @param[in] executeType Execution/commit timing mode used for the request.
     */
    void soloTrack(int track, ExecuteType executeType = Immediate);

    // fills

    /**
     * @brief Fills track.
     *
     * @param[in] track Track model operated on by this component.
     * @param[in] fill Whether fill is enabled for this operation.
     * @param[in] hold Whether hold is enabled for this operation.
     */
    void fillTrack(int track, bool fill, bool hold = false);
    /**
     * @brief Fills all.
     *
     * @param[in] fill Whether fill is enabled for this operation.
     * @param[in] hold Whether hold is enabled for this operation.
     */
    void fillAll(bool fill, bool hold = false);

    // pattern change

    /**
     * @brief Selects track pattern.
     *
     * @param[in] track Track model operated on by this component.
     * @param[in] pattern Pattern index or pattern data addressed by the operation.
     * @param[in] executeType Execution/commit timing mode used for the request.
     */
    void selectTrackPattern(int track, int pattern, ExecuteType executeType = Immediate);
    /**
     * @brief Selects pattern.
     *
     * @param[in] pattern Pattern index or pattern data addressed by the operation.
     * @param[in] executeType Execution/commit timing mode used for the request.
     */
    void selectPattern(int pattern, ExecuteType executeType = Immediate);

    // snapshots

    /**
     * @brief Captures mutable project/play state for a later revert or commit.
     */
    void createSnapshot();
    /**
     * @brief Restores snapshot.
     *
     * @param[in] targetPattern Pattern selected as the target of the operation.
     */
    void revertSnapshot(int targetPattern = -1);
    /**
     * @brief Commits the current state and discards the snapshot.
     *
     * @param[in] targetPattern Pattern selected as the target of the operation.
     */
    void commitSnapshot(int targetPattern = -1);
    /**
     * @brief Returns the snapshot active.
     *
     * @return `true` if snapshot active; otherwise `false`.
     */
    bool snapshotActive() const { return _snapshot.active; }

    // requests

    /**
     * @brief Reports whether the PlayState can cel mute requests.
     */
    void cancelMuteRequests();
    /**
     * @brief Reports whether the PlayState can cel pattern requests.
     */
    void cancelPatternRequests();
    /**
     * @brief Applies requests deferred until the current commit point.
     */
    void commitLatchedRequests() { _executeLatchedRequests = true; }

    /**
     * @brief Reports whether the PlayState has immediate requests.
     *
     * @return `true` if immediate requests; otherwise `false`.
     */
    bool hasImmediateRequests() const { return _hasImmediateRequests; }
    /**
     * @brief Reports whether the PlayState has synced requests.
     *
     * @return `true` if synced requests; otherwise `false`.
     */
    bool hasSyncedRequests() const { return _hasSyncedRequests; }
    /**
     * @brief Reports whether the PlayState has latched requests.
     *
     * @return `true` if latched requests; otherwise `false`.
     */
    bool hasLatchedRequests() const { return _hasLatchedRequests; }

    // song

    /**
     * @brief Starts playback using the configured song arrangement.
     *
     * @param[in] slot Zero-based project/storage slot index.
     * @param[in] executeType Execution/commit timing mode used for the request.
     */
    void playSong(int slot, ExecuteType executeType = Immediate);
    /**
     * @brief Stops song.
     *
     * @param[in] executeType Execution/commit timing mode used for the request.
     */
    void stopSong(ExecuteType executeType = Immediate);

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

    //----------------------------------------
    // Routing
    //----------------------------------------

    /**
     * @brief Writes routed.
     *
     * @param[in] target Target object/value affected by the operation.
     * @param[in] tracks Track collection read or modified by the operation.
     * @param[in] intValue Integer representation of the value.
     * @param[in] floatValue Floating-point representation of the value.
     */
    void writeRouted(Routing::Target target, uint8_t tracks, int intValue, float floatValue);

private:
    /**
     * @brief Selects track pattern unsafe.
     *
     * @param[in] track Track model operated on by this component.
     * @param[in] pattern Pattern index or pattern data addressed by the operation.
     * @param[in] executeType Execution/commit timing mode used for the request.
     */
    void selectTrackPatternUnsafe(int track, int pattern, ExecuteType executeType = Immediate);

    /**
     * @brief Notifies registered observers about a state change.
     *
     * @param[in] executeType Execution/commit timing mode used for the request.
     */
    void notify(ExecuteType executeType) {
        _hasImmediateRequests |= (executeType == Immediate);
        _hasSyncedRequests |= (executeType == Synced);
        _hasLatchedRequests |= (executeType == Latched);
    }

    /**
     * @brief Returns the execute latched requests.
     *
     * @return `true` if execute latched requests; otherwise `false`.
     */
    bool executeLatchedRequests() const { return _executeLatchedRequests; }

    /**
     * @brief Clears immediate requests.
     */
    void clearImmediateRequests() { _hasImmediateRequests = false; }
    /**
     * @brief Clears synced requests.
     */
    void clearSyncedRequests() { _hasSyncedRequests = false; }
    /**
     * @brief Clears latched requests.
     */
    void clearLatchedRequests() { _hasLatchedRequests = false; _executeLatchedRequests = false; }

    /**
     * @brief Active project model used by this component.
     */
    Project &_project; ///< Active project model used by this component.

    /**
     * @brief Fixed-capacity storage for track states.
     */
    std::array<TrackState, CONFIG_TRACK_COUNT> _trackStates; ///< Per-track runtime playback/request state indexed by track number.
    SongState _songState; ///< Runtime song-playback state, including current slot/repeat progress and pending transitions.

    /**
     * @brief Whether execute latched requests is true in the current state.
     */
    bool _executeLatchedRequests; ///< True while the play-state update is allowed to commit requests latched for the next synchronization point.
    /**
     * @brief Whether has immediate requests is true in the current state.
     */
    bool _hasImmediateRequests; ///< True when at least one immediate play-state request is pending.
    /**
     * @brief Whether has synced requests is true in the current state.
     */
    bool _hasSyncedRequests; ///< True when at least one synchronized play-state request is pending.
    /**
     * @brief Whether has latched requests is true in the current state.
     */
    bool _hasLatchedRequests; ///< True when at least one latched play-state request is pending.

    /**
     * @brief Snapshot pattern index constant used by this component.
     */
    static constexpr int SnapshotPatternIndex = CONFIG_PATTERN_COUNT; ///< Sentinel pattern index reserved for the temporary performance snapshot.

    /**
     * @brief Whether this object is currently active.
     */
    struct {
        bool active;
        uint8_t lastSelectedPatternIndex;
        uint8_t lastTrackPatternIndex[CONFIG_TRACK_COUNT];
    } _snapshot;

    friend class Project;
    friend class Engine;
};
