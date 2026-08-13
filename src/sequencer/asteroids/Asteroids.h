/**
 * @file Asteroids.h
 * @brief Declares the Asteroids component used by the sequencer Asteroids easter egg.
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

#include "core/math/Vec2.h"
#include "core/gfx/Canvas.h"

#include <array>

namespace asteroids {

class Game;

/**
 * @brief Stores the input state consumed by the enclosing operation or simulation step.
 */
struct Inputs {
    /**
     * @brief Packs the current game input buttons into a single byte.
     */
    union {
        struct {
            uint8_t left : 1;
            uint8_t right : 1;
            uint8_t thrust : 1;
            uint8_t shoot : 1;
        };
        uint8_t keys = 0;
    };
};

/**
 * @brief Stores output values produced by the enclosing operation or simulation step.
 */
struct Outputs {
    /**
     * @brief Packs the current game gate outputs into a single byte.
     */
    union {
        struct {
            uint8_t thurst : 1;
            uint8_t shoot : 1;
            uint8_t explosion: 1;
        };
        uint8_t gates = 0;
    };
};

//-----------------------------------------------------------------------------
// Pool
//-----------------------------------------------------------------------------

/**
 * @brief Implements pool operations and maintains the associated runtime state.
 */
template<typename T, size_t N>
/**
 * @brief Provides fixed-capacity object storage with explicit allocation and release.
 */
class Pool {
public:
    /**
     * @brief Returns the capacity.
     *
     * @return Maximum number of elements the container can hold.
     */
    size_t capacity() const {
        return N;
    }

    /**
     * @brief Returns the size.
     *
     * @return Number of size represented by this object.
     */
    size_t size() const {
        size_t size = 0;
        for (const auto &item : _items) {
            if (item.used) {
                ++size;
            }
        }
        return size;
    }

    /**
     * @brief Resets the Pool to its initial runtime state.
     */
    void reset() {
        for (auto &item : _items) {
            item.used = false;
        }
    }

    /**
     * @brief Returns the allocate.
     *
     * @return Pointer to the allocate; `nullptr` when no value is available.
     */
    T *allocate() {
        for (auto &item : _items) {
            if (!item.used) {
                item.used = true;
                return &item.object;
            }
        }
        return nullptr;
    }

    template<typename Func>
    /**
     * @brief Invokes the supplied callback for every stored item.
     *
     * @param[in] func Function/callback invoked by the operation.
     */
    void forEach(Func func) {
        for (auto &item : _items) {
            if (item.used) {
                func(item.object);
            }
        }
    }

    template<typename Func>
    /**
     * @brief Removes each item selected by the supplied callback/predicate.
     *
     * @param[in] func Function/callback invoked by the operation.
     */
    void forEachRemove(Func func) {
        for (auto &item : _items) {
            if (item.used) {
                if (func(item.object)) {
                    item.used = false;
                }
            }
        }
    }

private:
    /**
     * @brief Stores the fields required to represent item.
     */
    struct Item {
        /**
         * @brief Whether used is true in the current state.
         */
        bool used = false; ///< True when this pool/file slot currently contains a live entry.
        T object; ///< In-place object storage for an allocated Asteroids pool slot.
    };

    /**
     * @brief Fixed-capacity Asteroids object-pool slots.
     */
    std::array<Item, N> _items; ///< Fixed-capacity pool slots storing live and reusable game objects.
};

//-----------------------------------------------------------------------------
// Body
//-----------------------------------------------------------------------------

/**
 * @brief Implements body operations and maintains the associated runtime state.
 */
class Body {
public:
    /**
     * @brief Initializes body.
     *
     * @param[in] mass Physical mass used by the Asteroids simulation.
     * @param[in] radius Collision/body radius in game-world units.
     * @param[in] position Position in the coordinate system used by the caller.
     * @param[in] velocity Linear velocity vector in game-world units per second.
     * @param[in] angle Angle in the unit used by the geometry/game subsystem.
     * @param[in] angleVelocity Angular velocity used by the game/geometry subsystem.
     */
    void initBody(float mass, float radius, const Vec2 &position, const Vec2 &velocity, float angle, float angleVelocity);

    /**
     * @brief Returns the mass.
     *
     * @return Body mass in game-simulation units.
     */
    float mass() const { return _mass; }

