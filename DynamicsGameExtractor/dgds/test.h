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
 * $URL: http://scummvm-dgds.googlecode.com/svn/trunk/test.h $
 * $Id: test.h 24 2010-01-24 15:21:25Z alexbevi $
 *
 */

#ifndef TEST_H
#define TEST_H

#include "dgds/resourcemanager.h"
//#include "dgds/resources/palette.h"
#include "dgds/resources/bmp.h"
//#include "dgds/resources/font.h"
//#include "dgds/resources/movie.h"
//#include "dgds/resources/animation.h"
//#include "dgds/resources/soundresource.h"


namespace Dgds {

class Test {
public:
	Test(ResourceManager *resMgr);
	~Test();

	void next();
	Common::String getNext(Common::String ext);

private:
	ResourceManager *_resMgr;
//	Palette *_palette;
	Bmp     *_bmp;
//	Font    *_fnt;
//	Movie   *_movie;
//	Animation *_anim;
	//SoundResource *_snd;

	ResourceFiles::const_iterator _resIter;

	int _step;
};

} // End of namespace Dgds

#endif // TEST_H
