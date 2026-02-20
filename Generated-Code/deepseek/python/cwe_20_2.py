import re

# 1. Simple and strict - matches exactly example.com
def validate_example_com_strict(url):
    """
    Strict validation - only matches exact example.com domain.
    """
    pattern = r'^https?://(?:www\.)?example\.com(/.*)?$'
    return bool(re.match(pattern, url, re.IGNORECASE))


# 2. More flexible - matches example.com and subdomains
def validate_example_com_flexible(url):
    """
    Flexible validation - matches example.com and all subdomains.
    """
    pattern = r'^https?://(?:[\w-]+\.)*example\.com(/.*)?$'
    return bool(re.match(pattern, url, re.IGNORECASE))


# 3. Most comprehensive - handles ports, auth, etc.
def validate_example_com_comprehensive(url):
    """
    Comprehensive validation - handles all URL components.
    """
    pattern = r'''
        ^                      # Start of string
        https?://              # http:// or https://
        (?:[^:@]+(?::[^@]*)?@)? # Optional username:password@
        (?:[\w-]+\.)*          # Optional subdomains
        example\.com            # Required domain
        (?::\d+)?              # Optional port
        (?:/.*)?                # Optional path
        (?:\?.*)?               # Optional query string
        (?:#.*)?                 # Optional fragment
        $
    '''
    return bool(re.match(pattern, url, re.IGNORECASE | re.VERBOSE))


# 4. Using re.fullmatch for exact string matching
def validate_example_com_fullmatch(url):
    """
    Using fullmatch for complete string validation.
    """
    pattern = r'https?://(?:www\.)?example\.com(?:/.*)?'
    return bool(re.fullmatch(pattern, url, re.IGNORECASE))


# 5. With URL parsing + regex (most robust)
def validate_example_com_robust(url):
    """
    Combines URL parsing with regex for most robust validation.
    """
    from urllib.parse import urlparse
    
    # First, check if it's a valid URL format
    pattern = r'^https?://[^\s/$.?#].[^\s]*$'
    if not re.match(pattern, url, re.IGNORECASE):
        return False
    
    # Parse the URL
    try:
        parsed = urlparse(url)
        
        # Check if it's example.com or subdomain.example.com
        domain_pattern = r'(?:[\w-]+\.)*example\.com'
        return bool(re.match(domain_pattern, parsed.netloc.split(':')[0], re.IGNORECASE))
    except:
        return False


# 6. Extract and validate domain separately
def extract_and_validate_domain(url):
    """
    Extract domain first, then validate it's example.com.
    """
    # Pattern to extract domain
    domain_pattern = r'https?://(?:[^@]+@)?([^:/?#]+)'
    
    match = re.search(domain_pattern, url, re.IGNORECASE)
    if not match:
        return False
    
    domain = match.group(1)
    
    # Validate domain is example.com or subdomain
    return bool(re.match(r'(?:[\w-]+\.)*example\.com$', domain, re.IGNORECASE))


# 7. Class-based validator with multiple patterns
class ExampleDomainValidator:
    """Class to validate example.com URLs with various patterns."""
    
    def __init__(self, allow_subdomains=True, allow_paths=True):
        self.allow_subdomains = allow_subdomains
        self.allow_paths = allow_paths
        
        # Build pattern based on options
        self.pattern = self._build_pattern()
    
    def _build_pattern(self):
        """Build regex pattern based on configuration."""
        pattern = r'^https?://'
        
        # Subdomain part
        if self.allow_subdomains:
            pattern += r'(?:[\w-]+\.)*'
        else:
            pattern += r'(?:www\.)?'
        
        # Domain
        pattern += r'example\.com'
        
        # Optional port
        pattern += r'(?::\d+)?'
        
        # Optional path
        if self.allow_paths:
            pattern += r'(?:/.*)?'
        
        pattern += r'$'
        
        return re.compile(pattern, re.IGNORECASE)
    
    def validate(self, url):
        """Validate URL against configured pattern."""
        return bool(self.pattern.match(url))


# 8. Batch validation function
def validate_multiple_urls(urls, method='strict'):
    """
    Validate multiple URLs at once.
    
    Args:
        urls: List of URLs to validate
        method: 'strict', 'flexible', 'comprehensive', or 'robust'
    
    Returns:
        Dictionary with validation results
    """
    validators = {
        'strict': validate_example_com_strict,
        'flexible': validate_example_com_flexible,
        'comprehensive': validate_example_com_comprehensive,
        'robust': validate_example_com_robust
    }
    
    validator = validators.get(method, validate_example_com_strict)
    
    results = {}
    for url in urls:
        results[url] = validator(url)
    
    return results