    /**
     * @brief Returns the radius.
     *
     * @return Collision/body radius in game-world units.
     */
    float radius() const { return _radius; }
    /**
     * @brief Returns the radius sqr.
     *
     * @return Squared collision radius in game-world units.
     */
    float radiusSqr() const { return _radiusSqr; }

    /**
     * @brief Returns the position.
     *
     * @return Reference to the position.
     */
    const Vec2 &position() const { return _position; }
    /**
     * @brief Sets the position.
     *
     * @param[in] position Position in the coordinate system used by the caller.
     */
    void setPosition(const Vec2 &position) { _position = position; }

    /**
     * @brief Returns the velocity.
     *
     * @return Reference to the velocity.
     */
    const Vec2 &velocity() const { return _velocity; }
    /**
     * @brief Sets the velocity.
     *
     * @param[in] velocity Linear velocity vector in game-world units per second.
     */
    void setVelocity(const Vec2 &velocity) { _velocity = velocity; }

    /**
     * @brief Returns the angle.
     *
     * @return Current body orientation angle in radians.
     */
    float angle() const { return _angle; }
    /**
     * @brief Sets the angle.
     *
     * @param[in] angle Angle in the unit used by the geometry/game subsystem.
     */
    void setAngle(float angle) { _angle = angle; }

    /**
     * @brief Returns the angle velocity.
     *
     * @return Current angular velocity in radians per second.
     */
    float angleVelocity() const { return _angleVelocity; }
    /**
     * @brief Sets the angle velocity.
     *
     * @param[in] angleVelocity Angular velocity used by the game/geometry subsystem.
     */
    void setAngleVelocity(float angleVelocity) { _angleVelocity = angleVelocity; };

    /**
     * @brief Handles collision.
     *
     * @param[in] body1 First collision body participating in the contact operation.
     * @param[in] body2 Second collision body participating in the contact operation.
     *
     * @return `true` if handle collision; otherwise `false`.
     */
    static bool handleCollision(Body &body1, Body &body2);

protected:
    /**
     * @brief Runtime value representing mass.
     */
    float _mass; ///< Mass value used by the game physics model.
    /**
     * @brief Collision/body radius in game-world units.
     */
    float _radius; ///< Collision/body radius in game-world units.
    /**
     * @brief Runtime value representing radius sqr.
     */
    float _radiusSqr; ///< Squared collision/render radius cached for the game object.
    Vec2 _position; ///< Object position in the Asteroids world coordinate system.
    Vec2 _velocity; ///< Object velocity in the Asteroids world coordinate system.
    /**
     * @brief Runtime value representing angle.
     */
    float _angle; ///< Current angular position.
    /**
     * @brief Runtime value representing angle velocity.
     */
    float _angleVelocity; ///< Angular velocity used by the game/geometry subsystem.
};

//-----------------------------------------------------------------------------
// Player
//-----------------------------------------------------------------------------

/**
 * @brief Implements player operations and maintains the associated runtime state.
 */
class Player : public Body {
public:
    /**
     * @brief Constructs a Player instance.
     *
     * @param[in] game Asteroids game state owning or receiving the object/event.
     */
    Player(Game &game);

    /**
     * @brief Resets the Player to its initial runtime state.
     */
    void reset();

    /**
     * @brief Marks the runtime/game object as destroyed or inactive.
     */
    void kill() { _killed = true; }
    /**
     * @brief Returns the killed.
     *
     * @return `true` if killed; otherwise `false`.
     */
    bool killed() const { return _killed; }

    /**
     * @brief Returns the score.
     *
     * @return Current game score.
     */
    int score() const { return _score; }
    /**
     * @brief Sets the score.
     *
     * @param[in] score Score value used by the generator/game heuristic.
     */
    void setScore(int score) { _score = score; }
    /**
     * @brief Adds score.
     *
     * @param[in] score Score value used by the generator/game heuristic.
     */
    void addScore(int score) { _score += score; }

    /**
     * @brief Returns the shooting.
     *
     * @return `true` if shooting; otherwise `false`.
     */
    bool shooting() const { return _shootTime > ShootInterval - ShootGate; }

