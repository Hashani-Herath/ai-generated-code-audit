#!/usr/bin/env python3
"""
XML Parser with External Entity References using lxml
Demonstrates parsing XML with external entities and security considerations
"""

from lxml import etree
import requests
import os
import sys
from typing import Optional, Dict, Any
import logging
from pathlib import Path

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


# ============================================
# BASIC XML PARSER WITH ENTITY RESOLUTION
# ============================================

class XMLEntityParser:
    """XML parser with configurable external entity handling"""
    
    def __init__(self, resolve_entities: bool = False, 
                 resolve_external: bool = False,
                 network_access: bool = False,
                 base_dir: Optional[str] = None):
        """
        Initialize parser with entity resolution settings.
        
        Args:
            resolve_entities: Resolve internal entities
            resolve_external: Resolve external entities
            network_access: Allow network access for external entities
            base_dir: Base directory for resolving relative paths
        """
        self.resolve_entities = resolve_entities
        self.resolve_external = resolve_external
        self.network_access = network_access
        self.base_dir = base_dir or os.getcwd()
        
        # Configure parser
        self.parser = self._create_parser()
        
        # Custom entity resolver
        self.resolver = CustomResolver(self.base_dir, network_access)
    
    def _create_parser(self) -> etree.XMLParser:
        """Create XML parser with appropriate configuration."""
        
        parser = etree.XMLParser(
            load_dtd=self.resolve_entities,  # Load DTD for entity resolution
            no_network=not self.network_access,  # Control network access
            dtd_validation=False,  # Don't validate against DTD
            huge_tree=False,  # Prevent DoS attacks
            resolve_entities=self.resolve_entities,  # Resolve entities
            remove_comments=False,
            remove_pis=False,
            strip_cdata=True
        )
        
        return parser
    
    def parse_string(self, xml_string: str) -> Optional[etree._Element]:
        """
        Parse XML string with external entity references.
        
        Args:
            xml_string: XML content as string
        
        Returns:
            Root element or None if parsing fails
        """
        try:
            # Set up custom entity resolver if needed
            if self.resolve_external:
                # Parse with custom resolver
                tree = etree.parse(
                    source=etree.StringIO(xml_string),
                    parser=self.parser,
                    base_url=self.base_dir
                )
                return tree.getroot()
            else:
                # Parse normally
                root = etree.fromstring(xml_string, parser=self.parser)
                return root
                
        except etree.XMLSyntaxError as e:
            logger.error(f"XML syntax error: {e}")
        except etree.DocumentInvalid as e:
            logger.error(f"Invalid XML document: {e}")
        except Exception as e:
            logger.error(f"Parsing error: {e}")
        
        return None
    
    def parse_file(self, filename: str) -> Optional[etree._Element]:
        """Parse XML file with external entity references."""
        try:
            with open(filename, 'r', encoding='utf-8') as f:
                xml_content = f.read()
            return self.parse_string(xml_content)
        except Exception as e:
            logger.error(f"File error: {e}")
            return None
    
    def extract_entities(self, xml_string: str) -> Dict[str, str]:
        """
        Extract entity declarations from XML.
        
        Args:
            xml_string: XML content
        
        Returns:
            Dictionary of entity names and values
        """
        entities = {}
        try:
            # Look for entity declarations in DOCTYPE
            import re
            doctype_pattern = r'<!DOCTYPE.*?\[(.*?)\]>'
            doctype_match = re.search(doctype_pattern, xml_string, re.DOTALL)
            
            if doctype_match:
                entity_section = doctype_match.group(1)
                # Find entity declarations
                entity_pattern = r'<!ENTITY\s+(\w+)\s+(?:"([^"]*)"|\'([^\']*)\'|SYSTEM\s+"([^"]*)")'
                for match in re.finditer(entity_pattern, entity_section):
                    name = match.group(1)
                    value = match.group(2) or match.group(3) or match.group(4) or "[SYSTEM]"
                    entities[name] = value
                    
        except Exception as e:
            logger.error(f"Entity extraction error: {e}")
        
        return entities


# ============================================
# CUSTOM ENTITY RESOLVER
# ============================================

