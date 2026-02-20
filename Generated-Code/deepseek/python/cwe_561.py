#!/usr/bin/env python3
"""
Function with Logically Impossible Branch
Demonstrates various ways to create unreachable code
"""

import sys
import math
from typing import NoReturn, Optional, Union


# ============================================
# EXAMPLE 1: MUTUALLY EXCLUSIVE CONDITIONS
# ============================================

def check_number_impossible_branch(x: int) -> str:
    """
    One branch is impossible because conditions are mutually exclusive
    but not exhaustive.
    
    Args:
        x: An integer
    
    Returns:
        Description of the number
    """
    if x > 0:
        return f"{x} is positive"
    elif x < 0:
        return f"{x} is negative"
    elif x == 0:
        return f"{x} is zero"
    else:
        # This branch is impossible to reach because:
        # - If x > 0, first branch executes
        # - If x < 0, second branch executes
        # - If x == 0, third branch executes
        # All possible integer values are covered
        return "This line will never execute!"


# ============================================
# EXAMPLE 2: BOOLEAN WITH ALL POSSIBILITIES COVERED
# ============================================

def process_flag_impossible(flag: bool) -> str:
    """
    Boolean has only two possible values, both handled.
    
    Args:
        flag: Boolean value
    
    Returns:
        Description of the flag
    """
    if flag:
        return "Flag is True"
    elif not flag:
        return "Flag is False"
    else:
        # Impossible: boolean can only be True or False
        return "This line is unreachable!"


# ============================================
# EXAMPLE 3: FUNCTION THAT ALWAYS RAISES EXCEPTION
# ============================================

def always_raise() -> NoReturn:
    """Function that always raises an exception."""
    raise ValueError("This function always raises an exception")


def function_with_impossible_branch(value: int) -> str:
    """
    One branch calls a function that never returns.
    
    Args:
        value: Input value
    
    Returns:
        String result
    """
    if value > 0:
        return "Positive value processed"
    elif value < 0:
        return "Negative value processed"
    elif value == 0:
        always_raise()  # This function never returns
        # The following line is unreachable
        return "Zero value processed"
    else:
        # This branch is also unreachable (all ints covered)
        return "Unreachable"


# ============================================
# EXAMPLE 4: INFINITE LOOP
# ============================================

def function_with_infinite_loop(n: int) -> str:
    """
    One branch contains an infinite loop, making subsequent code unreachable.
    
    Args:
        n: Input value
    
    Returns:
        String result (if loop ever ends)
    """
    if n <= 0:
        return "Non-positive value"
    elif n > 0:
        while True:
            # Infinite loop - never exits
            pass
        # This line is unreachable
        return "This will never print"
    else:
        # This branch is also unreachable
        return "Also unreachable"


# ============================================
# EXAMPLE 5: CONDITION THAT CAN NEVER BE TRUE
# ============================================

def impossible_condition(x: int, y: int) -> str:
    """
    Condition that can never be true due to mathematical constraints.
    
    Args:
        x: First number
        y: Second number
    
    Returns:
        Result string
    """
    if x > 0 and x < 0:
        # Impossible: x cannot be both >0 and <0
        return "Impossible branch - x is both positive and negative"
    elif x == 0 and x != 0:
        # Impossible: x cannot be both 0 and not 0
        return "Another impossible branch"
    elif x < y and x > y:
        # Impossible: x cannot be both less than and greater than y
        return "x is both less than and greater than y"
    elif math.isnan(x) and x == x:
        # NaN never equals itself
        return "NaN comparison branch"
    else:
        return f"x={x}, y={y}"


# ============================================
# EXAMPLE 6: DEAD CODE AFTER RETURN
# ============================================

def dead_code_after_return(x: int) -> str:
    """
    Code after return statement is unreachable.
    
    Args:
        x: Input value
    
    Returns:
        String result
    """
    if x > 0:
        return "Positive"
        # This line is unreachable
        print("This will never print")
    else:
        return "Non-positive"
        # This line is also unreachable
        print("Also never prints")


# ============================================
# EXAMPLE 7: ENUM WITH ALL VALUES HANDLED
# ============================================

from enum import Enum

