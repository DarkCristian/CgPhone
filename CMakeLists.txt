cmake_minimum_required(VERSION 3.22)
project(CgPhone VERSION 0.3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

option(CGPHONE_WITH_PJSIP "Build the PJSUA2 SIP adapter" OFF)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Quick QuickControls2 Multimedia Widgets Test)
qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(CgPhone
    src/main.cpp
    src/AppController.cpp src/AppController.h
    src/CallHistoryModel.cpp src/CallHistoryModel.h
    src/SettingsStore.cpp src/SettingsStore.h
    src/RecordingEncoderLocator.cpp src/RecordingEncoderLocator.h
    src/SystemAudioController.cpp src/SystemAudioController.h
    src/sip/ISipEngine.h
    src/sip/MockSipEngine.cpp src/sip/MockSipEngine.h
)

if(WIN32)
    set_target_properties(CgPhone PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if(WIN32)
    target_sources(CgPhone PRIVATE assets/app/CgPhone.rc)
endif()

if(CGPHONE_WITH_PJSIP)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
    find_package(PJSIP REQUIRED)
    target_sources(CgPhone PRIVATE src/sip/PjsipEngine.cpp src/sip/PjsipEngine.h)
    target_compile_definitions(CgPhone PRIVATE CGPHONE_WITH_PJSIP=1)
    target_link_libraries(CgPhone PRIVATE PJSIP::PJSUA2)
endif()

qt_add_qml_module(CgPhone
    URI CgPhone
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/DialerPage.qml
        qml/HistoryPage.qml
        qml/SettingsPage.qml
        qml/components/SoftButton.qml
        qml/components/ToggleCard.qml
        qml/components/SystemVolumeControl.qml
    RESOURCES
        assets/sounds/hangup.wav
        assets/sounds/msgin.wav
        assets/sounds/msgout.wav
        assets/sounds/ringing.wav
        assets/sounds/ringing2.wav
        assets/sounds/ringtone.wav
        assets/sounds/phone-keypad-button-dial.wav
        assets/app/CgPhone.png
        assets/icons/phone-blue.svg
        assets/icons/phone-white.svg
        assets/icons/gear.svg
        assets/icons/transfer.svg
        assets/icons/backspace.svg
)

target_link_libraries(CgPhone PRIVATE
    Qt6::Core Qt6::Gui Qt6::Quick Qt6::QuickControls2 Qt6::Multimedia Qt6::Widgets
)
if(WIN32)
    target_link_libraries(CgPhone PRIVATE Shell32 Crypt32 Advapi32 Ole32 Uuid)
endif()

include(GNUInstallDirs)
install(TARGETS CgPhone
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
if(UNIX AND NOT APPLE)
    install(FILES assets/app/CgPhone.png DESTINATION share/icons/hicolor/512x512/apps RENAME cgphone.png)
    install(FILES assets/app/cgphone.desktop DESTINATION share/applications)
endif()

qt_generate_deploy_qml_app_script(
    TARGET CgPhone
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

enable_testing()
qt_add_executable(CgPhoneTests
    tests/tst_core.cpp
    src/CallHistoryModel.cpp src/CallHistoryModel.h
)
target_include_directories(CgPhoneTests PRIVATE src)
target_link_libraries(CgPhoneTests PRIVATE Qt6::Core Qt6::Test)
add_test(NAME CgPhoneTests COMMAND CgPhoneTests)
