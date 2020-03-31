"""
Example dummy gimbal device component, implemented in PyMAVLink.
At the current time of writing, there are some issues with implementing a dummy
gimbal device component with the MAVLinkPassthrough module.
This example uses the libpilot MAVLink component library (https://gitlab.avhs.best/amadoruavs/pilot/pilot).

Originally authored by: Vincent Wang (2020-03-30).
"""
import argparse
import time
from pilot import Component
from pilot.util import MavlinkId
from pymavlink import mavutil

class DummyGimbalDevice(Component):
    def __init__(self, mavlink_url: str, mavlink_id: MavlinkId):
        super().__init__(mavlink_url, mavlink_id, mavutil.mavlink.MAV_TYPE_GIMBAL)

        self.discovered = False
        self.starttime = time.time()
        self.flags = 8

        self.angular_velocity_x = 0
        self.angular_velocity_y = 0
        self.angular_velocity_z = 0
        self.q = [0, 0, 0, 0]

        self.gimbal_info = {"vendor_name": [0]*32,
                            "model_name": [0]*32,
                            "firmware_version": 1,
                            "cap_flags": 512,
                            "tilt_max": 3.14,
                            "tilt_min": -3.14,
                            "tilt_rate_max": 0.5,
                            "pan_max": 3.14,
                            "pan_min": -3.14,
                            "pan_rate_max": 0.5}

        self.register_callback(self._discover_self)
        self.register_callback(self._set_attitude)
        self.register_callback(self._autopilot_state_for_gimbal_device)
        self.register_callback(self._report_status)

    def _activate(self, source_id):
        """
        Activate the component.
        """
        super()._activate(source_id)

    def _deactivate(self, source_id):
        """
        Deactivate the component.
        """
        super()._deactivate(source_id)

    def _discover_self(self, msg, data):
        """
        Report gimbal capabilities as part of the gimbal device
        discovery process outlined in https://mavlink.io/en/services/gimbal_v2.html.
        """
        if not hasattr(msg, "id"):
            return


        if msg.id == mavutil.mavlink.MAVLINK_MSG_ID_COMMAND_LONG:
            msg_data = msg.to_dict()

            # If the message is not directed at us, we should not respond
            if msg_data.get("target_system") != self.mavlink_id.system_id or \
                    msg_data.get("target_component") != self.mavlink_id.component_id:
                return

            if msg.command == mavutil.mavlink.MAV_CMD_REQUEST_MESSAGE:
                print("Sending gimbal device information")
                self.mavlink_conn.mav.gimbal_device_information_send(round(1000*(time.time() - self.starttime)),
                                                                     self.gimbal_info["vendor_name"],
                                                                     self.gimbal_info["model_name"],
                                                                     self.gimbal_info["firmware_version"],
                                                                     self.gimbal_info["cap_flags"],
                                                                     self.gimbal_info["tilt_max"],
                                                                     self.gimbal_info["tilt_min"],
                                                                     self.gimbal_info["tilt_rate_max"],
                                                                     self.gimbal_info["pan_max"],
                                                                     self.gimbal_info["pan_min"],
                                                                     self.gimbal_info["pan_rate_max"])

    def _set_attitude(self, msg, data):
        """
        Implement the GIMBAL_DEVICE_SET_ATTITUDE command.
        This command responds with GIMBAL_DEVICE_ATTITUDE_STATUS as outlined
        in the Gimbal v2 Protocol (https://mavlink.io/en/services/gimbal_v2.html).
        """
        if not hasattr(msg, "id"):
            return

        if msg.id == mavutil.mavlink.MAVLINK_MSG_ID_GIMBAL_DEVICE_SET_ATTITUDE:
            msg_data = msg.to_dict()

            # If the message is not directed at us, we should not respond
            if msg_data.get("target_system") != self.mavlink_id.system_id or \
                    msg_data.get("target_component") != self.mavlink_id.component_id:
                return

            # Set our attitude
            self.angular_velocity_x = msg_data.get("angular_velocity_x")
            self.angular_velocity_y = msg_data.get("angular_velocity_y")
            self.angular_velocity_z = msg_data.get("angular_velocity_z")
            self.q = msg_data.get("q")
            self.flags = msg_data.get("flags")

            # Respond with GIMBAL_DEVICE_ATTITUDE_STATUS
            self.mavlink_conn.mav.gimbal_device_attitude_status_send(round(1000*(time.time() - self.starttime)),
                                                                     self.flags,
                                                                     self.q,
                                                                     self.angular_velocity_x,
                                                                     self.angular_velocity_y,
                                                                     self.angular_velocity_z,
                                                                     0)

            print("Received SET_ATTITUDE! data:", msg.to_dict())

    def _autopilot_state_for_gimbal_device(self, msg, data):
        """
        Receive the AUTOPILOT_STATE_FOR_GIMBAL_DEVICE message.

        Normally, you would use the data in this message to control a gimbal,
        but as there is no physical gimbal to control we will just print it instead.
        """
        if not hasattr(msg, "id"):
            return

        if msg.id == mavutil.mavlink.MAVLINK_MSG_ID_AUTOPILOT_STATE_FOR_GIMBAL_DEVICE:
            msg_data = msg.to_dict()

            # If the message is not directed at us, we should not respond
            if msg_data.get("target_system") != self.mavlink_id.system_id or \
                    msg_data.get("target_component") != self.mavlink_id.component_id:
                return

            print("Received AUTOPILOT_STATE_FOR_GIMBAL_DEVICE! data:", msg_data)

    def _report_status(self, msg, data):
        """
        Report GIMBAL_DEVICE_ATTITUDE_STATUS at a rate of 10hz.
        """
        if "gm_prev_time" not in data or time.time() - data["gm_prev_time"] >= (1 / 10):
            self.mavlink_conn.mav.gimbal_device_attitude_status_send(round(1000*(time.time() - self.starttime)),
                                                                     self.flags,
                                                                     self.q,
                                                                     self.angular_velocity_x,
                                                                     self.angular_velocity_y,
                                                                     self.angular_velocity_z,
                                                                     0)
            data["gm_prev_time"] = time.time()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("mavlink_url", help="URL of the MAVLink system (e.g. udp:localhost:14550).")
    parser.add_argument("gimbal_sid", help="System ID of the gimbal device")
    parser.add_argument("gimbal_cid", help="Component ID of the gimbal device [154, 171-175]")
    args = parser.parse_args()

    device_id = MavlinkId(system_id=int(args.gimbal_sid), component_id=int(args.gimbal_cid))

    device = DummyGimbalDevice(args.mavlink_url, device_id)
    device.start()
