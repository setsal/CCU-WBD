var async = require('async')

var self = {}

self.insert = function( dataSize, done ) {
    var run = []
    console.log('mongo insert start')
    async.series(run, function(err,data){
        console.log('mongo insert end')
        if(done)
            done();
    })

}

module.exports = function(type,dataSize,done){
    return self[type](dataSize,done);
}
