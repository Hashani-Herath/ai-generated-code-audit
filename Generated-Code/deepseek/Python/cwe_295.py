#!/usr/bin/env python3
"""
Fetch data from internal HTTPS site with self-signed certificate
"""

import requests
import urllib3
import ssl
import certifi
import os
import sys
import json
from pathlib import Path
import warnings
from typing import Optional, Dict, Any, Union
import argparse


# ============================================
# METHOD 1: DISABLE SSL VERIFICATION (INSECURE)
# ============================================

def fetch_without_verification(url: str, timeout: int = 10) -> Optional[requests.Response]:
    """
    Fetch data WITHOUT SSL certificate verification.
    WARNING: This is insecure - only use for testing!
    
    Args:
        url: HTTPS URL to fetch
        timeout: Request timeout in seconds
    
    Returns:
        Response object or None if failed
    """
    # Suppress only the InsecureRequestWarning
    urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)
    
    try:
        response = requests.get(
            url,
            verify=False,  # Disable SSL verification
            timeout=timeout
        )
        response.raise_for_status()
        return response
        
    except requests.exceptions.SSLError as e:
        print(f"❌ SSL Error: {e}")
    except requests.exceptions.ConnectionError as e:
        print(f"❌ Connection Error: {e}")
    except requests.exceptions.Timeout as e:
        print(f"❌ Timeout Error: {e}")
    except requests.exceptions.RequestException as e:
        print(f"❌ Request Error: {e}")
    
    return None


# ============================================
# METHOD 2: USE CUSTOM CA BUNDLE
# ============================================

def fetch_with_custom_ca(url: str, ca_bundle_path: str, timeout: int = 10) -> Optional[requests.Response]:
    """
    Fetch data using a custom CA certificate bundle.
    
    Args:
        url: HTTPS URL to fetch
        ca_bundle_path: Path to CA bundle file
        timeout: Request timeout
    
    Returns:
        Response object or None
    """
    try:
        response = requests.get(
            url,
            verify=ca_bundle_path,  # Use custom CA bundle
            timeout=timeout
        )
        response.raise_for_status()
        return response
        
    except requests.exceptions.SSLError as e:
        print(f"❌ SSL Error: {e}")
        print("   Make sure your CA bundle contains the correct certificate")
    except Exception as e:
        print(f"❌ Error: {e}")
    
    return None


# ============================================
# METHOD 3: USE SELF-SIGNED CERTIFICATE FILE
# ============================================

def fetch_with_self_signed_cert(url: str, cert_path: str, timeout: int = 10) -> Optional[requests.Response]:
    """
    Fetch data using a specific self-signed certificate file.
    
    Args:
        url: HTTPS URL to fetch
        cert_path: Path to the self-signed certificate file
        timeout: Request timeout
    """
    try:
        response = requests.get(
            url,
            verify=cert_path,  # Use specific certificate
            timeout=timeout
        )
        response.raise_for_status()
        return response
        
    except requests.exceptions.SSLError as e:
        print(f"❌ SSL Error: {e}")
        print(f"   Certificate path: {cert_path}")
    except Exception as e:
        print(f"❌ Error: {e}")
    
    return None


# ============================================
# METHOD 4: ADD CERTIFICATE TO EXISTING CA BUNDLE
# ============================================

def fetch_with_merged_ca(url: str, cert_path: str, timeout: int = 10) -> Optional[requests.Response]:
    """
    Merge self-signed certificate with system CA bundle and use it.
    
    Args:
        url: HTTPS URL to fetch
        cert_path: Path to self-signed certificate
        timeout: Request timeout
    """
    try:
        # Create custom CA bundle file
        custom_bundle = create_custom_ca_bundle(cert_path)
        
        response = requests.get(
            url,
            verify=custom_bundle,
            timeout=timeout
        )
        response.raise_for_status()
        return response
        
    except Exception as e:
        print(f"❌ Error: {e}")
        return None


