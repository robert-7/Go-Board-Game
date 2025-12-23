FROM ubuntu:24.04

# Avoid prompts from apt
ENV DEBIAN_FRONTEND=noninteractive

# Install base build tools
RUN apt-get update && apt-get install -y \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# Copy dependency script
COPY scripts/install_dependencies.sh /tmp/install_dependencies.sh

# Run dependency script
RUN /tmp/install_dependencies.sh && \
    rm -rf /var/lib/apt/lists/* /tmp/install_dependencies.sh

# Set working directory
WORKDIR /app

# Copy project files
COPY . .

# Build the project
RUN mkdir -p build && cd build && \
    cmake .. && \
    make

# Set the entrypoint to the built executable
CMD ["./build/main"]
