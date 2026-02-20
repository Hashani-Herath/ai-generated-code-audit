#!/usr/bin/env python3
"""
JWT Token Decoder
Decodes and prints JWT token payload using PyJWT
"""

import jwt
import json
import sys
from datetime import datetime
from typing import Optional, Dict, Any


# ============================================
# BASIC JWT DECODE FUNCTION
# ============================================

def decode_jwt(token: str, secret: Optional[str] = None, verify: bool = False) -> Dict:
    """
    Decode a JWT token and return its payload.
    
    Args:
        token: JWT token string
        secret: Secret key for verification (optional)
        verify: Whether to verify signature (default: False)
    
    Returns:
        Dict: Decoded payload
    """
    try:
        if verify and secret:
            # Verify signature
            payload = jwt.decode(token, secret, algorithms=['HS256'])
        else:
            # Just decode without verification
            payload = jwt.decode(token, options={"verify_signature": False})
        
        return payload
        
    except jwt.ExpiredSignatureError:
        print("❌ Token has expired")
        raise
    except jwt.InvalidTokenError as e:
        print(f"❌ Invalid token: {e}")
        raise


# ============================================
# DECODE AND PRINT PAYLOAD
# ============================================

def decode_and_print_jwt(token: str, secret: Optional[str] = None):
    """
    Decode a JWT token and print its payload in a formatted way.
    
    Args:
        token: JWT token string
        secret: Secret key for verification (optional)
    """
    print("=" * 60)
    print("🔐 JWT TOKEN DECODER")
    print("=" * 60)
    
    try:
        # Try to decode with verification if secret provided
        if secret:
            print(f"\n📝 Verifying with secret: {secret[:4]}...{secret[-4:] if len(secret) > 8 else ''}")
            payload = jwt.decode(token, secret, algorithms=['HS256', 'RS256'])
        else:
            print("\n⚠️  Decoding without signature verification")
            payload = jwt.decode(token, options={"verify_signature": False})
        
        # Print token info
        print("\n📋 DECODED PAYLOAD:")
        print("-" * 40)
        
        # Pretty print the payload
        print_json_pretty(payload)
        
        # Print header information
        header = jwt.get_unverified_header(token)
        print("\n📋 HEADER:")
        print("-" * 40)
        print_json_pretty(header)
        
        # Print token parts
        parts = token.split('.')
        print("\n📋 TOKEN PARTS:")
        print("-" * 40)
        print(f"  Header:     {parts[0][:30]}...")
        print(f"  Payload:    {parts[1][:30]}...")
        print(f"  Signature:  {parts[2][:30]}..." if len(parts) > 2 else "  No signature")
        
        # Analyze standard claims
        analyze_claims(payload)
        
    except jwt.ExpiredSignatureError:
        print("\n❌ ERROR: Token has expired")
        # Still show payload
        try:
            payload = jwt.decode(token, options={"verify_signature": False})
            print("\n📋 EXPIRED PAYLOAD (for debugging):")
            print_json_pretty(payload)
        except:
            pass
            
    except jwt.InvalidSignatureError:
        print("\n❌ ERROR: Invalid signature")
        
    except jwt.InvalidTokenError as e:
        print(f"\n❌ ERROR: Invalid token - {e}")
        
    except Exception as e:
        print(f"\n❌ ERROR: {e}")


# ============================================
# VERIFY AND DECODE
# ============================================

def verify_and_decode(token: str, secret: str, algorithms: list = ['HS256']) -> Optional[Dict]:
    """
    Verify and decode a JWT token.
    
    Args:
        token: JWT token
        secret: Secret key
        algorithms: List of allowed algorithms
    
    Returns:
        Dict: Decoded payload or None if invalid
    """
    try:
        payload = jwt.decode(
            token,
            secret,
            algorithms=algorithms,
            options={
                'verify_signature': True,
                'verify_exp': True,
                'verify_nbf': True,
                'verify_iat': True,
                'verify_aud': True,
                'verify_iss': True
            }
        )
        return payload
        
    except jwt.ExpiredSignatureError:
        print("❌ Token has expired")
    except jwt.InvalidAudienceError:
        print("❌ Invalid audience")
    except jwt.InvalidIssuerError:
        print("❌ Invalid issuer")
    except jwt.InvalidTokenError as e:
        print(f"❌ Invalid token: {e}")
    
    return None


# ============================================
# DECODE WITH OPTIONS
# ============================================

def decode_with_options(token: str, secret: str = None, **options):
    """
    Decode JWT with custom options.
    
    Args:
        token: JWT token
        secret: Secret key
        **options: Additional decode options
    
    Returns:
        Dict: Decoded payload
    """
    decode_options = {
        'verify_exp': True,
        'verify_nbf': True,
        'verify_iat': True,
        'verify_aud': False,
        'verify_iss': False,
        'require': []
    }
    
    # Update with provided options
    decode_options.update(options)
    
    try:
        if secret:
            payload = jwt.decode(
                token,
                secret,
                algorithms=['HS256', 'RS256', 'ES256'],
                options=decode_options
            )
        else:
            payload = jwt.decode(
                token,
                options={"verify_signature": False}
            )
        
        return payload
        
    except jwt.PyJWTError as e:
        print(f"❌ Decode error: {e}")
        return None


# ============================================
# UTILITY FUNCTIONS
# ============================================

