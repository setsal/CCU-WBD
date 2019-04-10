package main

import (
    "context"
    "bufio"
    "fmt"
    "log"
    "os"
	"regexp"
	"strings"
	"time"
    "strconv"
	"github.com/joho/godotenv"
    "github.com/olivere/elastic"
	// "reflect"
)

var client *elastic.Client
var host = "http://localhost:9200"

type Post struct {
    Url         string      `json:"url"`
    Title       string      `json:"title"`
    Content     string      `json:"content"`
    ViewCount   int         `json:"viewCount"`
    Res         int         `json:"res"`
    Duration    int         `json:"duration"`
}


func init(){

    // Database connection
    var err error
    client, err = elastic.NewClient()
    if err != nil {
        log.Fatal(err)
    }
    info, code, err := client.Ping(host).Do(context.Background())
    if err != nil {
        log.Fatal(err)
    }
    fmt.Printf("Elasticsearch returned with code %d and version %s\n", code, info.Version.Number)

    // Delete the index again
	_, err = client.DeleteIndex("test").Do(context.Background())
	if err != nil {
		// Handle error
		panic(err)
    }


    // Create an index
    _, err = client.CreateIndex("test").Do(context.Background())
    if err != nil {
    	// Handle error
    	panic(err)
    }


}

func main(){

    //dot env file load
    err := godotenv.Load()
    if err != nil {
      log.Fatal("Error loading .env file")
    }

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
    postCounter := 1

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
			insert(post, postCounter)
            postCounter++
			// Reset the Counter
			count = 0

		} else if ( len(strings.Split(line, ":")) == 1 ) {
			// Split the data but it null
			count++

		} else {
			// Split data with each tag
			switch count {
				case 0:
					post.Url = strings.Split(line, "@url:")[1]
					break;
				case 1:
					post.Title = strings.Split(line, "@title:")[1]
					break;
				case 2:
					post.Content = strings.Split(line, "@content:")[1]
					break;
				case 3:
					post.ViewCount, _ = strconv.Atoi(strings.Split(line, "@viewCount:")[1])
					break;
				case 4:
					post.Res, _ = strconv.Atoi(strings.Split(line, "@res:")[1])
					break
				case 5:
					post.Duration, _ = strconv.Atoi(strings.Split(line, "@duration:")[1])
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

func insert(data Post, cnt int) {
    id := strconv.Itoa(cnt);
    res, err := client.Index().
                    Index("test").
                    Type("post").
                    Id(id).
                    BodyJson(data).
                    Refresh("wait_for").
                    Do(context.Background())
    if err != nil {
        log.Fatal(err)
    }
    fmt.Printf("Indexed post %s to index %s, type %s\n", res.Id, res.Index, res.Type)
}
