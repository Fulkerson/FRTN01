#!/usr/bin/env python
import socket
import batchtank
import ConfigParser


class BatchProcess(object):
    """Used to control parts of the batch process.

    Properties:
        mixer    -- Rate of mixer    (0..255)
        out_pump -- Rate of out pump (0..255)
        in_pump  -- Rate of in pump  (0..255)
        cooler   -- Rate of cooler   (0..255)
        heater   -- Rate of heater   (0..255)
        temp     -- Temperature      (???)
        level    -- Level            (???)


    Example:
        >>> tank = BatchProcess(socket)

        >>> tank.mixer = 255
        >>> print tank.mixer
        255
        >>> tank.mixer = 50
        >>> print tank.mixer
        50
    
        >>> print tank.temp
        340
    """
    def __init__(self, socket):
        """
        Args:
            socket -- socket ready for transceiving
        """
        self.sock = socket

    @property
    def mixer(self):
        return self._get(batchtank.MIXER_RATE)

    @mixer.setter
    def mixer(self, value):
        self._set(batchtank.MIXER, value)

    @property
    def out_pump(self):
        return self._get(batchtank.OUT_PUMP_RATE)

    @out_pump.setter
    def out_pump(self, value):
        self._set(batchtank.OUT_PUMP, value)

    @property
    def in_pump(self):
        return self._get(batchtank.IN_PUMP_RATE)

    @in_pump.setter
    def in_pump(self, value):
        self._set(batchtank.IN_PUMP, value)

    @property
    def cooler(self):
        return self._get(batchtank.COOLER_RATE)

    @cooler.setter
    def cooler(self, value):
        self._set(batchtank.COOLER, value)

    @property
    def heater(self):
        return self._get(batchtank.HEATER_RATE)

    @heater.setter
    def heater(self, value):
        self._set(batchtank.HEATER, value)

    @property
    def temp(self):
        return self._get(batchtank.TEMP)

    @property
    def level(self):
        return self._get(batchtank.LEVEL)

    def _set(self, output, value):
        if value > 255:
            value = 255
        elif value < 0:
            value = 0
        bm = batchtank.BaseMessage()
        sig = bm.signal.add()
        sig.ref = value
        sig.value = value
        sig.type = output
        print bm
        return None
        bm.SerializeToSocket(self.sock)

    def _get(self, sensor):
        bm = batchtank.BaseMessage()
        bm.getSensor.append(sensor)

        bm.SerializeToSocket(self.sock)
        bm.Clear()
        bm.ParseFromSocket(self.sock)

        return bm.sample[0].value


if __name__ == "__main__":
    import IPython
    from IPython.config.loader import Config
    cfg = Config()

    ini = ConfigParser.ConfigParser()
    ini.read("config.ini")
    port = ini.getint("General", "port")
    hostname = ini.get("General", "hostname")
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print "Connecting to %s:%d" % (hostname, port)
    sock.connect((hostname, port))
    print "Connected"

    namespace = {"tank" : BatchProcess(sock)}
    banner = namespace["tank"].__doc__
    banner += ("\nAn instance of the process have been started and is "
               "available in your\nnamespace by the name 'tank'")

    IPython.embed(config=cfg, user_ns=namespace, banner2=banner)

