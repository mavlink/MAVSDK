# Makefile to build DroneCore
#
# The targets are mostly for convenience around cmake.
#

ifndef VERBOSE
	# Don't show make output about changing directories
	MAKEFLAGS += --no-print-directory
endif

# Check for ninja and use it if available
NINJA_BUILD := $(shell ninja --version 2>/dev/null)

ifdef NINJA_BUILD
	CMAKE_GENERATOR ?= "Ninja"
	MAKE = ninja
	MAKE_ARGS =
else
	CMAKE_GENERATOR ?= "Unix Makefiles"
	MAKE = make
	# Use 8 processes in parallel for the usual make
	MAKE_ARGS = -j8
endif

# Get additional arguments after target argument. (e.g. make release install)
ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
# and turn them into do-nothing targets
$(eval $(ARGS):;@:)

# Default is "Debug", also possible is "Release"
BUILD_TYPE ?= "Debug"

# Default is no DROPDEBUG
DROP_DEBUG ?= 0


CURRENT_DIR := $(shell pwd)
ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
CURL_BUILD_DIR := $(ROOT_DIR)/core/third_party/curl-android-ios

# Set default cmake here but replace with special version for Android build.
CMAKE_BIN = cmake

INSTALL_PREFIX ?= /usr/local
BUILD_DRONECORESERVER ?= NO

# Function to create build_* directory and call make there.
define cmake-build
+@$(eval BUILD_DIR = build/$@)

+@if [ ! -e $(BUILD_DIR)/CMakeCache.txt ]; then \
	mkdir -p $(BUILD_DIR) \
	&& (cd $(BUILD_DIR) \
        && $(CMAKE_BIN) $(ROOT_DIR) $(1) \
        -DEXTERNAL_DIR:STRING=$(EXTERNAL_DIR) \
        -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
        -DCMAKE_INSTALL_PREFIX=$(INSTALL_PREFIX) \
		-DCMAKE_BUILD_DRONECORESERVER=$(BUILD_DRONECORESERVER) \
		-DDROP_DEBUG=$(DROP_DEBUG) \
        -G$(CMAKE_GENERATOR)) \
	|| (rm -rf $(BUILD_DIR)) \
fi

+@(echo "Build dir: $(BUILD_DIR)" \
	&& (cd $(BUILD_DIR) && $(MAKE) $(MAKE_ARGS) $(ARGS)) \
  )
endef


all: default

default:
	$(call cmake-build)

ios: ios_curl
	$(call cmake-build, \
		-DCMAKE_TOOLCHAIN_FILE=iOS.cmake \
		-DIOS_PLATFORM:STRING=OS)

ios_simulator: ios_curl
	$(call cmake-build, \
		-DCMAKE_TOOLCHAIN_FILE=iOS.cmake \
		-DIOS_PLATFORM:STRING=SIMULATOR)

android_x86: android_curl
	$(call cmake-build, \
		-DCMAKE_TOOLCHAIN_FILE=$(ANDROID_TOOLCHAIN_CMAKE) \
		-DANDROID_STL:STRING=c++_static \
		-DANDROID_ABI=x86)

android_x86_64: android_curl
	$(call cmake-build, \
		-DCMAKE_TOOLCHAIN_FILE=$(ANDROID_TOOLCHAIN_CMAKE) \
		-DANDROID_STL:STRING=c++_static \
		-DANDROID_ABI=x86_64)

android_armeabi: android_curl
	$(call cmake-build, \
		-DCMAKE_TOOLCHAIN_FILE=$(ANDROID_TOOLCHAIN_CMAKE) \
		-DANDROID_STL:STRING=c++_static \
		-DANDROID_ABI=armeabi)

android_armeabi-v7a: android_curl
	$(call cmake-build, \
		-DCMAKE_TOOLCHAIN_FILE=$(ANDROID_TOOLCHAIN_CMAKE) \
		-DANDROID_STL:STRING=c++_static \
		-DANDROID_ABI=armeabi-v7a \
		-DANDROID_PLATFORM=android-16)

android_arm64-v8a: android_curl
	$(call cmake-build, \
		-DCMAKE_TOOLCHAIN_FILE=$(ANDROID_TOOLCHAIN_CMAKE) \
		-DANDROID_STL:STRING=c++_static \
		-DANDROID_ABI=arm64-v8a)

android_curl: android_env_check
	+@if [ ! -e $(CURL_BUILD_DIR)/prebuilt-with-ssl/android ]; then \
		$(CURL_BUILD_DIR)/curl-compile-scripts/build_Android.sh; \
	fi

ios_curl:
	+@if [ ! -e $(CURL_BUILD_DIR)/prebuilt-with-ssl/ios ]; then \
		$(CURL_BUILD_DIR)/curl-compile-scripts/build_iOS.sh; \
	fi


android: \
    android_x86 \
    android_x86_64 \
    android_armeabi \
    android_armeabi-v7a \
    android_arm64-v8a

fix_style:
	@$(ROOT_DIR)/fix_style.sh $(ROOT_DIR)
ifdef EXTERNAL_DIR
	@$(ROOT_DIR)/fix_style.sh $(EXTERNAL_DIR)
endif

run_all_tests: default
	${MAKE} -C build/default check

run_unit_tests: default
	build/default/core/unit_tests_runner

run_integration_tests: default
	build/default/integration_tests/integration_tests_runner

distclean:
	@rm -rf build/
	@rm -rf logs/

clean:
	@if [ -d build ]; then find build -mindepth 2 -delete -path "build/*/third_party"; fi

android_env_check:
ifndef ANDROID_TOOLCHAIN_CMAKE
	$(error ANDROID_TOOLCHAIN_CMAKE is undefined, please point the \
	    environment variable to build/cmake/android.toolchain.cmake from android-ndk.)
endif
ifndef NDK_ROOT
	$(error NDK_ROOT is undefined, please point the environment variable to android-ndk root.)
endif
ifndef ANDROID_CMAKE_BIN
	$(error ANDROID_CMAKE_BIN is undefined, please point the \
	    environment variable to cmake/3.6.3155560/bin/cmake from android-sdk.)
else
	@# We need cmake from the Android SDK (currently at 3.6 because 3.7 fails the test).
	$(eval CMAKE_BIN = $(ANDROID_CMAKE_BIN))
endif

.PHONY:
	clean fix_style run_all_tests run_unit_tests run_integration_tests android_env_check
