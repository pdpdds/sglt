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
 * $URL: http://scummvm-dgds.googlecode.com/svn/trunk/resources/movie.cpp $
 * $Id: movie.cpp 23 2010-01-24 12:46:31Z alexbevi $
 *
 */

#include "dgds/resources/movie.h"
#include "dgds/compression.h"

namespace Dgds {

Movie::Movie() {
}

Movie::~Movie() {
	_chunks.clear();
}

bool Movie::init(Resource *res) {
	bool ret = false;

	Resource *ver = res->getSubResource("VER:");
	if (!ver)
		error("VER tag not found for Movie Resource");
	Resource *pag = res->getSubResource("PAG:");
	if (!pag)
		error("PAG tag not found for Movie Resource");
	Resource *tt3 = res->getSubResource("TT3:");
	if (!tt3)
		error ("TT3 tag not found for Movie Resource");

	_version = ver->to_s();
	delete ver;

	pag->seek(0);
	_pages = pag->readUint16LE();
	delete pag;

	tt3->seek(1);
	uint32 tt3size = tt3->readUint32LE();
	Common::SeekableReadStream *decomp = decompLZW(tt3, tt3size);
	Resource *chunkdata = new Resource(decomp, false);

	while (!chunkdata->eos()) {
		MovieChunk *mc = new MovieChunk;
		uint32 code = chunkdata->readUint16LE();
		uint32 size = code & 0x000F;
		code &= 0xFFF0;
		mc->code = code;
		if (code == 0x1110 && size == 1) {
			uint32 id = chunkdata->readUint16LE();
			printf("%d\n", id);
			mc->data.push_back(id);
		} else if (size == 15) {
			mc->name = chunkdata->to_s(false);
			if ((chunkdata->size() - chunkdata->pos()) & 1)
				chunkdata->skip(1);
		} else {
			for (uint32 i = 0; i < size; i++)
				mc->data.push_back(chunkdata->readSint16LE());
		}
		_chunks.push_back(mc);
	}

	delete chunkdata;
	delete tt3;

	/*
	Resource *r = new Resource(decomp, false);
	r->dump(Common::String(getName()) + Common::String(".TT3.LZW"), false);
	delete r;
	*/

	debugC(kDebugResources, "[%s] Version[%s] Pages[%d]", getName(), _version.c_str(), _pages);






	return ret;
}

} // end of namespace Dgds
