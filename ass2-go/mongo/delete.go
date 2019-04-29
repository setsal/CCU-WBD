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

    var delRes *mongo.DeleteResult
	//  Start
	start := time.Now()
    if delRes, err = collection.DeleteMany(ctx, bson.M{"postid": bson.M{"$lte": 110000}}); err != nil {
        log.Fatal(err)
    }
    fmt.Printf("Delete %d record\n", delRes.DeletedCount)
	end := time.Now()

	fmt.Println("MonogoDB delete total time:",end.Sub(start).Seconds())

}
