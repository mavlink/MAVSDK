#!/bin/bash

set -e

# This creates a version such as "v1.2.3-5-g123abc".
version=`git describe --always --tags`
# We want to extract 1.2.3 from it.
version=`echo $version | sed 's/v\([0-9]*\.[0-9]*\.[0-9]*\).*$/\1/'`

make clean
make BUILD_TYPE=Release

common_args="--input-type dir  \
    --version $version \
    --maintainer julian@oes.ch \
    --url http://dronecore.io \
    --license BSD-3-Clause \
    --force"

library_files="\
    core/system.h=/usr/include/dronecore/system.h \
    core/dronecore.h=/usr/include/dronecore/dronecore.h \
    build/default/core/libdronecore.so=/usr/lib/libdronecore.so \
    plugins/action/action.h=/usr/include/dronecore/action.h \
    plugins/follow_me/follow_me.h=/usr/include/dronecore/follow_me.h \
    plugins/gimbal/gimbal.h=/usr/include/dronecore/gimbal.h \
    plugins/info/info.h=/usr/include/dronecore/info.h \
    plugins/logging/logging.h=/usr/include/dronecore/logging.h \
    plugins/mission/mission.h=/usr/include/dronecore/mission.h \
    plugins/mission/mission_item.h=/usr/include/dronecore/mission_item.h \
    plugins/offboard/offboard.h=/usr/include/dronecore/offboard.h \
    plugins/telemetry/telemetry.h=/usr/include/dronecore/telemetry.h"

echo "#!/bin/sh" > run_ldconfig
echo "/sbin/ldconfig" >> run_ldconfig

if cat /etc/os-release | grep -q 'Ubuntu\|Debian'
then
    echo "Building DEB package"
    fpm $common_args \
        --output-type deb \
        --depends libtinyxml2-dev \
        --depends libcurl4-openssl-dev \
        --deb-no-default-config-files \
        --name libdronecore-dev \
        --provides libdronecore-dev \
        --after-install run_ldconfig \
        --after-remove run_ldconfig \
        $library_files

elif cat /etc/os-release | grep -q 'Fedora\|RedHat'
then
    echo "Building RPM package"
    fpm $common_args \
        --output-type rpm \
        --depends tinyxml2-devel \
        --depends libcurl-devel \
        --name libdronecore-devel \
        --provides libdronecore-devel \
        --after-install run_ldconfig \
        --after-remove run_ldconfig \
        $library_files
fi

rm run_ldconfig
