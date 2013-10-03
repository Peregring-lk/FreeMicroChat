// ContactList.cpp

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

#include "ContactList.hpp"

ContactList::ContactList() : _last(0)
{}

string ContactList::getContact(unsigned id) const
{
    auto i = _contactList.find(id);

    if (i == _contactList.end())
        return string();
    else
        return i->second;
}

unsigned ContactList::getContact(const string& name) const
{
    if (!name.empty())
        for (auto &i : _contactList)
            if (i.second == name)
                return i.first;

    return ContactList::ID_NULL;
}

bool ContactList::hasContact(unsigned id) const
{
    auto i = _contactList.find(id);

    return i != _contactList.end();
}

bool ContactList::hasContact(const string& name) const
{
    for (auto &i : _contactList)
        if (i.second == name)
            return true;

    return false;
}

unsigned ContactList::addPhantomContact()
{
    ++_last;

    _phantomList.insert(_last);

    return _last;
}

bool ContactList::releasePhantomContact(unsigned id)
{
    _phantomList.erase(id);
}

bool ContactList::checkinPhantomContact(unsigned id, const string& name)
{
    auto i = _phantomList.find(id);

    if (i != _phantomList.end()) {
        _contactList[id] = name;

        _phantomList.erase(id);

        return true;
    }
    else
        return false;
}

bool ContactList::releaseContact(unsigned id)
{
    return (bool)_contactList.erase(id);
}

string ContactList::serialize() const
{
    ostringstream oss;
    text_oarchive ar(oss);

    ar << *this;

    return oss.str();
}

void ContactList::deserialize(string serialized)
{
    istringstream is(serialized);
    text_iarchive ar(is);

    ar >> *this;
}

void ContactList::printContactList() const
{
    for (auto &i : _contactList)
        cout << i.second << " ";

    cout << endl << flush;
}

void ContactList::serialize(text_oarchive& ar, const unsigned int version)
{
    ar & _contactList;
}

void ContactList::serialize(text_iarchive& ar, const unsigned int version)
{
    _contactList.clear();
    _phantomList.clear();

    ar & _contactList;
}
