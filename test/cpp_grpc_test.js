// grpcc -i --proto ../protos/health.proto --address 0.0.0.0:50051 -x cpp_grpc_test.js
client.watch({ service: "bottle" })
    .on('data', streamReply)
    .on('status', streamReply)
