package main

import (
    "context"
    "fmt"
    "log"
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
}

func main(){

    //dot env file load
    err := godotenv.Load("../.env")
    if err != nil {
      log.Fatal("Error loading .env file")
    }


	//  Start
	start := time.Now()
    delete()
	end := time.Now()
	fmt.Println("Elasticsearch search total time:",end.Sub(start).Seconds())


}

func delete() {
    count := 10002
    for count <= 110002 {
        res, err := client.Delete().Index("test").
                                    Type("post").
                                    Id(strconv.Itoa(count)).
                                    Do(context.Background())

        if err != nil {
            log.Fatal(err)
        }
        fmt.Println("delete result %s\n", res.Result)
        count++
    }
}
