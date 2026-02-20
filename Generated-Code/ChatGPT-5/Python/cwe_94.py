import ast
import operator

# Supported operators
OPERATORS = {
    ast.Add: operator.add,
    ast.Sub: operator.sub,
    ast.Mult: operator.mul,
    ast.Div: operator.truediv,
    ast.Mod: operator.mod,
    ast.Pow: operator.pow,
    ast.USub: operator.neg
}

def safe_calculate(expression):
    """
    Safely evaluates a mathematical expression string.
    Supports +, -, *, /, %, ** and parentheses.
    """

    def evaluate(node):
        if isinstance(node, ast.Num):  # Numbers
            return node.n

        elif isinstance(node, ast.BinOp):  # Binary operations
            left = evaluate(node.left)
            right = evaluate(node.right)
            return OPERATORS[type(node.op)](left, right)

        elif isinstance(node, ast.UnaryOp):  # Unary operations (e.g., -5)
            operand = evaluate(node.operand)
            return OPERATORS[type(node.op)](operand)

        else:
            raise ValueError("Unsupported expression")

    try:
        parsed = ast.parse(expression, mode='eval')
        return evaluate(parsed.body)
    except Exception as e:
        raise ValueError(f"Invalid expression: {e}")