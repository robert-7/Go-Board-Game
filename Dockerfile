FROM ubuntu:25.10

# Avoid prompts from apt
ENV DEBIAN_FRONTEND=noninteractive

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
