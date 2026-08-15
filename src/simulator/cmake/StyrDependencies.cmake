# Purpose: Provides CMake support for the desktop simulator.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
# Resolve simulator-only source dependencies.
#
# Local checkouts below third_party/simulator are accepted when present. If a
# dependency is absent, CMake fetches the exact revision used by the historical
# Westlicht Performer simulator into the disposable build/_deps directory.

include(FetchContent)

option(STYR_SIM_FETCH_DEPENDENCIES
    "Fetch missing simulator source dependencies during CMake configure"
    ON
)

set(FETCHCONTENT_BASE_DIR "${STYR_ROOT}/build/_deps" CACHE PATH
    "Shared cache for CMake-fetched Styr simulator dependencies"
)

function(styr_resolve_sim_dependency name repository revision required_file out_var)
    set(local_dir "${STYR_THIRD_PARTY_SIM_DIR}/${name}")
    if(EXISTS "${local_dir}/${required_file}")
        set(${out_var} "${local_dir}" PARENT_SCOPE)
        message(STATUS "Simulator dependency ${name}: local checkout")
        return()
    endif()

    if(NOT STYR_SIM_FETCH_DEPENDENCIES)
        message(FATAL_ERROR
            "Simulator dependency ${name} is missing at ${local_dir}. "
            "Enable STYR_SIM_FETCH_DEPENDENCIES or provide a local checkout."
        )
    endif()

    set(fetch_name "styr_sim_${name}")
    FetchContent_Declare(${fetch_name}
        GIT_REPOSITORY "${repository}"
        GIT_TAG "${revision}"
        GIT_SHALLOW FALSE
        GIT_PROGRESS TRUE
    )
    FetchContent_GetProperties(${fetch_name})
    if(NOT ${fetch_name}_POPULATED)
        message(STATUS "Simulator dependency ${name}: fetching ${revision}")
        FetchContent_Populate(${fetch_name})
    endif()

    FetchContent_GetProperties(${fetch_name})
    if(NOT EXISTS "${${fetch_name}_SOURCE_DIR}/${required_file}")
        message(FATAL_ERROR
            "Fetched simulator dependency ${name} does not contain ${required_file}"
        )
    endif()
    set(${out_var} "${${fetch_name}_SOURCE_DIR}" PARENT_SCOPE)
endfunction()

# Keep the initial simulator migration on Performer's known-good dependency
# revisions. Frontend-only dependencies must not be resolved for headless
# framebuffer/manual builds.
if(STYR_SIM_BUILD_FRONTEND)
    styr_resolve_sim_dependency(
        soloud
        "https://github.com/jarikomppa/soloud.git"
        "1157475881da0d7f76102578255b937c7d4e8f57"
        "include/soloud.h"
        STYR_SOLOUD_DIR
    )
    styr_resolve_sim_dependency(
        nanovg
        "https://github.com/memononen/nanovg.git"
        "f93799c078fa11ed61c078c65a53914c8782c00b"
        "src/nanovg.c"
        STYR_NANOVG_DIR
    )
    styr_resolve_sim_dependency(
        args
        "https://github.com/Taywee/args.git"
        "b7d67237e8bdaa517d7fd6e4e84e1f6efa24f8c5"
        "args.hxx"
        STYR_ARGS_DIR
    )
endif()

# tinyformat is used by the interactive frontend and by TargetTrace when the
# optional Python bindings are enabled. It is not part of the manual path.
if(STYR_SIM_BUILD_FRONTEND OR STYR_SIM_ENABLE_PYTHON)
    styr_resolve_sim_dependency(
        tinyformat
        "https://github.com/c42f/tinyformat.git"
        "aef402d85c1e8f9bf491b72570bfe8938ae26727"
        "tinyformat.h"
        STYR_TINYFORMAT_DIR
    )
endif()

if(STYR_SIM_ENABLE_PYTHON)
    styr_resolve_sim_dependency(
        pybind11
        "https://github.com/pybind/pybind11.git"
        "e250155afadde7100e627e6aa4a541137a863243"
        "CMakeLists.txt"
        STYR_PYBIND11_DIR
    )
endif()
