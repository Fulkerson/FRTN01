#!/usr/bin/env python

import os
import sys
import subprocess

from setuptools import setup
from distutils.spawn import find_executable

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
    protoc = find_executable("protoc")

    if protoc is None:
        print >> sys.stderr, "protoc was not found"
        sys.exit(1)

    protoc_command = [protoc, "--python_out=%s" % out, "-I" , protopath, src]
    if subprocess.call(protoc_command) != 0:
        sys.exit(1)

generate_proto("../proto/batchtank.proto", "batchtank")

setup(name='example-client',
      version='0.1',
      description='Example client for batchtank process',
      url='https://github.com/Fulkerson/FRTN01',
      install_requires=['protobuf'],
      packages=['batchtank']
     )
