var async = require('async')
var fs = require('fs')
var readline = require('readline');
var self = {}

self.insert = function( dataSize, done ) {
    var run = []

	run.push(function(callback){
        var inputStream = fs.createReadStream('./data/youtube.rec.100000');
        // 將讀取資料流導入 Readline 進行處理
        var lineReader = readline.createInterface({ input: inputStream });
        var count = 6
        var testfunction = function() {
            var count = 6
            lineReader.on('line', function(line) {
                console.log(line)
                count = count - 1;
                if ( count == 0 ) {
                    return;
                }
            })
        }
        testfunction()
        // lineReader.on('line', function(line) {
        //     // 取得一行行結果
        //     if ( /^\s*$/.test(line) ){
        //         return;
        //     }
        //     if (line.match(/(@url:)(.+)$/)){
        //         console.log
        //     }
        //     else if ( line.match(/(@title:)(.+)$/) ){
        //
        //     }
        //     else if ( line.match(/(@content:)(.+)$/) ){
        //
        //     }
        //     else if ( line.match(/(@viewCount:)(.+)$/) ){
        //
        //     }
        //     else if ( line.match(/(@res:)(.+)$/) ){
        //
        //     }
        //     else if ( line.match(/(@duration:)(.+)$/) ){
        //
        //     }
        //     console.log(line);
    });

    console.time('mariadb insert')
    async.series(run, function(err,data){
        console.timeEnd('mariadb insert')
        if(done)
            done();
    })
}

module.exports = function(type,dataSize,done){
    return self[type](dataSize,done);
}
