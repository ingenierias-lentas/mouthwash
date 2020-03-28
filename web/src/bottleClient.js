const { Shot, ShotCall } = require('./bottle_pb')
const { BottleClient } = require('./bottle_grpc_web_pb')

const grpc = require('grpc')

var bottleService = new BottleClient(
    'http://localhost:50051',
    grpc.credentials.createInsecure()
)

var req = new ShotCall()
req.setCommand(0)

bottleService.TakeShot(req, {}, (err, resp) => {
    if (err) {
        console.log(err)
    } else {
        console.log(resp)
    }
})
