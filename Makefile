ROOT=.
include $(ROOT)/mk/hdr.mk
include $(ROOT)/mk/rbus.mk

DIRS =	lib	\
	include

.PHONY: doc
include $(ROOT)/mk/dir.mk

