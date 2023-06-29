#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>

using namespace boost;


int main() {
    try {
        asio::io_context ioContext;

        asio::ip::tcp::resolver resolver(ioContext);
        asio::ip::tcp::resolver::results_type endpoints = resolver.resolve("localhost", "8000");

        asio::ip::tcp::socket socket(ioContext);
        asio::connect(socket, endpoints);

        std::cout << "Connected to the chat server\n";

        std::thread readThread([&socket]() {
            try {
                asio::streambuf buffer;
                while (true) {
                    // Read message from the server
                    asio::read_until(socket, buffer, '\n');
                    std::string message(asio::buffers_begin(buffer.data()), asio::buffers_end(buffer.data()));
                    buffer.consume(buffer.size());

                    // Print the received message
                    std::cout << message;
                }
            } catch (const std::exception& ex) {
                std::cerr << "Exception in readThread: " << ex.what() << std::endl;
            }
        });

        // Send messages to the server
        std::string message;
        while (std::getline(std::cin, message)) {
            message += '\n';
            asio::write(socket, asio::buffer(message));
        }

        readThread.join();
    } catch (const std::exception& ex) {
        std::cerr << "Exception in main: " << ex.what() << std::endl;
    }

    return 0;
}


