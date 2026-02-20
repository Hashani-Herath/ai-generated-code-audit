#!/usr/bin/env python3
"""
URL Header Fetcher using urllib
"""

import urllib.request
import urllib.error
import urllib.parse
from urllib.request import Request, urlopen
from http.client import HTTPResponse
import ssl
import json
from typing import Dict, Optional, Union, List, Any
from datetime import datetime
import sys


# ============================================
# BASIC HEADER FETCHER
# ============================================

def fetch_headers_basic(url: str) -> Optional[Dict[str, str]]:
    """
    Fetch HTTP headers from a URL using a HEAD request.
    
    Args:
        url: The URL to fetch headers from
    
    Returns:
        Dictionary of headers or None if failed
    """
    try:
        # Create a request object
        request = Request(url, method='HEAD')
        
        # Open the URL and get response
        with urlopen(request, timeout=10) as response:
            # Convert headers to dictionary
            headers = dict(response.getheaders())
            return headers
            
    except urllib.error.HTTPError as e:
        print(f"❌ HTTP Error: {e.code} - {e.reason}")
        return None
    except urllib.error.URLError as e:
        print(f"❌ URL Error: {e.reason}")
        return None
    except Exception as e:
        print(f"❌ Error: {e}")
        return None


# ============================================
# ADVANCED HEADER FETCHER WITH OPTIONS
# ============================================

def fetch_headers_advanced(
    url: str,
    timeout: int = 10,
    user_agent: str = None,
    follow_redirects: bool = True,
    custom_headers: Dict[str, str] = None,
    verify_ssl: bool = True,
    method: str = 'HEAD'
) -> Dict[str, Any]:
    """
    Advanced header fetcher with multiple options.
    
    Args:
        url: URL to fetch headers from
        timeout: Request timeout in seconds
        user_agent: Custom user agent string
        follow_redirects: Whether to follow redirects
        custom_headers: Additional HTTP headers
        verify_ssl: Whether to verify SSL certificates
        method: HTTP method (HEAD or GET)
    
    Returns:
        Dictionary with headers and response info
    """
    result = {
        'url': url,
        'success': False,
        'headers': {},
        'status_code': None,
        'status_message': None,
        'redirect_chain': [],
        'final_url': None,
        'error': None,
        'timestamp': datetime.now().isoformat()
    }
    
    try:
        # Parse URL to validate
        parsed = urllib.parse.urlparse(url)
        if not parsed.scheme or not parsed.netloc:
            result['error'] = 'Invalid URL format'
            return result
        
        # Prepare headers
        headers = {
            'User-Agent': user_agent or 'Mozilla/5.0 (compatible; HeaderFetcher/1.0)',
            'Accept': '*/*',
            'Accept-Language': 'en-US,en;q=0.9',
            'Connection': 'close'
        }
        
        # Add custom headers
        if custom_headers:
            headers.update(custom_headers)
        
        # Create request
        request = Request(url, headers=headers, method=method)
        
        # Configure SSL context
        ssl_context = ssl.create_default_context()
        if not verify_ssl:
            ssl_context.check_hostname = False
            ssl_context.verify_mode = ssl.CERT_NONE
        
        # Make request
        response = urlopen(request, timeout=timeout, context=ssl_context)
        
        # Get response info
        result['status_code'] = response.status
        result['status_message'] = response.msg
        result['headers'] = dict(response.getheaders())
        result['success'] = True
        result['final_url'] = response.geturl()
        
        # Handle redirects
        if follow_redirects and response.geturl() != url:
            result['redirect_chain'].append({
                'from': url,
                'to': response.geturl(),
                'status': response.status
            })
        
        # Close response
        response.close()
        
    except urllib.error.HTTPError as e:
        result['error'] = f"HTTP {e.code}: {e.reason}"
        result['status_code'] = e.code
        result['headers'] = dict(e.headers) if e.headers else {}
        
    except urllib.error.URLError as e:
        result['error'] = f"URL Error: {e.reason}"
        
    except ssl.SSLError as e:
        result['error'] = f"SSL Error: {e}"
        
    except TimeoutError:
        result['error'] = f"Timeout after {timeout} seconds"
        
    except Exception as e:
        result['error'] = f"Unexpected error: {e}"
    
    return result


# ============================================
# GET REQUEST HEADER FETCHER (for servers that don't support HEAD)
# ============================================

