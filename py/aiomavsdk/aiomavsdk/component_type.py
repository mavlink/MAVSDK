# Re-export from pymavsdk — no async wrapper needed for pure enums.
from pymavsdk.component_type import ComponentType

__all__ = ["ComponentType"]
