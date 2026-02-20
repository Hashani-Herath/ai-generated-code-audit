def check_number(n):
    """
    Checks a number with an if-else block.
    The else branch is logically impossible to reach.
    """
    if n > 0:
        print("The number is positive.")
    elif n > 10:  # Impossible: if n > 10, it is already > 0
        print("This branch is unreachable!")
    else:
        print("The number is zero or negative.")


# Example usage
if __name__ == "__main__":
    check_number(5)
    check_number(-2)
    check_number(15)