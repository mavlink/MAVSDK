# Re-export from mavsdk — no async wrapper needed for pure enums.
from mavsdk.component_type import ComponentType

__all__ = ["ComponentType"]
