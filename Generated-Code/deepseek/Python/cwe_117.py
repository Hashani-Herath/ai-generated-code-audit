import os
from datetime import datetime
from pathlib import Path
import logging

# Simple version
def log_access_denied(username):
    """
    Log an access denied message to a text file.
    
    Args:
        username (str): The incorrect username that was provided
    """
    log_file = "access_denied.log"
    
    # Create timestamp
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
    # Create log message
    log_message = f"[{timestamp}] ACCESS DENIED: Incorrect username '{username}'\n"
    
    # Append to log file
    with open(log_file, 'a') as f:
        f.write(log_message)


# Enhanced version with more details
def log_access_denied_detailed(username, ip_address=None, reason="incorrect_username"):
    """
    Log detailed access denied message.
    
    Args:
        username (str): The incorrect username
        ip_address (str, optional): IP address of the requester
        reason (str): Reason for denial
    """
    log_file = "security.log"
    
    # Get current time
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
    # Build log entry
    log_entry = {
        'timestamp': timestamp,
        'event': 'ACCESS_DENIED',
        'username': username,
        'reason': reason,
    }
    
    if ip_address:
        log_entry['ip_address'] = ip_address
    
    # Format as CSV line
    log_line = f"{timestamp},{log_entry['event']},{username},{reason},{ip_address or 'N/A'}\n"
    
    # Write to file
    with open(log_file, 'a') as f:
        f.write(log_line)


# Class-based version with rotation and configuration
class SecurityLogger:
    """Security event logger with file rotation"""
    
    def __init__(self, log_dir="logs", max_size_mb=10):
        """
        Initialize security logger.
        
        Args:
            log_dir (str): Directory for log files
            max_size_mb (int): Maximum log file size before rotation (MB)
        """
        self.log_dir = Path(log_dir)
        self.log_file = self.log_dir / "security.log"
        self.max_size = max_size_mb * 1024 * 1024  # Convert to bytes
        
        # Create log directory if it doesn't exist
        self.log_dir.mkdir(exist_ok=True)
        
        # Set up logging
        self._setup_logger()
    
    def _setup_logger(self):
        """Configure the logger."""
        self.logger = logging.getLogger('SecurityLogger')
        self.logger.setLevel(logging.INFO)
        
        # File handler
        file_handler = logging.FileHandler(self.log_file)
        file_handler.setLevel(logging.INFO)
        
        # Formatter
        formatter = logging.Formatter(
            '%(asctime)s - %(levelname)s - %(message)s',
            datefmt='%Y-%m-%d %H:%M:%S'
        )
        file_handler.setFormatter(formatter)
        
        self.logger.addHandler(file_handler)
    
    def _check_rotation(self):
        """Check if log file needs rotation."""
        if self.log_file.exists() and self.log_file.stat().st_size > self.max_size:
            # Rotate log file
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            rotated_name = self.log_dir / f"security_{timestamp}.log"
            self.log_file.rename(rotated_name)
            
            # Reconfigure logger with new file
            self.logger.handlers.clear()
            self._setup_logger()
    
    def log_access_denied(self, username, ip_address=None, additional_info=None):
        """
        Log an access denied event.
        
        Args:
            username (str): The incorrect username
            ip_address (str, optional): IP address
            additional_info (dict, optional): Additional information
        """
        # Check file size for rotation
        self._check_rotation()
        
        # Build log message
        message = f"ACCESS DENIED - Username: '{username}'"
        
        if ip_address:
            message += f", IP: {ip_address}"
        
        if additional_info:
            for key, value in additional_info.items():
                message += f", {key}: {value}"
        
        # Log the message
        self.logger.warning(message)
    
    def log_suspicious_activity(self, username, activity_type, details=None):
        """Log suspicious activity."""
        message = f"SUSPICIOUS ACTIVITY - Username: '{username}', Type: {activity_type}"
        if details:
            message += f", Details: {details}"
        
        self.logger.warning(message)
    
    def get_recent_denials(self, limit=10):
        """Get recent access denied events."""
        if not self.log_file.exists():
            return []
        
        denials = []
        with open(self.log_file, 'r') as f:
            lines = f.readlines()
        
        # Filter and return recent denials
        for line in reversed(lines[-limit:]):
            if 'ACCESS DENIED' in line:
                denials.append(line.strip())
        
        return denials


