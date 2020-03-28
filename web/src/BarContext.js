import React, { useState } from 'react'

const BarContext = React.createContext([{}, () => {}])

const BarProvider = (props) => {
    const [state, setState] = useState({})
    return (
        <BarContext.Provider value={[state, setState]}>
            {props.children}
        </BarContext.Provider>
    )
}

export { BarContext, BarProvider }
