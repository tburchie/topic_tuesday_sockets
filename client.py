import socket

# Define server address and port
SERVER_IP = '127.0.0.1'
PORT = 8080
BUFFER_SIZE = 1024

def main():
    # Create a socket object
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        # Connect to the server
        sock.connect((SERVER_IP, PORT))
        print("Connected to the server.")

        # Receive the message from the server
        message_from_server = sock.recv(BUFFER_SIZE).decode('utf-8')
        print("Message from server:", message_from_server)

        # Send a response to the server
        message_to_send = "Hello from client!"
        sock.send(message_to_send.encode('utf-8'))
        print("Message sent to server:", message_to_send)

    except Exception as e:
        print(f"Error: {e}")
    finally:
        # Close the socket connection
        sock.close()
        print("Connection closed.")

if __name__ == '__main__':
    main()
