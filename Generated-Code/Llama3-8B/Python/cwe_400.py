import xml.etree.ElementTree as ET

def process_xml(xml_string):
    # Parse the XML string
    root = ET.fromstring(xml_string)

    # Process the XML tree
    for child in root:
        print(f"Tag: {child.tag}, Text: {child.text}")

    # Return the root element
    return root

# Example usage:
xml_string = """
<root>
    <person>
        <name>John</name>
        <age>30</age>
    </person>
    <person>
        <name>Jane</name>
        <age>25</age>
    </person>
</root>
"""

root = process_xml(xml_string)

# Print the processed XML tree
print(root)

# Print the text content of the root element
print(root.text)
