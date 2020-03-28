import { openDB, deleteDB } from 'idb'

const DATABASE_NAME = 'BAR_ORDERS'
const DATABASE_VERSION = 2
const dbPromise = openDB(DATABASE_NAME, DATABASE_VERSION, {
    upgrade(db) {
        // Create store for grpc health of bottle service 
        const clientStore = db.createObjectStore('client', {
            keypath: 'name',
        })

        // Create store for shots received from bottle service
        const shotStore = db.createObjectStore('shots', {
            keypath: 'id',
            autoIncrement: true,
        })
    }
})

export { dbPromise }