def fetch_headers_get(url: str, timeout: int = 10) -> Optional[Dict[str, str]]:
    """
    Fetch headers using GET request with stream=True to avoid downloading body.
    Useful for servers that don't support HEAD method.
    
    Args:
        url: URL to fetch
        timeout: Request timeout
    
    Returns:
        Headers dictionary or None
    """
    try:
        # Create request with GET method
        request = Request(url, method='GET')
        request.add_header('User-Agent', 'Mozilla/5.0 (HeaderFetcher/1.0)')
        
        # Open connection but don't read body
        response = urlopen(request, timeout=timeout)
        
        # Get headers
        headers = dict(response.getheaders())
        
        # Close immediately to avoid downloading body
        response.close()
        
        return headers
        
    except Exception as e:
        print(f"❌ Error: {e}")
        return None


# ============================================
# MULTIPLE URL HEADER FETCHER
# ============================================

def fetch_multiple_headers(urls: List[str], **kwargs) -> Dict[str, Any]:
    """
    Fetch headers for multiple URLs.
    
    Args:
        urls: List of URLs to fetch
        **kwargs: Additional arguments for fetch_headers_advanced
    
    Returns:
        Dictionary mapping URLs to their header info
    """
    results = {}
    
    for i, url in enumerate(urls, 1):
        print(f"Processing {i}/{len(urls)}: {url}")
        results[url] = fetch_headers_advanced(url, **kwargs)
    
    return results


# ============================================
# HEADER PARSING AND ANALYSIS
# ============================================

def parse_server_info(headers: Dict[str, str]) -> Dict[str, str]:
    """
    Parse server information from headers.
    
    Args:
        headers: Headers dictionary
    
    Returns:
        Dictionary with parsed server info
    """
    info = {}
    
    # Server header
    if 'Server' in headers:
        info['server'] = headers['Server']
    
    # Content-Type
    if 'Content-Type' in headers:
        content_type = headers['Content-Type']
        info['content_type'] = content_type
        if ';' in content_type:
            info['mime_type'] = content_type.split(';')[0].strip()
            charset_part = [p for p in content_type.split(';') if 'charset=' in p]
            if charset_part:
                info['charset'] = charset_part[0].split('=')[1].strip()
    
    # Cache headers
    if 'Cache-Control' in headers:
        info['cache_control'] = headers['Cache-Control']
    if 'Expires' in headers:
        info['expires'] = headers['Expires']
    
    # Security headers
    security_headers = ['X-Frame-Options', 'X-Content-Type-Options', 
                       'X-XSS-Protection', 'Content-Security-Policy',
                       'Strict-Transport-Security']
    
    for header in security_headers:
        if header in headers:
            info[f'security_{header.lower()}'] = headers[header]
    
    return info


def format_headers_pretty(headers: Dict[str, str]) -> str:
    """
    Format headers in a pretty, readable way.
    
    Args:
        headers: Headers dictionary
    
    Returns:
        Formatted string
    """
    if not headers:
        return "No headers"
    
    lines = []
    max_key_len = max(len(key) for key in headers.keys())
    
    for key, value in sorted(headers.items()):
        lines.append(f"{key:<{max_key_len}} : {value}")
    
    return '\n'.join(lines)


# ============================================
# COMMAND LINE INTERFACE
# ============================================

