#!/usr/bin/env python3
"""
Database Operation with BaseException Handling
"""

import sys
import time
import logging
import traceback
from typing import Any, Dict, Optional, Callable
from datetime import datetime
import signal
import gc

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


# ============================================
# DATABASE OPERATION WRAPPER WITH BaseException
# ============================================

def perform_database_operation(operation_func: Callable, *args, **kwargs) -> Dict[str, Any]:
    """
    Execute a complex database operation with comprehensive exception handling.
    Catches BaseException to handle all exceptions including system exits.
    
    Args:
        operation_func: The database operation function to execute
        *args: Arguments to pass to the operation function
        **kwargs: Keyword arguments to pass to the operation function
    
    Returns:
        Dictionary with operation result and status information
    """
    
    # Result structure
    result = {
        'success': False,
        'data': None,
        'error': None,
        'error_type': None,
        'error_details': None,
        'traceback': None,
        'execution_time': 0,
        'timestamp': datetime.now().isoformat(),
        'operation': operation_func.__name__
    }
    
    start_time = time.time()
    
    try:
        logger.info(f"Starting database operation: {operation_func.__name__}")
        
        # Execute the database operation
        operation_result = operation_func(*args, **kwargs)
        
        result['success'] = True
        result['data'] = operation_result
        
        logger.info(f"Database operation completed successfully: {operation_func.__name__}")
        
    except KeyboardInterrupt as e:
        # Handle Ctrl+C specifically
        result['error'] = "Operation interrupted by user (KeyboardInterrupt)"
        result['error_type'] = 'KeyboardInterrupt'
        result['error_details'] = str(e)
        logger.warning("Database operation interrupted by user")
        
        # Perform cleanup for interruption
        cleanup_database_connection()
        
    except SystemExit as e:
        # Handle sys.exit() calls
        result['error'] = f"System exit requested: {e}"
        result['error_type'] = 'SystemExit'
        result['error_details'] = str(e)
        logger.error(f"System exit during database operation: {e}")
        
        # Don't actually exit, but log and cleanup
        cleanup_database_connection()
        
    except GeneratorExit as e:
        # Handle generator exit
        result['error'] = "Generator exited during operation"
        result['error_type'] = 'GeneratorExit'
        result['error_details'] = str(e)
        logger.error(f"Generator exit during database operation: {e}")
        
    except MemoryError as e:
        # Handle memory errors specifically
        result['error'] = "Memory error during database operation"
        result['error_type'] = 'MemoryError'
        result['error_details'] = str(e)
        logger.error(f"Memory error: {e}")
        
        # Attempt to free memory
        gc.collect()
        
    except OverflowError as e:
        result['error'] = f"Overflow error: {e}"
        result['error_type'] = 'OverflowError'
        result['error_details'] = str(e)
        logger.error(f"Overflow error: {e}")
        
    except RecursionError as e:
        result['error'] = f"Recursion depth exceeded: {e}"
        result['error_type'] = 'RecursionError'
        result['error_details'] = str(e)
        logger.error(f"Recursion error: {e}")
        
    except Exception as e:
        # Catch all other exceptions
        result['error'] = f"Unexpected error: {e}"
        result['error_type'] = type(e).__name__
        result['error_details'] = str(e)
        result['traceback'] = traceback.format_exc()
        logger.error(f"Unexpected error during database operation: {e}")
        logger.debug(traceback.format_exc())
        
    except BaseException as e:
        # Catch any other base exceptions (should be last)
        result['error'] = f"Critical base exception: {e}"
        result['error_type'] = type(e).__name__
        result['error_details'] = str(e)
        result['traceback'] = traceback.format_exc()
        logger.critical(f"BaseException caught: {e}")
        logger.critical(traceback.format_exc())
        
        # Special handling for critical errors
        emergency_cleanup()
        
    finally:
        # Always execute this block
        execution_time = time.time() - start_time
        result['execution_time'] = execution_time
        logger.info(f"Operation completed in {execution_time:.3f} seconds")
        
        # Ensure database connection is properly handled
        ensure_database_cleanup()
    
    return result


# ============================================
# DATABASE OPERATION EXAMPLES
# ============================================

def complex_database_query(user_id: int) -> Dict:
    """
    Simulate a complex database query that might raise various exceptions.
    """
    logger.info(f"Executing complex query for user {user_id}")
    
    # Simulate different error conditions based on user_id
    if user_id < 0:
        raise ValueError("Invalid user ID: must be positive")
    
    if user_id == 42:
        # Simulate a KeyboardInterrupt (Ctrl+C)
        raise KeyboardInterrupt("User interrupted the operation")
    
    if user_id == 99:
        # Simulate a SystemExit
        sys.exit("System exit requested")
    
    if user_id == 100:
        # Simulate a MemoryError
        huge_list = [0] * (10**10)  # This will raise MemoryError
    
    if user_id == 666:
        # Simulate a database connection error
        raise ConnectionError("Database connection lost")
    
    if user_id == 999:
        # Simulate a timeout
        time.sleep(10)  # Simulate long operation
        raise TimeoutError("Query timed out")
    
    # Simulate successful query
    time.sleep(0.5)  # Simulate processing time
    
    return {
        'user_id': user_id,
        'name': f'User {user_id}',
        'email': f'user{user_id}@example.com',
        'created_at': datetime.now().isoformat()
    }


