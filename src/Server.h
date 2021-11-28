
#pragma once

#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <utility>

#include <boost/asio.hpp>

#include "bulking/BulkAggregator.h"
#include "bulking/InputProcessor.h"

class Session : public std::enable_shared_from_this<Session> {
public:
  Session(boost::asio::ip::tcp::socket socket,
          bulking::BulkAggregator &bulkAggregator)
      : m_socket(std::move(socket)), m_inputProcessor(bulkAggregator) {}

  void Start() { DoRead(); }

private:
  static constexpr auto MAX_READ_LENGTH = 1024;

  void DoRead() {
    auto self(shared_from_this());
    m_socket.async_read_some(
        boost::asio::buffer(m_data, MAX_READ_LENGTH),
        [self](boost::system::error_code ec, std::size_t length) {
          self->OnRead(ec, length);
        });
  }

  void OnRead(boost::system::error_code ec, std::size_t length) {
    if (!ec) {
      std::stringstream ss(std::string{m_data, length});
      m_inputProcessor.ProcessStream(ss, false);
      DoRead();
    }
  }

  bulking::InputProcessor m_inputProcessor;
  boost::asio::ip::tcp::tcp::socket m_socket;
  char m_data[MAX_READ_LENGTH];
};

class Server {
public:
  Server(boost::asio::io_context &io_context,
         bulking::BulkAggregator &bulkAggregator, short port)
      : m_acceptor(
            io_context,
            boost::asio::ip::tcp::endpoint(
                boost::asio::ip::address::from_string("127.0.0.1"), port)),
        m_bulkAggregator(bulkAggregator) {
    DoAccept();
  }

private:
  void DoAccept() {
    m_acceptor.async_accept([this](boost::system::error_code ec,
                                   boost::asio::ip::tcp::socket socket) {
      if (!ec) {
        std::make_shared<Session>(std::move(socket), m_bulkAggregator)->Start();
      }

      DoAccept();
    });
  }

  boost::asio::ip::tcp::acceptor m_acceptor;
  bulking::BulkAggregator &m_bulkAggregator;
};
