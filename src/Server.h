
#pragma once

#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <utility>

#include <boost/asio.hpp>

#include "Storage.h"
#include "StorageCommand.h"

class Session : public std::enable_shared_from_this<Session> {
public:
  Session(boost::asio::ip::tcp::socket socket, Storage &storage)
      : m_socket(std::move(socket)), m_storage(storage) {}

  void Start() { DoRead(); }

private:
  static constexpr auto MAX_READ_LENGTH = 1024;

  void DoRead() {
    boost::asio::async_read_until(
        m_socket, m_buf, '\n',
        [self = shared_from_this()](boost::system::error_code ec,
                                    std::size_t length) {
          self->OnRead(ec, length);
        });
  }

  void OnRead(boost::system::error_code ec, std::size_t length) {
    if (!ec) {
      std::istream in(&m_buf);
      std::string commandStr;
      std::getline(in, commandStr);
      try {
        const auto command = StorageCommands::ParseCommand(commandStr);
        std::visit([this](const auto &cmd) { ExecuteCommand(cmd); }, command);
      } catch (std::exception &e) {
        WriteError(e.what());
      }
    }
  }

  void WriteOk() { DoWriteResult("OK"); }

  void WriteError(const std::string &errMsg) {
    DoWriteResult("ERR: " + errMsg);
  }

  void DoWriteResult(const std::string &result) {
    std::ostream out(&m_buf);
    out << result << std::endl;
    boost::asio::async_write(
        m_socket, m_buf,
        [self = shared_from_this()](boost::system::error_code ec, std::size_t) {
          if (ec) {
            std::cerr << "Error on writing result: " << ec.message()
                      << std::endl;
          }
          self->DoRead();
        });
  }

  void ExecuteCommand(const StorageCommands::Insert &cmd) {
    m_storage.Insert(cmd.table, cmd.id, cmd.name);
    WriteOk();
  }

  void ExecuteCommand(const StorageCommands::Truncate &cmd) {
    m_storage.Truncate(cmd.table);
    WriteOk();
  }

  void ExecuteCommand(const StorageCommands::Intersection &cmd) {
    SendJoinResult(m_storage.Intersection());
  }

  void ExecuteCommand(const StorageCommands::SymmetricDifference &cmd) {
    SendJoinResult(m_storage.SymmetricDifference());
  }

  void SendJoinResult(std::shared_ptr<JoinResult> joinRes) {
    auto it = joinRes->begin();
    SendJoinResult(std::move(joinRes), it);
  }

  void SendJoinResult(std::shared_ptr<JoinResult> joinRes,
                      JoinResult::iterator it) {
    if (it == joinRes->end()) {
      WriteOk();
      return;
    }

    std::ostream out(&m_buf);
    out << *it << std::endl;
    boost::asio::async_write(
        m_socket, m_buf,
        [self = shared_from_this(), joinRes = std::move(joinRes),
         it](boost::system::error_code ec, std::size_t) {
          if (ec) {
            std::cerr << "Error on sending join result: " << ec.message()
                      << std::endl;
          }
          self->SendJoinResult(std::move(joinRes), std::next(it));
        });
  }

  boost::asio::ip::tcp::tcp::socket m_socket;
  boost::asio::streambuf m_buf;
  Storage &m_storage;
};

class Server {
public:
  Server(boost::asio::io_context &io_context, uint16_t port)
      : m_acceptor(
            io_context,
            boost::asio::ip::tcp::endpoint(
                boost::asio::ip::address::from_string("127.0.0.1"), port)) {
    DoAccept();
  }

private:
  void DoAccept() {
    m_acceptor.async_accept([this](boost::system::error_code ec,
                                   boost::asio::ip::tcp::socket socket) {
      if (!ec) {
        std::make_shared<Session>(std::move(socket), m_storage)->Start();
      }

      DoAccept();
    });
  }

  boost::asio::ip::tcp::acceptor m_acceptor;
  Storage m_storage;
};
