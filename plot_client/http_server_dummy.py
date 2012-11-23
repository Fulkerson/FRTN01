from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
from random import SystemRandom
import json
import re

DIR = '../plot/'

class RequestHandler(BaseHTTPRequestHandler):
    _rand = SystemRandom()
    def do_GET(self):
        try:
            if (self.path == '/jquery.js' or self.path == '/jquery.flot.js'):
                f = open(DIR + self.path)

                self.send_response(200)
                self.send_header('Content-type', 'text/javascript')
                self.end_headers()

                self.wfile.write(f.read())
                f.close()
            elif (self.path.startswith('/data.php')):
                self.send_response(200)
                self.send_header('Cache-Control', 'no-cache, must-revalidate')
                self.send_header('Content-type', 'application/json;charset=utf-8')
                self.end_headers()

		out = '{"heater":{"ref":[' + str(int(self._rand.random() * 30 + 20)) + '],"out":[' + str(int(self._rand.random() * 30 + 20)) + '],"in":[' + str(int(self._rand.random() * 30 + 20)) + ']},"blender":{"ref":[' + str(int(self._rand.random() * 30 + 20)) + '],"out":[' + str(int(self._rand.random() * 30 + 20)) + '],"in":[' + str(int(self._rand.random() * 30 + 20)) + ']}}';
                print out
                self.wfile.write(out)
            elif (self.path == '/' or self.path == '/index.html' or self.path == '/index.htm'):
                # Serve index!
                f = open(DIR + 'index.htm')

                self.send_response(200)
                self.send_header('Content-type', 'text/html')
                self.end_headers()

                self.wfile.write(f.read())
                f.close()
            else:
                self.send_error(404, 'Aeh naee... Di blidde 404: ' + self.path)
        except IOError:
            self.send_error(404, 'Aeh naee... Di blidde 404.')

def main():
    try:
        server = HTTPServer(('', 80), RequestHandler)
        print 'Server running...'
        server.serve_forever()
    except KeyboardInterrupt:
        print 'Stopping server!'
        server.socket.close()

if __name__ == '__main__':
    main()

