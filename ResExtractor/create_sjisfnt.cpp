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
 */

#include <Windows.h>
#include <list>
#include "create_sjisfnt.h"

#include "common/endian.h"
#include "common/file.h"
#include "common/stream.h"
#include "common/config-manager.h"
#include "common/system.h"
#include "decompressor.h"

#include <stdio.h>
#include <stdlib.h>
#include "iconv.h"
#include <assert.h>
#include <errno.h>

struct resource_index_t {
	uint16 wOffset;
	uint16 wSize;
};

enum ResVersion {
	kResVersionUnknown,
	kResVersionSci0Sci1Early,
	kResVersionSci1Middle,
	kResVersionKQ5FMT,
	kResVersionSci1Late,
	kResVersionSci11,
	kResVersionSci11Mac,
	kResVersionSci2,
	kResVersionSci3
};

enum {
	SCI0_RESMAP_ENTRIES_SIZE = 6,
	SCI1_RESMAP_ENTRIES_SIZE = 6,
	KQ5FMT_RESMAP_ENTRIES_SIZE = 7,
	SCI11_RESMAP_ENTRIES_SIZE = 5
};

enum SciVersion {
	SCI_VERSION_NONE,
	SCI_VERSION_0_EARLY, // KQ4 early, LSL2 early, XMAS card 1988
	SCI_VERSION_0_LATE, // KQ4, LSL2, LSL3, SQ3 etc
	SCI_VERSION_01, // KQ1 and multilingual games (S.old.*)
	SCI_VERSION_1_EGA_ONLY, // SCI 1 EGA with parser (i.e. QFG2 only)
	SCI_VERSION_1_EARLY, // KQ5 floppy, SQ4 floppy, XMAS card 1990, Fairy tales, Jones floppy
	SCI_VERSION_1_MIDDLE, // LSL1, Jones CD
	SCI_VERSION_1_LATE, // Dr. Brain 1, EcoQuest 1, Longbow, PQ3, SQ1, LSL5, KQ5 CD
	SCI_VERSION_1_1, // Dr. Brain 2, EcoQuest 1 CD, EcoQuest 2, KQ6, QFG3, SQ4CD, XMAS 1992 and many more
	SCI_VERSION_2, // GK1, PQ4 floppy, QFG4 floppy
	SCI_VERSION_2_1, // GK2, KQ7, LSL6 hires, MUMG Deluxe, Phantasmagoria 1, PQ4CD, PQ:SWAT, QFG4CD, Shivers 1, SQ6, Torin
	SCI_VERSION_3 // LSL7, Lighthouse, RAMA, Phantasmagoria 2
};

enum ResourceType {
	kResourceTypeView = 0,
	kResourceTypePic,
	kResourceTypeScript,
	kResourceTypeText,
	kResourceTypeSound,
	kResourceTypeMemory,
	kResourceTypeVocab,
	kResourceTypeFont,
	kResourceTypeCursor,
	kResourceTypePatch,
	kResourceTypeBitmap,
	kResourceTypePalette,
	kResourceTypeCdAudio,
	kResourceTypeAudio,
	kResourceTypeSync,
	kResourceTypeMessage,
	kResourceTypeMap,
	kResourceTypeHeap,
	kResourceTypeAudio36,
	kResourceTypeSync36,
	kResourceTypeTranslation, // Currently unsupported

	// SCI2.1+ Resources
	kResourceTypeRobot,
	kResourceTypeVMD,
	kResourceTypeChunk,
	kResourceTypeAnimation,

	// SCI3 Resources
	kResourceTypeEtc,
	kResourceTypeDuck,
	kResourceTypeClut,
	kResourceTypeTGA,
	kResourceTypeZZZ,

	// Mac-only resources
	kResourceTypeMacIconBarPictN, // IBIN resources (icon bar, not selected)
	kResourceTypeMacIconBarPictS, // IBIS resources (icon bar, selected)
	kResourceTypeMacPict,        // PICT resources (inventory)

	kResourceTypeInvalid
};

