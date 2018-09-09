cmake_minimum_required(VERSION 3.10)
project(textgen)

set(CMAKE_CXX_STANDARD 17)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_USE_DEBUG_LIBS OFF) # ignore debug libs and
set(Boost_USE_RELEASE_LIBS ON)  # only find release libs
find_package(Boost 1.58.0 REQUIRED COMPONENTS program_options serialization)


#set(c_flags -funroll-loops)
#add_compile_options(
#        "$<$<COMPILE_LANGUAGE:C>:${c_flags}>"
#        "$<$<COMPILE_LANGUAGE:CXX>:${cxx_flags}>"
#)
#set_target_properties(some-target PROPERTIES COMPILE_OPTIONS "${c_flags}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funroll-loops" CACHE STRING "" FORCE)

if(Boost_FOUND)
    add_executable(textgen main.cpp dictionary.cpp dictionary.h hypercube.cpp hypercube.h)
    TARGET_LINK_LIBRARIES( textgen ${Boost_LIBRARIES} )
endif()