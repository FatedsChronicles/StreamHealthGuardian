cmake_minimum_required(VERSION 3.28...3.30)

# Pulls _name and _version from your template's bootstrap pipeline
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/bootstrap.cmake" NO_POLICY_SCOPE)

project(${_name} VERSION ${_version} LANGUAGES CXX)

# ---- Options ---------------------------------------------------------------
option(ENABLE_FRONTEND_API "Use obs-frontend-api for UI functionality" ON)
option(ENABLE_QT           "Use Qt functionality"                        ON)
option(PLUGIN_WERROR       "Treat warnings as errors"                    ON)
option(PLUGIN_BUILD_OVERLAY "Build overlay source"                       ON)

# ---- Template helpers (provided by your repo) ------------------------------
include(compilerconfig)
include(defaults)
include(helpers)

# ---- Target ----------------------------------------------------------------
add_library(${CMAKE_PROJECT_NAME} MODULE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- Dependencies ----------------------------------------------------------
find_package(libobs REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::libobs)

if(ENABLE_FRONTEND_API)
  find_package(obs-frontend-api REQUIRED)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::obs-frontend-api)
endif()

if(ENABLE_QT)
  find_package(Qt6 COMPONENTS Widgets Core REQUIRED)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets)

  # Silence some Clang warnings on macOS for Qt-generated headers
  target_compile_options(
    ${CMAKE_PROJECT_NAME}
    PRIVATE $<$<CXX_COMPILER_ID:Clang,AppleClang>:-Wno-quoted-include-in-framework-header -Wno-comma>
  )

  # Enable Qt's code generators
  set_target_properties(
    ${CMAKE_PROJECT_NAME}
    PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON
  )
endif()

# ---- Sources ---------------------------------------------------------------
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
  src/plugin.cpp
  src/logging.hpp
  src/version.hpp
  src/metrics_collector.hpp src/metrics_collector.cpp
  src/rules_engine.hpp      src/rules_engine.cpp
  src/actions.hpp           src/actions.cpp
  src/health_dock.hpp       src/health_dock.cpp
  src/properties.hpp        src/properties.cpp
  src/hotkeys.hpp           src/hotkeys.cpp
  src/events.hpp            src/events.cpp
  src/perf.hpp
)

if(PLUGIN_BUILD_OVERLAY)
  target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    src/overlay_source.hpp
    src/overlay_source.cpp
  )
endif()

# ---- Warnings-as-errors toggle --------------------------------------------
if(MSVC)
  target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /permissive- /W4 $<$<BOOL:${PLUGIN_WERROR}>:/WX>)
else()
  target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic $<$<BOOL:${PLUGIN_WERROR}>:-Werror>)
endif()

# ---- Output name via template helper --------------------------------------
# Ensures the binary is named after ${_name} (no "lib" prefix on Windows/macOS)
set_target_properties_plugin(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${_name})

# ---- Install (safe defaults for all OSes) ----------------------------------
# Some template builds define these paths; if not, provide sane fallbacks.
include(GNUInstallDirs)

if(NOT DEFINED OBS_PLUGIN_INSTALL_PATH)
  set(OBS_PLUGIN_INSTALL_PATH "${CMAKE_INSTALL_LIBDIR}/obs-plugins")
endif()

if(NOT DEFINED OBS_PLUGIN_DATA_INSTALL_PATH)
  set(OBS_PLUGIN_DATA_INSTALL_PATH "${CMAKE_INSTALL_DATADIR}/obs/obs-plugins")
endif()

# Module libraries (.so/.dylib) go to LIBRARY DESTINATION; on Windows, .dll goes to RUNTIME
install(TARGETS ${CMAKE_PROJECT_NAME}
  LIBRARY DESTINATION "${OBS_PLUGIN_INSTALL_PATH}"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

install(DIRECTORY data/ DESTINATION "${OBS_PLUGIN_DATA_INSTALL_PATH}/${_name}")

# NOTE: Do NOT include(CPack) here; your template/CI handles packaging.
