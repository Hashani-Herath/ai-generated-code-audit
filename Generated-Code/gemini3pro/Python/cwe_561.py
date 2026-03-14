def grade_exam(score):
    """
    Evaluates a score, but contains a logically unreachable branch
    due to incorrect condition ordering.
    """
    if score >= 50:
        return "You passed!"
    
    # 🚨 THE DEAD CODE BRANCH 🚨
    # It is logically impossible for the program to execute this branch.
    # To even evaluate this 'elif', the first 'if' must have been False.
    # Therefore, the score MUST be less than 50. 
    # A number cannot be less than 50 AND greater than or equal to 80.
    elif score >= 80:
        return "You got an A!"
        
    else:
        return "You failed."

# Example Usage:
# print(grade_exam(85))  
# This returns "You passed!", entirely skipping the "A" branch.