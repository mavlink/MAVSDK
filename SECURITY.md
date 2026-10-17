# Security Policy

## Reporting a vulnerability

Please report vulnerabilities via GitHub's
[private vulnerability reporting](https://github.com/mavlink/MAVSDK/security/advisories/new).
This makes it easy for us to credit you and issue a CVE. We don't use
embargoes, so feel free to also open a PR with a fix right away.

Please include:

- the affected component (C++ library, a specific plugin, mavsdk_server, a language wrapper),
- the MAVSDK version or commit,
- steps or input to reproduce (e.g. a MAVLink message sequence or a test case),
- what you think the impact is.

We will try to acknowledge reports within a week. MAVSDK is maintained by a
small team, so please be patient.

## Bounty

There are currently no bounties offered, but we will try to give credit where
credit is due as much as possible.

## Supported versions

Fixes are made on `main` and released in the next release. Older releases are
generally not patched.

## Threat model

Before reporting, please check whether the issue is in scope given the trust
model below.

### Trusted: the API user

MAVSDK is a library. The application calling the MAVSDK API is trusted. That
includes all arguments passed to the API, configuration, connection URLs, file
paths, and callbacks.

Issues that require the calling application to pass malicious or nonsensical
input (e.g. a crash when passing invalid arguments) are bugs, not security
vulnerabilities. Please report them as regular issues.

### Untrusted: MAVLink

MAVLink traffic is untrusted. MAVSDK should handle any MAVLink input, whether
malformed, unexpected, out of order, or malicious, without memory corruption,
crashes, or hangs.

MAVSDK does not implement MAVLink message signing yet, so MAVLink as used by
MAVSDK has no authentication: any party able to send packets on the link can
impersonate a vehicle, a ground station or a companion computer. Spoofed but
well-formed MAVLink messages being accepted as genuine is therefore expected
behavior and not a vulnerability in MAVSDK. Secure the link itself (e.g. an
encrypted radio or VPN) if this matters for your use case.

### Not secured: mavsdk_server and gRPC

`mavsdk_server` (the gRPC server used by the Python, Java/Kotlin, Swift and
other wrappers) is **not secured**:

- it uses unencrypted, unauthenticated gRPC,
- it listens on all interfaces (`0.0.0.0`) by default.

Anyone who can reach the gRPC port has full control over the connected
vehicles. This is a known limitation, not a vulnerability. Run `mavsdk_server`
only on trusted networks, ideally on the same host as the client, and firewall
the gRPC port.

The mavsdk_server/gRPC architecture is likely not the way forward for MAVSDK
and might be deprecated in the future, so reports about its lack of
authentication or encryption will not be treated as vulnerabilities.

## Severity

As a rough guide:

- **High:** breaking a security mechanism MAVSDK provides. Once MAVLink message
  signing is supported, this would be e.g. signing not being enforced or
  verified correctly, allowing unsigned or forged messages to be accepted when
  signing is enabled.
- **Medium:** memory corruption (e.g. buffer overflows) triggered by untrusted
  MAVLink input.
- **Low:** crashes, hangs, or excessive resource use (denial of service)
  triggered by untrusted MAVLink input.

Issues requiring malicious API input, and anything relating to the lack of
security in mavsdk_server/gRPC, are out of scope.
