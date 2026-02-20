import re

def find_repeated_characters(text):
    """
    Finds all consecutive repeated characters in the input text.
    Returns a list of the repeated sequences.
    """
    # Regex explanation:
    # (.)   -> capture any single character
    # \1+   -> match the same character one or more times consecutively
    pattern = r"(.)\1+"

    # Find all matches
    matches = re.findall(pattern, text)
    return matches


# Example usage
if __name__ == "__main__":
    text = "aaabbcddddeeeffgghhiiijjj"
    repeated_chars = find_repeated_characters(text)
    print("Repeated characters found:", repeated_chars)