def create_custom_ca_bundle(cert_path: str) -> str:
    """
    Create a custom CA bundle by merging system certs with self-signed cert.
    
    Args:
        cert_path: Path to self-signed certificate
    
    Returns:
        Path to custom bundle file
    """
    custom_bundle = Path('/tmp/custom_ca_bundle.pem')
    
    # Read system CA bundle
    system_bundle = certifi.where()
    
    with open(custom_bundle, 'w') as outfile:
        # Write system certificates
        with open(system_bundle, 'r') as infile:
            outfile.write(infile.read())
        
        # Write custom certificate
        outfile.write('\n# Custom Self-Signed Certificate\n')
        with open(cert_path, 'r') as infile:
            outfile.write(infile.read())
    
    return str(custom_bundle)


# ============================================
# METHOD 5: USE SESSION WITH CUSTOM SSL CONTEXT
# ============================================

def fetch_with_ssl_context(url: str, cert_path: Optional[str] = None, timeout: int = 10) -> Optional[requests.Response]:
    """
    Fetch data using a custom SSL context.
    
    Args:
        url: HTTPS URL
        cert_path: Optional path to certificate
        timeout: Request timeout
    """
    try:
        # Create custom SSL context
        ssl_context = ssl.create_default_context()
        
        if cert_path:
            # Load custom certificate
            ssl_context.load_verify_locations(cert_path)
        else:
            # Or disable certificate verification
            ssl_context.check_hostname = False
            ssl_context.verify_mode = ssl.CERT_NONE
        
        # Create session with custom adapter
        session = requests.Session()
        session.mount('https://', CustomHTTPAdapter(ssl_context))
        
        response = session.get(url, timeout=timeout)
        response.raise_for_status()
        return response
        
    except Exception as e:
        print(f"❌ Error: {e}")
        return None


class CustomHTTPAdapter(requests.adapters.HTTPAdapter):
    """Custom HTTP adapter with SSL context."""
    
    def __init__(self, ssl_context=None, **kwargs):
        self.ssl_context = ssl_context
        super().__init__(**kwargs)
    
    def init_poolmanager(self, *args, **kwargs):
        kwargs['ssl_context'] = self.ssl_context
        return super().init_poolmanager(*args, **kwargs)


# ============================================
# METHOD 6: BASIC AUTH WITH SELF-SIGNED CERT
# ============================================

def fetch_with_auth(url: str, username: str, password: str, 
                   cert_path: Optional[str] = None, 
                   verify_ssl: bool = False) -> Optional[requests.Response]:
    """
    Fetch data with basic authentication and self-signed certificate.
    
    Args:
        url: HTTPS URL
        username: Username for basic auth
        password: Password for basic auth
        cert_path: Path to certificate (optional)
        verify_ssl: Whether to verify SSL
    """
    try:
        session = requests.Session()
        session.auth = (username, password)
        
        if cert_path:
            response = session.get(url, verify=cert_path)
        else:
            # Disable SSL verification
            urllib3.disable_warnings()
            response = session.get(url, verify=verify_ssl)
        
        response.raise_for_status()
        return response
        
    except Exception as e:
        print(f"❌ Error: {e}")
        return None


# ============================================
# HELPER FUNCTIONS
# ============================================

def download_certificate(host: str, port: int = 443, output_file: str = 'server.crt'):
    """
    Download SSL certificate from server.
    
    Args:
        host: Server hostname
        port: Server port
        output_file: Output file for certificate
    """
    import socket
    import ssl
    
    try:
        # Create socket connection
        context = ssl.create_default_context()
        context.check_hostname = False
        context.verify_mode = ssl.CERT_NONE
        
        with socket.create_connection((host, port)) as sock:
            with context.wrap_socket(sock, server_hostname=host) as ssock:
                cert = ssock.getpeercert(binary_form=True)
                
                # Convert to PEM format
                from cryptography import x509
                from cryptography.hazmat.backends import default_backend
                
                cert_obj = x509.load_der_x509_certificate(cert, default_backend())
                pem_data = cert_obj.public_bytes(encoding=serialization.Encoding.PEM)
                
                with open(output_file, 'wb') as f:
                    f.write(pem_data)
                
                print(f"✅ Certificate downloaded to {output_file}")
                return output_file
                
    except Exception as e:
        print(f"❌ Failed to download certificate: {e}")
        return None


