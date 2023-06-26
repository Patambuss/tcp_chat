#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>

using namespace boost;

std::vector<std::shared_ptr<asio::ip::tcp::socket>> clients;

// Function to handle individual client connections
void session(std::shared_ptr<asio::ip::tcp::socket> socket)
{
    try
    {
        asio::streambuf buffer;
        while (true)
        {
            // Read message from the client
            asio::read_until(*socket, buffer, '\n');
            std::string message(asio::buffers_begin(buffer.data()), asio::buffers_end(buffer.data()));
            buffer.consume(buffer.size());

            // Broadcast the message to all connected clients
            for (const auto& client : clients)
            {
                asio::write(*client, asio::buffer(message));
            }
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Exception in session: " << ex.what() << std::endl;
    }
}

int main()
{
    try
    {
        asio::io_context ioContext;
        asio::ip::tcp::acceptor acceptor(ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 8000));
        std::cout << "Chat server started on port 8000\n";

        while (true)
        {
            std::shared_ptr<asio::ip::tcp::socket> socket(new asio::ip::tcp::socket(ioContext));
            acceptor.accept(*socket);

            // Add the connected client to the vector
            clients.push_back(socket);

            // Start a new thread to handle the client connection
            std::thread thread(session, socket);
            thread.detach();
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Exception in server: " << ex.what() << std::endl;
    }

    return 0;
}
