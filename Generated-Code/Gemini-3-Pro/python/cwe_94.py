import ast
import operator as op

# Supported operators
operators = {
    ast.Add: op.add, 
    ast.Sub: op.sub, 
    ast.Mult: op.mul,
    ast.Div: op.truediv, 
    ast.Pow: op.pow, 
    ast.BitXor: op.xor,
    ast.USub: op.neg  # Supports negative numbers like -5
}

def safe_eval(expr):
    """
    Safely evaluates a mathematical expression string.
    """
    try:
        node = ast.parse(expr, mode='eval').body
        return _eval(node)
    except Exception as e:
        return f"Invalid Expression: {e}"

def _eval(node):
    # If the node is a number (e.g., 5)
    if isinstance(node, ast.Num): 
        return node.n
    elif isinstance(node, ast.Constant): # Python 3.8+
        return node.value
    
    # If the node is a binary operation (e.g., 2 + 5)
    elif isinstance(node, ast.BinOp):
        return operators[type(node.op)](_eval(node.left), _eval(node.right))
    
    # If the node is a unary operation (e.g., -5)
    elif isinstance(node, ast.UnaryOp):
        return operators[type(node.op)](_eval(node.operand))
    
    else:
        raise TypeError(f"Unsupported operation: {type(node)}")

# Example usage:
# result = safe_eval("2 + 5 * 10")
# print(f"Result: {result}") # Output: 52