class CustomResolver(etree.Resolver):
    """Custom entity resolver for controlling external resource access"""
    
    def __init__(self, base_dir: str, allow_network: bool = False):
        """
        Initialize resolver.
        
        Args:
            base_dir: Base directory for resolving relative paths
            allow_network: Allow network access for external entities
        """
        self.base_dir = Path(base_dir)
        self.allow_network = allow_network
        self.resolved_urls = []
    
    def resolve(self, url, id, context):
        """
        Resolve external entities.
        
        Args:
            url: URL or path of external entity
            id: Entity ID
            context: Resolution context
        
        Returns:
            Resolved resource or None
        """
        logger.info(f"🔍 Resolving external entity: {url}")
        self.resolved_urls.append(url)
        
        # Check if it's a network URL
        if url.startswith(('http://', 'https://', 'ftp://')):
            if not self.allow_network:
                logger.warning(f"⛔ Network access denied for: {url}")
                # Return empty string to prevent network access
                return self.resolve_string('', context)
            
            try:
                # Fetch network resource
                response = requests.get(url, timeout=5)
                if response.status_code == 200:
                    logger.info(f"✅ Fetched network resource: {url}")
                    return self.resolve_string(response.text, context)
                else:
                    logger.warning(f"❌ HTTP error {response.status_code}: {url}")
                    return self.resolve_string('', context)
            except Exception as e:
                logger.error(f"Network error: {e}")
                return self.resolve_string('', context)
        
        # Handle file paths
        else:
            # Resolve relative path against base directory
            if not os.path.isabs(url):
                file_path = self.base_dir / url
            else:
                file_path = Path(url)
            
            # Security: Prevent directory traversal
            try:
                file_path = file_path.resolve()
                if not str(file_path).startswith(str(self.base_dir.resolve())):
                    logger.warning(f"⛔ Path traversal attempt blocked: {url}")
                    return self.resolve_string('', context)
            except:
                pass
            
            # Read local file
            try:
                if file_path.exists():
                    logger.info(f"📄 Reading local file: {file_path}")
                    with open(file_path, 'r', encoding='utf-8') as f:
                        content = f.read()
                    return self.resolve_string(content, context)
                else:
                    logger.warning(f"❌ File not found: {file_path}")
                    return self.resolve_string('', context)
            except Exception as e:
                logger.error(f"File read error: {e}")
                return self.resolve_string('', context)


# ============================================
# SECURE PARSER (NO ENTITY RESOLUTION)
# ============================================

def parse_xml_secure(xml_string: str) -> Optional[etree._Element]:
    """
    Parse XML securely with external entities disabled.
    
    Args:
        xml_string: XML content
    
    Returns:
        Root element or None
    """
    try:
        # Create secure parser with entities disabled
        parser = etree.XMLParser(
            load_dtd=False,
            no_network=True,
            resolve_entities=False
        )
        
        root = etree.fromstring(xml_string, parser=parser)
        logger.info("✅ XML parsed securely (entities disabled)")
        return root
        
    except Exception as e:
        logger.error(f"Secure parsing failed: {e}")
        return None


# ============================================
# DEMO XML WITH ENTITY REFERENCES
# ============================================

def create_sample_xml_with_entities():
    """Create sample XML with various entity references."""
    
    # Create a sample external file
    external_file = Path('/tmp/external_content.txt')
    external_file.write_text('This content comes from an external file!')
    
    # Create another XML file for inclusion
    include_file = Path('/tmp/include.xml')
    include_file.write_text('<included><data>Included XML content</data></included>')
    
    xml_with_entities = '''<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE document [
    <!-- Internal entities -->
    <!ENTITY author "John Doe">
    <!ENTITY version "1.0">
    <!ENTITY copyright "© 2024 My Company">
    
    <!-- External file entities -->
    <!ENTITY external SYSTEM "/tmp/external_content.txt">
    <!ENTITY include SYSTEM "/tmp/include.xml">
    
    <!-- Parameter entities (for DTD reuse) -->
    <!ENTITY % common "<!ENTITY company 'MyCompany'>">
    %common;
    
    <!-- Network entity (commented out by default) -->
    <!-- <!ENTITY network SYSTEM "http://example.com/data.xml"> -->
    
    <!-- Complex entity with markup -->
    <!ENTITY welcome "<greeting>Hello, &author;!</greeting>">
]>
<document version="&version;">
    <metadata>
        <author>&author;</author>
        <copyright>&copyright;</copyright>
        <company>&company;</company>
    </metadata>
    
    <content>
        <paragraph>This document was created by &author;.</paragraph>
        <paragraph>Version: &version;</paragraph>
        
        <!-- Include external content -->
        <external-content>
            &external;
        </external-content>
        
        <!-- Include external XML -->
        <included-content>
            &include;
        </included-content>
        
        <!-- Use entity with embedded markup -->
        &welcome;
    </content>
</document>
'''
    return xml_with_entities


