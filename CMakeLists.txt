cmake_minimum_required(VERSION 3.16...3.28)
project(smart_notes VERSION 0.1 LANGUAGES CXX)

# Автоматизация для Qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Важно: убрать хардкод пути к Qt!
# Вместо этого используем find_package
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Рекурсивный поиск всех исходных файлов
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/*.h"
    "src/*.ui"
)

# Добавляем ресурсы
set(RESOURCES "images/icon.qrc")

# Создание исполняемого файла
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(smart_notes
        MANUAL_FINALIZATION
        ${SOURCES}
        ${RESOURCES}
    )
else()
    add_executable(smart_notes
        ${SOURCES}
        ${RESOURCES}
    )
endif()

# Линковка Qt
target_link_libraries(smart_notes PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Инклюды
target_include_directories(smart_notes PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/note
    ${CMAKE_CURRENT_SOURCE_DIR}/src/folder
)

# Финализация для Qt6
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(smart_notes)
endif()

# Установка (опционально)
install(TARGETS smart_notes
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)