    /**
     * @brief Updates the Player for the current service cycle.
     *
     * @param[in] dt Elapsed time in seconds since the previous update.
     * @param[in] inputs Current game input state.
     * @param[in] handleInputs Whether handle inputs is enabled for this operation.
     */
    void update(float dt, Inputs &inputs, bool handleInputs);
    /**
     * @brief Draws this component into the supplied canvas or rendering target.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    void draw(Canvas &canvas);

private:
    /**
     * @brief Runtime value representing angle velocity.
     */
    const float AngleVelocity = 5.f; ///< Angular velocity used by the game/geometry subsystem.
    /**
     * @brief Runtime value representing acceleration.
     */
    const float Acceleration = 100.f; ///< Acceleration vector/magnitude used by the game object.
    /**
     * @brief Runtime value representing drag.
     */
    const float Drag = 0.5f; ///< Drag/deceleration coefficient used by the game physics.
    /**
     * @brief Runtime value representing shoot interval.
     */
    const float ShootInterval = 0.25f; ///< Minimum interval between player/projectile shots.
    /**
     * @brief Runtime value representing shoot gate.
     */
    const float ShootGate = 0.05f; ///< Whether shooting is currently gated/enabled.
    /**
     * @brief Runtime value representing shoot velocity.
     */
    const float ShootVelocity = 100.f; ///< Projectile launch velocity.
    /**
     * @brief Runtime value representing thrust particle interval.
     */
    const float ThrustParticleInterval = 0.05f; ///< Interval between emitted thrust particles.

    /**
     * @brief Reference to game owned by another component.
     */
    Game &_game; ///< Reference to game owned by another component.
    /**
     * @brief Whether killed is true in the current state.
     */
    bool _killed; ///< True after the Asteroids entity has been marked for removal from the active simulation.
    /**
     * @brief Runtime value representing score.
     */
    int _score; ///< Current game/generator score value.
    Vec2 _direction; ///< Normalized or intended travel direction in the Asteroids world coordinate system.
    /**
     * @brief Runtime value representing shoot time.
     */
    float _shootTime; ///< Time remaining/elapsed for the current shot cadence.
    /**
     * @brief Runtime value representing thrust particle time.
     */
    float _thrustParticleTime; ///< Time accumulator used to schedule thrust particles.
};

//-----------------------------------------------------------------------------
// Asteroid
//-----------------------------------------------------------------------------

/**
 * @brief Implements asteroid operations and maintains the associated runtime state.
 */
class Asteroid : public Body {
public:
    /**
     * @brief Returns the category.
     *
     * @return Collision/category identifier for this game object.
     */
    uint8_t category() const { return _category; }

    /**
     * @brief Creates a new runtime/game object.
     *
     * @param[in] category Category/type selector used to choose behavior or rendering.
     * @param[in] shape Curve/geometric shape selected by the operation.
     * @param[in] position Position in the coordinate system used by the caller.
     * @param[in] velocity Linear velocity vector in game-world units per second.
     * @param[in] angle Angle in the unit used by the geometry/game subsystem.
     * @param[in] angleVelocity Angular velocity used by the game/geometry subsystem.
     */
    void spawn(uint8_t category, uint8_t shape, const Vec2 &position, const Vec2 &velocity, float angle, float angleVelocity);

    /**
     * @brief Updates the Asteroid for the current service cycle.
     *
     * @param[in] dt Elapsed time in seconds since the previous update.
     */
    void update(float dt);
    /**
     * @brief Draws this component into the supplied canvas or rendering target.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    void draw(Canvas &canvas);

private:
    /**
     * @brief Runtime value representing category.
     */
    uint8_t _category; ///< Runtime category/type of the game/entity object.
    /**
     * @brief Runtime value representing shape.
     */
    uint8_t _shape; ///< Shape identifier/data used for rendering/collision.
    /**
     * @brief Runtime value representing scale.
     */
    float _scale; ///< Scale selection/definition stored by this state.
};

//-----------------------------------------------------------------------------
// Projectile
//-----------------------------------------------------------------------------

/**
 * @brief Implements projectile operations and maintains the associated runtime state.
 */
class Projectile {
public:
    /**
     * @brief Returns the position.
     *
     * @return Reference to the position.
     */
    const Vec2 &position() const { return _position; }

    /**
     * @brief Marks the runtime/game object as destroyed or inactive.
     */
    void kill() { _killed = true; }
    /**
     * @brief Returns the killed.
     *
     * @return `true` if killed; otherwise `false`.
     */
    bool killed() const { return _killed; }

