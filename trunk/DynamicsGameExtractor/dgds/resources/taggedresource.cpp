/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL: http://scummvm-dgds.googlecode.com/svn/trunk/resources/taggedresource.cpp $
 * $Id: taggedresource.cpp 16 2010-01-23 19:55:49Z alexbevi $
 *
 */

#include "dgds/resources/taggedresource.h"
#include "dgdsconstant.h"

#include "common/debug.h"

namespace Dgds {

TaggedResource::TaggedResource() {
	// TODO Auto-generated constructor stub

}

TaggedResource::~TaggedResource() {
	// TODO Auto-generated destructor stub
}

bool TaggedResource::load(ResourceManager *resman, Common::String name) {
	_name = name;
	Resource *res = resman->getResource(name);
	bool ret = false;

	if (res) {
		debugC(kDebugResources, "Loading [%s]", name.c_str());
		ret = init(res);
		delete res;
	} else
		warning ("Couldn't find [%s]", name.c_str());

	return ret;
}

} // end of namespace Dgds
