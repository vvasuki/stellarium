INCLUDE_DIRECTORIES(
     .
     gui
     ${CMAKE_BINARY_DIR}/plugins/ObservingLists/src
     ${CMAKE_BINARY_DIR}/plugins/ObservingLists/src/gui
)

LINK_DIRECTORIES(${BUILD_DIR}/src)

SET(ObservingLists_SRCS
     ObservingLists.hpp
     ObservingLists.cpp     
     gui/ObservingListsDialog.hpp
     gui/ObservingListsDialog.cpp
)

SET(ObservingListsDialog_UIS
     gui/observingListsDialog.ui
)

QT5_WRAP_UI(ObservingListsDialog_UIS_H ${ObservingListsDialog_UIS})

SET(ObservingLists_RES ../resources/ObservingLists.qrc)
QT5_ADD_RESOURCES(ObservingLists_RES_CXX ${ObservingLists_RES})

ADD_LIBRARY(ObservingLists-static STATIC ${ObservingLists_SRCS} ${ObservingLists_RES_CXX} ${ObservingListsDialog_UIS_H})
TARGET_LINK_LIBRARIES(ObservingLists-static Qt5::Core Qt5::Network Qt5::Widgets)
SET_TARGET_PROPERTIES(ObservingLists-static PROPERTIES OUTPUT_NAME "ObservingLists")
SET_TARGET_PROPERTIES(ObservingLists-static PROPERTIES COMPILE_FLAGS "-DQT_STATICPLUGIN")
ADD_DEPENDENCIES(AllStaticPlugins ObservingLists-static)

SET_TARGET_PROPERTIES(ObservingLists-static PROPERTIES FOLDER "plugins/ObservingLists")