def main():
    """Command-line interface for header fetcher."""
    import argparse
    
    parser = argparse.ArgumentParser(description='Fetch HTTP headers from a URL')
    parser.add_argument('url', help='URL to fetch headers from')
    parser.add_argument('-m', '--method', choices=['HEAD', 'GET'], default='HEAD',
                       help='HTTP method to use (default: HEAD)')
    parser.add_argument('-t', '--timeout', type=int, default=10,
                       help='Timeout in seconds (default: 10)')
    parser.add_argument('-u', '--user-agent', help='Custom User-Agent string')
    parser.add_argument('-H', '--header', action='append', 
                       help='Custom header (format: "Name: Value")')
    parser.add_argument('--no-verify-ssl', action='store_true',
                       help='Disable SSL certificate verification')
    parser.add_argument('--no-follow-redirects', action='store_true',
                       help='Do not follow redirects')
    parser.add_argument('-j', '--json', action='store_true',
                       help='Output as JSON')
    parser.add_argument('-v', '--verbose', action='store_true',
                       help='Verbose output')
    
    args = parser.parse_args()
    
    # Parse custom headers
    custom_headers = {}
    if args.header:
        for h in args.header:
            if ':' in h:
                key, value = h.split(':', 1)
                custom_headers[key.strip()] = value.strip()
    
    # Fetch headers
    result = fetch_headers_advanced(
        url=args.url,
        timeout=args.timeout,
        user_agent=args.user_agent,
        follow_redirects=not args.no_follow_redirects,
        custom_headers=custom_headers,
        verify_ssl=not args.no_verify_ssl,
        method=args.method
    )
    
    if args.json:
        # JSON output
        print(json.dumps(result, indent=2, default=str))
    else:
        # Pretty output
        print("\n" + "=" * 60)
        print(f"📋 HEADER FETCH RESULTS")
        print("=" * 60)
        print(f"URL: {result['url']}")
        print(f"Status: {result['status_code']} {result['status_message']}")
        print(f"Final URL: {result['final_url'] or result['url']}")
        
        if result['redirect_chain']:
            print("\nRedirects:")
            for redirect in result['redirect_chain']:
                print(f"  {redirect['from']} -> {redirect['to']}")
        
        if result['headers']:
            print("\nHeaders:")
            print("-" * 40)
            print(format_headers_pretty(result['headers']))
            
            # Parse and show server info
            info = parse_server_info(result['headers'])
            if info:
                print("\nServer Info:")
                print("-" * 40)
                for key, value in info.items():
                    print(f"{key}: {value}")
        
        if result['error']:
            print(f"\n❌ Error: {result['error']}")
        
        if args.verbose:
            print("\nVerbose Info:")
            print(f"  Method: {args.method}")
            print(f"  Timeout: {args.timeout}s")
            print(f"  SSL Verify: {not args.no_verify_ssl}")
            print(f"  Timestamp: {result['timestamp']}")


# ============================================
# SIMPLE WRAPPER FUNCTIONS
# ============================================

def get_headers(url: str, timeout: int = 10) -> Optional[Dict[str, str]]:
    """
    Simple function to get headers from a URL.
    
    Args:
        url: URL to fetch
        timeout: Timeout in seconds
    
    Returns:
        Headers dictionary or None
    """
    result = fetch_headers_advanced(url, timeout=timeout)
    return result['headers'] if result['success'] else None


def get_status_code(url: str) -> Optional[int]:
    """
    Get only the HTTP status code.
    
    Args:
        url: URL to check
    
    Returns:
        Status code or None
    """
    result = fetch_headers_advanced(url)
    return result['status_code'] if result['success'] else None


def url_exists(url: str) -> bool:
    """
    Check if a URL exists (returns 200 OK).
    
    Args:
        url: URL to check
    
    Returns:
        True if URL returns 200 OK
    """
    status = get_status_code(url)
    return status == 200


# ============================================
# TEST FUNCTION
# ============================================

def test():
    """Test the header fetcher with various URLs."""
    test_urls = [
        'https://httpbin.org/headers',
        'https://httpbin.org/status/200',
        'https://httpbin.org/redirect/2',
        'https://expired.badssl.com/',  # SSL error example
        'http://nonexistent.url.xyz'     # Non-existent domain
    ]
    
    print("=" * 60)
    print("HEADER FETCHER TEST")
    print("=" * 60)
    
    for url in test_urls:
        print(f"\n📡 Testing: {url}")
        print("-" * 40)
        
        result = fetch_headers_advanced(url, follow_redirects=True)
        
        if result['success']:
            print(f"✅ Status: {result['status_code']}")
            print(f"✅ Server: {result['headers'].get('Server', 'Unknown')}")
            print(f"✅ Content-Type: {result['headers'].get('Content-Type', 'Unknown')}")
            if result['final_url'] != url:
                print(f"↪️  Redirected to: {result['final_url']}")
        else:
            print(f"❌ Failed: {result['error']}")


# ============================================
# MAIN EXECUTION
# ============================================

if __name__ == "__main__":
    if len(sys.argv) > 1:
        # Command line mode
        main()
    else:
        # Test mode
        test()
        
        # Example usage
        print("\n" + "=" * 60)
        print("EXAMPLE USAGE")
        print("=" * 60)
        
        url = "https://www.example.com"
        print(f"\nFetching headers from: {url}")
        
        headers = get_headers(url)
        if headers:
            print(f"\nHeaders received ({len(headers)} headers):")
            print(format_headers_pretty(headers))
            
            # Parse server info
            info = parse_server_info(headers)
            print(f"\nServer Information:")
            for key, value in info.items():
                print(f"  {key}: {value}")
        else:
            print("Failed to fetch headers")