find_package(KDE4 REQUIRED)
set(CMAKE_AUTOMOC ON)


include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/build ${KDE4_INCLUDES} )

set(bdgui_LIBS
	${KDE4_KDEUI_LIBS} 
	${KDE4_KPARTS_LIBS} 
)
 set(CMAKE_AUTOUIC_OPTIONS -tr i18n -include klocalizedstring.h) 
 
QT4_WRAP_UI(bdgui_ui_h ${bdgui_ui})

qt4_add_resources(bdgui_rcc_h ${bdgui_RESOURCES})
add_definitions(-DKDE4)
SET(CMAKE_CXX_FLAGS -std=c++11)


find_package(QJSON "0.8.1" REQUIRED)


set(bdgui_LIBS ${bdgui_LIBS} ${QJSON_LIBRARIES})