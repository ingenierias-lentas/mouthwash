import React, { useEffect } from 'react'

import { useBottleClient, BottleServiceAvailability } from './useBottleClient'

const BarContainer = (props) => {
    const { servingStatus, reconnectToBottleService, checkBottleServiceConnection, takeShot } = useBottleClient()

    // Should go through reconnection loop if service is unavailable
    useEffect(reconnectToBottleService, [servingStatus])

    return (
        <div>
            {props.children}
        </div>
    )
}

export { BarContainer }
