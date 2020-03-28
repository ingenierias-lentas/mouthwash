protoc -Iprotos \
    --js_out=import_style=commonjs:./web/src \
    --grpc-web_out=import_style=commonjs,mode=grpcwebtext:./web/src \
    protos/bottle.proto protos/health.proto
