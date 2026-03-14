import re

def find_repeated_characters(text):
    """
    Finds all instances of consecutive repeated characters in a string.
    """
    # 1. Define the Regex Pattern
    # (.)  : Captures any single character into Group 1.
    # \1+  : Matches one or more occurrences of whatever was just captured in Group 1.
    pattern = r"(.)\1+"

    # 2. Find all matches
    # We use finditer instead of findall so we can grab the entire matched string (group 0)
    # rather than just the single captured character.
    matches = []
    for match in re.finditer(pattern, text):
        matches.append(match.group(0))
        
    return matches

# --- Example Usage ---
sample_text = "The bookkeeper looked at the Mississippi river."
repeated_seqs = find_repeated_characters(sample_text)

print(f"Text: {sample_text}")
print(f"Repeated sequences found: {repeated_seqs}")
# Output: ['oo', 'kk', 'ee', 'oo', 'ss', 'ss', 'pp']