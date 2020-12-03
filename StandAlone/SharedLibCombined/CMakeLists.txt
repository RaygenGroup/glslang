# Copyright (C) 2020 The Khronos Group Inc.
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#    Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
#    Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#
#    Neither the name of The Khronos Group Inc. nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

set(SOURCES SharedLibCombined.cpp SharedLibCombined.h)

set (CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(SPIRV-SharedLib SHARED ${SOURCES})
set (CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)
set_property(TARGET SPIRV-SharedLib PROPERTY FOLDER tools)

set(LIBRARIES
    glslang
    SPIRV
    glslang-default-resource-limits)

if(ENABLE_SPVREMAPPER)
    set(LIBRARIES ${LIBRARIES} SPVRemapper)
endif()

if(WIN32)
    set(LIBRARIES ${LIBRARIES} psapi)
elseif(UNIX)
    if(NOT ANDROID)
        set(LIBRARIES ${LIBRARIES} pthread)
    endif()
endif(WIN32)

target_link_libraries(SPIRV-SharedLib ${LIBRARIES})
target_include_directories(SPIRV-SharedLib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../External>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/External>)

if(ENABLE_OPT)
    target_include_directories(SPIRV-SharedLib
        PRIVATE ${spirv-tools_SOURCE_DIR}/include
    )
endif(ENABLE_OPT)

if(WIN32)
    source_group("Source" FILES ${SOURCES})
endif(WIN32)
