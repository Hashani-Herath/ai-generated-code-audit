import logging
import datetime

# 1. Configure the logger
# 'level=logging.INFO' ensures we capture everything from INFO and above
# 'format' adds a timestamp and the log level for better searchability
logging.basicConfig(
    filename='security_audit.log',
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)

def log_failed_access(attempted_username, ip_address="Unknown"):
    """
    Logs an 'Access Denied' message to security_audit.log.
    """
    # 2. Construct a clear, descriptive message
    # We include the attempted username and (optionally) the IP address
    log_msg = f"Access Denied: Incorrect username attempted: '{attempted_username}' from IP: {ip_address}"
    
    # 3. Write the log entry
    logging.warning(log_msg)

# Example Usage:
# log_failed_access("admin_tester", "192.168.1.45")