#!/usr/bin/env python
import IPython
from IPython.config.loader import Config
cfg = Config()


import socket
import batchtank
import ConfigParser

namespace = {}

namespace["MIXER"] = batchtank.MIXER
namespace["MIXER_RATE"] = batchtank.MIXER_RATE
namespace["OUT_PUMP"] = batchtank.OUT_PUMP
namespace["OUT_PUMP_RATE"] = batchtank.OUT_PUMP_RATE
namespace["IN_PUMP"] = batchtank.IN_PUMP
namespace["IN_PUMP_RATE"] = batchtank.IN_PUMP_RATE
namespace["LEVEL"] = batchtank.LEVEL
namespace["TEMP"] = batchtank.TEMP
namespace["COOLER"] = batchtank.COOLER
namespace["COOLER_RATE"] = batchtank.COOLER_RATE
namespace["HEATER"] = batchtank.HEATER
namespace["HEATER_RATE"] = batchtank.HEATER_RATE

class Controller(object):
    def __init__(self, confpath):
        ini = ConfigParser.ConfigParser()
        ini.read(confpath)
        port = ini.getint("General", "port")
        hostname = ini.get("General", "hostname")
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((hostname, port))

    def set(self, output, value):
        if value > 255:
            value = 255
        elif value < 0:
            value = 0
        bm = batchtank.BaseMessage()
        sig = bm.signal.add()
        sig.ref = value
        sig.value = value
        sig.type = output
        bm.SerializeToSocket(self.sock)

    def get(self, sensor):
        bm = batchtank.BaseMessage()
        bm.getSensor.append(sensor)
        bm.SerializeToSocket(self.sock)
        bm.Clear()
        bm.ParseFromSocket(self.sock)
        print bm

    def getall(self):
        bm = batchtank.BaseMessage()
        for _, s in namespace.items():
            bm.getSensor.append(s)
        bm.SerializeToSocket(self.sock)
        bm.Clear()
        bm.ParseFromSocket(self.sock)
        print bm

namespace["proc"] = Controller("config.ini")

banner = """Usage:
proc.get(sensor)
proc.set(output, value)
sensor = MIXER_RATE | HEATER_RATE | COOLER_RATE | IN_PUMP_RATE | OUT_PUMP_RATE | LEVEL | TEMP
output = MIXER | HEATER | COOLER | IN_PUMP | OUT_PUMP

Example:
  proc.get(MIXER_RATE)
"""

IPython.embed(config=cfg, user_ns=namespace, banner2=banner)

