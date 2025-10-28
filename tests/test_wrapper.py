import pytest
from pymavsdk import CMAVSDK
from pymavsdk.exceptions import LibraryNotFoundError, FunctionCallError


def test_library_loading():
    '''Test that library can be loaded'''
    # This will fail if library is not found
    try:
        mavsdk = CMAVSDK()
        assert mavsdk._lib is not None
    except LibraryNotFoundError:
        pytest.skip("cmavsdk library not found")


def test_custom_path():
    '''Test loading library from custom path'''
    with pytest.raises(OSError):
        CMAVSDK(lib_path="/nonexistent/path/libcmavsdk.so")


def test_context_manager():
    '''Test context manager functionality'''
    try:
        with CMAVSDK() as mavsdk:
            assert mavsdk._lib is not None
    except LibraryNotFoundError:
        pytest.skip("cmavsdk library not found")


# Add more tests based on actual cmavsdk functions
