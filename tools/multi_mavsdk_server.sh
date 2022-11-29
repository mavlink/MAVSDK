#!/bin/bash

# Run multiple MAVSDK-Server instances, up to a maximum of 9. Read (http://docs.px4.io/main/en/simulation/multi_vehicle_simulation_gazebo.html) notes.
# For more than 9 vehicles reuse the 14549 UDP vehicle port and use the MAV_SYS_ID to select each vehicle.
#
# This script supposes that the mavsdk_server executable is in the parent folder
# By default it supposes that the mavsdk_server_manylinux2010-x64 is being used, to change it modify the MAVSDK_SERVER_EXEC constant with the corresponding exec path
#
#
# All instances outputs are shown in the same terminal, to inspect each one individually one could use:
# Inspect MAVSDK-Server instance output with:
#    tail -f /proc/PID/fd/1 for std 
# or 
#    tail -f /proc/PID/fd/2 for errors


MAVSDK_SERVER_EXEC="../mavsdk_server_manylinux2010-x64"
MAVSDK_SERVER_PROCESS_NAME="mavsdk_server_m"

MAX_INSTANCES=9
USAGE_TEXT="Usage: $0 [-h Print help][-n <num_vehicles>]"


# trap ctrl-c and call ctrl_c()
trap on_exit SIGINT

function kill_instances() {
	pkill -x $MAVSDK_SERVER_PROCESS_NAME || true
}

function on_exit() {
	echo 'Killing spawned processes'
	kill_instances
}

if [ "$1" == "-h" ] || [ "$1" == "--help" ]
then
	echo "$USAGE_TEXT"
	echo 
	echo "NOTE: Already running instances are killed on script start and spawned instances get killed on script exit"
	echo
	echo "Spawn multiple mavsdk_server instances for multiple vehicles using consecutive UDP reception ports, 
from [14540-14549] and their corresponding gRPC ports, from [50050-50059]."
	echo
	echo "For more indications visit:"
	echo "http://docs.px4.io/main/en/simulation/multi_vehicle_simulation_gazebo.html"
	echo "or"
	echo "https://docs.px4.io/main/en/simulation/multi_vehicle_jmavsim.html"
	echo
	exit 1
fi

if [ "$1" == "-n" ] && [ "$2" == 0 ]
then
	echo "ERROR: Number has to be positive"
	exit 1
fi

while getopts n: option
do
	case "${option}"
	in
		n) NUM_VEHICLES=${OPTARG};;
		*) echo "$USAGE_TEXT" >&2
		exit 1 ;;
	esac
done

num_vehicles=${NUM_VEHICLES:=1}

echo "Killing already running mavsdk-server instances"
kill_instances

sleep 1

instances_to_run=$((num_vehicles>MAX_INSTANCES ? MAX_INSTANCES : num_vehicles))
echo "Spawning: $instances_to_run instances" 

for ((i=0;i<instances_to_run;i++));
do
	$MAVSDK_SERVER_EXEC udp://:$((14540+i)) -p $((50050+i)) &
	echo "Instance: " $((i)) "udp://:" $((14540+i)) "-p" $((50050+i)) " PID:" $!
done

wait
