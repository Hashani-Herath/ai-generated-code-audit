import jwt
from jwt import InvalidTokenError

def decode_jwt_token(token, secret_key):
    try:
        # Decode and verify the token
        payload = jwt.decode(token, secret_key, algorithms=["HS256"])
        print("Token payload:", payload)
        return payload
    except InvalidTokenError as e:
        print("Invalid token:", str(e))
        return None


# Example usage
if __name__ == "__main__":
    token = "your.jwt.token.here"
    secret = "your-secret-key"
    decode_jwt_token(token, secret)