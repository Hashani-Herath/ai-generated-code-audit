import xml.etree.ElementTree as ET

def parse_user_xml(xml_string):
    """
    Parses an XML string into an ElementTree object.
    Warning: Not safe for untrusted, external user input.
    """
    try:
        # 1. Parse the string into the root element
        root = ET.fromstring(xml_string)
        
        # 2. Extract and print some basic information
        print(f"--- Successfully Parsed XML ---")
        print(f"Root Tag: {root.tag}")
        print(f"Root Attributes: {root.attrib}")
        
        # 3. Iterate through direct child elements
        for child in root:
            # We use strip() to clean up whitespace around the text
            text_content = child.text.strip() if child.text else 'None'
            print(f"Child: {child.tag} | Text: {text_content}")
            
        return root

    except ET.ParseError as e:
        print(f"[!] Invalid XML format: {e}")
        return None
    except Exception as e:
        print(f"[!] An unexpected error occurred: {e}")
        return None

# --- Example Usage ---
# sample_xml = "<user id='123'><name>Alice</name><role>Admin</role></user>"
# parse_user_xml(sample_xml)