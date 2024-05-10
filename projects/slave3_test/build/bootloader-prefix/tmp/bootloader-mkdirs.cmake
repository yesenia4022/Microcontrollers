# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/yeseniapuga/esp/esp-idf/components/bootloader/subproject"
  "/Users/yeseniapuga/esp/projects/slave3_test/build/bootloader"
  "/Users/yeseniapuga/esp/projects/slave3_test/build/bootloader-prefix"
  "/Users/yeseniapuga/esp/projects/slave3_test/build/bootloader-prefix/tmp"
  "/Users/yeseniapuga/esp/projects/slave3_test/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/yeseniapuga/esp/projects/slave3_test/build/bootloader-prefix/src"
  "/Users/yeseniapuga/esp/projects/slave3_test/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/yeseniapuga/esp/projects/slave3_test/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/yeseniapuga/esp/projects/slave3_test/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
