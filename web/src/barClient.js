const { Shot, ShotCall } = require('./bottle_pb')
const { BottleClient } = require('./bottle_grpc_web_pb')
const { HealthCheckRequest, HealthCheckResponse } = require('./health_pb')
const { HealthClient } = require('./health_grpc_web_pb')

var hc, bc
const BarClient = () => {
    function GetHealthClient() {
        if (!hc) { return ResetHealthClient() }

        return hc
    }

    function ResetHealthClient() {
        hc = new HealthClient('http://0.0.0.0:50050')        
        return hc
    }

    function GetBottleClient() {
        if (!bc) { return ResetBottleClient() }

        return bc
    }

    function ResetBottleClient() {
        bc = new BottleClient('http://0.0.0.0:50050')        
        return bc
    }

    return {
        GetHealthClient,
        ResetHealthClient,
        GetBottleClient,
        ResetBottleClient,
    }
}

module.exports = { BarClient } 
