# Use an official Ubuntu image with build essentials and OpenSSL for HTTPS/TCP support
FROM ubuntu:24.04

# Install build tools and libraries for HTTPS/TCP (gcc, cmake, openssl, etc.)
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    openssl \
    libssl-dev \
    libcjson-dev \
    ca-certificates \
    libpq-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy all project files into the container
COPY . .

RUN ls -l /app

# Create build directory
RUN mkdir -p build

# Run cmake and make in build directory
#WORKDIR /app/build
RUN cd build && \
    cmake .. && \
    make
#RUN cmake .. && make
# Expose HTTPS port
EXPOSE 8442

# Default command: run the executable
CMD ["./build/CServerDocker"]