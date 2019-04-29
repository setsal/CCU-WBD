package main

import (
    "bufio"
    "fmt"
    "log"
    "os"
	"regexp"
	"strings"
	"time"
	"github.com/joho/godotenv"
	"database/sql"
	_ "github.com/go-sql-driver/mysql"
	// "reflect"
)

var db *sql.DB

func init(){

	err := godotenv.Load()
    if err != nil {
      log.Fatal("Error loading .env file")
    }

	// Database connection
	dsn := os.Getenv("MYSQL_DB_USER") + ":" + os.Getenv("MYSQL_DB_PASS") + "@/" + os.Getenv("MYSQL_DB_NAME")
	db, _ = sql.Open("mysql", dsn)
}

func main(){

	// File reading pointer
    f, err := os.Open("../ass3/data/youtube.rec.50")
    if err != nil {
        log.Fatal(err)
    }
    defer func() {
        if err = f.Close(); err != nil {
        log.Fatal(err)
    }
    }()

	// Scan the file line by line
    scanner := bufio.NewScanner(f)

	// line counter
	count := 0

	// data array
	var array [6]string

	r, _ := regexp.Compile(`^\s*$`)
	// r_url := regexp.MustCompile("(@url:)(.+)$")

	//  Start
	start := time.Now()
    for scanner.Scan() {

		line := scanner.Text()

		if ( r.MatchString(line) || line[0:1] != "@" ){
			// Line format invalid
		} else if ( line == "@" ) {
			// Insert Data to Database
			fmt.Println("Insert Data")
			insert(array[:])
			// Reset the Counter
			count = 0

		} else if ( len(strings.Split(line, ":")) == 1 ) {
			// Split the data but it null
			count++

		} else {
			// Split data with each tag
			switch count {
				case 0:
					array[0] = strings.Split(line, "@url:")[1]
					break;
				case 1:
					array[1] = strings.Split(line, "@title:")[1]
					break;
				case 2:
					array[2] = strings.Split(line, "@content:")[1]
					break;
				case 3:
					array[3] = strings.Split(line, "@viewCount:")[1]
					break;
				case 4:
					array[4] = strings.Split(line, "@res:")[1]
					break
				case 5:
					array[5] = strings.Split(line, "@duration:")[1]
					break;
			}
			count++
		}
    }
	end := time.Now()
	fmt.Println("MariaDB insert total time:",end.Sub(start).Seconds())

	// Error in reading file
    err = scanner.Err()
    if err != nil {
        log.Fatal(err)
    }


}

func insert(data []string) {
	_, err := db.Exec(
		"INSERT INTO test ( url, title, content, viewCount, res, duration ) VALUES (?, ?, ?, ?, ?, ?)",
		data[0],
		data[1],
		data[2],
		data[3],
		data[4],
		data[5],
	)
	if err != nil {
		// log.Fatal(err)
	}
}
