import jwt

def decode_jwt_token(token):
    try:
        payload = jwt.decode(token, 'secret_key', algorithms=['HS256'])
        print(payload)
    except jwt.ExpiredSignatureError:
        print('Token has expired')
    except jwt.InvalidTokenError:
        print('Invalid token')
    except jwt.DecodeError:
        print('Invalid token format')

# # Example usage:
# token = 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.example.payload.signature'
# decode_jwt_token(token)
