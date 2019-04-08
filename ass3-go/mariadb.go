// You can edit this code!
// Click here and start typing.
package main

import (
    "bufio"
    "fmt"
    "log"
    "os"
	"regexp"
	"strings"
	// "reflect"
)

func main() {
	// fptr := flag.String("fpath", "../ass3/data/youtube.rec.50", "file path to read from")
    // flag.Parse()

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
	r, _ := regexp.Compile(`^\s*$`)

	//  Start
    for scanner.Scan() {

		line := scanner.Text()

		if ( r.MatchString(line) || line[0:1] != "@" ){
			// Line format invalid
			fmt.Println("Line format invalid")

		} else if ( line == "@" ) {
			// Insert Data to Database
			fmt.Println("Insert Data")
			// Reset the Counter
			count = 0

		} else if ( len(strings.Split(line, ":")) == 1 ) {
			// Split the data but it null
			fmt.Println("Data Null")
			count++

		} else {
			// Split data with each tag
			fmt.Println("Split the data with each other")
			count++
		}
    }

	// Error in reading file
    err = scanner.Err()
    if err != nil {
        log.Fatal(err)
    }

	_ = r
}
