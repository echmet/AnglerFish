cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(AnglerFish)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(FLATPAK_BUILD "Application is being built as Flatpak package" OFF)
option(TEST_CRASHHANDLER "Include crash handler test" OFF)

if (WIN32)
    option(ECHMET_FORCE_WINXP "Define target Windows version to Windows XP")
endif ()

if (WIN32)
    if (MINGW)
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9.2)
            message(FATAL_ERROR "MinGW 4.9.2 or newer is required to build AnglerFish")
        endif ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-unused-function -ftree-vectorize -msse2")
        add_definitions("-DECHMET_COMPILER_MINGW")
        set(DEFAULT_SYMVER_LINK "-Wl,--default-symver")
    elseif (MSYS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-unused-function -ftree-vectorize -msse2")
        add_definitions("-DECHMET_COMPILER_MSYS")
        set(DEFAULT_SYMVER_LINK "-Wl,--default-symver")
    elseif (MSVC)
        add_definitions("-DECHMET_COMPILER_MSVC")
        set(DEFAULT_SYMVER_LINK " ")
        if (ECHMET_FORCE_WINXP)
            add_definitions("-D_WIN32_WINNT=0x0501")
        endif ()
    else()
        message(FATAL_ERROR "Compiler not recognized or not supported")
    endif ()
elseif (UNIX)
    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR
        ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
        if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" AND
            CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9.2)
            message(FATAL_ERROR "GCC 4.9.2 or newer is required to build AnglerFish")
        elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" AND
                CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.4)
            message(FATAL_ERROR "Clang 3.4 or newer is required to build AnglerFish")
        endif ()

        add_definitions("-DECHMET_COMPILER_GCC_LIKE")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")

        if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
        elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
            set(CMAKE_CXX_FLAGS
                "${CMAKE_CXX_FLAGS} -Weverything -Wpedantic -Wno-c++98-compat -Wno-global-constructors -Wno-exit-time-destructors -Wno-padded -Wno-weak-vtables -fcomment-block-commands=retval")
        endif ()
    endif ()
else ()
    message(FATAL_ERROR "Platform not recognized or not supported")
endif ()

add_definitions("-DQWT_DLL")

if (FLATPAK_BUILD)
    add_definitions("-DAF_FLATPAK_BUILD")
endif ()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)

include_directories(${INCLUDE_DIRECTORIES}
                    "${CMAKE_CURRENT_SOURCE_DIR}/src")

if (NOT "${ECHMET_CORE_LIBS_DIR}" STREQUAL "")
    include_directories(${INCLUDE_DIRECTORIES}
                        "${ECHMET_CORE_LIBS_DIR}/include/ECHMET/CoreLibs")
    link_directories(${LINK_DIRECTORIES}
                     "${ECHMET_CORE_LIBS_DIR}/lib")
endif ()

if (NOT "${ECHMET_UPDATECHECK_DIR}" STREQUAL "")
    include_directories(${INCLUDE_DIRECTORIES}
                        "${ECHMET_UPDATECHECK_DIR}/include")
    link_directories(${LINK_DIRECTORIES}
                     "${ECHMET_UPDATECHECK_DIR}/lib")
endif ()

if (NOT "${EMPFITTER_DIR}" STREQUAL "")
    include_directories(${INCLUDE_DIRECTORIES}
                        "${EMPFITTER_DIR}/include/ECHMET")
    link_directories(${LINK_DIRECTORIES}
                     "${EMPFITTER_DIR}/lib")
endif ()

if (NOT "${QWTPLOT_DIR}" STREQUAL "")
    include_directories(${INCLUDE_DIRECTORIES}
                        SYSTEM "${QWTPLOT_DIR}/include")
    link_directories(${LINK_DIRECTORIES}
                     "${QWTPLOT_DIR}/lib")
endif ()

if (NOT "${SQLITE3_DIR}" STREQUAL "")
    include_directories(${INCLUDE_DIRECTORIES}
                        SYSTEM "${SQLITE3_DIR}")
    link_directories(${LINK_DIRECTORIES}
                     "${SQLITE3_DIR}")
endif ()

set(CMAKE_CXX_FLAGS
    ${CMAKE_CXX_FLAGS}
    ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}
    ${Qt5Gui_EXECUTABLE_COMPILE_FLAGS}
    ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}
    )