    /**
     * @brief Creates a new runtime/game object.
     *
     * @param[in] position Position in the coordinate system used by the caller.
     * @param[in] direction Direction used for traversal, rotation or sequence advancement.
     * @param[in] velocity Linear velocity vector in game-world units per second.
     */
    void spawn(const Vec2 &position, const Vec2 &direction, const Vec2 &velocity);

    /**
     * @brief Updates the Projectile for the current service cycle.
     *
     * @param[in] dt Elapsed time in seconds since the previous update.
     */
    void update(float dt);
    /**
     * @brief Draws this component into the supplied canvas or rendering target.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    void draw(Canvas &canvas);

private:
    Vec2 _position; ///< Object position in the Asteroids world coordinate system.
    Vec2 _direction; ///< Normalized or intended travel direction in the Asteroids world coordinate system.
    Vec2 _velocity; ///< Object velocity in the Asteroids world coordinate system.
    /**
     * @brief Whether killed is true in the current state.
     */
    bool _killed; ///< True after the Asteroids entity has been marked for removal from the active simulation.
};

//-----------------------------------------------------------------------------
// Particle
//-----------------------------------------------------------------------------

/**
 * @brief Implements particle operations and maintains the associated runtime state.
 */
class Particle {
public:
    /**
     * @brief Stores the configurable parameters consumed by the enclosing algorithm.
     */
    struct Params {
        /**
         * @brief Runtime value representing life time.
         */
        float lifeTime; ///< Remaining or elapsed lifetime of the runtime object.
        /**
         * @brief Size of the associated data in bytes/elements as defined by this type.
         */
        float size; ///< Size of the associated data in bytes/elements as defined by this type.
        /**
         * @brief Runtime value representing drag.
         */
        float drag; ///< Drag/deceleration coefficient used by the game physics.
    };

    /**
     * @brief Returns the killed.
     *
     * @return `true` if killed; otherwise `false`.
     */
    bool killed() const { return _killed; }

    /**
     * @brief Creates a new runtime/game object.
     *
     * @param[in] position Position in the coordinate system used by the caller.
     * @param[in] direction Direction used for traversal, rotation or sequence advancement.
     * @param[in] velocity Linear velocity vector in game-world units per second.
     * @param[in] params Parameter set controlling the generated pattern or game object.
     */
    void spawn(const Vec2 &position, const Vec2 &direction, const Vec2 &velocity, const Params &params);

    /**
     * @brief Updates the Particle for the current service cycle.
     *
     * @param[in] dt Elapsed time in seconds since the previous update.
     */
    void update(float dt);
    /**
     * @brief Draws this component into the supplied canvas or rendering target.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    void draw(Canvas &canvas);

private:
    /**
     * @brief Params, in milliseconds.
     */
    const Params *_params; ///< Parameter set controlling the current game object; not owned by this instance.
    /**
     * @brief Runtime value representing time.
     */
    float _time; ///< Timestamp or elapsed time in the unit defined by the owning component.
    Vec2 _position; ///< Object position in the Asteroids world coordinate system.
    Vec2 _direction; ///< Normalized or intended travel direction in the Asteroids world coordinate system.
    Vec2 _velocity; ///< Object velocity in the Asteroids world coordinate system.
    /**
     * @brief Whether killed is true in the current state.
     */
    bool _killed; ///< True after the Asteroids entity has been marked for removal from the active simulation.
};

//-----------------------------------------------------------------------------
// Game
//-----------------------------------------------------------------------------

/**
 * @brief Implements game operations and maintains the associated runtime state.
 */
class Game {
public:
    /**
     * @brief Enumerates the supported state values.
     */
    enum State {
        Intro, ///< Selects the intro state.
        Start, ///< Selects the start state.
        Play, ///< Selects the play state.
        Win, ///< Selects the win state.
        Lose, ///< Selects the lose state.
    };

    /**
     * @brief Constructs a Game instance.
     */
    Game();

    /**
     * @brief Initializes the Game and its runtime resources.
     */
    void init();

    /**
     * @brief Resets the Game to its initial runtime state.
     */
    void reset();

    /**
     * @brief Initializes a new game level.
     *
     * @param[in] level Level or range selection to apply.
     */
    void levelStart(int level);
    /**
     * @brief Returns the level finished.
     *
     * @return `true` if level finished; otherwise `false`.
     */
    bool levelFinished();

    /**
     * @brief Applies a new runtime state and performs the transition-specific bookkeeping.
     *
     * @param[in] state New state or logical level to apply.
     */
    void setState(State state);

