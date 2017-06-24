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
 * $URL: http://scummvm-dgds.googlecode.com/svn/trunk/resources/bmp.cpp $
 * $Id: bmp.cpp 12 2010-01-23 12:01:58Z alexbevi $
 *
 */

#include "dgds/resources/bmp.h"
#include "dgdsconstant.h"

namespace Dgds {

Bmp::Bmp() :
	_numInf(0), _dataInf(NULL), _numOff(0), _dataOff(NULL) {
}

Bmp::~Bmp() {
	empty();
}

void Bmp::empty() {
	_numInf = 0;
	if (_dataInf) {
		delete [] _dataInf;
		_dataInf = NULL;
	}

	_numOff = 0;
	if (_dataOff) {
		delete [] _dataOff;
		_dataOff = NULL;
	}
}

bool Bmp::init(Resource *res) {
	empty();

	bool ret = false;

	Resource *resBMP = res->getSubResource("BMP:");
	if (resBMP) {
		ret = loadINF(resBMP->getSubResource("INF:"));
		if (ret)
			ret = loadOFF(resBMP->getSubResource("OFF:"));
		delete resBMP;
	} else {
		warning("The resource doesn't contain a bitmap");
	}
	delete res;
	res = nullptr;

	return ret;
}

bool Bmp::loadINF(Resource *resINF) {
	bool ret = false;

	if (resINF) {
		_numInf = resINF->readUint16LE();
		_dataInf = new uint16[_numInf * 2];

		for (int i = 0; i < _numInf; i++)
			_dataInf[i * 2] = resINF->readUint16LE();

		for (int i = 0; i < _numInf; i++)
			_dataInf[i * 2 + 1] = resINF->readUint16LE();

	//	for (int i = 0; i < _numInf; i++)
			//printf("INF: %d,%d\n", _dataInf[i * 2], _dataInf[i * 2 + 1]);

		delete resINF;
		ret = true;
	} else {
		warning("The resource doesn't contain bitmap information");
	}

	return ret;
}

bool Bmp::loadOFF(Resource *resOFF) {
	bool ret = false;

	if (resOFF) {
		if (resOFF->size() % 4) {
			// This image doesn't contain offsets
			if (resOFF->readUint16LE() == 0xFFFF)
				ret = true;
		} else {
			_numOff = resOFF->size() / 4;
			if (_numOff == _numInf)
				ret = true;

			_dataOff = new uint32[_numOff];
			for (int i = 0; i < _numOff; i++) {
				_dataOff[i] = resOFF->readUint32LE();
			//	printf("OFF: %u\n", _dataOff[i]);
			}
		}

		delete resOFF;
	} else {
		// The resource doesn't contain bitmap offsets
		ret = true;
	}

	return ret;
}

} // End of namespace Dgds
