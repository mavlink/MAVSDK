// asio_throw_exception.cpp
//
// With ASIO_NO_EXCEPTIONS, Asio only declares asio::detail::throw_exception<>
// and leaves it to the application to provide a definition (see
// asio/detail/throw_exception.hpp).  MAVSDK builds with -fno-exceptions
// (see cmake/compiler_flags.cmake), so we define the function here to call
// std::terminate(), and explicitly instantiate it for every exception type
// that Asio emits.

#include "mavsdk_export.hpp"
#include "asio/detail/throw_exception.hpp"
#include "asio/error.hpp"
#include "asio/execution_context.hpp"
#include "asio/execution/bad_executor.hpp"
#include "asio/ip/bad_address_cast.hpp"

#include <new>
#include <stdexcept>
#include <system_error>

namespace asio {
namespace detail {

template<typename Exception> void throw_exception(const Exception& e)
{
    (void)e;
    std::terminate();
}

// Explicit instantiations for all exception types Asio uses internally.
// These must be exported so that unit tests (and any other consumer that
// includes Asio headers with ASIO_NO_EXCEPTIONS) can resolve them when
// linking against the shared library.
//
// Sources:
//   service_already_exists, invalid_service_owner → detail/impl/service_registry.ipp
//   bad_executor                                  → execution/impl/bad_executor.ipp
//   system_error, bad_alloc                       → general async machinery
//   bad_address_cast                              → ip/impl/address{,_v4,_v6}.ipp
//   out_of_range                                  → ip/impl/address_v{4,6}.ipp
template MAVSDK_TEST_EXPORT void
throw_exception<asio::service_already_exists>(const asio::service_already_exists&);
template MAVSDK_TEST_EXPORT void
throw_exception<asio::invalid_service_owner>(const asio::invalid_service_owner&);
template MAVSDK_TEST_EXPORT void
throw_exception<asio::execution::bad_executor>(const asio::execution::bad_executor&);
template MAVSDK_TEST_EXPORT void throw_exception<std::system_error>(const std::system_error&);
template MAVSDK_TEST_EXPORT void throw_exception<std::bad_alloc>(const std::bad_alloc&);
template MAVSDK_TEST_EXPORT void
throw_exception<asio::ip::bad_address_cast>(const asio::ip::bad_address_cast&);
template MAVSDK_TEST_EXPORT void throw_exception<std::out_of_range>(const std::out_of_range&);

} // namespace detail
} // namespace asio
