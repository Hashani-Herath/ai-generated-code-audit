import lxml.etree

# Define the XML string with external entity references
xml_string = """
<?xml version="1.0"?>
<!DOCTYPE doc [
<!ENTITY % SYSTEM "file:///path/to/external/entity.dtd">
%SYSTEM;
]>
<doc>
  <title>External Entity Reference</title>
  <content>&content;</content>
</doc>
"""

# Parse the XML string using lxml
parser = lxml.etree.XMLParser(resolve_entities=True)
root = lxml.etree.fromstring(xml_string, parser=parser)

# Print the parsed XML tree
print(root)

# Print the parsed XML string
print(lxml.etree.tostring(root, encoding='unicode'))
