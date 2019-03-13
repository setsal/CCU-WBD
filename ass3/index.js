var async = require('async')

var dataSize = 1000

async.series([
    /* mariadb insert */
    function(callback) {
        require('./mariadb')( 'insert', dataSize, callback )
    }
    //
    // /* mongodb insert */
    // function(callback) {
    //     require('./mongo')( 'insert', dataSize, callback )
    // },
    // /* elasticsearch insert */
    // function(callback) {
    //     require('./elasticsearch')( 'insert', dataSize, callback )
    // }

],function() {
    process.exit()
})
