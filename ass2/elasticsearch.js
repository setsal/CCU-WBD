var async = require('async')

var self = {}

self.insert = function( dataSize, done ) {
    console.log('elasticsearch insert start')
    var run = []
    async.series(run, function(err,data){
        console.log('elasticsearch insert end')
        if(done)
            done();
    })
}
module.exports = function(type,dataSize,done){
    return self[type](dataSize,done);
}
