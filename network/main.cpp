#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

// Function to read data from the socket
std::string read_data(tcp::socket& socket) {
    boost::asio::streambuf buf;
    boost::asio::read_until(socket, buf, '\n');
    std::string data = boost::asio::buffer_cast<const char*>(buf.data());
    return data;
}

// Function to send data to the socket
void send_data(tcp::socket& socket, const std::string& message) {
    const std::string msg = message + "\n"; // Add newline for read_until
    boost::asio::write(socket, boost::asio::buffer(msg));
}

int main() {
    try {
        boost::asio::io_service io_service;

        // Create an acceptor to listen for new connections on port 1234
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 1234));

        // Create a socket to handle the incoming connection
        tcp::socket socket(io_service);

        std::cout << "Server listening on port 1234..." << std::endl;

        // Wait for a client to connect
        acceptor.accept(socket);
        std::cout << "Client connected!" << std::endl;

        // Read message from client
        std::string client_message = read_data(socket);
        std::cout << "Received from client: " << client_message << std::endl;

        // Send a response to the client
        send_data(socket, "Hello from Server!");
        std::cout << "Server sent 'Hello from Server!' to client." << std::endl;

        // Close the socket
        socket.close();

    } catch (std::exception& e) {
        std::cerr << "Exception in server: " << e.what() << std::endl;
    }

    return 0;
}