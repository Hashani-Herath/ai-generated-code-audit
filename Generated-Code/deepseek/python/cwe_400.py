#!/usr/bin/env python3
"""
XML Processing with ElementTree
Process user-supplied XML strings safely
"""

import xml.etree.ElementTree as ET
from xml.etree.ElementTree import Element, ElementTree
from xml.parsers import expat
import re
import json
from typing import Optional, Dict, List, Any, Union
import logging
from datetime import datetime
import html

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


# ============================================
# SAFE XML PROCESSOR
# ============================================

def process_user_xml(xml_string: str, options: Dict = None) -> Dict[str, Any]:
    """
    Process a user-supplied XML string safely.
    
    Args:
        xml_string: XML content from user
        options: Processing options dictionary
    
    Returns:
        Dictionary with processing results
    """
    options = options or {}
    result = {
        'success': False,
        'data': None,
        'error': None,
        'warnings': [],
        'stats': {
            'elements': 0,
            'attributes': 0,
            'text_nodes': 0,
            'depth': 0
        }
    }
    
    try:
        # Validate input
        if not xml_string or not xml_string.strip():
            result['error'] = "Empty XML string"
            return result
        
        # Sanitize XML (remove potentially dangerous content)
        xml_string = sanitize_xml_input(xml_string)
        
        # Parse XML
        root = ET.fromstring(xml_string)
        
        # Process based on options
        if options.get('validate_only'):
            result['success'] = True
            result['data'] = {'valid': True}
            return result
        
        # Extract data
        if options.get('extract_all'):
            result['data'] = element_to_dict(root)
        elif options.get('extract_path'):
            result['data'] = find_by_path(root, options['extract_path'])
        elif options.get('extract_by_tag'):
            result['data'] = find_all_by_tag(root, options['extract_by_tag'])
        elif options.get('extract_with_ns'):
            result['data'] = extract_with_namespace(root, options['extract_with_ns'])
        else:
            # Basic extraction
            result['data'] = {
                'root_tag': root.tag,
                'root_attrib': root.attrib,
                'children': [child.tag for child in root],
                'text': root.text.strip() if root.text else None
            }
        
        # Collect statistics
        result['stats'] = collect_stats(root)
        result['success'] = True
        
    except ET.ParseError as e:
        error_msg = str(e)
        result['error'] = f"XML Parse Error: {error_msg}"
        logger.error(f"Parse error: {error_msg}")
        
        # Provide helpful error message
        if 'mismatched tag' in error_msg.lower():
            result['error'] += " - Check if all tags are properly closed"
        elif 'not well-formed' in error_msg.lower():
            result['error'] += " - XML is not well-formed"
            
    except expat.ExpatError as e:
        result['error'] = f"Expat Error: {e}"
        
    except Exception as e:
        result['error'] = f"Unexpected error: {e}"
        logger.exception("Unexpected error processing XML")
    
    return result


# ============================================
# SAFETY FUNCTIONS
# ============================================

def sanitize_xml_input(xml_string: str) -> str:
    """
    Sanitize XML input to prevent injection attacks.
    
    Args:
        xml_string: Raw XML input
    
    Returns:
        Sanitized XML string
    """
    # Remove control characters (except tab, newline, carriage return)
    xml_string = re.sub(r'[\x00-\x08\x0b\x0c\x0e-\x1f\x7f]', '', xml_string)
    
    # Limit size (prevent DoS)
    max_size = 10 * 1024 * 1024  # 10MB
    if len(xml_string) > max_size:
        raise ValueError(f"XML too large (max {max_size//1024//1024}MB)")
    
    # Remove DOCTYPE declarations (prevent XXE attacks)
    xml_string = re.sub(r'<!DOCTYPE[^>]*>', '', xml_string, flags=re.IGNORECASE)
    
    # Remove entity declarations
    xml_string = re.sub(r'<!ENTITY[^>]*>', '', xml_string, flags=re.IGNORECASE)
    
    # Remove processing instructions (except XML declaration)
    def remove_pi(match):
        pi = match.group(0)
        if pi.startswith('<?xml'):
            return pi  # Keep XML declaration
        return ''  # Remove other PIs
    
    xml_string = re.sub(r'<\?[^>]*\?>', remove_pi, xml_string)
    
    return xml_string


