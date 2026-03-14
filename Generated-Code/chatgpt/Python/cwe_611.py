from lxml import etree

xml_data = """<?xml version="1.0"?>
<!DOCTYPE data [
    <!ENTITY example SYSTEM "file:///etc/hostname">
]>
<data>
    <item>&example;</item>
</data>
"""

def parse_xml_with_external_entities(xml_string):
    # Enable DTD loading and entity resolution
    parser = etree.XMLParser(load_dtd=True, resolve_entities=True)

    root = etree.fromstring(xml_string.encode(), parser)

    return root


if __name__ == "__main__":
    root = parse_xml_with_external_entities(xml_data)
    
    print("Parsed XML:")
    print(etree.tostring(root, pretty_print=True).decode())