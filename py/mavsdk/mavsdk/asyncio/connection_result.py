# Re-export from mavsdk — no async wrapper needed for pure enums.
from mavsdk.connection_result import ConnectionResult

__all__ = ["ConnectionResult"]
