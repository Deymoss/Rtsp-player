cmake_minimum_required(VERSION 3.22)
set(PROJECT_NAME rtsp-player)
project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick Qml Widgets OpenGL Concurrent Xml WebSockets SerialPort Network Core5Compat)

include_directories("include/", "src/")
file(GLOB PROJECT_SOURCES "main.cpp" "include/*.h" "src/*.cpp" "qml.qrc" "resources.qrc")
if(UNIX)
find_package(PkgConfig REQUIRED)
pkg_check_modules(gstreamer-full REQUIRED IMPORTED_TARGET gstreamer-full-1.0)
pkg_check_modules(glib REQUIRED IMPORTED_TARGET glib-2.0)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 5)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET untitled APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${PROJECT_NAME}
          ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Quick
  Qt${QT_VERSION_MAJOR}::Qml
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::OpenGL
  Qt${QT_VERSION_MAJOR}::Concurrent
  Qt${QT_VERSION_MAJOR}::Xml
  Qt${QT_VERSION_MAJOR}::WebSockets
  Qt${QT_VERSION_MAJOR}::SerialPort
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::Core5Compat
  PkgConfig::glib
  PkgConfig::gstreamer-full)

set(template_content
    "BASE_DIR=\$(dirname \"\$(readlink -f \"\$0\")\")
export PATH=\"\$BASE_DIR\"/bin/:\$PATH
export LD_LIBRARY_PATH=\"\$BASE_DIR\"/lib/:\"\$BASE_DIR\":\$LD_LIBRARY_PATH
export QML_IMPORT_PATH=\"\$BASE_DIR\"/qml/:\$QML_IMPORT_PATH
export QML2_IMPORT_PATH=\"\$BASE_DIR\"/qml/:\$QML2_IMPORT_PATH
export QT_PLUGIN_PATH=\"\$BASE_DIR\"/plugins/:\$QT_PLUGIN_PATH
export QTWEBENGINEPROCESS_PATH=\"\$BASE_DIR\"/bin//QtWebEngineProcess
export QTDIR=\"\$BASE_DIR\"
export CQT_PKG_ROOT=\"\$BASE_DIR\"
export CQT_RUN_FILE=\"\$BASE_DIR/rtsp-player.sh\"

export QT_QPA_PLATFORM_PLUGIN_PATH=\"\$BASE_DIR\"/plugins//platforms:\$QT_QPA_PLATFORM_PLUGIN_PATH

\"\$BASE_DIR/bin/rtsp-player\" \"\$@\"
")

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/Seintinel_viewer.tmp ${template_content})
configure_file(${CMAKE_CURRENT_BINARY_DIR}/Seintinel_viewer.tmp ${CMAKE_CURRENT_BINARY_DIR}/rtsp-player.sh @ONLY)

# Устанавливаем файл скрипта
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rtsp-player.sh DESTINATION ${CMAKE_INSTALL_PREFIX} PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)

endif()

if(WIN32)
    find_package(PkgConfig REQUIRED)
#    pkg_check_modules(gstreamer-plugins-good REQUIRED IMPORTED_TARGET gstreamer-plugins-good-1.0)
    pkg_check_modules(gstreamer REQUIRED IMPORTED_TARGET gstreamer-1.0)
    pkg_check_modules(gstreamer-plugins-base REQUIRED IMPORTED_TARGET gstreamer-plugins-base-1.0)
    pkg_check_modules(gstreamer-video REQUIRED IMPORTED_TARGET gstreamer-video-1.0)
    pkg_check_modules(gstreamer-gl REQUIRED IMPORTED_TARGET gstreamer-gl-1.0)
    pkg_check_modules(gstreamer-sdp REQUIRED IMPORTED_TARGET gstreamer-sdp-1.0)
    pkg_check_modules(gstreamer-app REQUIRED IMPORTED_TARGET gstreamer-app-1.0)
    pkg_check_modules(glib REQUIRED IMPORTED_TARGET glib-2.0)

    if(${QT_VERSION_MAJOR} GREATER_EQUAL 5)
        qt_add_executable(${PROJECT_NAME}
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
        )
    # Define target properties for Android with Qt 6 as:
    #    set_property(TARGET untitled APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
    #                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
    # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
    else()
        if(ANDROID)
            add_library(${PROJECT_NAME} SHARED
                ${PROJECT_SOURCES}
            )
    # Define properties for Android with Qt 5 after find_package() calls as:
    #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
        else()
            add_executable(${PROJECT_NAME}
              ${PROJECT_SOURCES}
            )
        endif()
    endif()

    target_link_libraries(${PROJECT_NAME}
      PRIVATE Qt${QT_VERSION_MAJOR}::Core
      Qt${QT_VERSION_MAJOR}::Quick
      Qt${QT_VERSION_MAJOR}::Qml
      Qt${QT_VERSION_MAJOR}::Widgets
      Qt${QT_VERSION_MAJOR}::OpenGL
      Qt${QT_VERSION_MAJOR}::Concurrent
      Qt${QT_VERSION_MAJOR}::Xml
      Qt${QT_VERSION_MAJOR}::WebSockets
      Qt${QT_VERSION_MAJOR}::SerialPort
      Qt${QT_VERSION_MAJOR}::Network
      Qt${QT_VERSION_MAJOR}::Core5Compat
      PkgConfig::glib
      PkgConfig::gstreamer
      PkgConfig::gstreamer-gl
      PkgConfig::gstreamer-plugins-base
      PkgConfig::gstreamer-sdp
      PkgConfig::gstreamer-app
      PkgConfig::gstreamer-video)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 5)
    qt_import_qml_plugins(${PROJECT_NAME})
    qt_finalize_executable(${PROJECT_NAME})
endif()

qt_generate_deploy_qml_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
)

install(SCRIPT ${deploy_script})

