var async = require('async')

var dataSize = 1000

async.series([
    function(callback) {
        require('./mariadb')( 'insert', dataSize, callback )
    },
    function(callback) {
        require('./mongo')( 'insert', dataSize, callback )
    },
    function(callback) {
        require('./elasticsearch')( 'insert', dataSize, callback )
    }

],function() {
    process.exit()
})
