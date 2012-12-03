import os
import sys
import subprocess

def generate_proto(src, out):

    protopath, srcname = os.path.split(src)
    output = srcname.replace(".proto", "_pb2.py")
    if not os.path.exists(src):
        print "Can't find required file: " + src
        sys.exit(-1)

    if (os.path.exists(output) and
        os.path.exists(src) and
        os.path.getmtime(src) > os.path.getmtime(output)):
        return

    protoc_command = ["protoc", "--python_out=%s" % out, "-I" , protopath, src]
    if subprocess.call(protoc_command) != 0:
        sys.exit(1)

try:
    from batchtank_pb2 import *
except ImportError:
    generate_proto("../proto/batchtank.proto", "batchtank") 
    from batchtank_pb2 import *

##########################
##### Monkeypatching #####
##########################

# HACK: Java has something called writeDelimitedTo that prepends size
#       as a varint, python api does not so we patch it in.

import google.protobuf.message

if not 'SerializeToSocket' in dir(google.protobuf.message.Message):
    import google.protobuf.internal.encoder
    import io


    def _monkey_wdelimit(self, soc):
        msg = self.SerializeToString()
        google.protobuf.internal.encoder._EncodeVarint(soc.send, len(msg))
        #print "=================="
        #print "Sending %s bytes of data" % len(msg)
        #print self
        #print "=================="
        soc.send(msg)

    google.protobuf.message.Message.SerializeToSocket = _monkey_wdelimit

if not 'ParseFromSocket' in dir(google.protobuf.message.Message):
    import google.protobuf.internal.decoder

    class SocketBuffer(object):
        def __init__(self, soc):
            self._buffer = io.BytesIO()
            self._soc = soc

        def __getitem__(self, pos):
            while len(self._buffer.getvalue()) < (pos + 1):
                self._buffer.write(self._soc.recv(1))
                self._soc.setblocking(1)
            return self._buffer.getvalue()[pos]

    def _monkey_pdelimit(self, soc):
        old = soc.gettimeout()
        buf = SocketBuffer(soc)
        nbytes, _ = google.protobuf.internal.decoder._DecodeVarint(buf, 0)
        #print "=================="
        #print "Receiving %d bytes of data." % nbytes
        soc.settimeout(0.0)
        msg = soc.recv(nbytes)
        soc.settimeout(old)
        self.ParseFromString(msg)
        #print self
        #print "=================="

    google.protobuf.message.Message.ParseFromSocket = _monkey_pdelimit


##########################
######### END ############
##########################


