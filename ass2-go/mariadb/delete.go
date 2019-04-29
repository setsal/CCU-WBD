package main

import (
    "fmt"
    "log"
    "os"
	"time"
	"github.com/joho/godotenv"
	"database/sql"
	_ "github.com/go-sql-driver/mysql"
	// "reflect"
)

var db *sql.DB

func init(){

	err := godotenv.Load("../.env")
    if err != nil {
      log.Fatal("Error loading .env file")
    }

	// Database connection
	dsn := os.Getenv("MYSQL_DB_USER") + ":" + os.Getenv("MYSQL_DB_PASS") + "@/" + os.Getenv("MYSQL_DB_NAME")
	db, _ = sql.Open("mysql", dsn)
}

func main(){
	//  Start
	start := time.Now()
    delete()
	end := time.Now()
	fmt.Println("MariaDB delete total time:",end.Sub(start).Seconds())
}

func delete() {

    // Delete 1% data
	_, err := db.Exec( "DELETE FROM test WHERE id >= 500000 and id < 999999" )

    /* Delete 10% data
	_, err := db.Exec(
		"DELETE FROM test WHERE id >= 1 and id < 100000"
	)
    */

	if err != nil {
        //ignore the error
		// log.Fatal(err)
	}
}
