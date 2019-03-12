var async = require('async')

var self = {}

self.insert = function( dataSize, done ) {
    var run = []
    console.log('mariadb insert start')
    async.series(run, function(err,data){
        console.log('mariadb insert end')
        if(done)
            done();
    })
}

module.exports = function(type,dataSize,done){
    return self[type](dataSize,done);
}
