
# c-server-docker

This project provides a Dockerized C server setup. It allows you to run a C-based server application inside a container for simplified deployment and management.

## Features

- Containerized C server for easy deployment
- Simple configuration and setup
- SSL/TLS support for secure communication
- Hot-reload support for server code changes
- Customizable environment variables for configuration

## Getting Started

### Prerequisites

- Docker installed on your system

### Building and Running

1. Clone the repository:
    ```bash
    git clone https://github.com/yourusername/c-server-docker.git
    cd c-server-docker
    ```
2. Build the Docker image:
    ```bash
    docker build -t c-server .
    ```
3. Run the container (using environment variables for configuration):
    ```bash
    docker run -d \
      -p 443:443 \
      -e SERVER_PORT=443 \
      -e CERT_PATH=/certs/server.crt \
      -e KEY_PATH=/certs/server.key \
      -v $(pwd)/certs:/certs:ro \
      c-server
    ```

## Adding Certificates

To enable SSL/TLS, add your certificate and key files:

1. Place your certificate (`server.crt`) and private key (`server.key`) in the `certs/` directory.
2. Ensure the Docker run command mounts the `certs/` directory and sets the correct environment variables.
3. Rebuild the Docker image if you update any server code:
    ```bash
    docker build -t c-server .
    ```
4. Start the container as shown above.

**Note:** Ensure your certificate and key files are valid and match your domain/server requirements.

## Configuration

You can customize the server using environment variables:

- `SERVER_PORT`: Port for the server to listen on (default: 443)
- `CERT_PATH`: Path to the SSL certificate file
- `KEY_PATH`: Path to the SSL private key file

## License

This project is licensed under the MIT License.
