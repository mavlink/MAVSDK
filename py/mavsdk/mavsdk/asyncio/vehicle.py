# Re-export from mavsdk — no async wrapper needed for pure enums.
from mavsdk.vehicle import Vehicle

__all__ = ["Vehicle"]