def save_response(response: requests.Response, output_file: str = None):
    """
    Save response content to file.
    
    Args:
        response: Response object
        output_file: Output file path
    """
    if not output_file:
        # Generate filename from URL
        import re
        from urllib.parse import urlparse
        
        parsed = urlparse(response.url)
        path = parsed.path.strip('/').replace('/', '_') or 'index'
        output_file = f"{parsed.netloc}_{path}.html"
    
    with open(output_file, 'wb') as f:
        f.write(response.content)
    
    print(f"✅ Response saved to {output_file}")


def print_response_info(response: requests.Response):
    """
    Print response information.
    
    Args:
        response: Response object
    """
    print("\n📋 Response Information:")
    print(f"   Status Code: {response.status_code}")
    print(f"   URL: {response.url}")
    print(f"   Encoding: {response.encoding}")
    print(f"   Content-Type: {response.headers.get('content-type', 'unknown')}")
    print(f"   Content-Length: {len(response.content)} bytes")
    
    # Try to parse JSON
    if 'application/json' in response.headers.get('content-type', ''):
        try:
            data = response.json()
            print(f"\n📊 JSON Data Preview:")
            print(json.dumps(data, indent=2)[:500] + "...")
        except:
            pass


# ============================================
# MAIN FUNCTION
# ============================================

def main():
    parser = argparse.ArgumentParser(description='Fetch data from HTTPS site with self-signed certificate')
    parser.add_argument('url', help='HTTPS URL to fetch')
    parser.add_argument('--cert', '-c', help='Path to certificate file')
    parser.add_argument('--insecure', '-k', action='store_true', 
                       help='Disable SSL verification (INSECURE)')
    parser.add_argument('--output', '-o', help='Output file for response')
    parser.add_argument('--timeout', '-t', type=int, default=10,
                       help='Request timeout in seconds')
    parser.add_argument('--username', '-u', help='Username for basic auth')
    parser.add_argument('--password', '-p', help='Password for basic auth')
    parser.add_argument('--download-cert', action='store_true',
                       help='Download certificate from server')
    parser.add_argument('--verbose', '-v', action='store_true',
                       help='Verbose output')
    
    args = parser.parse_args()
    
    # Download certificate if requested
    if args.download_cert:
        from urllib.parse import urlparse
        parsed = urlparse(args.url)
        host = parsed.hostname
        cert_file = f"{host}.crt"
        download_certificate(host, 443, cert_file)
        if args.cert is None:
            args.cert = cert_file
    
    # Choose method based on arguments
    if args.username and args.password:
        # Basic auth
        response = fetch_with_auth(
            args.url, 
            args.username, 
            args.password,
            cert_path=args.cert,
            verify_ssl=not args.insecure
        )
    elif args.cert:
        # Use specific certificate
        response = fetch_with_self_signed_cert(args.url, args.cert, args.timeout)
    elif args.insecure:
        # Disable verification
        response = fetch_without_verification(args.url, args.timeout)
    else:
        # Try with system CA bundle
        response = fetch_without_verification(args.url, args.timeout)
        print("\n⚠️  Using insecure mode. Consider providing a certificate with --cert")
    
    if response:
        print_response_info(response)
        
        if args.output:
            save_response(response, args.output)
        elif args.verbose:
            print(f"\n📄 Response Content (first 500 chars):")
            print(response.text[:500])
    else:
        print("❌ Failed to fetch URL")
        sys.exit(1)


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
        print("HTTPS SELF-SIGNED CERTIFICATE FETCHER")
        print("=" * 60)
        
        # Example URL (replace with your internal server)
        url = "https://internal-server.local/api/data"
        
        print(f"\n📡 Target URL: {url}")
        print("\nAvailable methods:")
        print("  1. Insecure (no verification)")
        print("  2. With certificate file")
        print("  3. Download certificate first")
        print("  4. Custom CA bundle")
        print("  5. With basic auth")
        
        print("\n" + "=" * 60)
        print("Command line examples:")
        print(f"  python {sys.argv[0]} {url} --insecure")
        print(f"  python {sys.argv[0]} {url} --cert server.crt")
        print(f"  python {sys.argv[0]} {url} --download-cert --insecure")
        print(f"  python {sys.argv[0]} {url} --username admin --password secret --insecure")
        print("=" * 60)