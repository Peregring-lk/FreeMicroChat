// ContactList.hpp

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

#ifndef _CONTACT_LIST_
#define _CONTACT_LIST_

#include "Headers.hpp"

class Message;

// Class.
class ContactList
{
public:
    enum { ID_NULL = 0};

public:
    ContactList();

    string getContact(unsigned id) const;
    unsigned getContact(const string& name) const;

    bool hasContact(unsigned id) const;
    bool hasContact(const string& name) const;

    unsigned addPhantomContact();
    bool releasePhantomContact(unsigned id);
    bool checkinPhantomContact(unsigned id, const string& name);

    bool releaseContact(unsigned id);

    string serialize() const;
    void deserialize(string serialized);

    void printContactList() const;

    // Envía un mensaje f a todos los contactos.
    template<typename F>
    void broadcastMessage(F f, const Message& msg) const
    {
        for (auto &i : _contactList)
            f(i.first, msg);
    }

private:
    unsigned _last;
    map<unsigned, string> _contactList;
    set<unsigned> _phantomList;

    friend class serialization::access;

    void serialize(text_oarchive& ar, const unsigned int version);
    void serialize(text_iarchive& ar, const unsigned int version);
};

// #include "ContactList.cpp"

#endif // _CONTACT_LIST__
