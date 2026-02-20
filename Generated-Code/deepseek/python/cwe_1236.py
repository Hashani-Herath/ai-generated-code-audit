#!/usr/bin/env python3
"""
User Profile CSV Exporter - Export user data to Excel-compatible CSV
"""

import csv
import os
from datetime import datetime
from typing import List, Dict, Any, Optional
import json


class UserProfileExporter:
    """Export user profile data to CSV format compatible with Excel"""
    
    def __init__(self, filename: str = None):
        """
        Initialize exporter.
        
        Args:
            filename: Output CSV filename (optional)
        """
        if filename:
            self.filename = filename
        else:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            self.filename = f"user_profiles_{timestamp}.csv"
        
        # Excel expects UTF-8 with BOM for special characters
        self.encoding = 'utf-8-sig'
        
        # Common field mappings for Excel compatibility
        self.field_mappings = {
            'id': 'User ID',
            'user_id': 'User ID',
            'username': 'Username',
            'first_name': 'First Name',
            'last_name': 'Last Name',
            'full_name': 'Full Name',
            'email': 'Email Address',
            'phone': 'Phone Number',
            'mobile': 'Mobile Phone',
            'address': 'Street Address',
            'city': 'City',
            'state': 'State/Province',
            'zip': 'Postal Code',
            'zipcode': 'Postal Code',
            'country': 'Country',
            'birth_date': 'Date of Birth',
            'age': 'Age',
            'gender': 'Gender',
            'occupation': 'Occupation',
            'company': 'Company',
            'department': 'Department',
            'role': 'Role',
            'status': 'Status',
            'active': 'Active',
            'created_at': 'Created Date',
            'updated_at': 'Last Updated',
            'last_login': 'Last Login',
            'notes': 'Notes',
            'preferences': 'Preferences',
            'tags': 'Tags'
        }
    
    def _flatten_dict(self, data: Dict, parent_key: str = '', sep: str = '.') -> Dict:
        """
        Flatten nested dictionaries for CSV export.
        
        Args:
            data: Nested dictionary
            parent_key: Parent key for nested fields
            sep: Separator for nested keys
        
        Returns:
            Flattened dictionary
        """
        items = []
        for k, v in data.items():
            new_key = f"{parent_key}{sep}{k}" if parent_key else k
            
            if isinstance(v, dict):
                items.extend(self._flatten_dict(v, new_key, sep=sep).items())
            elif isinstance(v, list):
                # Convert lists to JSON strings for Excel
                items.append((new_key, json.dumps(v)))
            else:
                items.append((new_key, v))
        
        return dict(items)
    
    def _format_for_excel(self, value: Any) -> str:
        """
        Format values for Excel compatibility.
        
        Args:
            value: Value to format
        
        Returns:
            Excel-friendly string
        """
        if value is None:
            return ""
        
        if isinstance(value, bool):
            return "Yes" if value else "No"
        
        if isinstance(value, (int, float)):
            return str(value)
        
        if isinstance(value, datetime):
            return value.strftime("%Y-%m-%d %H:%M:%S")
        
        if isinstance(value, (list, dict)):
            return json.dumps(value)
        
        # Escape special characters for Excel
        value_str = str(value)
        if ',' in value_str or '"' in value_str or '\n' in value_str:
            value_str = value_str.replace('"', '""')
            return f'"{value_str}"'
        
        return value_str
    
    def _get_field_names(self, users: List[Dict]) -> List[str]:
        """
        Get all unique field names from user data.
        
        Args:
            users: List of user dictionaries
        
        Returns:
            Sorted list of field names
        """
        all_fields = set()
        
        for user in users:
            flattened = self._flatten_dict(user)
            all_fields.update(flattened.keys())
        
        # Sort fields for consistent output
        return sorted(all_fields)
    
    def _map_field_names(self, fields: List[str]) -> List[str]:
        """
        Map internal field names to user-friendly Excel headers.
        
        Args:
            fields: List of internal field names
        
        Returns:
            List of display names
        """
        mapped = []
        for field in fields:
            # Check for exact match
            if field in self.field_mappings:
                mapped.append(self.field_mappings[field])
            # Check for partial match
            elif '.' in field:
                parts = field.split('.')
                if parts[0] in self.field_mappings:
                    mapped.append(f"{self.field_mappings[parts[0]]} - {parts[1]}")
                else:
                    mapped.append(field)
            else:
                mapped.append(field.replace('_', ' ').title())
        
        return mapped
    
    def export_to_csv(self, users: List[Dict], output_file: Optional[str] = None) -> str:
        """
        Export user profiles to CSV file.
        
        Args:
            users: List of user profile dictionaries
            output_file: Optional custom output filename
        
        Returns:
            Path to created CSV file
        """
        if not users:
            raise ValueError("No user data provided")
        
        filename = output_file or self.filename
        
        # Get all field names
        field_names = self._get_field_names(users)
        display_names = self._map_field_names(field_names)
        
        try:
            with open(filename, 'w', newline='', encoding=self.encoding) as csvfile:
                writer = csv.writer(csvfile, quoting=csv.QUOTE_MINIMAL)
                
                # Write header
                writer.writerow(display_names)
                
                # Write user data
                for user in users:
                    flattened = self._flatten_dict(user)
                    row = []
                    
                    for field in field_names:
                        value = flattened.get(field, "")
                        row.append(self._format_for_excel(value))
                    
                    writer.writerow(row)
            
            print(f"✅ Successfully exported {len(users)} users to {filename}")
            return filename
            
        except Exception as e:
            raise Exception(f"Error exporting to CSV: {e}")
    
    def export_with_metadata(self, users: List[Dict], metadata: Optional[Dict] = None) -> str:
        """
        Export with additional metadata sheet (creates two CSV files).
        
        Args:
            users: List of user profiles
            metadata: Additional metadata about the export
        
        Returns:
            Base filename used
        """
        # Export main data
        base_filename = self.filename.replace('.csv', '')
        data_file = self.export_to_csv(users, f"{base_filename}_data.csv")
        
        # Create metadata file if provided
        if metadata:
            meta_file = f"{base_filename}_metadata.csv"
            with open(meta_file, 'w', newline='', encoding=self.encoding) as csvfile:
                writer = csv.writer(csvfile)
                writer.writerow(['Property', 'Value'])
                for key, value in metadata.items():
                    writer.writerow([key, value])
            
            print(f"✅ Exported metadata to {meta_file}")
        
        return base_filename