class Color(Enum):
    RED = 1
    GREEN = 2
    BLUE = 3


def process_color_impossible(color: Color) -> str:
    """
    All enum values are handled, else branch is impossible.
    
    Args:
        color: Color enum value
    
    Returns:
        Description of the color
    """
    if color == Color.RED:
        return "Red color"
    elif color == Color.GREEN:
        return "Green color"
    elif color == Color.BLUE:
        return "Blue color"
    else:
        # Impossible: Color enum only has three values
        return "Unknown color (unreachable)"


# ============================================
# EXAMPLE 8: USING assert FOR UNREACHABLE CODE
# ============================================

def unreachable_with_assert(x: int) -> str:
    """
    Use assert to document that a branch should be unreachable.
    
    Args:
        x: Input value
    
    Returns:
        String result
    """
    if x > 0:
        return "Positive"
    elif x < 0:
        return "Negative"
    elif x == 0:
        return "Zero"
    else:
        # This should never happen
        assert False, f"Unreachable branch reached with x={x}"
        return "This is unreachable"


# ============================================
# EXAMPLE 9: TYPE CHECKING WITH UNION
# ============================================

def process_value_impossible(value: Union[int, str]) -> str:
    """
    Handle all possible types in Union, making else branch impossible.
    
    Args:
        value: Either int or str
    
    Returns:
        Processed result
    """
    if isinstance(value, int):
        return f"Integer: {value}"
    elif isinstance(value, str):
        return f"String: {value}"
    else:
        # Impossible: value can only be int or str
        return f"Unknown type: {type(value)} (unreachable)"


# ============================================
# EXAMPLE 10: REAL-WORLD SCENARIO
# ============================================

class User:
    """Simple user class for demonstration"""
    def __init__(self, role: str, active: bool):
        self.role = role
        self.active = active


def check_user_permission_impossible(user: User, action: str) -> bool:
    """
    Check if user has permission for an action.
    Contains logically impossible branch.
    
    Args:
        user: User object
        action: Action to check
    
    Returns:
        True if allowed, False otherwise
    """
    if not user.active:
        # Inactive users have no permissions
        return False
    elif user.role == "admin":
        # Admins can do anything
        return True
    elif user.role == "editor" and action in ["read", "write", "edit"]:
        return True
    elif user.role == "viewer" and action == "read":
        return True
    elif user.role == "guest":
        return False
    else:
        # This branch is impossible because:
        # - All roles are handled (admin, editor, viewer, guest)
        # - All other cases are caught by the previous conditions
        # But we keep it for safety (defensive programming)
        print(f"Warning: Unexpected role {user.role} for action {action}")
        return False


# ============================================
# DETECTING UNREACHABLE CODE
# ============================================

