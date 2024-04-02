# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/sdli/esp/esp-idf/components/bootloader/subproject"
  "/Users/sdli/esp/Microcontrollers/projects/arm_control/build/bootloader"
  "/Users/sdli/esp/Microcontrollers/projects/arm_control/build/bootloader-prefix"
  "/Users/sdli/esp/Microcontrollers/projects/arm_control/build/bootloader-prefix/tmp"
  "/Users/sdli/esp/Microcontrollers/projects/arm_control/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/sdli/esp/Microcontrollers/projects/arm_control/build/bootloader-prefix/src"
  "/Users/sdli/esp/Microcontrollers/projects/arm_control/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/sdli/esp/Microcontrollers/projects/arm_control/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/sdli/esp/Microcontrollers/projects/arm_control/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