set(AnglerFish_SRCS
    "src/main.cpp"
    "src/globals.cpp"
    "src/afcrashhandler.cpp"
    "src/crashevent.cpp"
    "src/crasheventcatcher.cpp"
    "src/softwareupdater.cpp"
    "src/softwareupdateresult.cpp"
    "src/metatyperegistrator.cpp"
    "src/ui/afmainwindow.cpp"
    "src/ui/fitplotwidget.cpp"
    "src/ui/buffersinputwidget.cpp"
    "src/ui/bufferwidget.cpp"
    "src/ui/experimentalmobilitywidget.cpp"
    "src/ui/buffercompositionwidget.cpp"
    "src/ui/editconstituentdialog.cpp"
    "src/ui/pickconstituentfromdbdialog.cpp"
    "src/ui/databasetableview.cpp"
    "src/ui/editchargeswidget.cpp"
    "src/ui/returnacceptabletableview.cpp"
    "src/ui/copiableitemstableview.cpp"
    "src/ui/analytedatawidget.cpp"
    "src/ui/editchargeswidgetsimple.cpp"
    "src/ui/editchargeswidgetestimates.cpp"
    "src/ui/doubleclickableqwtplotzoomer.cpp"
    "src/ui/aboutdialog.cpp"
    "src/ui/crashhandlerdialog.cpp"
    "src/ui/elementaries/floatingvaluelineedit.cpp"
    "src/ui/elementaries/modifyconstituentchargepushbutton.cpp"
    "src/ui/ioniceffectscorrections.cpp"
    "src/ui/operationinprogressdialog.cpp"
    "src/ui/softwareupdatewidget.cpp"
    "src/ui/autoupdatecheckdialog.cpp"
    "src/ui/checkforupdatedialog.cpp"
    "src/ui/toggletracepointsdialog.cpp"
    "src/ui/summarizedialog.cpp"
    "src/ui/mobilityconstraintsdialog.cpp"
    "src/ui/enterexperimentalphdialog.cpp"
    "src/ui/warnpossiblyunreliabledialog.cpp"
    "src/ui/internal_models/constituentchargesmodel.cpp"
    "src/ui/internal_models/constituentchargesmodelfixable.cpp"
    "src/ui/internal_models/databaseconstituentsphyspropstablemodel.cpp"
    "src/ui/internal_models/buffercompositionmodel.cpp"
    "src/ui/internal_models/mobilityconstraintsmodel.cpp"
    "src/database/db_constituent.cpp"
    "src/database/db_constituentsdatabase.cpp"
    "src/gearbox/doubletostringconvertor.cpp"
    "src/gearbox/additionalfloatingvalidator.cpp"
    "src/gearbox/inumberformatchangeable.cpp"
    "src/gearbox/chemicalbuffer.cpp"
    "src/gearbox/chemicalbuffersmodel.cpp"
    "src/gearbox/gdmproxy.cpp"
    "src/gearbox/gdmproxy_p.cpp"
    "src/gearbox/gdmproxyimpl.cpp"
    "src/gearbox/databaseproxy.cpp"
    "src/gearbox/floatingvaluedelegate.cpp"
    "src/gearbox/constituentmanipulator.cpp"
    "src/gearbox/analyteestimates.cpp"
    "src/gearbox/fitresultsmodel.cpp"
    "src/gearbox/mobilitycurvemodel.cpp"
    "src/gearbox/ioniceffectsmodel.cpp"
    "src/gearbox/calcworker.cpp"
    "src/gearbox/gearboxprivate.cpp"
    "src/gearbox/gearbox.cpp"
    "src/gearbox/curveutility.cpp"
    "src/gearbox/limitmobilityconstraintsmodel.cpp"
    "src/gearbox/utility.cpp"
    "src/gdm/conversion/conversion.cpp"
    "src/gdm/core/basiccomposition.cpp"
    "src/gdm/core/composition.cpp"
    "src/gdm/core/gdm.cpp"
    "src/gdm/core/common/chargeinterval.cpp"
    "src/gdm/core/common/mobility.cpp"
    "src/gdm/core/complexation/chargecombination.cpp"
    "src/gdm/core/complexation/complexation.cpp"
    "src/gdm/core/complexation/complexform.cpp"
    "src/gdm/core/constituent/constituent.cpp"
    "src/gdm/core/constituent/physicalproperties.cpp"
    "src/gdm/core/detail/complexationmap.cpp"
    "src/gdm/core/detail/complexationmapkey.cpp"
    "src/calculators/caesinterface.cpp"
    "src/calculators/empfitterinterface.cpp"
    "src/crashhandling/crashhandlerbase.cpp"
    "src/persistence/persistence.cpp"
    "src/persistence/peakmastercompatibility.cpp"
    "src/persistence/peakmastercompatibility_deserialize.cpp"
    "src/persistence/peakmastercompatibility_serialize.cpp"
    "src/persistence/datakeys.cpp"
    "src/persistence/serializecommon.cpp"
    "src/persistence/deserializecommon.cpp"
    "src/persistence/entiresetup.cpp"
    "src/persistence/entiresetup_serialize.cpp"
    "src/persistence/entiresetup_deserialize.cpp"
    "src/persistence/types.cpp"
    "src/persistence/swsettings.cpp"
    "src/summary/abstractsummarizer.cpp"
    "src/summary/commonoptions.cpp"
    "src/summary/summarizerfactory.cpp"
    "src/summary/csvsummarizer.cpp"
    "src/summary/utility.cpp"
    "src/summary/ui/csvsummarizeroptionsdialog.cpp"
    "afres.qrc"
    )