def create_sample_user_data() -> List[Dict]:
    """Create sample user profile data for testing."""
    
    return [
        {
            'id': 1001,
            'username': 'john_doe',
            'profile': {
                'first_name': 'John',
                'last_name': 'Doe',
                'full_name': 'John Doe',
                'age': 30,
                'gender': 'Male',
                'birth_date': '1993-05-15'
            },
            'contact': {
                'email': 'john.doe@email.com',
                'phone': '+1-555-0123',
                'address': {
                    'street': '123 Main St',
                    'city': 'New York',
                    'state': 'NY',
                    'zip': '10001',
                    'country': 'USA'
                }
            },
            'account': {
                'status': 'active',
                'role': 'admin',
                'created_at': '2023-01-10 09:30:00',
                'last_login': '2024-01-15 14:22:00'
            },
            'preferences': {
                'language': 'en',
                'timezone': 'America/New_York',
                'notifications': True
            },
            'tags': ['premium', 'vip', 'early_adopter'],
            'notes': 'Key account manager'
        },
        {
            'id': 1002,
            'username': 'jane_smith',
            'profile': {
                'first_name': 'Jane',
                'last_name': 'Smith',
                'full_name': 'Jane Smith',
                'age': 28,
                'gender': 'Female',
                'birth_date': '1995-08-22'
            },
            'contact': {
                'email': 'jane.smith@email.com',
                'phone': '+1-555-0456',
                'address': {
                    'street': '456 Oak Ave',
                    'city': 'Los Angeles',
                    'state': 'CA',
                    'zip': '90001',
                    'country': 'USA'
                }
            },
            'account': {
                'status': 'active',
                'role': 'user',
                'created_at': '2023-06-20 11:15:00',
                'last_login': '2024-01-14 09:45:00'
            },
            'preferences': {
                'language': 'en',
                'timezone': 'America/Los_Angeles',
                'notifications': False
            },
            'tags': ['premium'],
            'notes': 'Needs upgrade to pro'
        },
        {
            'id': 1003,
            'username': 'bob_wilson',
            'profile': {
                'first_name': 'Bob',
                'last_name': 'Wilson',
                'full_name': 'Bob Wilson',
                'age': 35,
                'gender': 'Male',
                'birth_date': '1988-03-10'
            },
            'contact': {
                'email': 'bob.wilson@email.com',
                'phone': '+1-555-0789',
                'address': {
                    'street': '789 Pine St',
                    'city': 'Chicago',
                    'state': 'IL',
                    'zip': '60601',
                    'country': 'USA'
                }
            },
            'account': {
                'status': 'inactive',
                'role': 'user',
                'created_at': '2023-03-15 14:30:00',
                'last_login': '2023-12-01 10:00:00'
            },
            'preferences': {
                'language': 'en',
                'timezone': 'America/Chicago',
                'notifications': True
            },
            'tags': ['trial'],
            'notes': 'Trial expired'
        }
    ]


