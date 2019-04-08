require('dotenv').config();
const mysql = require('mysql')
const pool = mysql.createPool({
    connectionLimit : 10,
    host            : process.env.MYSQL_DB_HOST,
    user            : process.env.MYSQL_DB_USER,
    password        : process.env.MYSQL_DB_PASS,
    database        : process.env.MYSQL_DB_NAME
})

module.exports=function (sql) {
    return new Promise(function (resolve, reject) {
        pool.getConnection(function(err,conn){
            console.log('123')
            if(err){
                reject(err);
            }else{
                conn.query(sql,function(err,rows,fields){
                    //釋放連接
                    conn.release();
                    //傳遞Promise回調對象
                    resolve({"err":err,
                            "rows":rows,
                            "fields":fields});
                });
            }
        });
    });
};
