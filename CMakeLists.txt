# DEPENDENCIES LIST: # python is for matplotlib
# leptonica is for tesseract
# tesseract if for reading words

cmake_minimum_required(VERSION 2.8)
project( DisplayImage )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -DBOOST_LOG_DYN_LINK -DDEBUG -DWITHOUT_NUMPY -Wno-psabi -Wall")

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

#include_directories( "/usr/include/ruby-2.5.0/" "/usr/include/x86_64-linux-gnu/ruby-2.5.0" )
include_directories( ${OpenCV_INCLUDE_DIRS} "/usr/local/include/eigen3/" )
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/SQLiteCpp)

include_directories(${CMAKE_CURRENT_LIST_DIR}/thirdparty)
include_directories(${CMAKE_CURRENT_LIST_DIR}/thirdparty/Eigen)
include_directories(${CMAKE_CURRENT_LIST_DIR}/thirdparty/SQLiteCpp/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/thirdparty/simple-web-server)

set(MyLibs ${MyLibs} ${OpenCV_LIBS} ${TESSERACT_LIBRARIES} ${LEPTONICA_LIBRARIES} ${Boost_LIBRARIES} ${Boost_LOG_LIBRARY} ${Boost_LOG_SETUP_LIBRARY} ${PYTHON_LIBRARIES} SQLiteCpp sqlite3 dl pthread )
# ruby-2.5 gmp crypt m
#set(MyDirs ${MyDirs} ${Python2_INCLUDE_DIRS} ${Python2_NumPy_INCLUDE_DIRS} /usr/local/include/simple-web-server/ )
set(MyDirs ${MyDirs} ${Python2_INCLUDE_DIRS} ${Python2_NumPy_INCLUDE_DIRS} )

include_directories(${MyDirs})
link_libraries(${MyLibs})

ADD_LIBRARY(SlaveLibs
  src/lib/ArduinoMock.cpp
)

ADD_LIBRARY(MyLibsModule
  src/lib/linux.cpp
  src/lib/serial.cpp
  src/lib/ArduinoMock.cpp
  src/utils/io_common.cpp
  src/utils/utils.cpp
#  src/core/program.cpp
  src/core/pinpoint.cpp
#  src/core/deserialize.cpp
  src/core/heda.cpp
  src/core/model.cpp
  src/core/database.cpp
#  src/core/fake_program.cpp
  src/core/writer/writer.cpp
  src/core/reader/reader.cpp
  src/core/tess_parser.cpp
  src/core/position.cpp
  src/lib/lib.cpp
  src/lib/hr_code.cpp
  src/lib/opencv.cpp
#  src/core/input_parser.cpp
  src/helper/logging.cpp
  src/core/parser.cpp
)
link_libraries(MyLibsModule)

#message(STATUS "HERE!!!")
#message(STATUS ${Python2_INCLUDE_DIRS})
#message(STATUS "THERE!!!")

#add_executable( run_tests src/test/test.cpp )

add_executable( test_stepper_motor src/test/stepper_motor.cpp )
target_link_libraries( test_stepper_motor ${MyLibs} ${SlaveLibs} )

#add_executable( heda src/heda.cpp )

#add_executable( test_db src/test/db.cpp )

#add_executable( test_jar_position_analyzer src/test/jar_position_analyzer.cpp )

#add_executable( test_axis src/test/axis.cpp )

#add_executable( test_parser src/test/parser.cpp )

#add_executable( test_heda src/test/heda.cpp )

#add_executable( simulation src/simulation.cpp )

add_executable( server src/server.cpp )

add_executable( benchmark src/benchmark.cpp )

add_executable( rasp_server src/rasp_server.cpp )

#add_executable( simulation src/server.cpp )
#target_link_libraries( simulation ${MyLibs}  )
#target_include_directories( simulation PRIVATE "/usr/local/include/simple-web-server/" )
#target_compile_definitions(simulation PUBLIC SIMULATION)

enable_testing()
add_test( regularTests bin/run_tests )
add_test( testJarPositionAnalyzer bin/test_jar_position_analyzer )
add_test( testAxis bin/test_axis )

add_custom_target(check COMMAND make && ${CMAKE_CTEST_COMMAND} --verbose)
