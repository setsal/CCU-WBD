package main

import (
    "context"
    "fmt"
    "log"
	"time"
	"github.com/joho/godotenv"
    "go.mongodb.org/mongo-driver/mongo"
    "go.mongodb.org/mongo-driver/bson"
    "go.mongodb.org/mongo-driver/mongo/options"
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
    ctx, _ := context.WithTimeout(context.Background(), 10*time.Second)
    client, err := mongo.Connect(ctx, options.Client().ApplyURI("mongodb://127.0.0.1:27017"))
    if err != nil {
        log.Fatal(err)
    }
    collection := client.Database("test").Collection("test")



    var post Post
    var postArray []Post
    var cursor *mongo.Cursor


	//  Start
	start := time.Now()
    if cursor, err = collection.Find(ctx, bson.M{"content": bson.M{"$regex": ".sport."}}); err != nil {
        log.Fatal(err)
    }

    if err = cursor.Err(); err != nil {
        log.Fatal(err)
    }
    defer cursor.Close(context.Background())
    for cursor.Next(context.Background()) {
        if err = cursor.Decode(&post); err != nil {
            log.Fatal(err)
        }
        postArray = append(postArray, post)
    }
    for _, v := range postArray {
        fmt.Println(v)
    }

	end := time.Now()

	fmt.Println("MonogoDB search total time:",end.Sub(start).Seconds())


}
