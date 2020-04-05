"""
Example dummy gimbal manager component, implemented in PyMAVLink.
At the current time of writing, there are some issues with implementing a dummy
gimbal manager component with the MAVLinkPassthrough module.
This example uses the libpilot MAVLink component library (https://gitlab.avhs.best/amadoruavs/pilot/pilot).

Some TODOS:
- This dummy implementation does not implement gimbal manager flags > 16.
  While this should not be a problem for pure testing purposes, it would
  probably be nice to implement those flags in some capacity.

- This gimbal manager currently DOES NOT implement deconfliction. Again, while it should work fine
  for testing, it would probably be good to implement this in the future.

Originally authored by: Vincent Wang (2020-03-30).
"""
import argparse
import time
from pilot import Component
from pilot.util import MavlinkId
from pymavlink import mavutil

class DummyGimbalManager(Component):
    def __init__(self, mavlink_url: str, mavlink_id: MavlinkId, device_id: MavlinkId):
        super().__init__(mavlink_url, mavlink_id, mavutil.mavlink.MAV_TYPE_ONBOARD_CONTROLLER)

        self.discovered = False
        self.gimbal_discovered = False
        self.starttime = time.time()
        self.device_id = device_id
        self.flags = 8
        self.gimbal_info = {"firmware_version": 0,
                            "cap_flags": 0,
                            "tilt_max": 0,
                            "tilt_min": 0,
                            "tilt_rate_max": 0,
                            "pan_max": 0,
                            "pan_min": 0,
                            "pan_rate_max": 0}

        self.register_callback(self._request_gimbal_info)
        self.register_callback(self._discover_gimbal)
        self.register_callback(self._discover_self)
        self.register_callback(self._set_attitude)
        self.register_callback(self._report_status)

        self.register_callback(self._do_gimbal_manager_attitude)

    def _activate(self, source_id):
        """
        Activate the component.
        """
        super()._activate()

    def _deactivate(self, source_id):
        """
        Deactivate the component.
        """
        super()._deactivate(source_id)

    def _request_gimbal_info(self, msg, data):
        """
        Request gimbal information as part of the gimbal device
        discovery process outlined in https://mavlink.io/en/services/gimbal_v2.html.
        Will retry at a rate of 1Hz until successful.
        """
        if not self.gimbal_discovered and ("prev_rq_time" not in data or time.time() - data["prev_rq_time"] >= 1):
            print("requesting gimbal information from", self.device_id)
            self.mavlink_conn.mav.command_long_send(self.device_id.system_id,
                                                    self.device_id.component_id,
                                                    mavutil.mavlink.MAV_CMD_REQUEST_MESSAGE,
                                                    0,
                                                    mavutil.mavlink.MAVLINK_MSG_ID_GIMBAL_DEVICE_INFORMATION,
                                                    0, 0, 0, 0, 0, 0)
            data["prev_rq_time"] = time.time()

    def _discover_gimbal(self, msg, data):
        """
        Receive gimbal capabilities and store them.
        """
        if not hasattr(msg, "id"):
            return

        if msg.id == mavutil.mavlink.MAVLINK_MSG_ID_GIMBAL_DEVICE_INFORMATION:
            info = msg.to_dict()
            print("gimbaldev", data)
            self.gimbal_info = info
            self.gimbal_discovered = True

    def _discover_self(self, msg, data):
        """
        Report gimbal capabilities as part of the gimbal manager
        discovery process outlined in https://mavlink.io/en/services/gimbal_v2.html.
        """
        if not hasattr(msg, "id"):
            return


        if msg.id == mavutil.mavlink.MAVLINK_MSG_ID_COMMAND_LONG:
            msg_data = msg.to_dict()

            # We only want to respond to a request to our specific component
            if msg_data.get("target_system") != self.mavlink_id.system_id or \
                    msg_data.get("target_component") != self.mavlink_id.component_id:
                return

            if msg.command == mavutil.mavlink.MAV_CMD_REQUEST_MESSAGE and \
                    msg_data["param1"] == mavutil.mavlink.MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION:
                #We don't want to be discovered until we have the gimbal device info
                if not self.gimbal_discovered:
                    self.mavlink_conn.mav.command_ack_send(msg.id, 5)
                    return
                else:
                    self.mavlink_conn.mav.command_ack_send(msg.id, 0)

                print("Sending gimbal manager information")
                self.mavlink_conn.mav.gimbal_manager_information_send(round(1000*(time.time() - self.starttime)),
                                                                      self.gimbal_info["cap_flags"],
                                                                      self.device_id.component_id,
                                                                      self.gimbal_info["tilt_max"],
                                                                      self.gimbal_info["tilt_min"],
                                                                      self.gimbal_info["tilt_rate_max"],
                                                                      self.gimbal_info["pan_max"],
                                                                      self.gimbal_info["pan_min"],
                                                                      self.gimbal_info["pan_rate_max"])

    def _set_attitude(self, msg, data):
        """
        Implement the GIMBAL_MANAGER_SET_ATTITUDE command.
        This basically just forwards the request to the gimbal device.
        Because this is a dummy device, gimbal flags > 16 are ignored and
        reset to the default (GIMBAL_DEVICE_FLAGS_PITCH_LOCK).
        """
        if not hasattr(msg, "id"):
            return

        if msg.id == mavutil.mavlink.MAVLINK_MSG_ID_GIMBAL_MANAGER_SET_ATTITUDE:
            msg_data = msg.to_dict()

            # We only want to respond to a request to our specific component
            if msg_data.get("target_system") != self.mavlink_id.system_id or \
                    msg_data.get("target_component") != self.mavlink_id.component_id:
                return

            # TODO: If we feel like it we can implement the more complicated gimbal manager flags
            # However since this is a dummy implementation anyway leaving them unimplemented and
            # falling back to default should be an acceptable approach
            self.flags = msg_data["flags"] if msg_data["flags"] <= 16 else 8
            self.mavlink_conn.gimbal_device_set_attitude_send(self.device_id.system_id,
                                                              self.device_id.component_id,
                                                              self.flags,
                                                              msg_data["q"],
                                                              msg_data["angular_velocity_x"],
                                                              msg_data["angular_velocity_y"],
                                                              msg_data["angular_velocity_z"])
            print("Set attitude! data:", msg_data)

    def _do_gimbal_manager_attitude(self, msg, data):
        """
        Implement the DO_GIMBAL_MANAGER_ATTITUDE command.
        Since this is a dummy implementation we will just print an acknowledgement.
        """
        if not hasattr(msg, "id"):
            return

        if msg.id == mavutil.mavlink.MAVLINK_MSG_ID_COMMAND_LONG:
            msg_data = msg.to_dict()

            # We only want to respond to a request to our specific component
            if msg_data.get("param1") != self.device_id.component_id:
                return

            if msg.command == mavutil.mavlink.MAV_CMD_DO_GIMBAL_MANAGER_ATTITUDE:
                print("Received DO_GIMBAL_MANAGER_ATTITUDE! data:", msg.to_dict())
                self.mavlink_conn.mav.command_ack_send(msg.id, 0)

    def _do_set_roi_location(self, msg, data):
        """
        Implement the DO_SET_ROI_LOCATION command.
        Since this is a dummy implementation we will just print an acknowledgement.
        """
        if not hasattr(msg, "id"):
            return

        if msg.id == mavutil.mavlink.MAVLINK_MSG_ID_COMMAND_LONG:
            msg_data = msg.to_dict()

            # We only want to respond to a request to our specific component
            if msg_data.get("param1") != self.device_id.component_id:
                return

            if msg.command == mavutil.mavlink.MAV_CMD_DO_SET_ROI_LOCATION:
                print("Received DO_SET_ROI_LOCATION!: data:", msg.to_dict())
                self.mavlink_conn.mav.command_ack_send(msg.id, 0)

    def _do_set_roi_wpnext_offset(self, msg, data):
        """
        Implement the DO_SET_ROI_WPNEXT_OFFSET command.
        Since this is a dummy implementation we will just print an acknowledgement.
        """
        if not hasattr(msg, "id"):
            return

        if msg.id == mavutil.mavlink.MAVLINK_MSG_ID_COMMAND_LONG:
            msg_data = msg.to_dict()

            # We only want to respond to a request to our specific component
            if msg_data.get("param1") != self.device_id.component_id:
                return

            if msg.command == mavutil.mavlink.MAV_CMD_DO_SET_ROI_WPNEXT_OFFSET:
                print("Received DO_SET_ROI_WPNEXT_OFFSET! data:", msg.to_dict())
                self.mavlink_conn.mav.command_ack_send(msg.id, 0)

    def _do_set_roi_sysid(self, msg, data):
        """
        Implement the DO_SET_ROI_SYSID command.
        Since this is a dummy implementation we will just print an acknowledgement.
        """
        if not hasattr(msg, "id"):
            return

        if msg.id == mavutil.mavlink.MAVLINK_MSG_ID_COMMAND_LONG:
            msg_data = msg.to_dict()

            # We only want to respond to a request to our specific component
            if msg_data.get("param1") != self.device_id.component_id:
                return

            if msg.command == mavutil.mavlink.MAV_CMD_DO_SET_ROI_SYSID:
                print("Received DO_SET_ROI_SYSID! data:", msg.to_dict())
                self.mavlink_conn.mav.command_ack_send(msg.id, 0)

    def _do_set_roi_none(self, msg, data):
        """
        Implement the DO_SET_ROI_NONE command.
        Since this is a dummy implementation we will just print an acknowledgement.
        """
        if not hasattr(msg, "id"):
            return

        if msg.id == mavutil.mavlink.MAVLINK_MSG_ID_COMMAND_LONG:
            msg_data = msg.to_dict()

            # We only want to respond to a request to our specific component
            if msg_data.get("param1") != self.device_id.component_id:
                return

            if msg.command == mavutil.mavlink.MAV_CMD_DO_SET_ROI_NONE:
                print("Received DO_SET_ROI_NONE! data:", msg.to_dict())
                self.mavlink_conn.mav.command_ack_send(msg.id, 0)

    def _do_gimbal_manager_track_point(self, msg, data):
        """
        Implement the DO_GIMBAL_MANAGER_TRACK_POINT command.
        Since this is a dummy implementation we will just print an acknowledgement.
        """
        if not hasattr(msg, "id"):
            return

        if msg.id == mavutil.mavlink.MAVLINK_MSG_ID_COMMAND_LONG:
            msg_data = msg.to_dict()

            # We only want to respond to a request to our specific component
            if msg_data.get("param1") != self.device_id.component_id:
                return

            if msg.command == mavutil.mavlink.MAV_CMD_DO_GIMBAL_MANAGER_TRACK_POINT:
                print("Received DO_GIMBAL_MANAGER_TRACK_POINT! data:", msg.to_dict())
                self.mavlink_conn.mav.command_ack_send(msg.id, 0)

    def _do_gimbal_manager_track_rectangle(self, msg, data):
        """
        Implement the DO_GIMBAL_MANAGER_TRACK_RECTANGLE command.
        Since this is a dummy implementation we will just print an acknowledgement.
        """
        if not hasattr(msg, "id"):
            return

        if msg.id == mavutil.mavlink.MAVLINK_MSG_ID_COMMAND_LONG:
            msg_data = msg.to_dict()

            # We only want to respond to a request to our specific component
            if msg_data.get("param1") != self.device_id.component_id:
                return

            if msg.command == mavutil.mavlink.MAV_CMD_DO_GIMBAL_MANAGER_TRACK_RECTANGLE:
                print("Received DO_GIMBAL_MANAGER_TRACK_RECTANGLE! data:", msg.to_dict())
                self.mavlink_conn.mav.command_ack_send(msg.id, 0)

    def _report_status(self, msg, data):
        """
        Report GIMBAL_MANAGER_STATUS at a rate of 5hz.
        """
        if "gm_prev_time" not in data or time.time() - data["gm_prev_time"] >= (1 / 5):
            self.mavlink_conn.mav.gimbal_manager_status_send(round(1000*(time.time() - self.starttime)),
                                                             self.flags)
            data["gm_prev_time"] = time.time()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("mavlink_url", help="URL of the MAVLink system (e.g. udp:localhost:14553).")
    parser.add_argument("gimbal_sid", help="System ID of the gimbal device")
    parser.add_argument("gimbal_cid", help="Component ID of the gimbal device")
    parser.add_argument("--manager-sid", help="System ID of the manager", default=254)
    parser.add_argument("--manager-cid", help="Component ID of the manager [25-99]", default=25)
    args = parser.parse_args()

    manager_id = MavlinkId(system_id=int(args.manager_sid), component_id=int(args.manager_cid))
    device_id = MavlinkId(system_id=int(args.gimbal_sid), component_id=int(args.gimbal_cid))

    manager = DummyGimbalManager(args.mavlink_url, manager_id, device_id)
    manager.start()
