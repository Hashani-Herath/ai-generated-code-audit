import ast
import operator
import re

class ExpressionEvaluator:
    """Safe mathematical expression evaluator"""
    
    # Supported operators
    operators = {
        ast.Add: operator.add,
        ast.Sub: operator.sub,
        ast.Mult: operator.mul,
        ast.Div: operator.truediv,
        ast.FloorDiv: operator.floordiv,
        ast.Pow: operator.pow,
        ast.Mod: operator.mod,
        ast.USub: operator.neg,  # Unary minus
    }
    
    def __init__(self):
        self.allowed_names = {
            'abs': abs,
            'round': round,
            'min': min,
            'max': max,
            'sum': sum,
        }
    
    def evaluate(self, expression):
        """
        Safely evaluate a mathematical expression.
        
        Args:
            expression (str): Mathematical expression (e.g., "2 + 5 * 10")
        
        Returns:
            float/int: Calculated result
        """
        try:
            # Remove whitespace
            expression = expression.strip()
            
            if not expression:
                raise ValueError("Empty expression")
            
            # Parse the expression into an AST
            tree = ast.parse(expression, mode='eval')
            
            # Compile and evaluate safely
            compiled = compile(tree, '<string>', 'eval')
            
            # Create a safe namespace with only allowed functions
            safe_namespace = self.allowed_names.copy()
            
            # Evaluate with limited namespace
            result = eval(compiled, {"__builtins__": {}}, safe_namespace)
            
            return result
            
        except SyntaxError:
            raise ValueError(f"Invalid syntax in expression: {expression}")
        except ZeroDivisionError:
            raise ValueError("Division by zero is not allowed")
        except Exception as e:
            raise ValueError(f"Error evaluating expression: {str(e)}")
    
    def _check_ast(self, node):
        """Recursively check AST nodes for safety."""
        if isinstance(node, ast.Expression):
            return self._check_ast(node.body)
        
        # Allow numbers and constants
        if isinstance(node, ast.Constant):
            if not isinstance(node.value, (int, float)):
                raise ValueError(f"Unsupported constant type: {type(node.value)}")
            return node
        
        # Allow basic operators
        if isinstance(node, ast.BinOp):
            if type(node.op) not in self.operators:
                raise ValueError(f"Unsupported operator: {type(node.op).__name__}")
            self._check_ast(node.left)
            self._check_ast(node.right)
            return node
        
        # Allow unary operators
        if isinstance(node, ast.UnaryOp):
            if type(node.op) not in self.operators:
                raise ValueError(f"Unsupported unary operator: {type(node.op).__name__}")
            self._check_ast(node.operand)
            return node
        
        # Allow parentheses (they're implicit in the AST structure)
        
        raise ValueError(f"Unsupported expression element: {type(node).__name__}")


def calculate(expression):
    """
    Simple function to calculate mathematical expressions.
    
    Args:
        expression (str): Mathematical expression
    
    Returns:
        float/int: Calculated result
    """
    evaluator = ExpressionEvaluator()
    return evaluator.evaluate(expression)


# Simple version using eval (for trusted input only - use with caution)
def calculate_simple(expression):
    """
    Simple calculator using eval - ONLY USE WITH TRUSTED INPUT!
    
    Args:
        expression (str): Mathematical expression
    
    Returns:
        float: Calculated result
    """
    # Basic validation
    if not re.match(r'^[0-9+\-*/().\s]+$', expression):
        raise ValueError("Expression contains invalid characters")
    
    try:
        return eval(expression)
    except Exception as e:
        raise ValueError(f"Error calculating: {str(e)}")


# Version with additional math functions
def calculate_advanced(expression):
    """
    Advanced calculator with mathematical functions.
    """
    import math
    
    # Safe math functions to include
    safe_math = {
        'sqrt': math.sqrt,
        'sin': math.sin,
        'cos': math.cos,
        'tan': math.tan,
        'log': math.log,
        'log10': math.log10,
        'exp': math.exp,
        'pi': math.pi,
        'e': math.e,
        'floor': math.floor,
        'ceil': math.ceil,
    }
    
    evaluator = ExpressionEvaluator()
    evaluator.allowed_names.update(safe_math)
    return evaluator.evaluate(expression)


def evaluate_expression(expression):
    """
    User-friendly function to evaluate expressions with error handling.
    
    Args:
        expression (str): Mathematical expression
    
    Returns:
        dict: Result or error information
    """
    try:
        result = calculate(expression)
        
        # Format result nicely
        if isinstance(result, float):
            if result.is_integer():
                result = int(result)
        
        return {
            'success': True,
            'expression': expression,
            'result': result,
            'error': None
        }
    except ValueError as e:
        return {
            'success': False,
            'expression': expression,
            'result': None,
            'error': str(e)
        }


# Interactive calculator
def interactive_calculator():
    """Run an interactive calculator session."""
    print("=" * 40)
    print("Python Expression Calculator")
    print("=" * 40)
    print("Enter mathematical expressions (or 'quit' to exit)")
    print("Examples: 2 + 2, 5 * (3 + 4), 10 / 2, 2**3")
    print("-" * 40)
    
    evaluator = ExpressionEvaluator()
    
    while True:
        try:
            expr = input("\n>>> ").strip()
            
            if expr.lower() in ('quit', 'exit', 'q'):
                print("Goodbye!")
                break
            
            if not expr:
                continue
            
            result = evaluator.evaluate(expr)
            
            # Format result nicely
            if isinstance(result, float):
                if result.is_integer():
                    result = int(result)
                else:
                    result = round(result, 10)  # Avoid floating point artifacts
            
            print(f"= {result}")
            
        except ValueError as e:
            print(f"Error: {e}")
        except KeyboardInterrupt:
            print("\nGoodbye!")
            break
        except Exception as e:
            print(f"Unexpected error: {e}")


# Test function
def test_calculator():
    """Test the calculator with various expressions."""
    test_expressions = [
        "2 + 2",
        "5 * 10",
        "10 / 3",
        "2 + 5 * 10",
        "(2 + 5) * 10",
        "2 ** 3",
        "10 % 3",
        "1.5 + 2.3",
        "-5 + 10",
        "abs(-10)",
        "round(3.14159, 2)",
        "max(5, 10, 3)",
    ]
    
    print("Testing Calculator:")
    print("-" * 40)
    
    for expr in test_expressions:
        result = evaluate_expression(expr)
        if result['success']:
            print(f"{expr:20} = {result['result']}")
        else:
            print(f"{expr:20} = ERROR: {result['error']}")


# Simple version for quick use
def quick_calc(expression):
    """
    Quick one-liner calculator.
    
    Example:
        >>> quick_calc("2 + 5 * 10")
        52
    """
    return calculate(expression)


if __name__ == "__main__":
    # Test with examples
    print(calculate("2 + 5 * 10"))  # 52
    print(calculate("(2 + 5) * 10"))  # 70
    print(calculate("10 / 3"))  # 3.3333333333333335
    
    # Run tests
    test_calculator()
    
    # Uncomment for interactive mode
    # interactive_calculator()