    /**
     * @brief Updates the Game for the current service cycle.
     *
     * @param[in] dt Elapsed time in seconds since the previous update.
     * @param[in] inputs Current game input state.
     * @param[out] outputs Output-state structure populated by the game/engine update.
     */
    void update(float dt, Inputs &inputs, Outputs &outputs);
    /**
     * @brief Draws this component into the supplied canvas or rendering target.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    void draw(Canvas &canvas);

    /**
     * @brief Creates an asteroid with the requested initial state.
     *
     * @param[in] category Category/type selector used to choose behavior or rendering.
     * @param[in] position Position in the coordinate system used by the caller.
     * @param[in] velocity Linear velocity vector in game-world units per second.
     * @param[in] angle Angle in the unit used by the geometry/game subsystem.
     * @param[in] angleVelocity Angular velocity used by the game/geometry subsystem.
     */
    void spawnAsteroid(uint8_t category, const Vec2 &position, const Vec2 &velocity, float angle, float angleVelocity);
    /**
     * @brief Creates an asteroid with randomized initial state.
     *
     * @param[in] category Category/type selector used to choose behavior or rendering.
     */
    void spawnRandomAsteroid(uint8_t category);

    /**
     * @brief Creates a projectile from the current player state.
     *
     * @param[in] position Position in the coordinate system used by the caller.
     * @param[in] direction Direction used for traversal, rotation or sequence advancement.
     * @param[in] velocity Linear velocity vector in game-world units per second.
     */
    void spawnProjectile(const Vec2 &position, const Vec2 &direction, const Vec2 &velocity);

    /**
     * @brief Creates a visual particle effect element.
     *
     * @param[in] position Position in the coordinate system used by the caller.
     * @param[in] direction Direction used for traversal, rotation or sequence advancement.
     * @param[in] velocity Linear velocity vector in game-world units per second.
     * @param[in] params Parameter set controlling the generated pattern or game object.
     */
    void spawnParticle(const Vec2 &position, const Vec2 &direction, const Vec2 &velocity, const Particle::Params &params);
    /**
     * @brief Creates particle/effect state for an explosion.
     *
     * @param[in] position Position in the coordinate system used by the caller.
     * @param[in] velocity Linear velocity vector in game-world units per second.
     */
    void spawnExplosion(const Vec2 &position, const Vec2 &velocity);

    /**
     * @brief Splits an asteroid into smaller asteroid objects.
     *
     * @param[in] asteroid Asteroid instance evaluated or modified by the operation.
     */
    void divideAsteroid(Asteroid &asteroid);

private:
    /**
     * @brief Draws texts.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] title Title text displayed by the UI.
     * @param[in] msg Raw MIDI message byte to process.
     */
    void drawTexts(Canvas &canvas, const char *title, const char *msg);
    /**
     * @brief Draws hud.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    void drawHUD(Canvas &canvas);
    /**
     * @brief Draws shadow text.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] x Horizontal coordinate or scalar x component.
     * @param[in] y Vertical coordinate or scalar y component.
     * @param[in] color Color value/index to draw, store, or emit.
     * @param[in] str Str consumed by `drawShadowText()`.
     */
    void drawShadowText(Canvas &canvas, int x, int y, uint8_t color, const char *str);

    /**
     * @brief Initializes asteroid shapes.
     */
    static void initAsteroidShapes();

    State _state; ///< Current runtime state.
    /**
     * @brief Runtime value representing level.
     */
    int _level; ///< Current game/processing level.
    /**
     * @brief Most recently observed keys.
     */
    uint8_t _lastKeys = 0; ///< Previous input-key bit mask used to detect game key transitions.

    /**
     * @brief Runtime value representing time.
     */
    float _time; ///< Timestamp or elapsed time in the unit defined by the owning component.

    Player _player; ///< Player ship state for the Asteroids mini-game.
    Pool<Asteroid, 16> _asteroids; ///< Fixed-capacity object pool containing active asteroids entities.
    Pool<Projectile, 16> _projectiles; ///< Fixed-capacity object pool containing active projectiles entities.
    Pool<Particle, 32> _particles; ///< Fixed-capacity object pool containing active particles entities.

    /**
     * @brief Runtime value representing explosion time.
     */
    float _explosionTime = 0.f; ///< Time accumulator/duration for explosion effects.
};

} // namespace asteroids
