package main

import (
    "fmt"
    "log"
    "context"
	"github.com/joho/godotenv"
    "github.com/olivere/elastic"
	// "reflect"
)

var client *elastic.Client
var host = "http://localhost:9200/"

type Post struct {
    Url         string  `json:"url"`
    Title       string  `json:"title"`
    Content     string  `json:"content"`
}

type Tweet struct {
	User    string `json:"user"`
	Message string `json:"message"`
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

    // // Delete the index again
	// _, err = client.CreateIndex("tweets").Do(context.Background())
	// if err != nil {
	// 	// Handle error
	// 	panic(err)
    // }
}

func main(){

    err := godotenv.Load()
    if err != nil {
      log.Fatal("Error loading .env file")
    }

    post := Post{Url: "olivere", Title: "asda", Content: "content"}
    _, err = client.Index().
        Index("test").
        Type("post").
        Id("10").
        BodyJson(post).
        Refresh("wait_for").
        Do(context.Background())
    if err != nil {
        // Handle error
        panic(err)
    }

}