static const ResourceType s_resTypeMapSci0[] = {
	kResourceTypeView, kResourceTypePic, kResourceTypeScript, kResourceTypeText,          // 0x00-0x03
	kResourceTypeSound, kResourceTypeMemory, kResourceTypeVocab, kResourceTypeFont,       // 0x04-0x07
	kResourceTypeCursor, kResourceTypePatch, kResourceTypeBitmap, kResourceTypePalette,   // 0x08-0x0B
	kResourceTypeCdAudio, kResourceTypeAudio, kResourceTypeSync, kResourceTypeMessage,    // 0x0C-0x0F
	kResourceTypeMap, kResourceTypeHeap, kResourceTypeAudio36, kResourceTypeSync36,       // 0x10-0x13
	kResourceTypeTranslation                                                              // 0x14
};

// TODO: 12 should be "Wave", but SCI seems to just store it in Audio resources
static const ResourceType s_resTypeMapSci21[] = {
	kResourceTypeView, kResourceTypePic, kResourceTypeScript, kResourceTypeAnimation,     // 0x00-0x03
	kResourceTypeSound, kResourceTypeEtc, kResourceTypeVocab, kResourceTypeFont,          // 0x04-0x07
	kResourceTypeCursor, kResourceTypePatch, kResourceTypeBitmap, kResourceTypePalette,   // 0x08-0x0B
	kResourceTypeInvalid, kResourceTypeAudio, kResourceTypeSync, kResourceTypeMessage,    // 0x0C-0x0F
	kResourceTypeMap, kResourceTypeHeap, kResourceTypeChunk, kResourceTypeAudio36,        // 0x10-0x13
	kResourceTypeSync36, kResourceTypeTranslation, kResourceTypeRobot, kResourceTypeVMD,  // 0x14-0x17
	kResourceTypeDuck, kResourceTypeClut, kResourceTypeTGA, kResourceTypeZZZ              // 0x18-0x1B
};

static const char *const s_resourceTypeNames[] = {
	"view", "pic", "script", "text", "sound",
	"memory", "vocab", "font", "cursor",
	"patch", "bitmap", "palette", "cdaudio",
	"audio", "sync", "message", "map", "heap",
	"audio36", "sync36", "xlate", "robot", "vmd",
	"chunk", "animation", "etc", "duck", "clut",
	"tga", "zzz", "macibin", "macibis", "macpict"
};

typedef struct tag_ResDesc
{
	int type;
	int number;
	uint32 fileoffset;
	int volnumber;
}ResDesc;

typedef std::list<ResDesc> listResDesc;
listResDesc g_listResDesc;
extern OSystem* g_system;

using namespace Sci;

bool ExtractRes();
int decompress(ResVersion volVersion, Common::SeekableReadStream *file, Common::DumpFile* outFile, uint16 num, byte type);
int readResourceInfo(ResVersion volVersion, Common::SeekableReadStream *file,
					 uint32 &szPacked, uint32 &szunPacked, ResourceCompression &compression);
ResourceType convertResType(byte type) ;
int AnalyzeMap(Common::String& str);
int ExtractText(char* filename);
bool ExtractSpaceQuestRes();

int _mapVersion = kResVersionSci2;
int main(int argc, char *argv[])
{
	g_system = new OSystem();
	g_system->initBackend();

	Common::FSNode dir(ConfMan.get("path"));

	// Add the game path to the directory search list
	SearchMan.addDirectory(dir.getPath(), dir, 0, 4);

	Common::String str = "resource.map";
	//str = "MESSAGE.MAP";
	AnalyzeMap(str);
	//ExtractRes();
	ExtractSpaceQuestRes();


	/*for(int i = 0; i <2000; i++)
	{
		char outFileName[50];
		sprintf(outFileName, "%s.%03d", s_resourceTypeNames[15], i);
		ExtractText(outFileName);
	}*/
	

	return 0;

	
}

