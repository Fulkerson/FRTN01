import socket
import asyncore
import time

import batchtank_pb2

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

class TestClient(asyncore.dispatcher):
    def __init__(self, host, port):
        asyncore.dispatcher.__init__(self)
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.connect((host, port))
        self.register = True

    def handle_read(self):
        # self is a TCP socket 
        print
        print "==== handle_read ===="
        bm = batchtank_pb2.BaseMessage()
        bm.ParseFromSocket(self)
        print bm
        print "======== END ========"
        print

    def handle_write(self):
    #    return
        #print
        #print "==== handle_write ===="
    #    for i in xrange(20):
    #        bm = batchtank_pb2.BaseMessage()
    #        bm.register.periodTime = i*1000
    #        bm.SerializeToSocket(self)
    #        print bm
    #        time.sleep(1)
        if self.register:
            bm = batchtank_pb2.BaseMessage()
            reg = batchtank_pb2.Register()
            bm.register.periodTime = 1000
            sensors = bm.register.type.append(batchtank_pb2.HEATERSENSOR)
            bm.SerializeToSocket(self)
            self.register = False
        #print bm
        #print "======== END ========="
        #print



if __name__ == "__main__":
    TestClient("localhost", 54000)
    asyncore.loop()

