import http.cookies

def set_session_cookie(response, session_id):
    cookie = http.cookies.SimpleCookie()
    cookie['session_id'] = session_id
    response.headers['Set-Cookie'] = str(cookie)
