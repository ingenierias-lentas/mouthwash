/**
 * @fileoverview gRPC-Web generated client stub for mouthwash
 * @enhanceable
 * @public
 */

// GENERATED CODE -- DO NOT EDIT!



const grpc = {};
grpc.web = require('grpc-web');

const proto = {};
proto.mouthwash = require('./bottle_pb.js');

/**
 * @param {string} hostname
 * @param {?Object} credentials
 * @param {?Object} options
 * @constructor
 * @struct
 * @final
 */
proto.mouthwash.BottleClient =
    function(hostname, credentials, options) {
  if (!options) options = {};
  options['format'] = 'text';

  /**
   * @private @const {!grpc.web.GrpcWebClientBase} The client
   */
  this.client_ = new grpc.web.GrpcWebClientBase(options);

  /**
   * @private @const {string} The hostname
   */
  this.hostname_ = hostname;

};


/**
 * @param {string} hostname
 * @param {?Object} credentials
 * @param {?Object} options
 * @constructor
 * @struct
 * @final
 */
proto.mouthwash.BottlePromiseClient =
    function(hostname, credentials, options) {
  if (!options) options = {};
  options['format'] = 'text';

  /**
   * @private @const {!grpc.web.GrpcWebClientBase} The client
   */
  this.client_ = new grpc.web.GrpcWebClientBase(options);

  /**
   * @private @const {string} The hostname
   */
  this.hostname_ = hostname;

};


/**
 * @const
 * @type {!grpc.web.MethodDescriptor<
 *   !proto.mouthwash.ShotCall,
 *   !proto.mouthwash.Shot>}
 */
const methodDescriptor_Bottle_TakeShot = new grpc.web.MethodDescriptor(
  '/mouthwash.Bottle/TakeShot',
  grpc.web.MethodType.UNARY,
  proto.mouthwash.ShotCall,
  proto.mouthwash.Shot,
  /**
   * @param {!proto.mouthwash.ShotCall} request
   * @return {!Uint8Array}
   */
  function(request) {
    return request.serializeBinary();
  },
  proto.mouthwash.Shot.deserializeBinary
);


/**
 * @const
 * @type {!grpc.web.AbstractClientBase.MethodInfo<
 *   !proto.mouthwash.ShotCall,
 *   !proto.mouthwash.Shot>}
 */
const methodInfo_Bottle_TakeShot = new grpc.web.AbstractClientBase.MethodInfo(
  proto.mouthwash.Shot,
  /**
   * @param {!proto.mouthwash.ShotCall} request
   * @return {!Uint8Array}
   */
  function(request) {
    return request.serializeBinary();
  },
  proto.mouthwash.Shot.deserializeBinary
);


/**
 * @param {!proto.mouthwash.ShotCall} request The
 *     request proto
 * @param {?Object<string, string>} metadata User defined
 *     call metadata
 * @param {function(?grpc.web.Error, ?proto.mouthwash.Shot)}
 *     callback The callback function(error, response)
 * @return {!grpc.web.ClientReadableStream<!proto.mouthwash.Shot>|undefined}
 *     The XHR Node Readable Stream
 */
proto.mouthwash.BottleClient.prototype.takeShot =
    function(request, metadata, callback) {
  return this.client_.rpcCall(this.hostname_ +
      '/mouthwash.Bottle/TakeShot',
      request,
      metadata || {},
      methodDescriptor_Bottle_TakeShot,
      callback);
};


/**
 * @param {!proto.mouthwash.ShotCall} request The
 *     request proto
 * @param {?Object<string, string>} metadata User defined
 *     call metadata
 * @return {!Promise<!proto.mouthwash.Shot>}
 *     A native promise that resolves to the response
 */
proto.mouthwash.BottlePromiseClient.prototype.takeShot =
    function(request, metadata) {
  return this.client_.unaryCall(this.hostname_ +
      '/mouthwash.Bottle/TakeShot',
      request,
      metadata || {},
      methodDescriptor_Bottle_TakeShot);
};


module.exports = proto.mouthwash;

