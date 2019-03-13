require('dotenv').config()
var async = require('async')
var fs = require('fs')
var readline = require('readline');
var self = {}
var mysql = require('mysql');
var pool  = mysql.createPool({
    connectionLimit : 10,
    host            : process.env.MYSQL_DB_HOST,
    user            : process.env.MYSQL_DB_USER,
    password        : process.env.MYSQL_DB_PASS,
    database        : process.env.MYSQL_DB_NAME
});


function mysqlInsert(data){
    pool.getConnection(function(err, connection) {
        if (err) throw err; // not connected!

        // Use the connection
        var post = {url: data[0], title: data[1], content: data[2], viewCount: data[3], res: data[4], duration: data[5] }
        connection.query('INSERT INTO posts SET ?', post, function (error, results, fields) {
            // When done with the connection, release it.
            connection.release();
            // Handle error after the release.
            if (error) throw error;
        });
    });
}


self.insert = function( dataSize, done ) {
    var run = []

	run.push(function(callback){
        var inputStream = fs.createReadStream('./data/youtube.rec.100000');
        // 將讀取資料流導入 Readline 進行處理
        var lineReader = readline.createInterface({ input: inputStream });
        var count = 0
        var array = []

        lineReader.on('line', function(line) {
            if ( /^\s*$/.test(line) || line[0] != '@' ){
                return
            }
            else if ( line == '@' ) {
                mysqlInsert(array)
                count = 0
                return
            }
            else if (   line.trim().split(':')[1] == "" ) {
                array[count] = null
                count++
            }
            else {
                switch(count) {
                    case 0:
                        array[count] = line.match(/(@url:)(.+)$/)[2]
                        break
                    case 1:
                        array[count] = line.match(/(@title:)(.+)$/)[2]
                        break
                    case 2:
                        array[count] = line.match(/(@content:)(.+)$/)[2]
                        break
                    case 3:
                        array[count] = line.match(/(@viewCount:)(.+)$/)[2]
                        break
                    case 4:
                        array[count] = line.match(/(@res:)(.+)$/)[2]
                        break
                    case 5:
                        array[count] = line.match(/(@duration:)(.+)$/)[2]
                        break
                }
                count++
            }

            // console.log(line);
        });
    })

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