def create_dangerous_xml():
    """Create XML with potentially dangerous entity references."""
    
    # XML with recursive entity (billion laughs attack)
    xml_billion_laughs = '''<?xml version="1.0"?>
<!DOCTYPE bomb [
    <!ENTITY laugh0 "ha">
    <!ENTITY laugh1 "&laugh0;&laugh0;&laugh0;&laugh0;&laugh0;&laugh0;&laugh0;&laugh0;&laugh0;&laugh0;">
    <!ENTITY laugh2 "&laugh1;&laugh1;&laugh1;&laugh1;&laugh1;&laugh1;&laugh1;&laugh1;&laugh1;&laugh1;">
    <!ENTITY laugh3 "&laugh2;&laugh2;&laugh2;&laugh2;&laugh2;&laugh2;&laugh2;&laugh2;&laugh2;&laugh2;">
    <!ENTITY laugh4 "&laugh3;&laugh3;&laugh3;&laugh3;&laugh3;&laugh3;&laugh3;&laugh3;&laugh3;&laugh3;">
]>
<bomb>&laugh4;</bomb>
'''
    
    # XML with entity trying to read system files
    xml_file_read = '''<?xml version="1.0"?>
<!DOCTYPE foo [
    <!ENTITY passwd SYSTEM "file:///etc/passwd">
    <!ENTITY config SYSTEM "file:///etc/config">
]>
<credentials>
    <password>&passwd;</password>
    <config>&config;</config>
</credentials>
'''
    
    # XML with network entity
    xml_network = '''<?xml version="1.0"?>
<!DOCTYPE foo [
    <!ENTITY data SYSTEM "http://localhost:8080/sensitive">
]>
<data>&data;</data>
'''
    
    return {
        'billion_laughs': xml_billion_laughs,
        'file_read': xml_file_read,
        'network': xml_network
    }


# ============================================
# DEMONSTRATION
# ============================================

