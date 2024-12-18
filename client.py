import socket

def main():
    server_ip = input("Enter server IP address: ")
    server_port = 8080

    try:
        # Create a socket object
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # Connect to the server
        client_socket.connect((server_ip, server_port))
        print("Connected to the server.")

        while True:
            # Get user input
            message = input("Enter message to send (or type 'exit' to quit): ")

            if message.lower() == 'exit':
                print("Closing connection...")
                break

            # Send the message to the server
            client_socket.sendall(message.encode('utf-8'))

            # Receive the response from the server
            response = client_socket.recv(1024).decode('utf-8')
            if not response:
                print("Server closed the connection.")
                break

            print("Server response:", response)

    except Exception as e:
        print(f"An error occurred: {e}")

    finally:
        # Close the connection
        client_socket.close()
        print("Connection closed.")

if __name__ == "__main__":
    main()