def print_json_pretty(data: Dict):
    """Print JSON data in a pretty format."""
    print(json.dumps(data, indent=2, default=str))


def analyze_claims(payload: Dict):
    """Analyze and explain standard JWT claims."""
    claims = []
    
    if 'exp' in payload:
        exp_time = datetime.fromtimestamp(payload['exp'])
        now = datetime.now()
        time_left = exp_time - now
        status = "✅ Valid" if exp_time > now else "❌ Expired"
        claims.append(f"  exp (Expiration): {exp_time} [{status}]")
    
    if 'iat' in payload:
        iat_time = datetime.fromtimestamp(payload['iat'])
        claims.append(f"  iat (Issued At): {iat_time}")
    
    if 'nbf' in payload:
        nbf_time = datetime.fromtimestamp(payload['nbf'])
        status = "✅ Valid" if nbf_time <= datetime.now() else "⏳ Not yet valid"
        claims.append(f"  nbf (Not Before): {nbf_time} [{status}]")
    
    if 'iss' in payload:
        claims.append(f"  iss (Issuer): {payload['iss']}")
    
    if 'aud' in payload:
        claims.append(f"  aud (Audience): {payload['aud']}")
    
    if 'sub' in payload:
        claims.append(f"  sub (Subject): {payload['sub']}")
    
    if 'jti' in payload:
        claims.append(f"  jti (JWT ID): {payload['jti']}")
    
    if claims:
        print("\n📋 STANDARD CLAIMS:")
        print("-" * 40)
        for claim in claims:
            print(claim)


def get_token_info(token: str) -> Dict:
    """
    Get information about a JWT token without verification.
    
    Args:
        token: JWT token
    
    Returns:
        Dict: Token information
    """
    try:
        header = jwt.get_unverified_header(token)
        payload = jwt.decode(token, options={"verify_signature": False})
        
        return {
            'valid_format': True,
            'algorithm': header.get('alg', 'unknown'),
            'type': header.get('typ', 'JWT'),
            'payload': payload,
            'header': header,
            'parts': len(token.split('.'))
        }
        
    except Exception as e:
        return {
            'valid_format': False,
            'error': str(e)
        }


# ============================================
# COMMAND LINE INTERFACE
# ============================================

def main():
    """Command line interface for JWT decoding."""
    import argparse
    
    parser = argparse.ArgumentParser(description='Decode and print JWT token')
    parser.add_argument('token', help='JWT token to decode')
    parser.add_argument('-s', '--secret', help='Secret key for verification')
    parser.add_argument('-v', '--verify', action='store_true', 
                       help='Verify signature (requires secret)')
    parser.add_argument('-j', '--json', action='store_true',
                       help='Output raw JSON payload only')
    parser.add_argument('--header', action='store_true',
                       help='Print header only')
    parser.add_argument('--payload', action='store_true',
                       help='Print payload only')
    
    args = parser.parse_args()
    
    if args.json:
        # JSON only output
        try:
            if args.verify and args.secret:
                payload = jwt.decode(args.token, args.secret, algorithms=['HS256'])
            else:
                payload = jwt.decode(args.token, options={"verify_signature": False})
            
            if args.header:
                header = jwt.get_unverified_header(args.token)
                print(json.dumps(header))
            else:
                print(json.dumps(payload, default=str))
                
        except Exception as e:
            print(json.dumps({'error': str(e)}))
            sys.exit(1)
    
    else:
        # Pretty print output
        if args.verify and args.secret:
            decode_and_print_jwt(args.token, args.secret)
        else:
            decode_and_print_jwt(args.token)


# ============================================
# EXAMPLE USAGE
# ============================================

if __name__ == "__main__":
    if len(sys.argv) > 1:
        # Command line mode
        main()
    else:
        # Demo mode
        print("=" * 60)
        print("JWT DECODER DEMO")
        print("=" * 60)
        
        # Create a sample JWT token for demonstration
        import time
        
        # Sample payload
        payload = {
            'user_id': 12345,
            'username': 'john_doe',
            'role': 'admin',
            'email': 'john@example.com',
            'iat': int(time.time()),
            'exp': int(time.time()) + 3600,  # Expires in 1 hour
            'iss': 'myapp.com',
            'sub': 'user-authentication'
        }
        
        # Secret key
        secret = 'my-secret-key-123'
        
        # Encode a sample token
        sample_token = jwt.encode(payload, secret, algorithm='HS256')
        
        print(f"\n📝 Sample JWT Token:")
        print(f"{sample_token}\n")
        
        # Decode without verification
        decode_and_print_jwt(sample_token)
        
        # Decode with verification
        print("\n" + "=" * 60)
        decode_and_print_jwt(sample_token, secret)
        
        # Test with invalid token
        print("\n" + "=" * 60)
        print("Testing with invalid token:")
        decode_and_print_jwt(sample_token + "invalid")
        
        # Get token info
        print("\n" + "=" * 60)
        info = get_token_info(sample_token)
        print(f"Token Info: {json.dumps(info, indent=2, default=str)}")
        
        print("\n" + "=" * 60)
        print("Command line examples:")
        print("  python jwt_decoder.py <token>")
        print("  python jwt_decoder.py <token> -s mysecret -v")
        print("  python jwt_decoder.py <token> --json")
        print("=" * 60)