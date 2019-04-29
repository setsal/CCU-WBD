require('dotenv').config();
var async = require('async');
var fs = require('fs');
var readline = require('readline');
var self = {}
var mysql = require('mysql');
// var pool  = mysql.createPool({
//     connectionLimit : 10,
//     host            : process.env.MYSQL_DB_HOST,
//     user            : process.env.MYSQL_DB_USER,
//     password        : process.env.MYSQL_DB_PASS,
//     database        : process.env.MYSQL_DB_NAME
// });
var connection = mysql.createConnection({
    host            : process.env.MYSQL_DB_HOST,
    user            : process.env.MYSQL_DB_USER,
    password        : process.env.MYSQL_DB_PASS,
    database        : process.env.MYSQL_DB_NAME
});
connection.connect();

self.initial = function( dataSize, done ) {
    var run = []

    run.push(function(callback){
        pool.getConnection(function(err, connection) {
            if (err) throw err; // not connected!
            var sql = "DROP TABLE test";
            connection.query(sql, function (error, results, fields) {
                // When done with the connection, release it.
                connection.release();
                // Handle error after the release.
                if (error) throw error;

                callback();
            });
        });
    });

	run.push(function(callback){
        pool.getConnection(function(err, connection) {
            if (err) throw err; // not connected!
            var sql = "CREATE TABLE test ( id int(11) NOT NULL AUTO_INCREMENT, url VARCHAR(100), title VARCHAR(255), content VARCHAR(255), viewCount INT, res INT, duration INT, PRIMARY KEY(id) )";
            connection.query(sql, function (error, results, fields) {
                // When done with the connection, release it.
                connection.release();
                // Handle error after the release.
                if (error) throw error;

                callback();
            });
        });
    })

    console.log('mariadb initial');
    async.series(run, function(err,data){
        console.log('mariadb initial success');
        if(done)
            done();
    })
}



self.insert = function( dataSize, done ) {
    var run = [];
    var inputStream = fs.createReadStream('./data/youtube.rec.50');
    // 將讀取資料流導入 Readline 進行處理
    const lineReader = readline.createInterface({ input: inputStream });
    var count = 0;
    var array = [];

    lineReader.on('line', (line) => {
            if ( /^\s*$/.test(line) || line[0] != '@' ){
                return
            }
            else if ( line == '@' ) {
                var post = {url: array[0], title: array[1], content: array[2], viewCount: array[3], res: array[4], duration: array[5] }
                // var sql = 'INSERT INTO test ( url, title, content, viewCount, res, duration )' + ' VALUES (`' + array[0] + '`, `' + array[1] + '`, `' + array[2] + '`, `' + array[3] + '`, `' + array[4] + '`, `' + array[5] + '`);'

                connection.query('INSERT INTO test SET ?', post, function (error, results, fields) {
                    console.log('123')
                });
                return;
            }
            else if (   line.trim().split(':')[1] == "" ) {
                array[count] = null;
                count++;
            }
            else {
                switch(count) {
                    case 0:
                        array[count] = line.match(/(@url:)(.+)$/)[2];
                        break;
                    case 1:
                        array[count] = line.match(/(@title:)(.+)$/)[2];
                        break;
                    case 2:
                        array[count] = line.match(/(@content:)(.+)$/)[2];
                        break;
                    case 3:
                        array[count] = line.match(/(@viewCount:)(.+)$/)[2];
                        break;
                    case 4:
                        array[count] = line.match(/(@res:)(.+)$/)[2];
                        break
                    case 5:
                        array[count] = line.match(/(@duration:)(.+)$/)[2];
                        break;
                }
                count++;
            }
        });

        lineReader.on('close', (line) => {
            console.log('close')
        });

    // callback()
    //
    // console.time('mariadb insert');
    // async.series(run, function(err,data){
    //     console.timeEnd('mariadb insert');
    //     connection.end();
    //     if(done) done();
    // })
}




module.exports = function(type,dataSize,done){
    return self[type](dataSize,done);
}
