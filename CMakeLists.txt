cmake_minimum_required(VERSION 3.10.0)
project(EncoderMLC VERSION 0.1.0)

include(CTest)
enable_testing()


#Bring the headers, such as Student.h into the project
include_directories(/usr/local/include/itpp)
include_directories(/usr/local/hdf5/include)
include_directories(include)




#Can manually add the sources using the set command as follows:
#set(SOURCES src/mainapp.cpp src/Student.cpp)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "src/*.cpp")

#For the shared library:
set ( PROJECT_LINK_LIBS libitpp.so libhdf5.so libhdf5_cpp.so)
link_directories( /usr/local/lib)
link_directories( /usr/local/hdf5/lib)



add_executable(EncRun ${SOURCES})

# target link libraries
target_link_libraries(EncRun ${PROJECT_LINK_LIBS})

set_target_properties(EncRun PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../test")

#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)
