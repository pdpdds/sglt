// DynamicsGameExtractor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common/endian.h"
#include "common/file.h"
#include "common/stream.h"
#include "common/config-manager.h"
#include "common/system.h"
#include "decompressor.h"

#include "dgds/game.h"
#include "dgds/resourcemanager.h"

using namespace Dgds;
extern OSystem* g_system;

Common::String findGDS(ResourceManager *_resMgr) {
	Common::String name;

	// Search a GDS file inside the volumes
	ResourceFiles::const_iterator _resIter = _resMgr->_resourceFiles.begin();
	while (!name.size() && _resIter != _resMgr->_resourceFiles.end()) {
		if (_resIter->_key.hasSuffix(".GDS"))
			name = _resIter->_key;

		++_resIter;
	}

	// Get just the name without the extension
	if (name.size()) {
		name = Common::String(name.c_str(), name.size() - 4);
		name.toUppercase();
	}

	return name;
}


int _tmain(int argc, _TCHAR* argv[])
{
	ResourceManager *_resMgr = NULL;

	g_system = new OSystem();
	g_system->initBackend();

	Common::FSNode dir(ConfMan.get("path"));

	// Add the game path to the directory search list
	SearchMan.addDirectory(dir.getPath(), dir, 0, 4);

	// Start the resource manager
	if (Common::File::exists("volume.rmf"))
		_resMgr = new ResourceManager("volume.rmf");
	if (!_resMgr && Common::File::exists("volume.vga"))
		_resMgr = new ResourceManager("volume.vga");
	if (!_resMgr && Common::File::exists("resource.map"))
		_resMgr = new ResourceManager("resource.map");
	if (!_resMgr)
		_resMgr = new ResourceManager("");

	Common::String gameName = findGDS(_resMgr);
	if(!gameName.size())
		return -1;

	_resMgr->dumpResources(ConfMan.get("path"), false);
	return 0;
}

