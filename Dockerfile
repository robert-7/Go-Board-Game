FROM ubuntu:24.04

# Avoid prompts from apt
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies and game libraries
# Matches dependencies in scripts/install_dependencies.sh
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    mesa-common-dev \
    libglu1-mesa-dev \
    libgl1-mesa-dev \
    freeglut3-dev \
    libglew-dev \
    libdevil-dev \
    git \
    && rm -rf /var/lib/apt/lists/*

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
