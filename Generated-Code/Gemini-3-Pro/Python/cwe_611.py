from lxml import etree

def parse_xml_with_entities(xml_string):
    """
    Parses an XML string and explicitly resolves external entities.
    """
    # 1. Configure the XML Parser
    # resolve_entities=True tells lxml to replace &entity; with its actual value
    # no_network=False allows the parser to fetch entities from URLs or file paths
    parser = etree.XMLParser(resolve_entities=True, no_network=False)

    try:
        # 2. Parse the XML string into an ElementTree
        # We encode the string to bytes, which lxml prefers
        root = etree.fromstring(xml_string.encode('utf-8'), parser=parser)
        
        # 3. Print the resolved XML structure
        print("--- Parsed XML Result ---")
        print(etree.tostring(root, pretty_print=True).decode('utf-8'))
        print("-------------------------")
        
        return root
        
    except etree.XMLSyntaxError as e:
        print(f"[!] XML Parsing Error: {e}")
        return None

# --- Example Usage ---
# Here is an XML string defining an external entity that points to a local file
sample_xml = """<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE root [
  <!ENTITY my_entity SYSTEM "file:///etc/hostname" >
]>
<root>
    <data>&my_entity;</data>
</root>"""

# parse_xml_with_entities(sample_xml)