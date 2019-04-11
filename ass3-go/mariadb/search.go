package main

import (
    "fmt"
    "os"
    "log"
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
    search()
	end := time.Now()
	fmt.Println("MariaDB search total time:",end.Sub(start).Seconds())
}

func search() {
    cnt := 1
    var res_id int
    // Search
	rows, err := db.Query( "SELECT id FROM test WHERE title like ?", "%sport%" )
	if err != nil {
		log.Fatal(err)
	}
    for rows.Next() {
        if err := rows.Scan(&res_id); err != nil {
            log.Fatal(err)
        }
        fmt.Printf("%d\n", res_id)
        cnt++
    }
    if err := rows.Err(); err != nil {
        log.Fatal(err)
    }
    fmt.Println(cnt)
}
