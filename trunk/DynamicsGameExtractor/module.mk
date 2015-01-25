MODULE := engines/dgds

MODULE_OBJS = \
	resources/animation.o \
	resources/bmp.o \
	resources/font.o \
	resources/movie.o \
	resources/palette.o \
	resources/resource.o \
	resources/soundresource.o \
	resources/taggedresource.o \
	compression.o \
	detection.o \
	dgds.o \
	game.o \
	resourcemanager.o \
	test.o

# This module can be built as a plugin
ifdef BUILD_PLUGINS
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
