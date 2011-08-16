ROOT= ../..
include $(ROOT)/mk/hdr.mk


PY=$(shell which python2 || echo python)

all:
	$(PY) setup.py build

clean:
	$(PY) setup.py clean

install:
	$(PY) setup.py install --prefix $(PREFIX)