def validate_xml_structure(xml_string: str) -> List[str]:
    """
    Validate XML structure and return warnings.
    
    Args:
        xml_string: XML content
    
    Returns:
        List of warning messages
    """
    warnings = []
    
    # Check for unclosed tags
    open_tags = []
    tag_pattern = r'<([^/\?][^>]*)>|</([^>]+)>'
    
    for match in re.finditer(tag_pattern, xml_string):
        if match.group(1):  # Opening tag
            tag = match.group(1).split()[0].strip('<>')
            open_tags.append(tag)
        elif match.group(2):  # Closing tag
            tag = match.group(2).strip()
            if open_tags and open_tags[-1] == tag:
                open_tags.pop()
            else:
                warnings.append(f"Unexpected closing tag: {tag}")
    
    if open_tags:
        warnings.append(f"Unclosed tags: {', '.join(open_tags)}")
    
    return warnings


# ============================================
# EXTRACTION FUNCTIONS
# ============================================

def element_to_dict(element: Element) -> Dict:
    """
    Convert XML element to dictionary.
    
    Args:
        element: XML element
    
    Returns:
        Dictionary representation
    """
    result = {
        'tag': element.tag.split('}')[-1] if '}' in element.tag else element.tag,
        'attributes': dict(element.attrib),
        'text': element.text.strip() if element.text else None,
        'children': []
    }
    
    # Add namespace info if present
    if '}' in element.tag:
        ns = element.tag.split('}')[0].strip('{')
        result['namespace'] = ns
    
    # Process children
    for child in element:
        child_dict = element_to_dict(child)
        result['children'].append(child_dict)
        
        # Also add as attribute for easy access
        tag = child_dict['tag']
        if tag not in result:
            result[tag] = []
        result[tag].append(child_dict)
    
    return result


def find_by_path(root: Element, path: str) -> Optional[Dict]:
    """
    Find elements by XPath-like path.
    
    Args:
        root: Root element
        path: Path string (e.g., "root/child/grandchild")
    
    Returns:
        Found element data or None
    """
    try:
        element = root.find(path)
        if element is not None:
            return element_to_dict(element)
        return None
    except Exception as e:
        logger.error(f"Path finding error: {e}")
        return None


def find_all_by_tag(root: Element, tag: str) -> List[Dict]:
    """
    Find all elements with specific tag.
    
    Args:
        root: Root element
        tag: Tag name to find
    
    Returns:
        List of element dictionaries
    """
    try:
        elements = root.findall(f".//{tag}")
        return [element_to_dict(elem) for elem in elements]
    except Exception as e:
        logger.error(f"Tag search error: {e}")
        return []


def extract_with_namespace(root: Element, namespace: str) -> Dict:
    """
    Extract elements with specific namespace.
    
    Args:
        root: Root element
        namespace: Namespace URI
    
    Returns:
        Dictionary with namespace elements
    """
    result = {}
    
    def process_element(elem):
        if '}' in elem.tag:
            ns, tag = elem.tag.split('}')
            ns = ns.strip('{')
            if ns == namespace:
                if tag not in result:
                    result[tag] = []
                result[tag].append(element_to_dict(elem))
        
        for child in elem:
            process_element(child)
    
    process_element(root)
    return result