int ExtractText(char* filename)
{	
	Common::SeekableReadStream *fileStream = 0;
	Common::String str = filename;

	const uint _headerSize = 8;
	const uint _recordSize = 10;

	if (!Common::File::exists(str))
		return -1;

	Common::File *file = new Common::File();
	if (!file->open(str))
		return -1;
	fileStream = file;

	int DataSize = fileStream->size();

	byte* data = new byte[DataSize];
	fileStream->read(data, DataSize);
	//int dummy = fileStream->readSint32LE();
	// dummy = fileStream->readSint16LE();

	int _messageCount = *(uint16*)(data + 6);

	if (_messageCount * _recordSize + _headerSize > DataSize)
		return false;

	const byte *recordPtr = data + _headerSize;

	for (uint i = 0; i < _messageCount; i++) {
		//if ((recordPtr[0] == tuple.noun) && (recordPtr[1] == tuple.verb)
		//	&& (recordPtr[2] == tuple.cond) && (recordPtr[3] == tuple.seq)) {
				//record.tuple = tuple;
			//	record.refTuple = MessageTuple();
				//record.talker = recordPtr[4];
				//record.string = (const char *)_data + READ_LE_UINT16(recordPtr + 5);
			//	return true;
		//}
		//recordPtr += _recordSize;
		Common::String name = (const char *)data + READ_LE_UINT16(recordPtr + 5);
			

		recordPtr += _recordSize;

	}

	delete file;

 	return 0;
}

int AnalyzeMap(Common::String& str)
{	
	Common::SeekableReadStream *fileStream = 0;

	if (!Common::File::exists(str))
		return -1;

	Common::File *file = new Common::File();
	if (!file->open(str))
		return -1;
	fileStream = file;

	int count = 0;

	resource_index_t resMap[32];
	memset(resMap, 0, sizeof(resource_index_t) * 32);
	byte type = 0, prevtype = 0;
	byte nEntrySize = _mapVersion == kResVersionSci11 ? SCI11_RESMAP_ENTRIES_SIZE : SCI1_RESMAP_ENTRIES_SIZE;

	// Read resource type and offsets to resource offsets block from .MAP file
	// The last entry has type=0xFF (0x1F) and offset equals to map file length
	do {
		type = fileStream->readByte() & 0x1F;
		resMap[type].wOffset = fileStream->readUint16LE();
		resMap[prevtype].wSize = (resMap[type].wOffset
			- resMap[prevtype].wOffset) / nEntrySize;
		prevtype = type;
	} while (type != 0x1F); // the last entry is FF

	// reading each type's offsets
	uint32 fileOffset = 0;
	for (type = 0; type < 32; type++) {
		if (resMap[type].wOffset == 0) // this resource does not exist in map
			continue;
		fileStream->seek(resMap[type].wOffset);


		for (int i = 0; i < resMap[type].wSize; i++) {
			uint16 number = fileStream->readUint16LE();
			int volume_nr = 0;
			if (_mapVersion == kResVersionSci11) {
				// offset stored in 3 bytes
				fileOffset = fileStream->readUint16LE();
				fileOffset |= fileStream->readByte() << 16;
				fileOffset <<= 1;
			} else {
				// offset/volume stored in 4 bytes
				fileOffset = fileStream->readUint32LE();
				if (_mapVersion < kResVersionSci11) {
					volume_nr = fileOffset >> 28; // most significant 4 bits
					fileOffset &= 0x0FFFFFFF;     // least significant 28 bits
				} else {
					// in SCI32 it's a plain offset
				}
			}
			if (fileStream->eos() || fileStream->err()) {
				delete fileStream;
				//printf("Error while reading %s", "message.map");
				return -1;
			}

			ResDesc Desc;
			Desc.type = type;
			Desc.number = number;
			Desc.volnumber = volume_nr;
			Desc.fileoffset = fileOffset;

			g_listResDesc.push_back(Desc);
			count++;
		}
	}

	delete fileStream;
	return 0;
}

bool ExtractSpaceQuestRes()
{
	

	listResDesc::iterator iter = g_listResDesc.begin();
int count = 0;
	for(iter; iter != g_listResDesc.end(); iter++)
	{
		ResDesc Desc = *iter;
		count++;

		char outFileName[50];
		sprintf(outFileName, "%s.%03d", s_resourceTypeNames[Desc.type], Desc.number);
		Common::DumpFile *outFile = new Common::DumpFile();
		outFile->open(outFileName);

		char ResourceFileName[50];
		sprintf(ResourceFileName, "%s.%03d", "resource", Desc.volnumber);
		Common::String str = ResourceFileName;

		Common::SeekableReadStream *fileStream = 0;

		if (!Common::File::exists(str))
			return false;

		Common::File *file = new Common::File();
		if (!file->open(str))
			return false;

		fileStream = file;

		//outFile->writeByte(Desc.type | 0x80); // 0x80 is required by old sierra sci, otherwise it wont accept the patch file
		//uint32 _headerSize = 0;
		//outFile->writeByte(_headerSize);
		//if (_headerSize > 0)
		//outFile->write(_header, _headerSize);

		fileStream->seek(Desc.fileoffset, SEEK_SET);

		if(strcmpi(outFileName, "text.000") == 0)
		{
			int i =1;
		}
		int error = decompress((ResVersion)kResVersionSci2, fileStream, outFile, Desc.number, Desc.type);

		outFile->finalize();
		outFile->close();
		delete outFile;
		delete fileStream;
		//DebugPrintf("Resource %s.%03d (located in %s) has been dumped to disk\n", argv[1], resNum, resource->getResourceLocation().c_str());

	}

	return true;
}

