import { useContext } from 'react'
import { BarContext } from './BarContext'
import { Shot, ShotCall } from './bottle_pb'
//import { BottleClient } from './bottle_grpc_web_pb'
import { HealthCheckRequest, HealthCheckResponse } from './health_pb'
//import { HealthClient } from './health_grpc_web_pb'
import { BarClient } from './barClient'

import { dbPromise } from './barDb'

const BottleServiceAvailability = {
    UNAVAILABLE : 0,
    AVAILABLE : 1
}

const { GetHealthClient, ResetHealthClient, GetBottleClient, ResetBottleClient } = BarClient()

const useBottleClient = () => {
    const [state, setState] = useContext(BarContext)

    function reconnectToBottleService() {
        // Check connection before connecting and setting state
        checkBottleServiceConnection(async function(servingStatus) {
            // Only set bottle service to IndexedDB if service is active
            if (servingStatus === HealthCheckResponse.ServingStatus.Serving) {
                // Set bottle service client if unset
                let bottleService = GetBottleClient()
                if (!bottleService) {
                    bottleService = ResetBottleClient() 
                }
                
                // Update state to include current state of bottle service
                // If availability already set, don't need to set it again
                let bottleServiceState 
                if (state.bottleServiceAvailability !== BottleServiceAvailability.AVAILABLE) {
                    bottleServiceState = {
                        bottleService,
                        bottleServiceAvailability: BottleServiceAvailability.AVAILABLE
                    }
                } else {
                    bottleServiceState = {
                        bottleService
                    }
                }
                setState(state => ({
                    ...state,
                    ...bottleServiceState,
                }))
            }
        })
    }

    function checkBottleServiceConnection(cb) {
        async function doCheckBottleServiceConnection(cb) {
            const db = await dbPromise

            let servingStatus
            // Setup health service in IndexedDB
            let healthService = GetHealthClient()
            if (!healthService) {
                healthService = ResetHealthClient()
            }

            // Setup health service request
            let req = new HealthCheckRequest()
            req.setService('BottleService')
            healthService.check(req, {}, function(err, res) {
                if (err) {
                    servingStatus = HealthCheckResponse.ServingStatus.Unknown
                } else {
                    servingStatus = res.ServingStatus
                }

                // Relay bottle service status to bottle client state
                if (servingStatus && (servingStatus !== state.servingStatus)) {
                    setState(state => ({
                        ...state,
                        servingStatus
                    }))
                }

                // Relay serving status to relevant callback
                cb(servingStatus)
            })
        }
        doCheckBottleServiceConnection(cb)
    }

    function takeShot() {
        async function doTakeShot() {
            const db = await dbPromise
            let bottleService = await db.get('client', 'bottle')
            // Only take new shot if bottle service exists currently
            if (!bottleService) {
                setState(state => ({
                    ...state,
                    bottleServiceAvailability: BottleServiceAvailability.UNAVAILABLE
                }))
            } else {
                checkBottleServiceConnection(function(servingStatus) {
                    // Only take new shot if bottle service is currently serving
                    if (servingStatus === HealthCheckResponse.ServingStatus.Serving) {
                        // Set bottle service availability to available if not set
                        if (!state.bottleServiceAvailability) {
                            setState(state => ({
                                ...state,
                                bottleServiceAvailability: BottleServiceAvailability.AVAILABLE
                            }))
                        }

                        // Take shot
                        let req = new ShotCall()
                        req.setCommand(0)
                        bottleService.TakeShot(req, {}, (err, resp) => {
                            if (err) {
                                //console.log(err)
                            } else {
                                //console.log(resp)
                            }
                        })
                    }
                })
            }
        }
        doTakeShot()
    }

    return {
        bottleServiceAvailability: state.bottleServiceAvailability,
        servingStatus: state.servingStatus,
        reconnectToBottleService,
        checkBottleServiceConnection,
        takeShot,
    }
}

export { useBottleClient, BottleServiceAvailability }
