python -m grpc_tools.protoc \
    --python_out ./python \
    --grpc_python_out ./python \
    -Iprotos \
    protos/mix.proto protos/health.proto