bool ExtractRes()
{
	Common::SeekableReadStream *fileStream = 0;
	Common::String str;
	str = "resource.msg";

	if (!Common::File::exists(str))
		return false;

	Common::File *file = new Common::File();
	if (!file->open(str))
		return false;
	fileStream = file;

	listResDesc::iterator iter = g_listResDesc.begin();

	for(iter; iter != g_listResDesc.end(); iter++)
	{
		ResDesc Desc = *iter;


		char outFileName[50];
		sprintf(outFileName, "%s.%03d", s_resourceTypeNames[Desc.type], Desc.number);
		Common::DumpFile *outFile = new Common::DumpFile();
		outFile->open(outFileName);

		//outFile->writeByte(Desc.type | 0x80); // 0x80 is required by old sierra sci, otherwise it wont accept the patch file
		//uint32 _headerSize = 0;
		//outFile->writeByte(_headerSize);
		//if (_headerSize > 0)
		//outFile->write(_header, _headerSize);

		fileStream->seek(Desc.fileoffset, SEEK_SET);
		int error = decompress(kResVersionSci2, fileStream, outFile, Desc.number, Desc.type);

		outFile->finalize();
		outFile->close();
		delete outFile;
		//DebugPrintf("Resource %s.%03d (located in %s) has been dumped to disk\n", argv[1], resNum, resource->getResourceLocation().c_str());

	}

	return true;
}

int decompress(ResVersion volVersion, Common::SeekableReadStream *file, Common::DumpFile* outFile, uint16 num, byte type) {
	int errorNum;
	uint32 szPacked = 0;
	uint32 szUnpacked = 0;
	ResourceCompression compression = kCompUnknown;

	// fill resource info
	errorNum = readResourceInfo(volVersion, file, szPacked, szUnpacked, compression);
	if (errorNum)
		return errorNum;

	// getting a decompressor
	Decompressor *dec = NULL;
	switch (compression) {
	case kCompNone:
		dec = new Decompressor;
		break;
	case kCompHuffman:
		dec = new DecompressorHuffman;
		break;
	case kCompLZW:
	case kCompLZW1:
	case kCompLZW1View:
	case kCompLZW1Pic:
		dec = new DecompressorLZW(compression);
		break;
	case kCompDCL:
		dec = new DecompressorDCL;
		break;
#ifdef ENABLE_SCI32
	case kCompSTACpack:
		dec = new DecompressorLZS;
		break;
#endif
	default:
		//	error("Resource %s: Compression method %d not supported", _id.toString().c_str(), compression);
		return SCI_ERROR_UNKNOWN_COMPRESSION;
	}

	byte* data = new byte[szUnpacked];
	errorNum = data ? dec->unpack(file, data, szPacked, szUnpacked) : SCI_ERROR_RESOURCE_TOO_BIG;
	if (errorNum)
	{
		int i = 1;
	}

	
	uint16 wCompression = 0;//kCompNone;

	outFile->writeByte(type);
	outFile->writeUint16LE(num);
	outFile->writeUint16LE(szPacked);
	outFile->writeUint16LE(szUnpacked);
	outFile->writeUint16LE(wCompression);

	(outFile)->write(data, szUnpacked);


	delete dec;
	return errorNum;
}

