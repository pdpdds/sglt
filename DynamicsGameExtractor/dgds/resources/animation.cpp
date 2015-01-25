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
 * $URL: http://scummvm-dgds.googlecode.com/svn/trunk/resources/animation.cpp $
 * $Id: animation.cpp 22 2010-01-24 03:29:18Z alexbevi $
 *
 */

#include "dgds/resources/animation.h"
#include "dgds/compression.h"
#include "dgdsconstant.h"

namespace Dgds {

Animation::Animation() {
	_script = 0;
}

Animation::~Animation() {
	if (_script)
		delete _script;

	_animationMap.clear();
}

bool Animation::init(Resource *res) {
	bool ret = false;

	Resource *ver = res->getSubResource("VER:");
	Resource *ads = res->getSubResource("ADS:");
	Resource *adsres = ads->getSubResource("RES:");
	Resource *scr    = ads->getSubResource("SCR:");

	if (ver == NULL || adsres == NULL || scr == NULL)
		return ret;

	_version = ver->to_s();
	delete ver;

	scr->seek(0);
	if (scr->readByte() != 0x02)
		error("[%s] Script Data Corruption", getName());

	uint32 size = scr->readUint32LE();

	assert(size != (uint32)scr->size());
	//Common::SeekableReadStream *decomp = decompLZW(scr, size);
	delete scr;
	//_script = new Resource(decomp, false);

	adsres->seek(0);
	uint32 resCount = adsres->readUint16LE();
	for (uint32 i = 0; i < resCount; i++) {
		adsres->skip(2); // skip over the index
		_animationMap.push_back(adsres->to_s(false));
	}

	delete adsres;
	delete ads;

	debugC(kDebugResources, "[%s] Version[%s] ResourceItems[%d]", getName(), _version.c_str(), resCount);
	for (uint32 i = 0; i < _animationMap.size(); i++) {
		debugC(kDebugResources, "Resource %2d: %s", i + 1, _animationMap[i].c_str());
	}

	return ret;
}

} // end of namespace Dgds
