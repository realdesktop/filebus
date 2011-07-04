import sys
#sys.path.insert(0, '/home/muromec/source/rbus/lib/python-rbus/build/lib.linux-armv7l-2.7')

import rbus
import time

class rbus_property(property):
    RBUS_PROP = True

r = rbus.RbusRoot(address="unix!/tmp/py.9p")
r.children_types = ['neko', 'baka']

class Nya(object):
    RBUS_TYPE = 'neko'
    __name__ = 'nyaaaa'

    @rbus_property
    def name(self):
        print 'ya here'
        return 'yoba'

    @rbus_property
    def id(self):
        return hex(id(self))

n = Nya()

r.append_child(n)

while True:
    sys.stderr.write('.')
    r.run()


