package main

import (
    "bufio"
    "fmt"
    "log"
    "os"
	"regexp"
	"strings"
	"time"
    "strconv"
	"github.com/joho/godotenv"
    "github.com/elastic/go-elasticsearch"
	// "reflect"
)

type Post struct {
    url         string
    title       string
    content     string
    viewCount   int
    res         int
    duration    int
}


func init(){

}

func main(){

    err := godotenv.Load()
    if err != nil {
      log.Fatal("Error loading .env file")
    }

	// Database connection
    es, _ := elasticsearch.NewDefaultClient()
    log.Println(es.Info())


	// File reading pointer
    f, err := os.Open("../ass3/data/youtube.rec.100000")
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
	var post Post

	r, _ := regexp.Compile(`^\s*$`)

	//  Start
	start := time.Now()
    for scanner.Scan() {

		line := scanner.Text()

		if ( r.MatchString(line) || line[0:1] != "@" ){
			// Line format invalid
		} else if ( line == "@" ) {
			// Insert Data to Database
			// insert(post)
			// Reset the Counter
			count = 0

		} else if ( len(strings.Split(line, ":")) == 1 ) {
			// Split the data but it null
			count++

		} else {
			// Split data with each tag
			switch count {
				case 0:
					post.url = strings.Split(line, "@url:")[1]
					break;
				case 1:
					post.title = strings.Split(line, "@title:")[1]
					break;
				case 2:
					post.content = strings.Split(line, "@content:")[1]
					break;
				case 3:
					post.viewCount, _ = strconv.Atoi(strings.Split(line, "@viewCount:")[1])
					break;
				case 4:
					post.res, _ = strconv.Atoi(strings.Split(line, "@res:")[1])
					break
				case 5:
					post.duration, _ = strconv.Atoi(strings.Split(line, "@duration:")[1])
					break;
			}
			count++
		}
    }
	end := time.Now()
	fmt.Println("Elasticsearch insert total time:",end.Sub(start).Seconds())

	// Error in reading file
    err = scanner.Err()
    if err != nil {
        log.Fatal(err)
    }

}

// func insert(data Post) {
//     if result, err := collection.InsertOne(ctx, data); err == nil {
//         log.Println(result)
//     } else {
//         log.Fatal(err)
//     }
// }
