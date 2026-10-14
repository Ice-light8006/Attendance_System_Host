cmake_minimum_required(VERSION 3.19)
project(Attendance_System LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network Sql)

qt_standard_project_setup()

qt_add_executable(Attendance_System
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    TcpServer.cpp
    TcpServer.h
    Protocol.cpp
    Protocol.h
    TcpConnection.h
    TcpConnection.cpp
    DeviceControl.cpp
    DeviceControl.h
    TcpClient.cpp
    TcpClient.h
    TcpClientPanel.cpp
    TcpClientPanel.h
    ControlPanel.cpp
    ControlPanel.h
    DatabaseManager.h
    DatabaseManager.cpp
    DatabasePanel.cpp
    DatabasePanel.h
    InformationDialog.cpp
    InformationDialog.h
    StudentProtocol.h
    UIDProtocol.h
    UIDPanel.cpp
    UIDPanel.h
)


target_link_libraries(Attendance_System
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Network
        Qt6::Sql
)

install(TARGETS Attendance_System
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Attendance_System
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
