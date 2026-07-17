#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Windows vs Linux Networking Abstraction ---
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib") // Tell MSVC to link Winsock
    typedef int socklen_t;
#else
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif
// -----------------------------------------------

// Core FSM and Protocol Headers
#include "include/fsm.h"
#include "include/queue.h"
#include "include/timer.h"
#include "include/logger.h"
#include "include/stats.h"
#include "protocols/harq.h"

// Mongoose WebSocket Library
#include "include/mongoose.h"

#define MAX_USERS 5
#define UDP_PORT 9000
#define WS_URL "ws://localhost:8000"

// Global System Components
EventQueue global_queue;
FSM_Instance harq_fsms[MAX_USERS];
HARQ_Context harq_ctxs[MAX_USERS];
Timer harq_timers[MAX_USERS];
struct mg_mgr mgr;

// Helper to get state names for JSON
const char* get_state_name(int id) {
    switch(id) {
        case HARQ_STATE_IDLE: return "Idle";
        case HARQ_STATE_NEW_TX: return "NewTransmission";
        case HARQ_STATE_WAITING_ACK: return "WaitingACK";
        case HARQ_STATE_RETX: return "Retransmission";
        case HARQ_STATE_COMPLETED: return "Completed";
        default: return "Unknown";
    }
}

// WebSocket Event Handler
static void ws_handler(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        
        // Upgrade all incoming HTTP requests on this port to a WebSocket connection
        mg_ws_upgrade(c, hm, NULL);
        printf("[WebSocket] Client connected!\n");
    }
}

// Broadcast JSON to frontend
void broadcast_json(int slot, int ue_id, int state_id, int retries) {
    char json_payload[256];
    snprintf(json_payload, sizeof(json_payload),
             "{\"slot\": %d, \"user\": %d, \"state\": \"%s\", \"retries\": %d}",
             slot, ue_id, get_state_name(state_id), retries);
             
    for (struct mg_connection *c = mgr.conns; c != NULL; c = c->next) {
        if (c->is_websocket) {
            mg_ws_send(c, json_payload, strlen(json_payload), WEBSOCKET_OP_TEXT);
        }
    }
    printf("[Broadcast] %s\n", json_payload);
}

// Set up the non-blocking UDP socket (Windows Compatible)
#ifdef _WIN32
SOCKET setup_udp_listener() {
    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(UDP_PORT);
    
    bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    
    // Windows non-blocking command
    u_long mode = 1;
    ioctlsocket(sockfd, FIONBIO, &mode);
    return sockfd;
}
#else
int setup_udp_listener() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(UDP_PORT);
    
    bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    
    // Linux/Mac non-blocking command
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    return sockfd;
}
#endif

int main() {
    // 0. Initialize Windows Sockets if on Windows
    #ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed to initialize Winsock.\n");
        return 1;
    }
    #endif

    printf("Starting FSMCore Real-Time Hub (Windows Mode)\n");
    printf("Listening for Simulator on UDP :%d\n", UDP_PORT);
    printf("WebSocket Server running on %s\n", WS_URL);
    printf("===========================================\n");

    // 1. Initialize Network Libraries
    mg_mgr_init(&mgr);
    mg_http_listen(&mgr, WS_URL, ws_handler, NULL);
    
    #ifdef _WIN32
        SOCKET udp_sock = setup_udp_listener();
    #else
        int udp_sock = setup_udp_listener();
    #endif

    // 2. Initialize FSM Components
    Queue_Init(&global_queue);
    for(int u = 0; u < MAX_USERS; u++) {
        Timer_Init(&harq_timers[u]);
        harq_ctxs[u].processId = 0;
        harq_ctxs[u].retryCount = 0;
        harq_ctxs[u].timer = &harq_timers[u];
        
        HARQ_Setup(&harq_fsms[u], &harq_ctxs[u]);
        FSM_Reset(&harq_fsms[u], HARQ_STATE_IDLE);
    }

    // 3. The Infinite Real-Time Loop
    char udp_buffer[1024];
    
    while (1) {
        mg_mgr_poll(&mgr, 10); // Poll WebSockets
        
        struct sockaddr_in cliaddr;
        socklen_t len = sizeof(cliaddr);
        
        // Receive UDP Data
        int n = recvfrom(udp_sock, udp_buffer, sizeof(udp_buffer) - 1, 
                         0, (struct sockaddr *)&cliaddr, &len);
                         
        if (n > 0) {
            udp_buffer[n] = '\0';
            int slot, ue_id, event_id;
            if (sscanf(udp_buffer, "%d,%d,%d", &slot, &ue_id, &event_id) == 3) {
                if (ue_id >= 0 && ue_id < MAX_USERS) {
                    Event e = { .id = event_id, .slot = slot, .ue_id = ue_id, .payload = NULL };
                    Queue_Push(&global_queue, e);
                }
            }
        }

        // Process the Queue
        while (!Queue_IsEmpty(&global_queue)) {
            Event currentEvent;
            Queue_Pop(&global_queue, &currentEvent);

            int u = currentEvent.ue_id;
            FSM_Instance *targetFSM = &harq_fsms[u];
            int prevStateId = targetFSM->currentStateId;
            
            FSM_ProcessEvent(targetFSM, &currentEvent);
            
            if (prevStateId != targetFSM->currentStateId) {
                broadcast_json(currentEvent.slot, u, targetFSM->currentStateId, harq_ctxs[u].retryCount);
            }
        }
    }

    mg_mgr_free(&mgr);
    #ifdef _WIN32
        WSACleanup();
    #endif
    return 0;
}