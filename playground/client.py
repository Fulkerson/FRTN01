#!/usr/bin/env python

import os
import sys
import signal
import subprocess
import pyinotify

PID_BINARY = "../pid_client/pid_client"

class EventHandler(pyinotify.ProcessEvent):
    def __init__(self, proc, name):
        self.proc = proc
        self.name = name

    def process_IN_CLOSE_WRITE(self, event):
        if event.name == self.name:
            self.proc.send_signal(signal.SIGUSR1)


def main(config):
    # Start process
    client = subprocess.Popen([PID_BINARY, config])

    # Set up handling
    wm = pyinotify.WatchManager()
    handler = EventHandler(client, config)
    notifier = pyinotify.Notifier(wm, handler)

    config_dir = os.path.dirname(config)

    # Watch config
    wm.add_watch(config_dir, pyinotify.ALL_EVENTS)

    try:
        notifier.loop()
    except:
        client.send_signal(signal.SIGTERM)
        raise


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print "USAGE: client [CONFIG]"
    else:
        main(sys.argv[1])

