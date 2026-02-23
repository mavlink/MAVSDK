# Re-export from pymavsdk — no async wrapper needed for pure enums.
from pymavsdk.connection_result import ConnectionResult

__all__ = ["ConnectionResult"]
