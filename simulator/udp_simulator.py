import socket
import time
import random

# Setup UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
TARGET = ("127.0.0.1", 9000)

print("Starting UDP Simulator for FSMCore...")
print("Transmitting events to C Backend on UDP port 9000...\n")

slot = 1
while True:
    # Pick a random user (0 or 1)
    user_id = random.randint(0, 1)
    
    # 10=Schedule, 11=PacketSent, 12=ACK, 13=NACK
    # We weight the choices so Schedule and PacketSent happen often, with occasional ACKs/NACKs
    event_id = random.choice([10, 11, 11, 12, 13]) 
    
    # Format: "Slot,UserID,EventID"
    message = f"{slot},{user_id},{event_id}"
    
    sock.sendto(message.encode(), TARGET)
    print(f"[Slot {slot}] Sent Event {event_id} for User {user_id}")
    
    slot += 1
    time.sleep(1) # 1 second per slot