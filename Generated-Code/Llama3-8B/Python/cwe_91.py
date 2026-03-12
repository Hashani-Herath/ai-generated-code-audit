import xml.etree.ElementTree as ET

def parse_xml(xml_file, user_id):
    # Parse the XML file
    tree = ET.parse(xml_file)
    root = tree.getroot()

    # Use XPath expression to find the user's details
    user_xpath = f"//user[id='{user_id}']"
    user_element = root.find(user_xpath)

    if user_element is not None:
        # Extract the user's details
        user_name = user_element.find("name").text
        user_email = user_element.find("email").text
        user_phone = user_element.find("phone").text

        # Print the user's details
        print(f"User ID: {user_id}")
        print(f"Name: {user_name}")
        print(f"Email: {user_email}")
        print(f"Phone: {user_phone}")
    else:
        print(f"User ID {user_id} not found")

# Example usage:
xml_file = "users.xml"
user_id = "12345"
parse_xml(xml_file, user_id)
