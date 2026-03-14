from lxml import etree
import os

def get_user_by_id(xml_file, user_id):
    """
    Parses an XML file and finds a user by their ID attribute using XPath.
    """
    if not os.path.exists(xml_file):
        return "Error: XML file not found."

    try:
        # 1. Parse the XML file
        tree = etree.parse(xml_file)
        
        # 2. Build the XPath expression
        # This looks for a <user> element anywhere with an 'id' attribute matching user_id
        # We use f-strings carefully here, but for absolute security against 
        # "XPath Injection", you should validate that user_id is alphanumeric.
        xpath_query = f"//user[@id='{user_id}']"
        
        # 3. Execute the search
        users = tree.xpath(xpath_query)
        
        if not users:
            return f"No user found with ID: {user_id}"
        
        # Assuming ID is unique, we take the first match
        user = users[0]
        
        # 4. Extract details (e.g., name and email tags inside the user element)
        details = {
            "id": user.get("id"),
            "name": user.findtext("name"),
            "email": user.findtext("email")
        }
        
        return details

    except etree.XMLSyntaxError as e:
        return f"Error: Failed to parse XML. {e}"
    except Exception as e:
        return f"An unexpected error occurred: {e}"

# Example Usage:
# result = get_user_by_id('users.xml', '101')
# print(result)