/**
 * @file Vector.h
 * @brief Declares the Vector component used by the simulator frontend.
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

#include <array>

namespace sim {

/**
 * @brief Provides vector behavior for the desktop simulator.
 */
template<typename T, unsigned Dim>
/**
 * @brief Provides vector behavior for the desktop simulator.
 */
class Vector {
public:
    /**
     * @brief Constructs a Vector instance.
     */
    Vector() = default;

    template<typename... Ts>
    /**
     * @brief Constructs a Vector instance.
     *
     * @param[in] a Left/first value used by the comparison or binary operation.
     * @param[in] b Right/second value used by the comparison or binary operation.
     * @param[in] ts Source vector/scalar whose elements are used to construct this vector.
     */
    explicit Vector(const T &a, const T &b, const Ts &... ts) : _v({{a, b, ts...}}) {}

    template<typename OtherT>
    /**
     * @brief Constructs a Vector instance.
     *
     * @param[in] other Other object/value used for comparison or combination.
     */
    explicit Vector(const Vector<OtherT, Dim> &other) {
        for (unsigned i = 0; i < Dim; ++i) {
            _v[i] = T(other[i]);
        }
    }

    /**
     * @brief Constructs a Vector instance.
     *
     * @param[in] c Character, color, or scalar value as defined by the called interface.
     */
    explicit Vector(T c) { _v.fill(c); }

    /**
     * @brief Implements operator[] for this type.
     *
     * @param[in] i Zero-based element index for the addressed collection.
     *
     * @return Reference to the operator[].
     */
    const T &operator[](unsigned i) const { return _v[i]; }
    /**
     * @brief Implements operator[] for this type.
     *
     * @param[in] i Zero-based element index for the addressed collection.
     *
     * @return Reference to the operator[].
     */
    T &operator[](unsigned i) { return _v[i]; }

    /**
     * @brief Returns the x.
     *
     * @return Reference to the x.
     */
    const T &x() const { return _v[0]; }
    /**
     * @brief Returns the y.
     *
     * @return Reference to the y.
     */
    const T &y() const { static_assert(Dim >= 2, "vector has no y coordinate"); return _v[1]; }
    /**
     * @brief Returns the z.
     *
     * @return Reference to the z.
     */
    const T &z() const { static_assert(Dim >= 3, "vector has no z coordinate"); return _v[2]; }
    /**
     * @brief Returns the w.
     *
     * @return Reference to the w.
     */
    const T &w() const { static_assert(Dim >= 4, "vector has no w coordinate"); return _v[3]; }

    /**
     * @brief Returns the x.
     *
     * @return Reference to the x.
     */
    T &x() { return _v[0]; }
    /**
     * @brief Returns the y.
     *
     * @return Reference to the y.
     */
    T &y() { static_assert(Dim >= 2, "vector has no y coordinate"); return _v[1]; }
    /**
     * @brief Returns the z.
     *
     * @return Reference to the z.
     */
    T &z() { static_assert(Dim >= 3, "vector has no z coordinate"); return _v[2]; }
    /**
     * @brief Returns the w.
     *
     * @return Reference to the w.
     */
    T &w() { static_assert(Dim >= 4, "vector has no w coordinate"); return _v[3]; }

    /**
     * @brief Implements operator- for this type.
     *
     * @return Vector with each component negated.
     */
    Vector operator-() const {
        Vector result;
        for (unsigned i = 0; i < Dim; ++i) {
            result._v[i] = -_v[i];
        }
        return result;
    }

    /**
     * @brief Implements operator+ for this type.
     *
     * @param[in] other Other object/value used for comparison or combination.
     *
     * @return Sum of the two operands.
     */
    Vector operator+(const Vector &other) const {
        Vector result;
        for (unsigned i = 0; i < Dim; ++i) {
            result._v[i] = _v[i] + other._v[i];
        }
        return result;
    }

    /**
     * @brief Implements operator- for this type.
     *
     * @param[in] other Other object/value used for comparison or combination.
     *
     * @return Difference between the two operands.
     */
    Vector operator-(const Vector &other) const {
        Vector result;
        for (unsigned i = 0; i < Dim; ++i) {
            result._v[i] = _v[i] - other._v[i];
        }
        return result;
    }

