if(NOT EXISTS "/home/amd/MAVSDK/cmake-build-debug/third_party/tinyxml2/tinyxml2/src/tinyxml2-build/install_manifest.txt")
  message(FATAL_ERROR "Cannot find install manifest: /home/amd/MAVSDK/cmake-build-debug/third_party/tinyxml2/tinyxml2/src/tinyxml2-build/install_manifest.txt")
endif(NOT EXISTS "/home/amd/MAVSDK/cmake-build-debug/third_party/tinyxml2/tinyxml2/src/tinyxml2-build/install_manifest.txt")

file(READ "/home/amd/MAVSDK/cmake-build-debug/third_party/tinyxml2/tinyxml2/src/tinyxml2-build/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")
foreach(file ${files})
  message(STATUS "Uninstalling $ENV{DESTDIR}${file}")
  if(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
    exec_program(
      "/tmp/mozilla_amd0/clion-2020.2.4/bin/cmake/linux/bin/cmake" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
      OUTPUT_VARIABLE rm_out
      RETURN_VALUE rm_retval
      )
    if(NOT "${rm_retval}" STREQUAL 0)
      message(FATAL_ERROR "Problem when removing $ENV{DESTDIR}${file}")
    endif(NOT "${rm_retval}" STREQUAL 0)
  else(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
    message(STATUS "File $ENV{DESTDIR}${file} does not exist.")
  endif(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
endforeach(file)
