# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/biglap/ESPRESSIF/esp-idf-v5.3.1/components/bootloader/subproject"
  "/home/biglap/PROGETTI/GEI/JGUARDIAN2p0_HUB/JGuardianHUB_JOKEY/build/bootloader"
  "/home/biglap/PROGETTI/GEI/JGUARDIAN2p0_HUB/JGuardianHUB_JOKEY/build/bootloader-prefix"
  "/home/biglap/PROGETTI/GEI/JGUARDIAN2p0_HUB/JGuardianHUB_JOKEY/build/bootloader-prefix/tmp"
  "/home/biglap/PROGETTI/GEI/JGUARDIAN2p0_HUB/JGuardianHUB_JOKEY/build/bootloader-prefix/src/bootloader-stamp"
  "/home/biglap/PROGETTI/GEI/JGUARDIAN2p0_HUB/JGuardianHUB_JOKEY/build/bootloader-prefix/src"
  "/home/biglap/PROGETTI/GEI/JGUARDIAN2p0_HUB/JGuardianHUB_JOKEY/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/biglap/PROGETTI/GEI/JGUARDIAN2p0_HUB/JGuardianHUB_JOKEY/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/biglap/PROGETTI/GEI/JGUARDIAN2p0_HUB/JGuardianHUB_JOKEY/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