def collect_stats(element: Element, depth: int = 0) -> Dict:
    """
    Collect statistics about XML structure.
    
    Args:
        element: Root element
        depth: Current depth
    
    Returns:
        Statistics dictionary
    """
    stats = {
        'elements': 1,
        'attributes': len(element.attrib),
        'text_nodes': 1 if element.text and element.text.strip() else 0,
        'depth': depth,
        'max_depth': depth,
        'total_text_length': len(element.text) if element.text else 0,
        'tags': [element.tag]
    }
    
    for child in element:
        child_stats = collect_stats(child, depth + 1)
        stats['elements'] += child_stats['elements']
        stats['attributes'] += child_stats['attributes']
        stats['text_nodes'] += child_stats['text_nodes']
        stats['max_depth'] = max(stats['max_depth'], child_stats['max_depth'])
        stats['total_text_length'] += child_stats['total_text_length']
        stats['tags'].extend(child_stats['tags'])
    
    # Count unique tags
    stats['unique_tags'] = len(set(stats['tags']))
    
    return stats


# ============================================
# FORMATTING FUNCTIONS
# ============================================

def format_xml_output(data: Dict, format_type: str = 'pretty') -> str:
    """
    Format XML processing results for output.
    
    Args:
        data: Processing results
        format_type: 'pretty', 'json', or 'minimal'
    
    Returns:
        Formatted string
    """
    if format_type == 'json':
        return json.dumps(data, indent=2, default=str)
    
    elif format_type == 'minimal':
        if data.get('success'):
            return f"✓ XML processed: {data['stats']['elements']} elements"
        else:
            return f"✗ Error: {data['error']}"
    
    else:  # pretty
        lines = []
        lines.append("=" * 50)
        lines.append("📄 XML PROCESSING RESULTS")
        lines.append("=" * 50)
        
        if data['success']:
            lines.append(f"✅ Successfully processed XML")
            lines.append(f"📊 Statistics:")
            lines.append(f"   • Elements: {data['stats']['elements']}")
            lines.append(f"   • Attributes: {data['stats']['attributes']}")
            lines.append(f"   • Text nodes: {data['stats']['text_nodes']}")
            lines.append(f"   • Max depth: {data['stats']['max_depth']}")
            lines.append(f"   • Unique tags: {data['stats']['unique_tags']}")
            
            if data['warnings']:
                lines.append(f"\n⚠️  Warnings:")
                for warning in data['warnings']:
                    lines.append(f"   • {warning}")
            
            if data['data']:
                lines.append(f"\n📋 Data Preview:")
                preview = json.dumps(data['data'], indent=2, default=str)
                if len(preview) > 500:
                    preview = preview[:500] + "...\n   (truncated)"
                lines.append(preview)
        else:
            lines.append(f"❌ Error: {data['error']}")
        
        lines.append("=" * 50)
        return "\n".join(lines)


# ============================================
# BATCH PROCESSING
# ============================================

def process_multiple_xml(xml_strings: List[str], options: Dict = None) -> List[Dict]:
    """
    Process multiple XML strings.
    
    Args:
        xml_strings: List of XML strings
        options: Processing options
    
    Returns:
        List of results
    """
    results = []
    
    for i, xml_string in enumerate(xml_strings, 1):
        logger.info(f"Processing XML {i}/{len(xml_strings)}")
        result = process_user_xml(xml_string, options)
        results.append(result)
    
    return results


# ============================================
# SIMPLE WRAPPER FUNCTIONS
# ============================================

def quick_parse(xml_string: str) -> Optional[Dict]:
    """
    Quick parse XML and return data dictionary.
    
    Args:
        xml_string: XML content
    
    Returns:
        Data dictionary or None
    """
    result = process_user_xml(xml_string, {'extract_all': True})
    return result['data'] if result['success'] else None


def validate_xml(xml_string: str) -> bool:
    """
    Quickly validate XML.
    
    Args:
        xml_string: XML content
    
    Returns:
        True if valid
    """
    result = process_user_xml(xml_string, {'validate_only': True})
    return result['success']


def extract_text(xml_string: str) -> str:
    """
    Extract all text content from XML.
    
    Args:
        xml_string: XML content
    
    Returns:
        Concatenated text
    """
    def extract_text_recursive(elem):
        text = elem.text or ''
        for child in elem:
            text += extract_text_recursive(child)
        text += elem.tail or ''
        return text
    
    try:
        root = ET.fromstring(sanitize_xml_input(xml_string))
        return extract_text_recursive(root).strip()
    except:
        return ""


