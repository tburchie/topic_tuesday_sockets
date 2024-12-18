import socket
import platform
import os

# Function to detect the OS
def get_os_name():
    return platform.system()

# Function to get local IP address (for Linux/macOS)
def get_local_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.settimeout(0)
    try:
        s.connect(('8.8.8.8', 1))  # Google DNS (arbitrary external IP)
        local_ip = s.getsockname()[0]
    except Exception as e:
        local_ip = None
    finally:
        s.close()
    return local_ip

# Function to get local IP address (for Windows)
def get_local_ip_windows():
    # Use Windows-specific method to get local IP
    local_ip = None
    import socket
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect(('8.8.8.8', 1))  # Connect to an external address to find local IP
        local_ip = s.getsockname()[0]
    except Exception as e:
        local_ip = None
    finally:
        s.close()
    return local_ip

def main():
    # Get OS and local IP address
    os_name = get_os_name()
    print(f"OS detected: {os_name}")

    if os_name == "Windows":
        local_ip = get_local_ip_windows()
    else:
        local_ip = get_local_ip()

    if local_ip is None:
        print("Error: Could not determine local IP address.")
        return
    
    print(f"Local IP address: {local_ip}")
    
    # Server's IP address (replace with your server's IP)
    server_ip = "192.168.1.10"  # Example IP, replace with actual server's IP address
    port = 8080
    
    # Create socket to connect to the server
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    try:
        client_socket.connect((server_ip, port))
        print(f"Connected to server at {server_ip}:{port}")

        # Send a message to the server
        message = "Hello from Python client!"
        client_socket.send(message.encode())

        print(f"Message sent to server: {message}")
        
        # Receive the response from the server
        response = client_socket.recv(1024).decode()
        print(f"Message from server: {response}")
        
    except Exception as e:
        print(f"Connection failed: {e}")
    finally:
        client_socket.close()

if __name__ == "__main__":
    main()
