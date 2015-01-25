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
 * $URL: http://scummvm-dgds.googlecode.com/svn/trunk/resources/animation.h $
 * $Id: animation.h 20 2010-01-24 03:16:23Z alexbevi $
 *
 */

#ifndef DGDS_ANIMATION_H_
#define DGDS_ANIMATION_H_

#include "dgds/resources/taggedresource.h"
#include "common/array.h"

namespace Dgds {

class Animation: public TaggedResource {
public:
	Animation();
	virtual ~Animation();

protected:
	bool init(Resource *res);

private:
	Resource *_script;
	Common::String _version;

	/**
	 * The animation map is a list of files loaded from an ADS
	 * RES file that contains filenames and an index. Since the index
	 * is incremental, theres no need to make this a hashmap.
	 */
	Common::Array<Common::String> _animationMap;

}; // end of class Animation

} // end of namespace Dgds

#endif /* DGDS_ANIMATION_H_ */
