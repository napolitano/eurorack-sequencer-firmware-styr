/**
 * @file Display.h
 * @brief Declares the Display component used by the simulator frontend.
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

#include "../Widget.h"

#include "nanovg.h"

#include <cstdint>
#include <algorithm>
#include <cmath>

namespace sim {

/**
 * @brief Provides display behavior for the desktop simulator.
 */
class Display : public Widget {
public:
    typedef std::shared_ptr<Display> Ptr;

    /**
     * @brief Constructs a Display instance.
     *
     * @param[in] pos Position in the coordinate system used by the caller.
     * @param[in] size Number of bytes or elements covered by the operation.
     * @param[in] resolution Pixel dimensions of the simulated display.
     *
     * @note Includes an unnamed `const Color &color = Color(0.8f, 0.9f, 0.f, 1.f)` input parameter as declared by the inherited/interface signature.
     */
    Display(const Vector2f &pos, const Vector2f &size, const Vector2i &resolution, const Color &color = Color(0.8f, 0.9f, 0.f, 1.f)) :
        /**
         * @brief Returns the widget.
         */
        Widget(pos, size),
        /**
         * @brief Returns the resolution.
         */
        _resolution(resolution),
        /**
         * @brief Returns the color.
         */
        _color(color),
        /**
         * @brief Performs the frame buffer operation for this Display.
         *
         * @param[in] uint32_t Uint32 t supplied to the operation.
         */
        _frameBuffer(new uint32_t[resolution.prod()])
    {
    }

    /**
     * @brief Returns the resolution.
     *
     * @return Reference to the resolution.
     */
    const Vector2i &resolution() { return _resolution; }

    /**
     * @brief Returns the color.
     *
     * @return Reference to the color.
     */
    const Color &color() const { return _color; }
          /**
           * @brief Returns the color.
           *
           * @return Reference to the color.
           */
          Color &color()       { return _color; }

    /**
     * @brief Draws this component into the supplied canvas or rendering target.
     *
     * @param[in] frameBuffer Framebuffer that receives drawing operations.
     */
    void draw(const uint8_t *frameBuffer) {
        const uint8_t *src = frameBuffer;
        uint32_t *dst = _frameBuffer.get();
        for (int i = 0; i < _resolution.prod(); ++i) {
            float s = std::min(uint8_t(15), *src++) * (1.f / 15.f);
            *dst++ = Color(s * _color.r(), s * _color.g(), s * _color.b(), 1.f).rgba();
        }
        _frameBufferDirty = true;
    }

    /**
     * @brief Updates the Display for the current service cycle.
     */
    virtual void update() override {
    }

    /**
     * @brief Renders this component into the supplied target.
     *
     * @param[in] renderer Renderer used to create or update frontend resources.
     */
    virtual void render(Renderer &renderer) override {
        auto nvg = renderer.nvg();
        const uint8_t *frameBuffer = reinterpret_cast<uint8_t *>(_frameBuffer.get());

        // Integer-only scaling for pixel-perfect LCD output.
        // This avoids fractional scaling blur and keeps each simulated LCD pixel crisp.
        const float maxScaleX = _size.x() / std::max(1, _resolution.x());
        const float maxScaleY = _size.y() / std::max(1, _resolution.y());
        const int integerScale = std::max(1, static_cast<int>(std::floor(std::min(maxScaleX, maxScaleY))));

        const Vector2f drawSize(_resolution.x() * integerScale, _resolution.y() * integerScale);
        Vector2f drawPos = _pos + (_size - drawSize) * 0.5f;

        // Snap to integer pixel coordinates so the nearest-neighbor sample grid stays stable.
        drawPos = Vector2f(std::round(drawPos.x()), std::round(drawPos.y()));

        // update texture
        if (_image == -1) {
            _image = nvgCreateImageRGBA(nvg, _resolution.x(), _resolution.y(), NVG_IMAGE_NEAREST, frameBuffer);
        } else {
            if (_frameBufferDirty) {
                nvgUpdateImage(nvg, _image, frameBuffer);
                _frameBufferDirty = false;
            }
        }

        _pattern = nvgImagePattern(nvg, drawPos.x(), drawPos.y(), drawSize.x(), drawSize.y(), 0.f, _image, 1.f);

        // Fill the LCD cavity first so centered letterbox margins blend with the panel.
        nvgBeginPath(nvg);
        nvgRect(nvg, _pos.x(), _pos.y(), _size.x(), _size.y());
        nvgFillColor(nvg, nvgRGBA(0, 0, 0, 255));
        nvgFill(nvg);

		nvgBeginPath(nvg);
        nvgRect(nvg, drawPos.x(), drawPos.y(), drawSize.x(), drawSize.y());
        nvgFillPaint(nvg, _pattern);
        nvgFill(nvg);

        renderer.setColor(Color(0.5f, 1.f));
        renderer.drawRect(_pos, _size);
    }

private:
    Vector2i _resolution; ///< Display resolution in pixels.
    /**
     * @brief Current drawing color/index.
     */
    Color _color; ///< Current drawing color/index.
    std::unique_ptr<uint32_t[]> _frameBuffer; ///< Owned RGBA frame buffer backing the simulated display.
    /**
     * @brief Whether frame buffer dirty is true in the current state.
     */
    bool _frameBufferDirty = true; ///< True when display pixels changed and the cached renderer image must be refreshed.
    /**
     * @brief Simulator value representing image.
     */
    int _image = -1; ///< Renderer image handle; `-1` means that no image resource is loaded.
    NVGpaint _pattern; ///< Generated pattern data retained between generator parameter edits and commit.
};

} // namespace sim
