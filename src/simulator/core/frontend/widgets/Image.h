/**
 * @file Image.h
 * @brief Declares the Image component used by the simulator frontend.
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

namespace sim {

/**
 * @brief Provides image behavior for the desktop simulator.
 */
class Image : public Widget {
public:
    typedef std::shared_ptr<Image> Ptr;

    /**
     * @brief Constructs a Image instance.
     *
     * @param[in] pos Position in the coordinate system used by the caller.
     * @param[in] size Number of bytes or elements covered by the operation.
     * @param[in] filename File name/path used for the operation.
     */
    Image(const Vector2f &pos, const Vector2f &size, const std::string &filename) :
        /**
         * @brief Returns the widget.
         */
        Widget(pos, size),
        /**
         * @brief Returns the filename.
         */
        _filename(filename)
    {}

    /**
     * @brief Updates the Image for the current service cycle.
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

        if (_image == -1) {
            _image = nvgCreateImage(nvg, _filename.c_str(), NVG_IMAGE_GENERATE_MIPMAPS);
            _pattern = nvgImagePattern(nvg, _pos.x(), _pos.y(), _size.x(), _size.y(), 0.f, _image, 1.f);;
        }

		nvgBeginPath(nvg);
        nvgRect(nvg, _pos.x(), _pos.y(), _size.x(), _size.y());
        nvgFillPaint(nvg, _pattern);
        nvgFill(nvg);
    }

private:
    std::string _filename; ///< Path of the image asset loaded by this widget.
    /**
     * @brief Simulator value representing image.
     */
    int _image = -1; ///< Renderer image handle; `-1` means that no image resource is loaded.
    NVGpaint _pattern; ///< Generated pattern data retained between generator parameter edits and commit.
};

} // namespace sim
