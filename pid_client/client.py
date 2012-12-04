#!/usr/bin/env python

import os
import signal
import subprocess
import pyinotify

PID_BINARY = "./pid_client"
PID_CONFIG = "pid_client.ini"


class EventHandler(pyinotify.ProcessEvent):
    def __init__(self, proc):
        self.proc = proc

    def process_IN_CLOSE_WRITE(self, event):
        if event.name == PID_CONFIG:
            self.proc.send_signal(signal.SIGUSR1)


def main():
    # Start process
    client = subprocess.Popen([PID_BINARY, PID_CONFIG])

    # Set up handling
    wm = pyinotify.WatchManager()
    handler = EventHandler(client)
    notifier = pyinotify.Notifier(wm, handler)

    config_dir = os.path.dirname(PID_CONFIG)

    # Watch config
    wm.add_watch(config_dir, pyinotify.ALL_EVENTS)

    try:
        notifier.loop()
    except:
        client.send_signal(signal.SIGTERM)
        raise


if __name__ == "__main__":
    main()

