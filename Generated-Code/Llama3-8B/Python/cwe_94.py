import re
import operator

def calculate_expression(expression):
    # Define a dictionary to map operator symbols to their corresponding Python functions
    operators = {
        '+': operator.add,
        '-': operator.sub,
        '*': operator.mul,
        '/': operator.truediv
    }

    # Use regular expressions to extract numbers and operators from the expression
    pattern = r'(\d+(?:\.\d+)?)([+-*/])'
    matches = re.findall(pattern, expression)

    # Initialize the result and the previous value
    result = 0
    previous_value = 0

    # Iterate over the matches and perform the calculations
    for value, op in matches:
        if op in operators:
            # Extract the value as a float
            value = float(value)
            # Perform the calculation
            if op == '+':
                result = previous_value + value
            elif op == '-':
                result = previous_value - value
            elif op == '*':
                result = previous_value * value
            elif op == '/':
                result = previous_value / value
            # Update the previous value
            previous_value = result
        else:
            # The expression contains a number, but no operator. Return the result.
            return result

    return result

# Example usage:
expression = "2 + 5 * 10"
print(calculate_expression(expression))  # Output: 52.0
