project(gui VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts)

set(PROJECT_SOURCES
    main.cc
    main_window.cc
    main_window.h
    main_window.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(gui
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        xe_paint_widget.h xe_paint_widget.cc
        xe_plot_widget.h xe_plot_widget.cc
        xe_sio2_paint_widget.h xe_sio2_paint_widget.cc
        xe_sio2_plot_widget.h xe_sio2_plot_widget.cc
        xe_xe_sio2_paint_widget.h xe_xe_sio2_paint_widget.cc
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET gui APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(gui SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(gui
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(gui PRIVATE
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::Charts
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.gui)
endif()
set_target_properties(gui PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS gui
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(gui)
endif()

set_source_files_properties(main.cc PROPERTIES
  SKIP_LINTING ON
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE base math modeling physics)
