const logContainer = document.getElementById('event-log');
let ws; // Global WebSocket variable

// Reconnection logic variables
let reconnectAttempts = 0;
const MAX_RECONNECT_ATTEMPTS = 5; // Stop trying after 5 failed attempts (15 seconds)

function connectWebSocket() {
    // Attempt to connect to the C backend
    ws = new WebSocket('ws://localhost:8000');

    ws.onopen = () => {
        logMessage("SYSTEM: Connected to FSMCore Backend");
        reconnectAttempts = 0; // Reset the counter on a successful connection!
    };

    ws.onclose = () => {
        if (reconnectAttempts < MAX_RECONNECT_ATTEMPTS) {
            reconnectAttempts++;
            logMessage(`SYSTEM: Connection lost. Retrying in 3 seconds... (Attempt ${reconnectAttempts}/${MAX_RECONNECT_ATTEMPTS})`);
            
            // Try again after 3 seconds
            setTimeout(connectWebSocket, 3000);
        } else {
            // Give up after max attempts
            logMessage("SYSTEM: Maximum reconnect attempts reached. Server is unreachable. Please restart the backend and refresh this page.");
        }
    };

    ws.onerror = (error) => {
        // If connection fails, close it to trigger onclose and handle the retry logic
        ws.close();
    };

    ws.onmessage = (event) => {
        // Parse the JSON payload sent by the C program
        const data = JSON.parse(event.data);
        
        // 1. Update Global Slot
        document.getElementById('global-slot').innerText = data.slot;

        // 2. Update Specific User Card
        const stateBadge = document.getElementById(`state-${data.user}`);
        const retryCount = document.getElementById(`retries-${data.user}`);
        
        if (stateBadge && retryCount) {
            stateBadge.innerText = data.state;
            retryCount.innerText = data.retries;

            // Change color based on state for visual impact
            if (data.state === "Completed") stateBadge.style.backgroundColor = "#28a745"; // Green
            else if (data.state === "Retransmission") stateBadge.style.backgroundColor = "#dc3545"; // Red
            else if (data.state === "WaitingACK") stateBadge.style.backgroundColor = "#ffc107"; // Yellow
            else stateBadge.style.backgroundColor = "#007bff"; // Blue
        }

        // 3. Append to Log Terminal
        logMessage(`[Slot ${data.slot}] User ${data.user} transitioned to ${data.state} (Retries: ${data.retries})`);
    };
}

function logMessage(message) {
    const entry = document.createElement('div');
    entry.innerText = message;
    logContainer.appendChild(entry);
    // Auto-scroll to bottom
    logContainer.scrollTop = logContainer.scrollHeight;
}

// Start the initial connection when the page loads
connectWebSocket();