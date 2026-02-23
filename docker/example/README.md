# MAVSDK Server Docker Example

This example demonstrates how to build and run MAVSDK Server using Docker Compose.

## Quick Start

1. **Copy the environment template**:
   ```bash
   cp .env.example .env
   ```

2. **Configure the MAVLink connection** (edit `.env`):
   ```bash
   MAV_CONNECTION=udp://:14540
   ```

3. **Build and run**:
   ```bash
   docker compose up --build
   ```

### Build Arguments

The Dockerfile supports the following build arguments:

- **MAVLINK_DIALECT**: Specify a custom MAVLink dialect
- **BUILD_THREADS**: Number of parallel build threads

## Usage Examples

### Run with custom connection

```bash
MAV_CONNECTION=udp://192.168.1.100:14550 docker compose up
```

## Network Configuration

The service uses `host` network mode by default, which allows direct access to network interfaces. This is useful for:
- Discovering drones on the local network
- Binding to specific ports without port mapping

To use standard bridge networking instead, modify the `compose.yaml`:

```yaml
services:
  mavsdk:
    ports:
      - "50051:50051"  # gRPC port
      - "14540:14540/udp"  # MAVLink UDP port
    # Remove or comment out:
    # network_mode: "host"
```