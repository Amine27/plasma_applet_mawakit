# Project Needs a name ofcourse
project(mawakit)
 
# Find the required Libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   ${QT_QTSQL_INCLUDE_DIR}
   ${KDE4_PHONON_INCLUDE_DIR}
   )

# We add our source code here
set(mawakit_SRCS src/mawakit.cpp src/location.cpp src/database.cpp src/manualcalc.cpp
                 src/notification.cpp src/display.cpp src/qibla.cpp src/monthlyprayer.cpp)

kde4_add_ui_files(mawakit_SRCS form/locationform.ui form/manualcalcform.ui form/notificationform.ui
                               form/displayform.ui form/qibladirectionform.ui form/monthlyprayerform.ui)

# Now make sure all files get to the right place
kde4_add_plugin(plasma_applet_mawakit ${mawakit_SRCS})
target_link_libraries(plasma_applet_mawakit
                      ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${QT_QTSQL_LIBRARY} ${KDE4_PHONON_LIBRARY})
 
install(TARGETS plasma_applet_mawakit
        DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES plasma_applet_mawakit.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
		
install(FILES data/countries.db data/mawakit.notifyrc data/hayy.ogg data/duaa.ogg data/compass.svg
		DESTINATION ${DATA_INSTALL_DIR}/mawakit)
		
add_subdirectory(po)
add_subdirectory(icons)
