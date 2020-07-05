# DEPENDENCIES LIST:
# python is for matplotlib
# leptonica is for tesseract
# tesseract if for reading words
# zbar is for reading qr codes (obselete)

cmake_minimum_required(VERSION 2.8)
project( DisplayImage )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -DBOOST_LOG_DYN_LINK -DDEBUG -DWITHOUT_NUMPY")

find_package(PythonLibs 2.7)
set(Python2_INCLUDE_DIRS "/usr/include/python2.7" )

find_package( OpenCV REQUIRED )

find_package( PkgConfig REQUIRED )
pkg_search_module( TESSERACT REQUIRED tesseract )
pkg_search_module( LEPTONICA REQUIRED lept )
include_directories( ${TESSERACT_INCLUDE_DIRS} )
include_directories( ${LEPTONICA_INCLUDE_DIRS} )
link_directories( ${TESSERACT_LIBRARY_DIRS} )
link_directories( ${LEPTONICA_LIBRARY_DIRS} )

find_package(Boost COMPONENTS program_options log log_setup system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})


include_directories( ${OpenCV_INCLUDE_DIRS} )
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(MyLibs ${MyLibs} ${OpenCV_LIBS} ${ZBAR_LIBRARIES}zbar ${TESSERACT_LIBRARIES} ${LEPTONICA_LIBRARIES} ${Boost_LIBRARIES} ${Boost_LOG_LIBRARY} ${Boost_LOG_SETUP_LIBRARY} ${PYTHON_LIBRARIES})
set(MyDirs ${MyDirs} ${Python2_INCLUDE_DIRS} ${Python2_NumPy_INCLUDE_DIRS} )
#message(STATUS "HERE!!!")
#message(STATUS ${Python2_INCLUDE_DIRS})
#message(STATUS "THERE!!!")

add_executable( detect_chessboard src/detect_chessboard.cpp )
target_link_libraries( detect_chessboard ${MyLibs} )

add_executable( scan_qr_code src/scan_qr_code.cpp )
target_link_libraries( scan_qr_code ${MyLibs} )

add_executable( capture src/capture.cpp )
target_link_libraries( capture ${MyLibs} )

#add_executable( write_font_sticker write_font_sticker.cpp )
#target_link_libraries( write_font_sticker ${OpenCV_LIBS} ${ZBAR_LIBRARIES}zbar )

add_executable( main src/main.cpp )
target_link_libraries( main ${MyLibs} )

add_executable( test_captures src/test_captures.cpp )
target_link_libraries( test_captures ${MyLibs} )

add_executable( sweep src/sweep.cpp )
target_link_libraries( sweep ${MyLibs} )

add_executable( video_capture src/video_capture.cpp )
target_link_libraries( video_capture ${MyLibs} )

add_executable( usbreset src/usbreset.c )
target_link_libraries( usbreset ${MyLibs} )

add_executable( run_tests src/test.cpp )
target_include_directories(run_tests PRIVATE ${MyDirs})
target_link_libraries( run_tests ${MyLibs} )

add_executable( fake src/fake.cpp )
target_include_directories(fake PRIVATE ${MyDirs})
target_link_libraries( fake ${MyLibs} )
