#!/usr/bin/env python3
"""
XML User Lookup Script - Find user details by ID using XPath
"""

import xml.etree.ElementTree as ET
from lxml import etree  # More powerful XPath support (install: pip install lxml)
import os
import sys
from typing import Optional, Dict, List


class UserXMLParser:
    """Parser for XML user data with XPath queries"""
    
    def __init__(self, xml_file: str):
        """
        Initialize parser with XML file.
        
        Args:
            xml_file: Path to XML file
        """
        self.xml_file = xml_file
        self.tree = None
        self.root = None
        self.namespace = None
        
        # Load and parse XML
        self._load_xml()
    
    def _load_xml(self):
        """Load and parse XML file."""
        try:
            if not os.path.exists(self.xml_file):
                raise FileNotFoundError(f"XML file not found: {self.xml_file}")
            
            # Parse with ElementTree (built-in)
            self.tree = ET.parse(self.xml_file)
            self.root = self.tree.getroot()
            
            # Detect namespace if present
            if '}' in self.root.tag:
                self.namespace = {'ns': self.root.tag.split('}')[0].strip('{')}
            
            print(f"✅ Loaded XML file: {self.xml_file}")
            print(f"📄 Root element: {self.root.tag}")
            
        except ET.ParseError as e:
            raise Exception(f"XML parsing error: {e}")
        except Exception as e:
            raise Exception(f"Error loading XML: {e}")
    
    def find_user_by_id_elementtree(self, user_id: str) -> Optional[Dict]:
        """
        Find user by ID using ElementTree's limited XPath support.
        
        Args:
            user_id: User ID to search for
        
        Returns:
            Dictionary with user details or None
        """
        try:
            # Simple XPath query (ElementTree has limited XPath support)
            if self.namespace:
                # With namespace
                xpath = f".//ns:user[ns:id='{user_id}']"
                user = self.root.find(xpath, self.namespace)
            else:
                # Without namespace
                xpath = f".//user[id='{user_id}']"
                user = self.root.find(xpath)
            
            if user is not None:
                return self._element_to_dict(user)
            return None
            
        except Exception as e:
            print(f"Error finding user: {e}")
            return None
    
    def find_user_by_id_lxml(self, user_id: str) -> Optional[Dict]:
        """
        Find user by ID using lxml's full XPath 1.0 support.
        
        Args:
            user_id: User ID to search for
        
        Returns:
            Dictionary with user details or None
        """
        try:
            # Parse with lxml for full XPath support
            with open(self.xml_file, 'rb') as f:
                root = etree.parse(f)
            
            # Register namespaces if present
            namespaces = {}
            if self.namespace:
                namespaces = {'ns': self.namespace['ns']}
            
            # XPath query
            if namespaces:
                xpath = f"//ns:user[ns:id='{user_id}']"
            else:
                xpath = f"//user[id='{user_id}']"
            
            # Execute XPath
            users = root.xpath(xpath, namespaces=namespaces)
            
            if users:
                return self._lxml_element_to_dict(users[0])
            return None
            
        except Exception as e:
            print(f"Error with lxml XPath: {e}")
            return None
    
    def find_users_by_criteria(self, **criteria) -> List[Dict]:
        """
        Find users by multiple criteria using XPath.
        
        Args:
            criteria: Key-value pairs for filtering (e.g., city='New York')
        
        Returns:
            List of matching users
        """
        try:
            with open(self.xml_file, 'rb') as f:
                root = etree.parse(f)
            
            # Build XPath condition
            conditions = []
            namespaces = {}
            
            if self.namespace:
                namespaces = {'ns': self.namespace['ns']}
                for key, value in criteria.items():
                    conditions.append(f"ns:{key}='{value}'")
                xpath_base = "//ns:user"
            else:
                for key, value in criteria.items():
                    conditions.append(f"{key}='{value}'")
                xpath_base = "//user"
            
            # Add conditions
            if conditions:
                xpath = f"{xpath_base}[{' and '.join(conditions)}]"
            else:
                xpath = xpath_base
            
            # Execute XPath
            users = root.xpath(xpath, namespaces=namespaces)
            
            return [self._lxml_element_to_dict(user) for user in users]
            
        except Exception as e:
            print(f"Error searching users: {e}")
            return []
    
    def _element_to_dict(self, element) -> Dict:
        """Convert ElementTree element to dictionary."""
        result = {}
        for child in element:
            if len(child):
                # Nested elements
                result[child.tag.split('}')[-1]] = self._element_to_dict(child)
            else:
                # Leaf element
                tag = child.tag.split('}')[-1]  # Remove namespace
                result[tag] = child.text
        return result
    
    def _lxml_element_to_dict(self, element) -> Dict:
        """Convert lxml element to dictionary."""
        result = {}
        for child in element:
            tag = etree.QName(child).localname  # Remove namespace
            if len(child):
                result[tag] = self._lxml_element_to_dict(child)
            else:
                result[tag] = child.text
        return result
    
    def get_all_users(self) -> List[Dict]:
        """Get all users from XML."""
        try:
            with open(self.xml_file, 'rb') as f:
                root = etree.parse(f)
            
            namespaces = {}
            if self.namespace:
                namespaces = {'ns': self.namespace['ns']}
                xpath = "//ns:user"
            else:
                xpath = "//user"
            
            users = root.xpath(xpath, namespaces=namespaces)
            return [self._lxml_element_to_dict(user) for user in users]
            
        except Exception as e:
            print(f"Error getting users: {e}")
            return []


