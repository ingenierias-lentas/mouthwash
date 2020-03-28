import React, { useEffect } from 'react'

import ReactDOM from 'react-dom'

import { BarProvider } from './BarContext'
import { useBottleClient, BottleServiceAvailability } from './useBottleClient'

import { BarContainer } from './BarContainer'
import * as serviceWorker from './serviceWorker'

const Index = () => {
    const { servingStatus, reconnectToBottleService, checkBottleServiceConnection, takeShot } = useBottleClient()

    return (
        <BarProvider>
            <BarContainer>
                <p>Here's a button</p>
                <button onClick={takeShot}>
                    Get a new one!
                </button>
            </BarContainer>
        </BarProvider>
    )
}

ReactDOM.render(
    <Index />,
    document.getElementById('index')
);

serviceWorker.unregister()
