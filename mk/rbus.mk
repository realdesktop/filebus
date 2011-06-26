VERSION = 0.1

COPYRIGHT = ©2011 Ilya Petrov

$(ROOT)/include/ixp.h: $(ROOT)/config.mk
CFLAGS += '-DVERSION=\"$(VERSION)\"' '-DCOPYRIGHT=\"$(COPYRIGHT)\"'

