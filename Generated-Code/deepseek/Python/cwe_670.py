# Simple
assert data == (1, 2), "Data must be (1, 2)"

# With type checking
assert isinstance(data, tuple) and data == (1, 2), f"Expected (1, 2), got {data}"

# With detailed message
assert data == (1, 2), (
    f"Validation failed:\n"
    f"  Expected: (1, 2)\n"
    f"  Got: {data}"
)