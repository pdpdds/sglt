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
 * $URL: http://scummvm-dgds.googlecode.com/svn/trunk/resources/taggedresource.h $
 * $Id: taggedresource.h 25 2010-02-02 03:59:10Z alexbevi $
 *
 */

#ifndef DGDS_TAGGEDRESOURCE_H_
#define DGDS_TAGGEDRESOURCE_H_

#include "common/hash-str.h"

#include "dgds/resourcemanager.h"

namespace Dgds {

enum ResourceTag {
	TAG_ADS = 0x3a534441,
	TAG_BMP = 0x3a504d42,
	TAG_FNT = 0x3a544e46,
	TAG_INF = 0x3a464e49,
	TAG_PAG = 0x3a474150,
	TAG_PAL = 0x3a4c4150,
	TAG_RES = 0x3a534552,
	TAG_SCR = 0x3a524353,
	TAG_SND = 0x3a444e53,
	TAG_TAG = 0x3a474154,
	TAG_TT3 = 0x3a335454,
	TAG_VER = 0x3a524556,
	TAG_VGA = 0x3a414756
};

struct ResourceEntry {
	uint16 id;
	Common::String name;
	Common::String filename;
	byte *data;
};

/**
 * ResourceTags are key-value pairs read from a Resources
 * subresource of TAG_TAG.
 */
typedef Common::HashMap<uint16, ResourceEntry> ResourceTags;

class TaggedResource {
public:
	TaggedResource();
	virtual ~TaggedResource();

	/**
	 * Load the filename from the resourcemanager
	 */
	bool load(ResourceManager *resman, Common::String name);


protected:
	const char *getName() { return _name.c_str(); }

	/**
	 * Initialize the TaggedResource to the base resource
	 * instance and do any class-specific initialization
	 */
	virtual bool init(Resource *res) = 0;

private:
	// XXX this is just here for convenience, so that
	// when we load() a resource, we can keep an internal reference
	// to the filename (mostly for debug output and such)
	Common::String _name;

}; // end of class TaggedResource

} // end of namespace Dgds

#endif /* DGDS_TAGGEDRESOURCE_H_ */
