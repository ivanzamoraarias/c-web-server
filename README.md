
# c-server-docker

This project provides a Dockerized C server setup for easy deployment and management. The server supports SSL/TLS, hot-reload, and flexible configuration via environment variables.

## Features

- Containerized C server for streamlined deployment
- Simple configuration and setup
- SSL/TLS support for secure communication
- Hot-reload support for server code changes
- Customizable environment variables

## Getting Started

### Prerequisites

- Docker (latest version recommended)

### Building and Running

1. Clone the repository:
    ```bash
    git clone https://github.com/yourusername/c-server-docker.git
    cd c-server-docker
    ```
2. Build the Docker image:
    ```bash
    docker build -t c-server-docker .
    ```
3. Run the container with your configuration:
    ```bash
    docker run -d \
      -p 443:443 \
      c-server-docker
    ```

## SSL/TLS Certificates

To enable SSL/TLS:

1. Place your certificate (`server.crt`) and private key (`server.key`) in the `certs/` directory.
2. Ensure the Docker run command mounts the `certs/` directory and sets the correct environment variables.
3. Rebuild the Docker image if you update server code:
    ```bash
    docker build -t c-server-docker .
    ```
4. Start the container as shown above.

**Note:** Use valid certificate and key files matching your domain/server.

## Configuration

Customize the server using environment variables:

- `SERVER_PORT`: Port for the server to listen on (default: 443)
- `CERT_PATH`: Path to the SSL certificate file (default: `server.crt`)
- `KEY_PATH`: Path to the SSL private key file (default: `server.key`)

## Hot-Reload Support

To enable hot-reload for server code changes, rebuild the Docker image and restart the container after making changes.

## License

This project is licensed under the MIT License.

