VERSION = 0.1

COPYRIGHT = ©2011 Ilya Petrov

$(ROOT)/include/rbus.h: $(ROOT)/config.mk
CFLAGS += '-DVERSION=\"$(VERSION)\"' '-DCOPYRIGHT=\"$(COPYRIGHT)\"'