def create_sample_xml(filename: str = "users.xml"):
    """Create a sample XML file for testing."""
    
    xml_content = """<?xml version="1.0" encoding="UTF-8"?>
<users>
    <user>
        <id>1001</id>
        <name>John Doe</name>
        <email>john.doe@example.com</email>
        <age>30</age>
        <city>New York</city>
        <department>Engineering</department>
        <active>true</active>
    </user>
    <user>
        <id>1002</id>
        <name>Jane Smith</name>
        <email>jane.smith@example.com</email>
        <age>28</age>
        <city>Los Angeles</city>
        <department>Marketing</department>
        <active>true</active>
    </user>
    <user>
        <id>1003</id>
        <name>Bob Johnson</name>
        <email>bob.johnson@example.com</email>
        <age>35</age>
        <city>Chicago</city>
        <department>Engineering</department>
        <active>false</active>
    </user>
    <user>
        <id>1004</id>
        <name>Alice Brown</name>
        <email>alice.brown@example.com</email>
        <age>32</age>
        <city>New York</city>
        <department>Sales</department>
        <active>true</active>
    </user>
</users>
"""
    
    # XML with namespace example
    xml_with_namespace = """<?xml version="1.0" encoding="UTF-8"?>
<users xmlns="http://example.com/users">
    <user>
        <id>2001</id>
        <name>Peter Parker</name>
        <email>peter.parker@example.com</email>
        <age>25</age>
        <city>New York</city>
    </user>
    <user>
        <id>2002</id>
        <name>Mary Jane</name>
        <email>mary.jane@example.com</email>
        <age>24</age>
        <city>Boston</city>
    </user>
</users>
"""
    
    with open(filename, 'w') as f:
        f.write(xml_content)
    
    with open('users_namespace.xml', 'w') as f:
        f.write(xml_with_namespace)
    
    print(f"✅ Created sample XML files: {filename} and users_namespace.xml")