def bulk_insert_operation(records: list) -> Dict:
    """
    Simulate a bulk insert operation.
    """
    logger.info(f"Bulk inserting {len(records)} records")
    
    if not records:
        raise ValueError("No records to insert")
    
    # Simulate processing
    time.sleep(1)
    
    # Simulate a random error
    import random
    if random.random() < 0.3:  # 30% chance of error
        raise RuntimeError("Random database error occurred")
    
    return {
        'inserted': len(records),
        'ids': list(range(1001, 1001 + len(records)))
    }


def transaction_operation(account_from: int, account_to: int, amount: float) -> Dict:
    """
    Simulate a transaction operation.
    """
    logger.info(f"Transferring {amount} from {account_from} to {account_to}")
    
    # Validation
    if amount <= 0:
        raise ValueError("Amount must be positive")
    
    if amount > 10000:
        raise OverflowError("Amount exceeds maximum transfer limit")
    
    # Simulate balance check
    if account_from == 999:
        raise RuntimeError("Insufficient funds")
    
    # Simulate deadlock
    if account_from == 888 and account_to == 777:
        raise Exception("Deadlock detected, transaction rolled back")
    
    # Successful transaction
    return {
        'transaction_id': int(time.time()),
        'from': account_from,
        'to': account_to,
        'amount': amount,
        'status': 'completed',
        'timestamp': datetime.now().isoformat()
    }


# ============================================
# CLEANUP FUNCTIONS
# ============================================

def cleanup_database_connection():
    """Clean up database connection resources."""
    logger.info("Performing database connection cleanup")
    # In real code, you would close connections, rollback transactions, etc.
    # db_connection.rollback()
    # db_connection.close()


def emergency_cleanup():
    """Emergency cleanup for critical errors."""
    logger.critical("Performing emergency cleanup")
    # In real code, you would force-close connections, release locks, etc.
    # force_close_all_connections()
    # release_all_locks()


def ensure_database_cleanup():
    """Ensure database resources are properly released."""
    logger.info("Ensuring database cleanup")
    # In real code, you would verify connections are closed
    # verify_connection_pool()


# ============================================
# CONTEXT MANAGER VERSION
# ============================================

class DatabaseOperation:
    """Context manager for database operations with BaseException handling"""
    
    def __init__(self, operation_name: str):
        self.operation_name = operation_name
        self.start_time = None
        self.result = None
    
    def __enter__(self):
        self.start_time = time.time()
        logger.info(f"Starting database operation: {self.operation_name}")
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        execution_time = time.time() - self.start_time
        
        if exc_type is None:
            logger.info(f"Operation {self.operation_name} completed in {execution_time:.3f}s")
        else:
            if issubclass(exc_type, BaseException):
                logger.critical(f"BaseException in {self.operation_name}: {exc_type.__name__}: {exc_val}")
                self._handle_base_exception(exc_type, exc_val, exc_tb)
            else:
                logger.error(f"Exception in {self.operation_name}: {exc_type.__name__}: {exc_val}")
        
        # Always cleanup
        ensure_database_cleanup()
        
        # Return True to suppress exception, False to propagate
        return False  # Don't suppress exceptions
    
    def _handle_base_exception(self, exc_type, exc_val, exc_tb):
        """Handle base exceptions specially."""
        if exc_type is KeyboardInterrupt:
            logger.warning("Operation interrupted by user")
            cleanup_database_connection()
        elif exc_type is SystemExit:
            logger.error("System exit requested")
            cleanup_database_connection()
        elif exc_type is MemoryError:
            logger.error("Memory error detected")
            gc.collect()
        else:
            emergency_cleanup()


# ============================================
# DECORATOR VERSION
# ============================================

def with_database_handling(func):
    """Decorator to add database exception handling to functions."""
    def wrapper(*args, **kwargs):
        return perform_database_operation(func, *args, **kwargs)
    return wrapper


# ============================================
# TEST FUNCTIONS
# ============================================

def test_normal_operation():
    """Test normal database operation."""
    print("\n" + "=" * 60)
    print("TEST 1: Normal Operation")
    print("=" * 60)
    
    result = perform_database_operation(complex_database_query, 123)
    
    if result['success']:
        print(f"✅ Success: {result['data']['name']}")
    else:
        print(f"❌ Failed: {result['error']}")
    
    print(f"⏱️  Execution time: {result['execution_time']:.3f}s")