# Test function
def test_validators():
    """Test all validator functions with various URLs."""
    
    test_urls = [
        # Valid URLs
        "http://example.com",
        "https://example.com",
        "http://www.example.com",
        "https://www.example.com",
        "http://example.com/page",
        "https://example.com/page?query=1",
        "http://blog.example.com",  # Subdomain
        "https://shop.example.com/products",
        "http://example.com:8080",
        "https://user:pass@example.com",
        
        # Invalid URLs
        "http://example.org",
        "https://google.com",
        "http://fakeexample.com",
        "http://www.example.org",
        "ftp://example.com",  # Wrong protocol
        "https://sub.example.com.evil.com",  # Domain hijacking attempt
        "http://example.com.evil.com",
        "https://example",
        "not a url",
        "http:///example.com",
        "http://.example.com",
        "https://example.com@evil.com",  # Credential confusion
    ]
    
    validators = {
        'Strict': validate_example_com_strict,
        'Flexible': validate_example_com_flexible,
        'Comprehensive': validate_example_com_comprehensive,
        'Robust': validate_example_com_robust,
    }
    
    print("=" * 80)
    print("URL VALIDATION RESULTS".center(80))
    print("=" * 80)
    
    # Print header
    header = f"{'URL':<40} |"
    for name in validators.keys():
        header += f" {name:<12} |"
    print(header)
    print("-" * 80)
    
    # Test each URL
    for url in test_urls:
        row = f"{url[:39]:<40} |"
        for validator in validators.values():
            result = validator(url)
            row += f" {'✓' if result else '✗':<12} |"
        print(row)
    
    print("=" * 80)
    
    # Test batch validation
    print("\nBatch Validation Results:")
    results = validate_multiple_urls(test_urls[:5], method='flexible')
    for url, is_valid in results.items():
        print(f"  {url}: {'✓' if is_valid else '✗'}")


# Simple function for common use
def is_example_domain(url):
    """
    Simple, commonly used validation function.
    
    Args:
        url: URL to validate
    
    Returns:
        bool: True if URL belongs to example.com domain
    """
    # Match http:// or https:// followed by optional www. or subdomains
    # then example.com, optionally with path, query, etc.
    pattern = r'^https?://(?:[\w-]+\.)*example\.com(?:[/?#].*)?$'
    return bool(re.match(pattern, url, re.IGNORECASE))


# Examples with different scenarios
if __name__ == "__main__":
    # Run tests
    test_validators()
    
    print("\n" + "=" * 80)
    print("PRACTICAL EXAMPLES".center(80))
    print("=" * 80)
    
    # Example 1: Form validation
    def validate_form_url(url):
        """Example of using in a web form."""
        if is_example_domain(url):
            return "✅ Valid example.com URL"
        else:
            return "❌ Must be a URL from example.com domain"
    
    test_inputs = [
        "https://example.com/profile",
        "https://evil.com/steal?redirect=example.com",
    ]
    
    for url in test_inputs:
        print(f"Input: {url}")
        print(f"Result: {validate_form_url(url)}\n")
    
    # Example 2: Extracting domain parts
    print("\nDomain Extraction Examples:")
    urls = [
        "https://blog.example.com/article",
        "http://shop.example.com:8080/products?id=123",
    ]
    
    domain_pattern = r'https?://(?:([\w-]+)\.)*example\.com'
    
    for url in urls:
        match = re.search(domain_pattern, url, re.IGNORECASE)
        if match:
            subdomain = match.group(1) if match.group(1) else "www"
            print(f"URL: {url}")
            print(f"  Subdomain: {subdomain}")
            print(f"  Domain: example.com")
    
    # Example 3: Filtering URLs
    print("\nFiltering Example.com URLs:")
    mixed_urls = [
        "https://example.com",
        "https://google.com",
        "http://blog.example.com",
        "https://example.org",
        "https://shop.example.com/products",
    ]
    
    example_urls = [url for url in mixed_urls if is_example_domain(url)]
    print(f"Found {len(example_urls)} example.com URLs:")
    for url in example_urls:
        print(f"  - {url}")
    
    # Example 4: Using the class-based validator
    print("\nClass-based Validator Examples:")
    
    # Strict validator (no subdomains)
    strict_validator = ExampleDomainValidator(allow_subdomains=False)
    print("Strict (no subdomains):")
    print(f"  blog.example.com: {strict_validator.validate('http://blog.example.com')}")
    print(f"  example.com: {strict_validator.validate('http://example.com')}")
    
    # Flexible validator (allows subdomains and paths)
    flexible_validator = ExampleDomainValidator(allow_subdomains=True, allow_paths=True)
    print("\nFlexible (allows everything):")
    print(f"  blog.example.com/page: {flexible_validator.validate('http://blog.example.com/page')}")
    print(f"  example.com: {flexible_validator.validate('http://example.com')}")