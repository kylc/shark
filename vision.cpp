#include <boost/asio.hpp>

#include "remote.h"

const std::vector<char> SYNC_STR { 'a', 'b', 'c', 'd' };

int main(int argc, char **argv) {
  boost::asio::io_service io;
  boost::asio::serial_port port(io, argv[1]);

  port.set_option(boost::asio::serial_port_base::baud_rate(57600));
  port.set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
  port.set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
  port.set_option(boost::asio::serial_port::character_size(8));

  while(true) {
    boost::asio::write(port, boost::asio::buffer(SYNC_STR));

    struct remote_msg_t msg {
      .speed = 1,
      .angle = -30
    };

    boost::asio::write(port, boost::asio::buffer(&msg, sizeof(msg)));
  }
}
