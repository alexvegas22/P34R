package main

import (
	"fmt"
	"log"
	"net"
	"strings"
    "os"
    "bufio"
    "flag"
)

type connection struct {
    connHost,connPort,connType string
}

func handleConnection(conn net.Conn) {
	defer conn.Close()
	buf := make([]byte, 1024)
	for {
		n, err := conn.Read(buf)
		if err != nil {
			log.Println(err)
			return
		}
		fmt.Printf("Received: %s", string(buf[:n]))
		data := strings.ToUpper(string(buf[:n]))
		_, err = conn.Write([]byte(data))
		if err != nil {
			log.Println(err)
			return
		}
	}
}

func connect(connection connection){
	conn, err := net.Dial(connection.connType, connection.connHost+":"+connection.connPort)
	if err != nil {
		fmt.Println("Error connecting:", err.Error())
		os.Exit(1)
	}

    reader := bufio.NewReader(os.Stdin)

    for {
        fmt.Print("> ")
        input, _ := reader.ReadString('\n')
        conn.Write([]byte(input))
        message, _  := bufio.NewReader(conn).ReadString('\n')

        log.Print("Server relay:", message)
    }
}

func setupNode(size int){
    fmt.Println("Setting up Node with a client size of :", size)
	addr, err := net.ResolveTCPAddr("tcp", ":8000")
	if err != nil {
		log.Fatal(err)
	}
	ln, err := net.ListenTCP("tcp", addr)
	if err != nil {
		log.Fatal(err)
	}
	defer ln.Close()
	fmt.Println("Listening on port 8000")
	for {
		conn, err := ln.Accept()
		if err != nil {
			log.Fatal(err)
		}
		go handleConnection(conn)
	}

}

func parseArgs() {
    
    clientCmd := flag.NewFlagSet("client", flag.ExitOnError)
    hostPtr := clientCmd.String("host", "localhost", "host ip")
    portPtr := clientCmd.String("port", "8080", "host port")

    nodeCmd := flag.NewFlagSet("node", flag.ExitOnError)
    sizePtr := nodeCmd.Int("size", 5, "node list size")

    if len(os.Args) < 2 {
        fmt.Println("expected 'client' or 'node' subcommands")
        os.Exit(1)
    }
    
    switch os.Args[1] {
    case "client" :
        fmt.Println("Client Mode")
        clientCmd.Parse(os.Args[2:])
        connect(connection{*hostPtr, *portPtr, "tcp"})
    case "node" :
        fmt.Println("Node Mode")
        nodeCmd.Parse(os.Args[2:])
        setupNode(*sizePtr)
    default:
        fmt.Println("expected 'client' or 'node' subcommands")
        os.Exit(1)
    }

    return
}

func main() {
    parseArgs()
}