def main():
    """Main interactive function."""
    
    print("=" * 50)
    print("XML USER LOOKUP TOOL")
    print("=" * 50)
    
    # Check if sample XML exists, create if not
    if not os.path.exists('users.xml'):
        create_sample_xml()
    
    # Initialize parser
    parser = UserXMLParser('users.xml')
    
    while True:
        print("\n📋 Options:")
        print("  1. Find user by ID")
        print("  2. Search users by criteria")
        print("  3. List all users")
        print("  4. Use XML with namespace")
        print("  5. Exit")
        
        choice = input("\nEnter your choice (1-5): ").strip()
        
        if choice == '1':
            # Find by ID
            user_id = input("Enter user ID: ").strip()
            
            # Try both methods
            print(f"\n🔍 Searching for user ID: {user_id}")
            
            # Method 1: ElementTree
            user = parser.find_user_by_id_elementtree(user_id)
            if user:
                print("\n✅ User found (ElementTree):")
                for key, value in user.items():
                    print(f"   {key}: {value}")
            else:
                print("\n❌ User not found with ElementTree")
            
            # Method 2: lxml (if available)
            try:
                user_lxml = parser.find_user_by_id_lxml(user_id)
                if user_lxml:
                    print("\n✅ User found (lxml):")
                    for key, value in user_lxml.items():
                        print(f"   {key}: {value}")
            except:
                print("\n⚠️  lxml not available, install with: pip install lxml")
        
        elif choice == '2':
            # Search by criteria
            print("\nEnter search criteria (leave empty to skip):")
            criteria = {}
            
            field = input("Field name (e.g., city, department): ").strip()
            if field:
                value = input(f"Value for {field}: ").strip()
                if value:
                    criteria[field] = value
            
            if criteria:
                print(f"\n🔍 Searching with criteria: {criteria}")
                users = parser.find_users_by_criteria(**criteria)
                
                if users:
                    print(f"\n✅ Found {len(users)} user(s):")
                    for i, user in enumerate(users, 1):
                        print(f"\n  User {i}:")
                        for key, value in user.items():
                            print(f"    {key}: {value}")
                else:
                    print("\n❌ No users found")
        
        elif choice == '3':
            # List all users
            print("\n📋 All users:")
            users = parser.get_all_users()
            for i, user in enumerate(users, 1):
                print(f"\n  User {i}:")
                for key, value in user.items():
                    print(f"    {key}: {value}")
        
        elif choice == '4':
            # Test with namespace XML
            ns_parser = UserXMLParser('users_namespace.xml')
            user_id = input("Enter user ID to find in namespace XML: ").strip()
            user = ns_parser.find_user_by_id_lxml(user_id)
            
            if user:
                print("\n✅ User found in namespace XML:")
                for key, value in user.items():
                    print(f"   {key}: {value}")
            else:
                print("\n❌ User not found")
        
        elif choice == '5' or choice.lower() == 'quit':
            print("Goodbye!")
            break
        
        else:
            print("Invalid choice, please try again")


# Simple function version
def find_user_by_id_simple(xml_file: str, user_id: str) -> Optional[Dict]:
    """
    Simple function to find user by ID using XPath.
    
    Args:
        xml_file: Path to XML file
        user_id: User ID to find
    
    Returns:
        User details or None
    """
    try:
        tree = ET.parse(xml_file)
        root = tree.getroot()
        
        # Simple XPath
        for user in root.findall('.//user'):
            id_elem = user.find('id')
            if id_elem is not None and id_elem.text == user_id:
                # Convert to dict
                user_dict = {}
                for child in user:
                    user_dict[child.tag] = child.text
                return user_dict
        
        return None
        
    except Exception as e:
        print(f"Error: {e}")
        return None


if __name__ == "__main__":
    # Command-line usage
    if len(sys.argv) > 2:
        xml_file = sys.argv[1]
        user_id = sys.argv[2]
        
        user = find_user_by_id_simple(xml_file, user_id)
        
        if user:
            print(f"User found:")
            for key, value in user.items():
                print(f"  {key}: {value}")
        else:
            print(f"User with ID {user_id} not found")
            sys.exit(1)
    else:
        # Interactive mode
        main()