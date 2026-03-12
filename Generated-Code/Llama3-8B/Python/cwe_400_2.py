import re

text = "helloooo world"

pattern = r"((\w)\2+)"
matches = re.findall(pattern, text)

for match in matches:
    print(f"Found repeated character '{match[1]}' repeated {len(match[0])} times")
