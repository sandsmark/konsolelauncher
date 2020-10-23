cmake_minimum_required(VERSION 3.6)

project(konsolelauncher LANGUAGES CXX C)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})
include(KDEInstallDirs)
include(KDECMakeSettings)
include(FeatureSummary)

find_package(Qt5 CONFIG REQUIRED
    Core
    DBus
    PrintSupport
    Widgets
)

find_package(KF5 REQUIRED
    Bookmarks
    Completion
    Config
    ConfigWidgets
    CoreAddons
    Crash
    DBusAddons
    GlobalAccel
    GuiAddons
    I18n
    IconThemes
    Init
    KIO
    NewStuff
    NewStuffCore
    Notifications
    NotifyConfig
    Parts
    Pty
    Service
    TextWidgets
    WidgetsAddons
    WindowSystem
    XmlGui
)
find_package(Fontconfig)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")

add_definitions(-DCMAKE_INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\")
add_definitions(-DLIB_INSTALL_DIR=\"${KDE_INSTALL_LIBDIR}\")

include(GNUInstallDirs)
set(LIBEXEC_PATH "${CMAKE_INSTALL_FULL_LIBDIR}/konsolelauncher/")
add_definitions(-DLIBEXEC_DIR=\"${LIBEXEC_PATH}\")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(zygote zygote.cpp)
add_executable(launch-konsole launcher.c)

target_link_libraries(zygote PRIVATE
    KF5::Bookmarks
    KF5::Crash
    KF5::DBusAddons
    KF5::GlobalAccel
    KF5::GuiAddons
    KF5::I18n
    KF5::IconThemes
    KF5::KIOWidgets
    KF5::NewStuff
    KF5::Notifications
    KF5::NotifyConfig
    KF5::Pty
    KF5::TextWidgets
    KF5::WindowSystem
    KF5::XmlGui
    Qt5::PrintSupport
    Qt5::Xml
    Fontconfig::Fontconfig
    )

install(TARGETS launch-konsole DESTINATION bin)
install(TARGETS zygote
    DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/konsolelauncher/"
    )