int readResourceInfo(ResVersion volVersion, Common::SeekableReadStream *file,
					 uint32 &szPacked, uint32& szUnpacked, ResourceCompression &compression) {
						 // SCI0 volume format:  {wResId wPacked+4 wUnpacked wCompression} = 8 bytes
						 // SCI1 volume format:  {bResType wResNumber wPacked+4 wUnpacked wCompression} = 9 bytes
						 // SCI1.1 volume format:  {bResType wResNumber wPacked wUnpacked wCompression} = 9 bytes
						 // SCI32 volume format :  {bResType wResNumber dwPacked dwUnpacked wCompression} = 13 bytes
						 uint16 w, number;
						 uint32 wCompression;//, szUnpacked;
						 ResourceType type;

						 if (file->size() == 0)
							 return SCI_ERROR_EMPTY_RESOURCE;

						 switch (volVersion) {
	case kResVersionSci0Sci1Early:
	case kResVersionSci1Middle:
		w = file->readUint16LE();
		type = convertResType(w >> 11);
		number = w & 0x7FF;
		szPacked = file->readUint16LE() - 4;
		szUnpacked = file->readUint16LE();
		wCompression = file->readUint16LE();
		break;
	case kResVersionSci1Late:
		type = convertResType(file->readByte());
		number = file->readUint16LE();
		szPacked = file->readUint16LE() - 4;
		szUnpacked = file->readUint16LE();
		wCompression = file->readUint16LE();
		//wCompression = szUnpacked;
		break;
	case kResVersionSci11:
		type = convertResType(file->readByte());
		number = file->readUint16LE();
		szPacked = file->readUint16LE();
		szUnpacked = file->readUint16LE();
		wCompression = file->readUint16LE();
		break;
#ifdef ENABLE_SCI32
	case kResVersionSci2:
	case kResVersionSci3:
		type = convertResType(file->readByte());
		number = file->readUint16LE();
		szPacked = file->readUint32LE();
		szUnpacked = file->readUint32LE();

		// The same comment applies here as in
		// detectVolVersion regarding SCI3. We ignore the
		// compression field for SCI3 games, but must presume
		// it exists in the file.
		wCompression = file->readUint16LE();

		if (volVersion == kResVersionSci3)
			wCompression = szPacked != szUnpacked ? 32 : 0;

		break;
#endif
	default:
		return SCI_ERROR_RESMAP_INVALID_ENTRY;
						 }

						 // check if there were errors while reading
						 if ((file->eos() || file->err()))
							 return SCI_ERROR_IO_ERROR;


						 // checking compression method
						 switch (wCompression) {
	case 0:
		compression = kCompNone;
		break;
	case 1:
		compression = (SCI_VERSION_1_1 <= SCI_VERSION_01) ? kCompLZW : kCompHuffman;
		break;
	case 2:
		compression = (SCI_VERSION_1_1 <= SCI_VERSION_01) ? kCompHuffman : kCompLZW1;
		break;
	case 3:
		compression = kCompLZW1View;
		break;
	case 4:
		compression = kCompLZW1Pic;
		break;
	case 18:
	case 19:
	case 20:
		compression = kCompDCL;
		break;
#ifdef ENABLE_SCI32
	case 32:
		compression = kCompSTACpack;
		break;
#endif
	default:
		compression = kCompUnknown;
						 }

						 return (compression == kCompUnknown) ? SCI_ERROR_UNKNOWN_COMPRESSION : SCI_ERROR_NONE;
}


ResourceType convertResType(byte type) {
	type &= 0x7f;

	int _mapVersion = kResVersionSci1Late;
	if (_mapVersion < kResVersionSci2) {
		// SCI0 - SCI2
		//if (type < ARRAYSIZE(s_resTypeMapSci0))
		return s_resTypeMapSci0[type];
	} 
	else {
		// SCI2.1+
		if (type < ARRAYSIZE(s_resTypeMapSci21)) {
			// LSL6 hires doesn't have the chunk resource type, to match
			// the resource types of the lowres version, thus we use the
			// older resource types here.
			// PQ4 CD and QFG4 CD are SCI2.1, but use the resource types of the
			// corresponding SCI2 floppy disk versions.
			//if (g_sci && (g_sci->getGameId() == GID_LSL6HIRES ||
			//	g_sci->getGameId() == GID_QFG4 || g_sci->getGameId() == GID_PQ4))
			//	return s_resTypeMapSci0[type];
			//else
				return s_resTypeMapSci21[type];
		}
	}

	return kResourceTypeInvalid;
}
