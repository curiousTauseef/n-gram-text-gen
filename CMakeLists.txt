# Markov chain N-gram text generator for fast work with big number of N. Can work fast with N-grams where N can me much bigger than 10.
# Copyright © 2018  Oleksandr Molchevskyi
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
# Contact email molchevsky@gmail.com

cmake_minimum_required(VERSION 3.10)
project(textgen)

set(CMAKE_CXX_STANDARD 14)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_USE_DEBUG_LIBS OFF) # ignore debug libs and
set(Boost_USE_RELEASE_LIBS ON)  # only find release libs

find_package(Boost 1.58.0 REQUIRED COMPONENTS program_options serialization)

if(MSVC AND (NOT MSVC_VERSION LESS 1910))
    # Get the CMAKE version string and make sure it's not a release candidate and >= 3.8.0
    if( (CMAKE_VERSION MATCHES "^3\\.8\\.0-rc") OR (CMAKE_VERSION VERSION_LESS 3.8.0))
        message(FATAL_ERROR "CMake 3.8.0 is the minimum version required to use Boost with Visual Studio 2017 or greater")
    endif()
endif()

if(MSVC AND ${Boost_FOUND})
    SET(BOOST_ROOT $ENV{BOOST_ROOT})
    message(STATUS "BOOST_ROOT in ${BOOST_ROOT}" )

    SET(Boost_INCLUDEDIR $ENV{BOOST_ROOT})
    message(STATUS "Boost_INCLUDEDIR in ${Boost_INCLUDEDIR}" )

    SET(Boost_LIBRARYDIR $ENV{Boost_LIBRARYDIR})
    message(STATUS "Boost_LIBRARYDIR in ${Boost_LIBRARYDIR}" )

    include_directories(${Boost_INCLUDEDIR})
endif()


#set(c_flags -funroll-loops)
#add_compile_options(
#        "$<$<COMPILE_LANGUAGE:C>:${c_flags}>"
#        "$<$<COMPILE_LANGUAGE:CXX>:${cxx_flags}>"
#)
#set_target_properties(some-target PROPERTIES COMPILE_OPTIONS "${c_flags}")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funroll-loops" CACHE STRING "" FORCE)

if(Boost_FOUND)
    add_executable(textgen main.cpp dictionary.cpp dictionary.h hypercube.cpp hypercube.h)
    TARGET_LINK_LIBRARIES( textgen ${Boost_LIBRARIES} )
endif()