    /**
     * @brief Implements operator* for this type.
     *
     * @param[in] other Other object/value used for comparison or combination.
     *
     * @return Product/scaled vector produced by the operation.
     */
    Vector operator*(const Vector &other) const {
        Vector result;
        for (unsigned i = 0; i < Dim; ++i) {
            result._v[i] = _v[i]*other._v[i];
        }
        return result;
    }

    /**
     * @brief Implements operator/ for this type.
     *
     * @param[in] other Other object/value used for comparison or combination.
     *
     * @return Quotient/scaled vector produced by the operation.
     */
    Vector operator/(const Vector &other) const {
        Vector result;
        for (unsigned i = 0; i < Dim; ++i) {
            result._v[i] = _v[i]/other._v[i];
        }
        return result;
    }

    /**
     * @brief Implements operator+ for this type.
     *
     * @param[in] a Left/first value used by the comparison or binary operation.
     *
     * @return Sum of the two operands.
     */
    Vector operator+(const T &a) const {
        Vector result;
        for (unsigned i = 0; i < Dim; ++i) {
            result._v[i] = _v[i] + a;
        }
        return result;
    }

    /**
     * @brief Implements operator- for this type.
     *
     * @param[in] a Left/first value used by the comparison or binary operation.
     *
     * @return Difference between the two operands.
     */
    Vector operator-(const T &a) const {
        Vector result;
        for (unsigned i = 0; i < Dim; ++i) {
            result._v[i] = _v[i] - a;
        }
        return result;
    }

    /**
     * @brief Implements operator* for this type.
     *
     * @param[in] a Left/first value used by the comparison or binary operation.
     *
     * @return Product/scaled vector produced by the operation.
     */
    Vector operator*(const T &a) const {
        Vector result;
        for (unsigned i = 0; i < Dim; ++i) {
            result._v[i] = _v[i]*a;
        }
        return result;
    }

    /**
     * @brief Implements operator/ for this type.
     *
     * @param[in] a Left/first value used by the comparison or binary operation.
     *
     * @return Quotient/scaled vector produced by the operation.
     */
    Vector operator/(const T &a) const {
        Vector result;
        for (unsigned i = 0; i < Dim; ++i) {
            result._v[i] = _v[i]/a;
        }
        return result;
    }

    /**
     * @brief Returns the prod.
     *
     * @return Component-wise/product result produced by this vector operation.
     */
    T prod() const {
        T result = T(1);
        for (unsigned i = 0; i < Dim; ++i) {
            result *= _v[i];
        }
        return result;
    }

private:
    /**
     * @brief Fixed-capacity storage for v.
     */
    std::array<T, Dim> _v; ///< Contiguous storage for the `Dim` vector components.
};

template<typename T, unsigned Dim>
Vector<T, Dim> operator+(const T &a, const Vector<T, Dim> &b) {
    Vector<T, Dim> result;
    for (unsigned i = 0; i < Dim; ++i) {
        result[i] = a + b[i];
    }
    return result;
}

template<typename T, unsigned Dim>
Vector<T, Dim> operator-(const T &a, const Vector<T, Dim> &b) {
    Vector<T, Dim> result;
    for (unsigned i = 0; i < Dim; ++i) {
        result[i] = a - b[i];
    }
    return result;
}

template<typename T, unsigned Dim>
Vector<T, Dim> operator*(const T &a, const Vector<T, Dim> &b) {
    Vector<T, Dim> result;
    for (unsigned i = 0; i < Dim; ++i) {
        result[i] = a*b[i];
    }
    return result;
}

template<typename T, unsigned Dim>
Vector<T, Dim> operator/(const T &a, const Vector<T, Dim> &b) {
    Vector<T, Dim> result;
    for (unsigned i = 0; i < Dim; ++i) {
        result[i] = a/b[i];
    }
    return result;
}

using Vector2i = Vector<int, 2>;
using Vector3i = Vector<int, 3>;
using Vector4i = Vector<int, 4>;

using Vector2f = Vector<float, 2>;
using Vector3f = Vector<float, 3>;
using Vector4f = Vector<float, 4>;

} // namespace sim