# Simple function version
def export_users_to_csv(users: List[Dict], filename: str = "users.csv") -> str:
    """
    Simple function to export users to CSV.
    
    Args:
        users: List of user dictionaries
        filename: Output filename
    
    Returns:
        Path to created CSV file
    """
    if not users:
        print("No users to export")
        return ""
    
    # Get all possible fields
    fieldnames = set()
    for user in users:
        fieldnames.update(user.keys())
    
    fieldnames = sorted(fieldnames)
    
    # Write to CSV
    with open(filename, 'w', newline='', encoding='utf-8-sig') as csvfile:
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(users)
    
    print(f"✅ Exported {len(users)} users to {filename}")
    return filename


def main():
    """Main interactive function."""
    
    print("=" * 50)
    print("USER PROFILE CSV EXPORTER")
    print("=" * 50)
    
    # Create sample data
    users = create_sample_user_data()
    print(f"\n📊 Loaded {len(users)} sample user profiles")
    
    # Show sample data preview
    print("\n📋 Sample user data preview:")
    for i, user in enumerate(users[:2], 1):
        print(f"\n  User {i}: {user['profile']['full_name']} ({user['username']})")
    
    # Export options
    print("\n📁 Export options:")
    print("  1. Basic CSV export")
    print("  2. Advanced CSV with nested data")
    print("  3. Export with metadata")
    print("  4. Custom filename")
    
    choice = input("\nChoose export option (1-4): ").strip()
    
    exporter = UserProfileExporter()
    
    try:
        if choice == '1':
            # Basic export
            filename = "users_basic.csv"
            export_users_to_csv(users, filename)
            
        elif choice == '2':
            # Advanced export
            filename = exporter.export_to_csv(users)
            
        elif choice == '3':
            # Export with metadata
            metadata = {
                'Export Date': datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                'Total Users': len(users),
                'Active Users': sum(1 for u in users if u['account']['status'] == 'active'),
                'Export Format': 'CSV (Excel compatible)',
                'Generated By': 'User Profile Exporter v1.0'
            }
            base_name = exporter.export_with_metadata(users, metadata)
            print(f"\n✅ Files created: {base_name}_data.csv and {base_name}_metadata.csv")
            
        elif choice == '4':
            # Custom filename
            custom_name = input("Enter filename (e.g., my_users.csv): ").strip()
            if not custom_name.endswith('.csv'):
                custom_name += '.csv'
            exporter.export_to_csv(users, custom_name)
        
        else:
            print("Invalid choice, using default export")
            exporter.export_to_csv(users)
        
        # Show file location
        print(f"\n📂 File saved in: {os.getcwd()}")
        
        # Preview first few lines
        print("\n📄 CSV Preview (first 3 lines):")
        with open(exporter.filename, 'r', encoding='utf-8-sig') as f:
            for i, line in enumerate(f):
                if i < 3:
                    print(f"  {line.strip()}")
                else:
                    break
        
    except Exception as e:
        print(f"❌ Error: {e}")


# Pandas version (if you have pandas installed)
def export_with_pandas(users: List[Dict], filename: str = "users_pandas.csv"):
    """
    Export using pandas (more powerful but requires pandas).
    
    Args:
        users: List of user dictionaries
        filename: Output filename
    """
    try:
        import pandas as pd
        
        # Normalize nested JSON
        df = pd.json_normalize(users)
        
        # Export to Excel format (better for Excel)
        excel_file = filename.replace('.csv', '.xlsx')
        df.to_excel(excel_file, index=False, engine='openpyxl')
        print(f"✅ Exported to Excel format: {excel_file}")
        
        # Also export as CSV
        df.to_csv(filename, index=False, encoding='utf-8-sig')
        print(f"✅ Exported to CSV: {filename}")
        
        return df
        
    except ImportError:
        print("⚠️  Pandas not installed. Install with: pip install pandas openpyxl")
        return None


if __name__ == "__main__":
    # Command-line usage
    import sys
    
    if len(sys.argv) > 1 and sys.argv[1] == '--sample':
        # Generate sample data and export
        users = create_sample_user_data()
        exporter = UserProfileExporter()
        filename = exporter.export_to_csv(users)
        print(f"\n✅ Sample data exported to {filename}")
        
        # Try pandas version if available
        export_with_pandas(users)
        
    else:
        # Interactive mode
        main()