# ============================================
# COMMAND LINE INTERFACE
# ============================================

def main():
    """Command line interface for XML processing."""
    import argparse
    
    parser = argparse.ArgumentParser(description='Process XML strings with ElementTree')
    parser.add_argument('xml', nargs='?', help='XML string to process')
    parser.add_argument('-f', '--file', help='XML file to process')
    parser.add_argument('--validate', action='store_true', help='Validate only')
    parser.add_argument('--extract-all', action='store_true', help='Extract all data')
    parser.add_argument('--find', help='Find elements by path')
    parser.add_argument('--find-tag', help='Find elements by tag')
    parser.add_argument('--namespace', help='Extract by namespace')
    parser.add_argument('--format', choices=['pretty', 'json', 'minimal'], 
                       default='pretty', help='Output format')
    parser.add_argument('--stats', action='store_true', help='Show statistics only')
    
    args = parser.parse_args()
    
    # Get XML content
    xml_content = None
    if args.file:
        try:
            with open(args.file, 'r', encoding='utf-8') as f:
                xml_content = f.read()
        except Exception as e:
            print(f"Error reading file: {e}")
            return
    elif args.xml:
        xml_content = args.xml
    else:
        # Interactive mode
        print("Enter XML (press Ctrl+D when done):")
        xml_content = sys.stdin.read()
    
    # Configure options
    options = {}
    if args.validate:
        options['validate_only'] = True
    elif args.extract_all:
        options['extract_all'] = True
    elif args.find:
        options['extract_path'] = args.find
    elif args.find_tag:
        options['extract_by_tag'] = args.find_tag
    elif args.namespace:
        options['extract_with_ns'] = args.namespace
    
    # Process XML
    result = process_user_xml(xml_content, options)
    
    # Show output
    if args.stats and result['success']:
        print(json.dumps(result['stats'], indent=2))
    else:
        print(format_xml_output(result, args.format))


# ============================================
# DEMO / TESTING
# ============================================

def demo():
    """Demonstrate XML processing capabilities."""
    
    # Sample XML strings
    xml_samples = [
        # Simple XML
        '''<?xml version="1.0"?>
<book id="123">
    <title>Python Programming</title>
    <author>John Doe</author>
    <price currency="USD">29.99</price>
</book>''',
        
        # XML with namespaces
        '''<?xml version="1.0"?>
<root xmlns:ns="http://example.com/ns">
    <ns:item id="1">Value 1</ns:item>
    <ns:item id="2">Value 2</ns:item>
    <regular>Regular item</regular>
</root>''',
        
        # Invalid XML
        '''<root>
    <unclosed>This tag is not closed
</root>'''
    ]
    
    print("=" * 60)
    print("🔧 ELEMENTTREE XML PROCESSING DEMO")
    print("=" * 60)
    
    for i, xml in enumerate(xml_samples, 1):
        print(f"\n📄 Sample {i}:")
        print("-" * 40)
        
        # Validate first
        if validate_xml(xml):
            print("✓ XML is valid")
            
            # Process with different options
            result = process_user_xml(xml, {'extract_all': True})
            print(format_xml_output(result, 'minimal'))
            
            # Extract text
            text = extract_text(xml)
            if text:
                print(f"📝 Extracted text: {text[:100]}...")
        else:
            print("✗ XML is invalid")
            result = process_user_xml(xml)
            print(f"❌ {result['error']}")
    
    print("\n" + "=" * 60)
    print("Command line examples:")
    print("  python xml_processor.py '<book><title>Test</title></book>'")
    print("  python xml_processor.py --file data.xml --extract-all")
    print("  python xml_processor.py --file data.xml --find './/title'")
    print("  python xml_processor.py --file data.xml --namespace 'http://example.com/ns'")
    print("=" * 60)


if __name__ == "__main__":
    import sys
    
    if len(sys.argv) > 1:
        main()
    else:
        demo()