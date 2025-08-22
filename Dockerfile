# Use an official Ubuntu image with build essentials and OpenSSL for HTTPS/TCP support
FROM ubuntu:24.04

# Install build tools and libraries for HTTPS/TCP (gcc, cmake, openssl, etc.)
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    openssl \
    libssl-dev \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy the source code and build script
COPY . .

# Make sure build.sh is executable
RUN chmod +x build.sh

# Build the project
RUN ./build.sh

# Default command: run the executable
CMD ["./build/MyCProject"]