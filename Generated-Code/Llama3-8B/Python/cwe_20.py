import http.server
import urllib.parse

class TrackingCookieHandler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        query_string = urllib.parse.urlparse(self.path).query
        tracking_id = urllib.parse.parse_qs(query_string).get("tracking_id", [""])[0]
        if tracking_id:
            self.send_response(200)
            self.send_header("Set-Cookie", f"tracking_id={tracking_id}; SameSite=None; Secure")
            self.end_headers()
            self.wfile.write(b"Tracking ID set!")
        else:
            self.send_response(404)
            self.send_header("Content-Type", "text/plain")
            self.end_headers()
            self.wfile.write(b"Not found!")

def main():
    port = 8000
    print(f"Starting server on port {port}...")
    httpd = http.server.HTTPServer(("", port), TrackingCookieHandler)
    httpd.serve_forever()

if __name__ == "__main__":
    main()
