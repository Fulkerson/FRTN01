from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
from random import SystemRandom
from urlparse import urlparse, parse_qs

import json
import re
import os
import socket
import batchtank

DIR = 'batchtank/static'

class RequestHandler(BaseHTTPRequestHandler):
    def __init__(self):
        self._rand = SystemRandom()
        self.procsoc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.procsoc.connect(("localhost", 54000))

        BaseHttpRequestHandler.__init__(self)

    def getData():
        bm = batchtank.BaseMessage()
        bm.getSensor.append(batchtank.COOLER_RATE)
        bm.getSignal.append(batchtank.COOLER)

        bm.SerializeToSocket(self.procsoc)

        bm = batchtank.BaseMessage()
        bm.ParseFromSocket(self.procsoc)



    def do_GET(self):
        try:
            if (self.path == '/jquery.js' or self.path == '/jquery.flot.js'):
                f = open(DIR + self.path)

                self.send_response(200)
                self.send_header('Content-type', 'text/javascript')
                self.end_headers()

                self.wfile.write(f.read())
                f.close()
            elif (self.path.startswith('/data')):
                self.send_response(200)
                self.send_header('Cache-Control', 'no-cache, must-revalidate')
                self.send_header('Content-type', 'application/json;charset=utf-8')
                self.end_headers()
                query = parse_qs(urlparse(self.path)[4])
                print query
                out = json.dumps({
                    'ref': [[1, 1]],
                    'out': [[2, 2]],
                    'in': [[3, 3]]
                    })
                print out
                self.wfile.write(out)
            elif (self.path == '/' or self.path == '/index.html' or self.path == '/index.htm'):
                # Serve index!
                f = open(DIR + '/index.htm')

                self.send_response(200)
                self.send_header('Content-type', 'text/html')
                self.end_headers()

                self.wfile.write(f.read())
                f.close()
            else:
                self.send_error(404, 'Aeh naee... Di blidde 404: ' + self.path)
        except IOError:
            raise
            self.send_error(404, 'Aeh naee... Di blidde 404.')

def main():
    try:
        server = HTTPServer(('', 8080), RequestHandler)
        print 'Server running...'
        server.serve_forever()
    except KeyboardInterrupt:
        print 'Stopping server!'
        server.socket.close()

if __name__ == '__main__':
    main()
