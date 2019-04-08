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

	// data array
	var array [6]string

	r, _ := regexp.Compile(`^\s*$`)
	// r_url := regexp.MustCompile("(@url:)(.+)$")

	//  Start
    for scanner.Scan() {

		line := scanner.Text()

		if ( r.MatchString(line) || line[0:1] != "@" ){
			// Line format invalid
		} else if ( line == "@" ) {
			// Insert Data to Database
			fmt.Println("Insert Data")
			// Reset the Counter
			count = 0

		} else if ( len(strings.Split(line, ":")) == 1 ) {
			// Split the data but it null
			count++

		} else {
			// Split data with each tag
			// fmt.Println("Split the data with each other")
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

	// Error in reading file
    err = scanner.Err()
    if err != nil {
        log.Fatal(err)
    }

	_ = r
}