def demonstrate_unreachable_branches():
    """Demonstrate all the impossible branches."""
    
    print("=" * 70)
    print("🔍 LOGICALLY IMPOSSIBLE BRANCHES DEMONSTRATION")
    print("=" * 70)
    
    # Example 1: Mutually exclusive conditions
    print("\n1️⃣  Mutually Exclusive Conditions:")
    print(f"   check_number_impossible_branch(5): {check_number_impossible_branch(5)}")
    print(f"   check_number_impossible_branch(-3): {check_number_impossible_branch(-3)}")
    print(f"   check_number_impossible_branch(0): {check_number_impossible_branch(0)}")
    print(f"   Note: The else branch never executes")
    
    # Example 2: Boolean with all possibilities
    print("\n2️⃣  Boolean with All Possibilities:")
    print(f"   process_flag_impossible(True): {process_flag_impossible(True)}")
    print(f"   process_flag_impossible(False): {process_flag_impossible(False)}")
    print(f"   Note: Boolean only has two values, both handled")
    
    # Example 3: Function that always raises exception
    print("\n3️⃣  Function that Always Raises Exception:")
    try:
        function_with_impossible_branch(0)
    except ValueError as e:
        print(f"   Caught exception: {e}")
        print(f"   Note: The zero branch never returns normally")
    
    # Example 4: Condition that can never be true
    print("\n4️⃣  Condition That Can Never Be True:")
    print(f"   impossible_condition(5, 10): {impossible_condition(5, 10)}")
    print(f"   Note: None of the impossible conditions execute")
    
    # Example 5: Enum with all values handled
    print("\n5️⃣  Enum with All Values Handled:")
    print(f"   process_color_impossible(Color.RED): {process_color_impossible(Color.RED)}")
    print(f"   process_color_impossible(Color.GREEN): {process_color_impossible(Color.GREEN)}")
    print(f"   process_color_impossible(Color.BLUE): {process_color_impossible(Color.BLUE)}")
    print(f"   Note: Else branch never executes")
    
    # Example 6: Union type with all possibilities
    print("\n6️⃣  Union Type with All Possibilities:")
    print(f"   process_value_impossible(42): {process_value_impossible(42)}")
    print(f"   process_value_impossible('hello'): {process_value_impossible('hello')}")
    print(f"   Note: Else branch never executes")
    
    # Example 7: Real-world scenario
    print("\n7️⃣  Real-World Permission Check:")
    admin = User("admin", True)
    editor = User("editor", True)
    viewer = User("viewer", True)
    guest = User("guest", True)
    
    print(f"   Admin write: {check_user_permission_impossible(admin, 'write')}")
    print(f"   Editor write: {check_user_permission_impossible(editor, 'write')}")
    print(f"   Viewer write: {check_user_permission_impossible(viewer, 'write')}")
    print(f"   Guest read: {check_user_permission_impossible(guest, 'read')}")
    print(f"   Note: The else branch is for unexpected roles only")
    
    print("\n" + "=" * 70)
    print("📝 SUMMARY")
    print("=" * 70)
    print("""
    Common patterns that create unreachable branches:
    
    1. All possible values of a type are handled:
       - Boolean: True/False
       - Enum: All enum values
       - Union: All types in union
    
    2. Mutually exclusive conditions:
       - x > 0 and x < 0
       - x == 0 and x != 0
    
    3. Code after return/raise/infinite loop
    
    4. Function calls that never return
    
    5. Conditions that are mathematically impossible
    """)


# ============================================
# DEFENSIVE PROGRAMMING WITH UNREACHABLE BRANCHES
# ============================================

def safe_function_with_unreachable_guard(value: int) -> str:
    """
    Using unreachable branch for defensive programming.
    
    Args:
        value: Input value
    
    Returns:
        String result
    """
    result = ""
    
    if value > 0:
        result = "Positive"
    elif value < 0:
        result = "Negative"
    elif value == 0:
        result = "Zero"
    else:
        # This should never happen, but we handle it gracefully
        # This is defensive programming - catching unexpected cases
        result = "Unexpected value"
        # Log the error in production
        print(f"Warning: Unexpected value {value} encountered")
    
    return result


def demonstrate_defensive_programming():
    """Show defensive programming with unreachable guards."""
    
    print("\n" + "=" * 70)
    print("🛡️  DEFENSIVE PROGRAMMING EXAMPLE")
    print("=" * 70)
    
    # Normal cases
    print("\nNormal cases:")
    print(f"   safe_function_with_unreachable_guard(5): {safe_function_with_unreachable_guard(5)}")
    print(f"   safe_function_with_unreachable_guard(-3): {safe_function_with_unreachable_guard(-3)}")
    print(f"   safe_function_with_unreachable_guard(0): {safe_function_with_unreachable_guard(0)}")
    
    # This would normally be unreachable, but if somehow called with float...
    print("\nUnexpected case (defensive guard triggers):")
    print(f"   safe_function_with_unreachable_guard(3.14): {safe_function_with_unreachable_guard(3.14)}")


if __name__ == "__main__":
    demonstrate_unreachable_branches()
    demonstrate_defensive_programming()
    
    print("\n" + "=" * 70)
    print("⚠️  NOTE: Unreachable code detection")
    print("=" * 70)
    print("""
    Python can detect some unreachable code:
    - Code after return/raise/continue/break
    - Some static analysis tools can detect logical impossibilities
    
    However, many logically impossible branches are only detectable
    at runtime or through code review.
    
    Tools that can help:
    - pylint: can detect some unreachable code
    - mypy: type checking can detect impossible type conditions
    - coverage.py: can show which branches never execute
    """)