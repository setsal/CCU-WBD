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
    "go.mongodb.org/mongo-driver/mongo"
    "go.mongodb.org/mongo-driver/mongo/options"
    // "go.mongodb.org/mongo-driver/bson"
	// "reflect"
)

type Post struct {
    Postid      int     `bson:postid`
    Url         string      `bson:"url"`
    Title       string      `bson:"title"`
    Content     string      `bson:"content"`
    ViewCount   int         `bson:"viewCount"`
    Res         int         `bson:"res"`
    Duration    int         `bson:"duration"`
}


func init(){

}

func main(){

    err := godotenv.Load("../.env")
    if err != nil {
      log.Fatal("Error loading .env file")
    }

	// Database connection
    ctx, _ := context.WithTimeout(context.Background(), 50*time.Second)
    client, err := mongo.Connect(ctx, options.Client().ApplyURI("mongodb://127.0.0.1:27017"))
    if err != nil {
        log.Fatal(err)
    }
    collection := client.Database("test").Collection("test")


	// File reading pointer
    f, err := os.Open("../../ass3/data/youtube.rec.1000000")
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
    pcount := 1
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
            post.Postid = pcount
            if result, err := collection.InsertOne(ctx, post); err == nil {
                fmt.Println(result)
                fmt.Println(pcount)
                pcount++
            } else {
                log.Fatal(err)
            }
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
	fmt.Println("MonogoDB insert total time:",end.Sub(start).Seconds())

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