if (WIN32)
    set(AnglerFish_SRCS
        ${AnglerFish_SRCS}
        AnglerFish.rc)
endif ()

if (WIN32)
    set(AnglerFish_SRCS
        ${AnglerFish_SRCS}
        "src/crashhandling/crashhandlerwindows.cpp"
        )
    add_definitions("-DCRASHHANDLING_WIN32")
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(AnglerFish_SRCS
        ${AnglerFish_SRCS}
        "src/crashhandling/crashhandlerlinux.cpp"
        "src/crashhandling/crashhandlerlinux_stacktrace.cpp"
        )
    add_definitions("-DCRASHHANDLING_LINUX")
    set(LINK_PTHREAD pthread)
else ()
    set(AnglerFish_SRCS
        ${AnglerFish_SRCS}
        "src/crashhandling/nullcrashhandler.cpp"
        )
endif ()

if (CMAKE_BUILD_TYPE MATCHES DEBUG)
    if (TEST_CRASHHANDLER)
        add_definitions("-DUSE_CRASHHANDLER -DAF_TEST_CRASHHANDLER")
    endif ()
else ()
    add_definitions("-DUSE_CRASHHANDLER")
    if (TEST_CRASHHANDLER)
        add_definitions("-DAF_TEST_CRASHHANDLER")
    endif ()
endif ()

if (WIN32)
    add_executable(AnglerFish WIN32 ${AnglerFish_SRCS})
else ()
    add_executable(AnglerFish ${AnglerFish_SRCS})
endif ()

if (WIN32)
    set(WIN32_DBGHELP
        "dbghelp")
endif ()

target_link_libraries(AnglerFish
                      PRIVATE Qt5::Core
                      PRIVATE Qt5::Gui
                      PRIVATE Qt5::Widgets
                      PRIVATE ECHMETShared
                      PRIVATE SysComp
                      PRIVATE CAES
                      PRIVATE IonProps
                      PRIVATE ElmigParamsFitter
                      PRIVATE qwt
                      PRIVATE sqlite3
                      PRIVATE ECHMETUpdateCheck
                      PRIVATE ${LINK_PTHREAD}
                      PRIVATE ${WIN32_DBGHELP}
                      )

install(TARGETS AnglerFish
        RUNTIME DESTINATION bin)

if (UNIX)
    install(FILES "AnglerFish.desktop" DESTINATION "share/applications")
    install(FILES "res/AFicon_16.png" DESTINATION "share/icons/hicolor/16x16/apps" RENAME "AnglerFish.png")
    install(FILES "res/AFicon_24.png" DESTINATION "share/icons/hicolor/24x24/apps" RENAME "AnglerFish.png")
    install(FILES "res/AFicon_32.png" DESTINATION "share/icons/hicolor/32x32/apps" RENAME "AnglerFish.png")
    install(FILES "res/AFicon_48.png" DESTINATION "share/icons/hicolor/48x48/apps" RENAME "AnglerFish.png")
    install(FILES "res/AFicon_64.png" DESTINATION "share/icons/hicolor/64x64/apps" RENAME "AnglerFish.png")
    install(FILES "res/AFicon_128.png" DESTINATION "share/icons/hicolor/128x128/apps" RENAME "AnglerFish.png")
endif ()
