from concurrent import futures
import logging
import time

import grpc

import mix_pb2
import mix_pb2_grpc
import health_pb2
import health_pb2_grpc
from google.protobuf import empty_pb2

class Mixer(mix_pb2_grpc.MixerServicer):
    def MixANewOne(self, request, context):
        return mix_pb2.Ingredients(instrument=0, paramValues=[])
    
    def ShotReview(self, request, context):
        return empty_pb2.Empty()


class Health(health_pb2_grpc.HealthServicer):
    def Check(self, request, context):
        servingStatus = health_pb2.HealthCheckResponse.ServingStatus.SERVING
        return health_pb2.HealthCheckResponse(status=servingStatus)

    def Watch(self, request, context):
        servingStatus = health_pb2.HealthCheckResponse.ServingStatus.SERVING
        response = health_pb2.HealthCheckResponse(status=servingStatus)
        while(True):
            time.sleep(1)
            yield response


def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    mix_pb2_grpc.add_MixerServicer_to_server(Mixer(), server)
    health_pb2_grpc.add_HealthServicer_to_server(Health(), server)
    server.add_insecure_port('[::]:60061')
    server.start()
    print("Listening on 0.0.0.0:60061")
    server.wait_for_termination()


if __name__ == '__main__':
    logging.basicConfig()
    serve()
