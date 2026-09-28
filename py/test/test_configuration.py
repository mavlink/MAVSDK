"""Configuration tests for the Python bindings.

Mavsdk copies the configuration it is created with, so the Configuration stays
usable, and is still released, after it has been passed in.
"""

from test_teardown import run_scenario


def test_read_configuration_after_creating_mavsdk():
    run_scenario("""
        from mavsdk import ComponentType, Configuration, Mavsdk

        configuration = Configuration.create_with_component_type(
            ComponentType.COMPANION_COMPUTER
        )
        system_id = configuration.system_id
        component_id = configuration.component_id
        mavsdk = Mavsdk(configuration)

        assert configuration.system_id == system_id
        assert configuration.component_id == component_id

        mavsdk.destroy()
    """)


def test_configuration_released_before_mavsdk():
    run_scenario("""
        import gc
        from mavsdk import ComponentType, Configuration, Mavsdk

        configuration = Configuration.create_with_component_type(
            ComponentType.GROUND_STATION
        )
        mavsdk = Mavsdk(configuration)
        del configuration
        gc.collect()

        assert mavsdk.version()
        mavsdk.destroy()
    """)
