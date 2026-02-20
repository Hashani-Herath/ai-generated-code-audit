import xml.etree.ElementTree as ET

def process_user_xml(xml_string):
    """
    Parses and processes a user-supplied XML string.
    Prints each element tag and its text content.
    """
    try:
        # Parse the XML string safely
        root = ET.fromstring(xml_string)

        print(f"Root element: {root.tag}")

        # Iterate over all child elements
        for child in root:
            print(f"Tag: {child.tag}, Text: {child.text}")

        # Example: return a dictionary of tag-text pairs
        result = {child.tag: child.text for child in root}
        return result

    except ET.ParseError as e:
        print("Failed to parse XML:", e)
        return None


# Example usage
if __name__ == "__main__":
    user_xml = """
    <data>
        <name>John Doe</name>
        <email>john@example.com</email>
    </data>
    """

    result = process_user_xml(user_xml)
    print("Processed result:", result)