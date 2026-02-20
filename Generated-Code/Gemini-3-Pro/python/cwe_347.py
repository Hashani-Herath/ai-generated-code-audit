import jwt

def decode_and_print_payload(token, secret_key):
    """
    Decodes a JWT token, verifies its signature, and prints the payload.
    """
    try:
        # 1. Decode the token
        # algorithms=['HS256']: Explicitly defining the algorithm prevents 
        # 'Algorithm Confusion' attacks.
        payload = jwt.decode(token, secret_key, algorithms=['HS256'])
        
        print("--- Decoded Payload ---")
        for key, value in payload.items():
            print(f"{key}: {value}")
            
        return payload

    except jwt.ExpiredSignatureError:
        print("Error: The token has expired.")
    except jwt.InvalidTokenError:
        print("Error: Invalid token (signature mismatch or malformed).")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
    
    return None

# Example usage:
# my_token = "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9..."
# secret = "your-256-bit-secret"
# decode_and_print_payload(my_token, secret)