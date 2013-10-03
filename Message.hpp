// Message.hpp

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

#ifndef _MESSAGE_
#define _MESSAGE_

#include "Headers.hpp"

#include "ContactList.hpp"

class Message
{
public:
    enum MessageType { MSG_NULL, MSG_EOF, MSG_CONTACT_LIST, MSG_NICK, MSG_NORMAL };

    Message();
    Message(PtrBuffer buffer);
    Message(MessageType msgType, string msg = "", unsigned target = ContactList::ID_NULL);

    MessageType messageType() const;
    const string& message() const;
    unsigned target() const;

    bool emptyMsg() const;

    string serialize() const;

private:
    MessageType _msgType;
    string _msg;
    unsigned _target;

    friend class serialization::access;

    template<class Archive> void serialize(Archive& ar, const unsigned int version)
    {
        ar & _msgType;
        ar & _msg;
        ar & _target;
    }

    void _checkMsg();
};

#endif // _MESSAGE_
