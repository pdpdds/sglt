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
 * $URL: http://scummvm-dgds.googlecode.com/svn/trunk/resources/soundresource.h $
 * $Id: soundresource.h 24 2010-01-24 15:21:25Z alexbevi $
 *
 */

#ifndef DGDS_SOUNDRESOURCE_H_
#define DGDS_SOUNDRESOURCE_H_

#include "dgds/resources/taggedresource.h"

namespace Dgds {

/**
 * DGDS Sound Resources are contained in *.SX files.
 * They are split into four (4) tag groups:
 *   TAG_TAG
 *   TAG_DAT
 *   TAG_FNM
 *   TAG_INF
 */
class SoundResource: public TaggedResource {
public:
	SoundResource();
	virtual ~SoundResource();

protected:
	bool init(Resource *res);

private:
	ResourceTags _tags;

}; // end of class SoundResource

} // end of namespace Dgds

#endif /* DGDS_SOUNDRESOURCE_H_ */