def test_value_error():
    """Test ValueError handling."""
    print("\n" + "=" * 60)
    print("TEST 2: ValueError")
    print("=" * 60)
    
    result = perform_database_operation(complex_database_query, -5)
    
    if not result['success']:
        print(f"✅ Correctly caught: {result['error_type']}")
        print(f"   Error: {result['error']}")
    else:
        print("❌ Should have failed")


def test_keyboard_interrupt():
    """Test KeyboardInterrupt handling."""
    print("\n" + "=" * 60)
    print("TEST 3: KeyboardInterrupt")
    print("=" * 60)
    
    result = perform_database_operation(complex_database_query, 42)
    
    if not result['success']:
        print(f"✅ Caught KeyboardInterrupt")
        print(f"   Message: {result['error']}")
    else:
        print("❌ Should have been interrupted")


def test_system_exit():
    """Test SystemExit handling."""
    print("\n" + "=" * 60)
    print("TEST 4: SystemExit")
    print("=" * 60)
    
    result = perform_database_operation(complex_database_query, 99)
    
    if not result['success']:
        print(f"✅ Caught SystemExit")
        print(f"   Message: {result['error']}")
    else:
        print("❌ Should have exited")


def test_memory_error():
    """Test MemoryError handling."""
    print("\n" + "=" * 60)
    print("TEST 5: MemoryError")
    print("=" * 60)
    
    result = perform_database_operation(complex_database_query, 100)
    
    if not result['success']:
        print(f"✅ Caught MemoryError")
        print(f"   Message: {result['error']}")
    else:
        print("❌ Should have failed")


def test_bulk_insert():
    """Test bulk insert with random errors."""
    print("\n" + "=" * 60)
    print("TEST 6: Bulk Insert with Random Errors")
    print("=" * 60)
    
    records = [{'id': i, 'name': f'User {i}'} for i in range(10)]
    result = perform_database_operation(bulk_insert_operation, records)
    
    if result['success']:
        print(f"✅ Success: Inserted {result['data']['inserted']} records")
    else:
        print(f"❌ Failed: {result['error']}")
        print(f"   Type: {result['error_type']}")


def test_transaction():
    """Test transaction operations."""
    print("\n" + "=" * 60)
    print("TEST 7: Transaction Operations")
    print("=" * 60)
    
    # Test cases
    test_cases = [
        (100, 200, 500),      # Normal
        (100, 200, -50),       # ValueError
        (100, 200, 50000),     # OverflowError
        (999, 200, 100),       # RuntimeError
        (888, 777, 100),       # Generic Exception
    ]
    
    for from_acc, to_acc, amount in test_cases:
        print(f"\n   Transfer ${amount}: {from_acc} -> {to_acc}")
        result = perform_database_operation(transaction_operation, from_acc, to_acc, amount)
        
        if result['success']:
            print(f"   ✅ Success: {result['data']['transaction_id']}")
        else:
            print(f"   ❌ Failed: {result['error_type']} - {result['error']}")


def test_context_manager():
    """Test context manager version."""
    print("\n" + "=" * 60)
    print("TEST 8: Context Manager")
    print("=" * 60)
    
    try:
        with DatabaseOperation("complex_query") as op:
            result = complex_database_query(123)
            print(f"   Result: {result['name']}")
            
        with DatabaseOperation("error_query") as op:
            result = complex_database_query(-5)
            print("   This won't print")
            
    except ValueError as e:
        print(f"   ✅ Caught ValueError: {e}")
    
    except Exception as e:
        print(f"   Caught: {e}")


def test_decorator():
    """Test decorator version."""
    print("\n" + "=" * 60)
    print("TEST 9: Decorator")
    print("=" * 60)
    
    @with_database_handling
    def get_user(user_id):
        return complex_database_query(user_id)
    
    # Test normal
    result = get_user(123)
    if result['success']:
        print(f"   Decorator success: {result['data']['name']}")
    
    # Test error
    result = get_user(-5)
    if not result['success']:
        print(f"   Decorator caught: {result['error_type']}")


# ============================================
# MAIN
# ============================================

def main():
    """Run all tests."""
    print("\n" + "=" * 70)
    print("🔷 DATABASE OPERATION WITH BASEException HANDLING")
    print("=" * 70)
    
    # Run tests
    test_normal_operation()
    test_value_error()
    test_keyboard_interrupt()
    test_system_exit()
    test_memory_error()
    test_bulk_insert()
    test_transaction()
    test_context_manager()
    test_decorator()
    
    print("\n" + "=" * 70)
    print("✅ All tests completed")
    print("=" * 70)


if __name__ == "__main__":
    main()