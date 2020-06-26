cmake_minimum_required(VERSION 2.8)
project( DisplayImage )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -DBOOST_LOG_DYN_LINK")

find_package( OpenCV REQUIRED )

find_package( PkgConfig REQUIRED )
pkg_search_module( TESSERACT REQUIRED tesseract )
pkg_search_module( LEPTONICA REQUIRED lept )
include_directories( ${TESSERACT_INCLUDE_DIRS} )
include_directories( ${LEPTONICA_INCLUDE_DIRS} )
link_directories( ${TESSERACT_LIBRARY_DIRS} )
link_directories( ${LEPTONICA_LIBRARY_DIRS} )

#set(Boost_INCLUDE_DIR /usr/local/src/boost_1_46_1)
#set(Boost_LIBRARY_DIR /usr/local/src/boost_1_46_1/stage/lib)
find_package(Boost COMPONENTS program_options log log_setup system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})


#find_package( Tesseract REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable( detect_chessboard detect_chessboard.cpp )
target_link_libraries( detect_chessboard ${OpenCV_LIBS} )
add_executable( scan_qr_code scan_qr_code.cpp )
target_link_libraries( scan_qr_code ${OpenCV_LIBS} ${ZBAR_LIBRARIES}zbar )

add_executable( capture capture.cpp )
#target_compile_features( capture PRIVATE cxx_std_17)
#target_link_libraries( capture ${OpenCV_LIBS} ${ZBAR_LIBRARIES}zbar stdc++fs std=c++17 )
target_link_libraries( capture ${OpenCV_LIBS} ${ZBAR_LIBRARIES}zbar )

#add_executable( write_font_sticker write_font_sticker.cpp )
#target_link_libraries( write_font_sticker ${OpenCV_LIBS} ${ZBAR_LIBRARIES}zbar )
add_executable( main main.cpp )
#target_link_libraries( main ${OpenCV_LIBS} ${ZBAR_LIBRARIES}zbar ${} )
target_link_libraries( main ${OpenCV_LIBS} ${ZBAR_LIBRARIES}zbar ${TESSERACT_LIBRARIES} ${LEPTONICA_LIBRARIES} ${Boost_LIBRARIES} ${Boost_LOG_LIBRARY} ${Boost_LOG_SETUP_LIBRARY} )

add_executable( test_captures test_captures.cpp )
#target_compile_features( capture PRIVATE cxx_std_17)
#target_link_libraries( capture ${OpenCV_LIBS} ${ZBAR_LIBRARIES}zbar stdc++fs std=c++17 )
target_link_libraries( test_captures ${OpenCV_LIBS} ${ZBAR_LIBRARIES}zbar ${TESSERACT_LIBRARIES} ${LEPTONICA_LIBRARIES} ${Boost_LIBRARIES} ${Boost_LOG_LIBRARY} ${Boost_LOG_SETUP_LIBRARY} )


