// Client.cpp

/*
  Copyright (C) 2012 Aarón Bueno Villares <abv150ci@gmail.com>
  Copyright (C) 2012 Juan Antonio Tejero Fernández <juanantonio.tejerofernandez@alum.uca.es>
  Copyright (C) 2012 María Inmaculada Labrador del Río <inmaculada.labrador@uca.es>

  This file is part of FreeMicroChat.

  FreeMicroChat is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  FreeMicroChat is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with FreeMicroChat.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Client.hpp"

Client::Client(const string& serverName, unsigned short port, const string& nick)
    : _service(new io_service), _serverName(serverName), _port(port), _nick(nick),
      _connectionList(_service), _idNick(1),_idConnection(1),  _lastEndNick(ContactList::ID_NULL), _connect(false)
{
    _connectionList.newConnection(_idConnection);
}

void Client::start()
{
    tcp::resolver resolver(*_service);
    tcp::resolver::query query(tcp::v4(), _serverName, lexical_cast<string>(_port));

    try {
        _connectionList.connect(_idConnection, *resolver.resolve(query));

        _connect = true;
    } catch (...) {
        cout << "Server not found." << endl;
    }

    if (_connect) {
        _talkLoop();

        io_service::work work(*_service);
        _service->run();

        _threads.join_all();
    }
}

void Client::_talkLoop()
{
    Message nick(Message::MSG_NICK, _nick);

    _connectionList.sendMsg(_idConnection, &Client::_sentNick, this, nick);
    _connectionList.recvMsg(_idConnection, &Client::_recvContactList, this);
}

void Client::_recvContactList(unsigned id, Message msg)
{
    if (msg.messageType() == Message::MSG_CONTACT_LIST) {
        _contactList.deserialize(msg.message());

        _idNick = _contactList.getContact(_nick);

        _connectionList.recvMsg(_idConnection, &Client::_newMsg, this);

        _threads.create_thread(bind(&Client::_readMsg, this));
    }
    else
        _connect = false;
}

void Client::_printPrompt(unsigned idSource, unsigned idTarget, string msg, ostream& os)
{
    if (_contactList.hasContact(idSource)) {
        os << _contactList.getContact(idSource);

        if (_contactList.hasContact(idTarget))
            os << "->" << _contactList.getContact(idTarget);

        os << "$> " << msg;
    }
}

void Client::_printQueueMsg()
{
    while (!_messageQueue.empty()) {
        string msg = _messageQueue.front();

        cout << msg << endl;

        _messageQueue.pop();
    }
}

void Client::_readMsg()
{
    if (_connect) {
        _printQueueMsg();
        _printPrompt(_idNick, _lastEndNick);

        string str;
        getline(cin, str);

        if (str == "exit")
            _exit();
        else if (str == "$") {
            _contactList.printContactList();
            _readMsg();
        }
        else {
            if (!str.empty())
                if (_contactList.hasContact(str))
                    _lastEndNick = _contactList.getContact(str);
                else if (_contactList.hasContact(_lastEndNick)) {
                    Message msg(Message::MSG_NORMAL, str, _lastEndNick);

                    _connectionList.sendMsg(_idConnection, &Client::_sentMsg, this, msg);
                }

            _readMsg();
        }
    }
    else {
        cout << "Server disconnected." << endl;
        _exit();
    }
}

void Client::_newMsg(unsigned id, Message msg)
{
    if (msg.messageType() != Message::MSG_EOF) {
        if (msg.messageType() == Message::MSG_NORMAL) {
            ostringstream os;

            _printPrompt(msg.target(), _idNick, msg.message(), os);

            _messageQueue.push(os.str());
        }
        else if (msg.messageType() == Message::MSG_CONTACT_LIST)
            _contactList.deserialize(msg.message());

        _connectionList.recvMsg(_idConnection, &Client::_newMsg, this);
    }
    else
        _connect = false;
}

void Client::_sentNick(unsigned id, Message msg)
{
    if (msg.messageType() == Message::MSG_EOF)
        _connect = false;
}

void Client::_sentMsg(unsigned id, Message msg)
{
    if (msg.messageType() == Message::MSG_EOF)
        _connect = false;
}

void Client::_exit()
{
    _connectionList.deleteConnection(_idConnection);
    _service->stop();
}
