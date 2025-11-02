# Multi-stage Dockerfile for Assembly Line Simulator
# Stage 1: Build C++ application
FROM ubuntu:22.04 AS cpp-builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    make \
    libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /build

# Copy source files
COPY Makefile .
COPY CMakeLists.txt .
COPY include/ ./include/
COPY src/ ./src/
COPY data/ ./data/
COPY config/ ./config/

# Build C++ application
RUN make release

# Stage 2: Python API
FROM python:3.11-slim

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libsqlite3-0 \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy Python API
COPY api/requirements.txt ./api/
RUN pip install --no-cache-dir -r api/requirements.txt

COPY api/ ./api/

# Copy built C++ executable from builder stage
COPY --from=cpp-builder /build/build/assembly_line ./build/assembly_line

# Copy data files
COPY data/ ./data/
COPY config/ ./config/

# Create database directory
RUN mkdir -p database

# Copy frontend build (build separately or use multi-stage)
# For now, we'll serve it from Python, or build it separately

# Expose ports
EXPOSE 8000

# Set environment variables
ENV PYTHONUNBUFFERED=1
ENV PORT=8000

# Run API server
WORKDIR /app
CMD ["python", "api/main.py"]