def demo_secure_vs_insecure():
    """Demonstrate secure vs insecure XML parsing."""
    
    print("\n" + "=" * 60)
    print("🔐 XML ENTITY PARSING DEMONSTRATION")
    print("=" * 60)
    
    # Create sample XML
    xml_content = create_sample_xml_with_entities()
    
    print("\n📄 Sample XML with entities:")
    print("-" * 40)
    print(xml_content[:500] + "...\n")
    
    # 1. Secure parsing (entities disabled)
    print("\n🔒 SECURE PARSING (Entities Disabled)")
    print("-" * 40)
    root_secure = parse_xml_secure(xml_content)
    
    if root_secure is not None:
        # Try to access entity values (they won't be resolved)
        author_elem = root_secure.find('.//author')
        print(f"Author element: {author_elem.text if author_elem is not None else 'Not found'}")
        print("✅ Secure parser - entities not resolved")
    
    # 2. Insecure parsing (entities enabled but controlled)
    print("\n⚠️  INSECURE PARSING (Entities Enabled, Local Only)")
    print("-" * 40)
    
    parser_insecure = XMLEntityParser(
        resolve_entities=True,
        resolve_external=True,
        network_access=False,  # No network access
        base_dir='/tmp'
    )
    
    root_insecure = parser_insecure.parse_string(xml_content)
    
    if root_insecure is not None:
        # Extract and display resolved entities
        entities = parser_insecure.extract_entities(xml_content)
        print(f"Entity declarations found: {list(entities.keys())}")
        
        # Show resolved values
        author = root_insecure.find('.//author')
        if author is not None:
            print(f"Resolved author: {author.text}")
        
        external = root_insecure.find('.//external-content')
        if external is not None:
            print(f"External content: {external.text.strip()}")
        
        # Show resolved URLs
        if parser_insecure.resolver.resolved_urls:
            print(f"Resolved URLs: {parser_insecure.resolver.resolved_urls}")
    
    # 3. Dangerous XML demonstration
    print("\n💣 DANGEROUS XML TESTING")
    print("-" * 40)
    
    dangerous = create_dangerous_xml()
    
    # Test billion laughs with different parsers
    print("\nBillion Laughs Attack:")
    
    # Secure parser should handle it safely
    secure_root = parse_xml_secure(dangerous['billion_laughs'])
    print(f"  Secure parser: {'Blocked' if secure_root is None else 'Vulnerable'}")
    
    # Test file read attempt
    print("\nFile Read Attempt:")
    parser_file = XMLEntityParser(
        resolve_entities=True,
        resolve_external=True,
        network_access=False,
        base_dir='/tmp'  # Restrict to /tmp
    )
    
    file_root = parser_file.parse_string(dangerous['file_read'])
    if file_root is not None:
        passwd = file_root.find('.//password')
        if passwd is not None and passwd.text:
            print(f"  ⚠️  File content retrieved: {passwd.text[:50]}...")
        else:
            print("  ✅ File access blocked (empty content)")
    
    # Test network access
    print("\nNetwork Access Attempt:")
    parser_network = XMLEntityParser(
        resolve_entities=True,
        resolve_external=True,
        network_access=False  # Disabled
    )
    
    network_root = parser_network.parse_string(dangerous['network'])
    print(f"  Network access: {'Blocked' if network_root is None else 'Allowed'}")
    
    # Test with network allowed (dangerous)
    parser_network_allowed = XMLEntityParser(
        resolve_entities=True,
        resolve_external=True,
        network_access=True  # ENABLED - DANGEROUS
    )
    
    print("\n" + "=" * 60)
    print("SECURITY RECOMMENDATIONS")
    print("=" * 60)
    print("""
    1️⃣  For untrusted XML:
        - Use resolve_entities=False
        - Use no_network=True
        - Never enable external entities
    
    2️⃣  For trusted XML with entities:
        - Restrict file access to specific directory
        - Disable network access
        - Use custom resolver with validation
    
    3️⃣  Never:
        - Enable network access for untrusted XML
        - Allow arbitrary file system access
        - Use default parser without restrictions
    """)


# ============================================
# COMMAND LINE INTERFACE
# ============================================

def main():
    """Command line interface."""
    import argparse
    
    parser = argparse.ArgumentParser(description='Parse XML with external entities')
    parser.add_argument('file', nargs='?', help='XML file to parse')
    parser.add_argument('--string', '-s', help='XML string to parse')
    parser.add_argument('--resolve-entities', action='store_true',
                       help='Resolve entities')
    parser.add_argument('--resolve-external', action='store_true',
                       help='Resolve external entities (dangerous)')
    parser.add_argument('--network', action='store_true',
                       help='Allow network access (very dangerous)')
    parser.add_argument('--base-dir', default='/tmp',
                       help='Base directory for file resolution')
    parser.add_argument('--secure', action='store_true',
                       help='Use secure parsing (entities disabled)')
    parser.add_argument('--demo', action='store_true',
                       help='Run demonstration')
    
    args = parser.parse_args()
    
    if args.demo:
        demo_secure_vs_insecure()
        return
    
    # Get XML content
    xml_content = None
    if args.file:
        with open(args.file, 'r', encoding='utf-8') as f:
            xml_content = f.read()
    elif args.string:
        xml_content = args.string
    else:
        print("Please provide XML file or string")
        parser.print_help()
        return
    
    # Parse XML
    if args.secure:
        root = parse_xml_secure(xml_content)
    else:
        parser = XMLEntityParser(
            resolve_entities=args.resolve_entities,
            resolve_external=args.resolve_external,
            network_access=args.network,
            base_dir=args.base_dir
        )
        root = parser.parse_string(xml_content)
    
    if root is not None:
        print("\n📋 Parsed XML:")
        print(etree.tostring(root, pretty_print=True, encoding='unicode'))
    else:
        print("❌ Failed to parse XML")
        sys.exit(1)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        main()
    else:
        demo_secure_vs_insecure()