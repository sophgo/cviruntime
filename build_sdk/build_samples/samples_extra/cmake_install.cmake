# Install script for directory: /data/forum_service/songpei/sync/v410_source/cviruntime/samples/samples_extra

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/data/forum_service/songpei/sync/v410_source/install/soc_sg2000_wevb_arm64_sd/tpu_64bit/cvitek_tpu_sdk/samples")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RELEASE")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/data/forum_service/songpei/sync/v410_source/host-tools/gcc/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples_extra" TYPE FILE PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ FILES
    "/data/forum_service/songpei/sync/v410_source/cviruntime/samples/samples_extra/run_alphapose_fused_preprocess.sh"
    "/data/forum_service/songpei/sync/v410_source/cviruntime/samples/samples_extra/run_detector_yolov3_fused_preprocess.sh"
    "/data/forum_service/songpei/sync/v410_source/cviruntime/samples/samples_extra/run_detector_yolov5_fused_preprocess.sh"
    "/data/forum_service/songpei/sync/v410_source/cviruntime/samples/samples_extra/run_detector_yolox_s.sh"
    "/data/forum_service/songpei/sync/v410_source/cviruntime/samples/samples_extra/run_insightface_fused_preprocess.sh"
    "/data/forum_service/songpei/sync/v410_source/cviruntime/samples/samples_extra/run_detector_ppyoloem_fused_preprocess.sh"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/samples_extra" TYPE DIRECTORY FILES "/data/forum_service/songpei/sync/v410_source/cviruntime/samples/samples_extra/data")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/data/forum_service/songpei/sync/v410_source/cviruntime/build_sdk/build_samples/samples_extra/alphapose_fused_preprocess/cmake_install.cmake")
  include("/data/forum_service/songpei/sync/v410_source/cviruntime/build_sdk/build_samples/samples_extra/detector_yolov3_fused_preprocess/cmake_install.cmake")
  include("/data/forum_service/songpei/sync/v410_source/cviruntime/build_sdk/build_samples/samples_extra/detector_yolov5_fused_preprocess/cmake_install.cmake")
  include("/data/forum_service/songpei/sync/v410_source/cviruntime/build_sdk/build_samples/samples_extra/detector_yolov5-face_fused_preprocess/cmake_install.cmake")
  include("/data/forum_service/songpei/sync/v410_source/cviruntime/build_sdk/build_samples/samples_extra/detector_yolox_s/cmake_install.cmake")
  include("/data/forum_service/songpei/sync/v410_source/cviruntime/build_sdk/build_samples/samples_extra/insightface_fused_preprocess/cmake_install.cmake")
  include("/data/forum_service/songpei/sync/v410_source/cviruntime/build_sdk/build_samples/samples_extra/detector_ppyoloem_fused_preprocess/cmake_install.cmake")
  include("/data/forum_service/songpei/sync/v410_source/cviruntime/build_sdk/build_samples/samples_extra/detector_yolov8n_fused_preprocess/cmake_install.cmake")

endif()