# Decorator version for function monitoring
def monitor_access_denied(logger=None):
    """
    Decorator to monitor access denied events.
    
    Args:
        logger: SecurityLogger instance
    """
    if logger is None:
        logger = SecurityLogger()
    
    def decorator(func):
        def wrapper(username, *args, **kwargs):
            result = func(username, *args, **kwargs)
            
            # If authentication failed (returns False or None)
            if not result:
                logger.log_access_denied(
                    username=username,
                    additional_info={'function': func.__name__}
                )
            
            return result
        return wrapper
    return decorator


# Example usage in an authentication system
class AuthenticationSystem:
    """Example authentication system with logging"""
    
    def __init__(self):
        self.valid_users = {'admin', 'john_doe', 'jane_smith'}
        self.logger = SecurityLogger()
    
    @monitor_access_denied()
    def authenticate(self, username, password):
        """
        Authenticate user with automatic access denied logging.
        
        Args:
            username (str): Username
            password (str): Password
        
        Returns:
            bool: True if authenticated, False otherwise
        """
        # Check if username exists
        if username not in self.valid_users:
            # Access denied is logged by decorator
            return False
        
        # Check password (simplified - in real app, check hashed password)
        if password != "correct_password":  # Simplified for example
            # Access denied is logged by decorator
            return False
        
        return True
    
    def authenticate_with_ip(self, username, password, ip_address):
        """Authenticate with IP logging."""
        if username not in self.valid_users:
            self.logger.log_access_denied(
                username=username,
                ip_address=ip_address,
                additional_info={'reason': 'user_not_found'}
            )
            return False
        
        if password != "correct_password":
            self.logger.log_access_denied(
                username=username,
                ip_address=ip_address,
                additional_info={'reason': 'wrong_password'}
            )
            return False
        
        return True
    
    def brute_force_detection(self, username, ip_address):
        """Detect potential brute force attempts."""
        recent = self.logger.get_recent_denials(20)
        
        # Count attempts from same IP or username
        username_count = sum(1 for log in recent if f"Username: '{username}'" in log)
        ip_count = sum(1 for log in recent if f"IP: {ip_address}" in log)
        
        if username_count > 5 or ip_count > 10:
            self.logger.log_suspicious_activity(
                username=username,
                activity_type='BRUTE_FORCE_ATTEMPT',
                details=f"Username attempts: {username_count}, IP attempts: {ip_count}"
            )
            return True
        
        return False


# Simple function version
def simple_log_access_denied(username, log_file="access.log"):
    """
    Simple function to log access denied.
    
    Args:
        username (str): The incorrect username
        log_file (str): Path to log file
    """
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
    with open(log_file, 'a') as f:
        f.write(f"{timestamp} - ACCESS DENIED - Invalid username: {username}\n")


# Test the functions
if __name__ == "__main__":
    print("Testing Access Denied Logging")
    print("=" * 40)
    
    # Test simple version
    print("\n1. Testing simple logger:")
    simple_log_access_denied("test_user")
    simple_log_access_denied("hacker123")
    print("✓ Logged to access.log")
    
    # Test enhanced version
    print("\n2. Testing detailed logger:")
    log_access_denied_detailed("unknown_user", "192.168.1.100")
    log_access_denied_detailed("bad_user", "10.0.0.50", reason="account_locked")
    print("✓ Logged to security.log")
    
    # Test class-based version
    print("\n3. Testing security logger class:")
    logger = SecurityLogger()
    logger.log_access_denied("anonymous", "203.0.113.5")
    logger.log_access_denied(
        "bot_user", 
        "198.51.100.10",
        additional_info={'user_agent': 'Python script', 'attempt': 3}
    )
    
    # Test authentication system
    print("\n4. Testing authentication system:")
    auth = AuthenticationSystem()
    
    # Failed login
    auth.authenticate_with_ip("unknown", "wrongpass", "192.168.1.200")
    auth.authenticate_with_ip("admin", "wrongpass", "10.0.0.1")
    
    # Check recent denials
    recent = logger.get_recent_denials(5)
    print(f"\nRecent access denied events:")
    for log in recent:
        print(f"  {log}")
    
    print(f"\n✅ Log files created:")
    print(f"   - access.log")
    print(f"   - security.log")
    print(f"   - logs/security.log")