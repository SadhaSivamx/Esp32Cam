from http.server import BaseHTTPRequestHandler, HTTPServer
import os

class FileUploadHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        file_data = self.rfile.read(content_length)
        # Create binary file named "img.jpg"
        with open("img.jpg", "wb") as f:
            f.write(file_data)
        self.send_response(200)
        self.end_headers()
        self.wfile.write(b'File data received successfully')

def run_server():
    host = '192.168.70.239'
    port = 5000

    server = HTTPServer((host, port), FileUploadHandler)
    print(f"Server started on http://{host}:{port}")

    try:
        server.serve_forever()
    except KeyboardInterrupt:
        server.shutdown()

if __name__ == '__main__':
    run_server()
