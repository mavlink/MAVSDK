# Re-export from pymavsdk — no async wrapper needed for pure enums.
from pymavsdk.vehicle import Vehicle

__all__ = ["Vehicle"]
