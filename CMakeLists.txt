cmake_minimum_required(VERSION 3.5)

project(smart_notes VERSION 0.1 LANGUAGES CXX)

# Включение автообработки UI, MOC и ресурсов
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Указываем путь к Qt6
set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/qt6")

# Ищем Qt6 или Qt5
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Источники проекта
set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    # Для Qt6 используем qt_add_executable
    qt_add_executable(smart_notes
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PROJECT_UI_FILES}  # Убедитесь, что файлы .ui здесь указаны
    )
else()
    # Для Qt5 используем обычное add_executable
    add_executable(smart_notes
        ${PROJECT_SOURCES}
        src/note/note.h src/note/note.cpp src/note/note.ui
        src/folder/folder.h src/folder/folder.cpp src/folder/folder.ui
        src/note/notelistwidget.h src/note/notelistwidget.cpp src/note/notelistwidget.ui
        src/note/models/modelsNote.h src/note/models/modelsNote.cpp


    )
endif()

# Линковка с библиотеками Qt
target_link_libraries(smart_notes PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(smart_notes PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
)

include_directories(${CMAKE_SOURCE_DIR}/src)

# Установка
include(GNUInstallDirs)
install(TARGETS smart_notes
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Финализация для Qt6
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(smart_notes)
endif()
