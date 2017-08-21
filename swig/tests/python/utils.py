from __future__ import print_function
fro nose.tools import with_setup

def setup_sitl():
    # launch sitl instance

def teardown_sitl():
    # shutdown sitl process

def with_device(fn):
    print "Setting up SITL device..."
    discovered_device = False
    dronecore = DroneCore()
    connection_result = dronecore.add_udp_connection()

    if (connection_result != DroneCore.ConnectionResult_SUCCESS):
        print "Connection failed: %s" % connection_result

    print "Waiting to discover device..."

    def registered(uuid):
        print "Discovered device with UUID: %s" % uuid
        discovered_device = True

    # dronecore.register_on_discover(registered)
    time.sleep(2)
    # print "Devices: %s" % dronecore.device_uuids()

    if (discovered_device):
        print "No device found, exiting."
        # ipdb.set_trace(context=10)
        # print dir(d)
        quit()

    device = dronecore.device()
    @with_setup(setup_sitl, teardown_stil)
    def test(*args, **kargs):
        return fn(device)
    return test

