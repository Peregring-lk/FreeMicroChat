// Message.cpp

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

#include "Message.hpp"

#include "ContactList.hpp"

Message::Message()
    : _msgType(MSG_NULL)
{}

Message::Message(PtrBuffer buffer)
{
    istream is(&(*buffer));
    text_iarchive ar(is);

    ar >> *this;
    is.ignore();

    _checkMsg();
}

Message::Message(MessageType msgType, string msg, unsigned target)
    : _msgType(msgType), _msg(msg), _target(target)
{
    _checkMsg();
}

Message::MessageType Message::messageType() const
{
    return _msgType;
}

const string& Message::message() const
{
    return _msg;
}

unsigned Message::target() const
{
    return _target;
}

bool Message::emptyMsg() const
{
    return _msg.empty();
}

string Message::serialize() const
{
    std::ostringstream oss;
    text_oarchive ar(oss);

    ar << *this;

    return oss.str();
}

void Message::_checkMsg()
{
    if (_msgType != MSG_NORMAL)
        _target = ContactList::ID_NULL;

    if (_msgType == MSG_NULL)
        _msg.clear();
}
