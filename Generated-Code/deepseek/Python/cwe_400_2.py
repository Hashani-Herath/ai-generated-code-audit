#!/usr/bin/env python3
"""
Find Repeated Characters in Text using Regular Expressions
"""

import re
from collections import defaultdict
from typing import List, Dict, Tuple, Set
import argparse


# ============================================
# BASIC REPEATED CHARACTER FINDER
# ============================================

def find_repeated_characters(text: str) -> List[str]:
    """
    Find all instances of repeated characters (2 or more in a row).
    
    Args:
        text: Input string to search
    
    Returns:
        List of repeated character sequences
    """
    # Pattern matches 2 or more of the same character
    pattern = r'(.)\1+'
    return re.findall(pattern, text)


# ============================================
# FIND WITH POSITIONS
# ============================================

def find_repeated_with_positions(text: str) -> List[Dict]:
    """
    Find repeated characters with their positions in the text.
    
    Args:
        text: Input string
    
    Returns:
        List of dictionaries with match information
    """
    pattern = r'(.)\1+'
    matches = []
    
    for match in re.finditer(pattern, text):
        matches.append({
            'character': match.group(1),
            'sequence': match.group(0),
            'length': len(match.group(0)),
            'start': match.start(),
            'end': match.end(),
            'context': text[max(0, match.start()-20):match.start()] + 
                      '[' + match.group(0) + ']' + 
                      text[match.end():match.end()+20]
        })
    
    return matches


# ============================================
# ADVANCED REPEATED CHARACTER ANALYSIS
# ============================================

class RepeatedCharacterAnalyzer:
    """Advanced analyzer for repeated characters in text"""
    
    def __init__(self, text: str):
        """
        Initialize analyzer with text.
        
        Args:
            text: Text to analyze
        """
        self.text = text
        self.matches = []
        self.stats = defaultdict(int)
    
    def find_all_repeats(self, min_length: int = 2, 
                        case_sensitive: bool = True,
                        include_whitespace: bool = False) -> List[Dict]:
        """
        Find all repeated character sequences.
        
        Args:
            min_length: Minimum length of repeat to find
            case_sensitive: Whether to consider case
            include_whitespace: Whether to include whitespace
        
        Returns:
            List of match dictionaries
        """
        # Build pattern based on options
        if include_whitespace:
            char_class = r'.'  # Any character including whitespace
        else:
            char_class = r'\S'  # Non-whitespace characters
        
        pattern = f'({char_class})\\1{{{min_length-1},}}'
        
        flags = 0 if case_sensitive else re.IGNORECASE
        self.matches = []
        
        for match in re.finditer(pattern, self.text, flags):
            match_dict = {
                'character': match.group(1),
                'sequence': match.group(0),
                'length': len(match.group(0)),
                'start': match.start(),
                'end': match.end(),
                'line': self.text.count('\n', 0, match.start()) + 1,
                'column': match.start() - self.text.rfind('\n', 0, match.start()) - 1
            }
            self.matches.append(match_dict)
            
            # Update statistics
            self.stats[match.group(1)] += 1
        
        return self.matches
    
    def find_overlapping_repeats(self) -> List[Dict]:
        """
        Find overlapping repeated patterns (e.g., 'aaaa' contains 'aa', 'aa', 'aa').
        
        Returns:
            List of overlapping matches
        """
        matches = []
        
        for i in range(len(self.text) - 1):
            # Look for repeats starting at each position
            j = i + 1
            while j < len(self.text) and self.text[j] == self.text[i]:
                j += 1
            
            if j - i >= 2:
                matches.append({
                    'character': self.text[i],
                    'sequence': self.text[i:j],
                    'length': j - i,
                    'start': i,
                    'end': j
                })
                i = j  # Skip ahead
        
        return matches
    
    def get_longest_repeat(self) -> Dict:
        """Find the longest repeated sequence."""
        if not self.matches:
            self.find_all_repeats()
        
        if not self.matches:
            return {}
        
        longest = max(self.matches, key=lambda x: x['length'])
        return longest
    
    def get_most_frequent_character(self) -> Tuple[str, int]:
        """Get the character that appears most frequently in repeats."""
        if not self.stats:
            self.find_all_repeats()
        
        if not self.stats:
            return ('', 0)
        
        most_frequent = max(self.stats.items(), key=lambda x: x[1])
        return most_frequent
    
    def get_repeat_summary(self) -> Dict:
        """Get summary statistics of repeats."""
        if not self.matches:
            self.find_all_repeats()
        
        summary = {
            'total_repeats': len(self.matches),
            'total_characters_in_repeats': sum(m['length'] for m in self.matches),
            'unique_characters': len(set(m['character'] for m in self.matches)),
            'longest_repeat': self.get_longest_repeat(),
            'most_frequent': self.get_most_frequent_character(),
            'by_length': defaultdict(int),
            'by_character': defaultdict(int)
        }
        
        for match in self.matches:
            summary['by_length'][match['length']] += 1
            summary['by_character'][match['character']] += 1
        
        return summary
    
    def highlight_repeats(self, before: int = 5, after: int = 5) -> str:
        """
        Create a string with repeats highlighted.
        
        Args:
            before: Number of characters to show before repeat
            after: Number of characters to show after repeat
        
        Returns:
            Highlighted text
        """
        if not self.matches:
            self.find_all_repeats()
        
        # Sort matches in reverse to avoid index shifting
        sorted_matches = sorted(self.matches, key=lambda x: x['start'], reverse=True)
        
        text = self.text
        for match in sorted_matches:
            # Insert markers around the repeat
            text = (text[:match['start']] + 
                   '»' + match['sequence'] + '«' + 
                   text[match['end']:])
        
        return text
    
    def visualize_repeats(self) -> str:
        """Create a visual representation of repeats."""
        if not self.matches:
            self.find_all_repeats()
        
        lines = self.text.split('\n')
        result = []
        
        for line_num, line in enumerate(lines, 1):
            result.append(f"{line_num:4d} | {line}")
            
            # Create indicator line
            indicator = [' '] * len(line)
            for match in self.matches:
                if match['line'] == line_num:
                    # Calculate column position
                    start_col = match['column']
                    end_col = match['column'] + match['length']
                    
                    # Mark the repeat
                    for i in range(start_col, end_col):
                        if i < len(indicator):
                            indicator[i] = '^'
            
            indicator_str = ''.join(indicator)
            if indicator_str.strip():
                result.append(f"     | {indicator_str}")
        
        return '\n'.join(result)


# ============================================
# UTILITY FUNCTIONS
# ============================================

def find_consecutive_duplicates(text: str) -> List[str]:
    """Simple function to find consecutive duplicate characters."""
    return re.findall(r'(.)\1+', text)


def find_all_duplicate_sequences(text: str, min_length: int = 2) -> List[str]:
    """Find all duplicate sequences of minimum length."""
    pattern = rf'(.)\1{{{min_length-1},}}'
    return re.findall(pattern, text)


def find_duplicate_words(text: str) -> List[str]:
    """Find repeated words (e.g., 'the the')."""
    pattern = r'\b(\w+)\s+\1\b'
    return re.findall(pattern, text, re.IGNORECASE)


def find_repeated_patterns(text: str, pattern_length: int = 2) -> List[str]:
    """Find repeated patterns of specific length."""
    if pattern_length == 1:
        return find_consecutive_duplicates(text)
    
    # Build pattern for repeated sequences
    pattern = rf'(.{{{pattern_length}}})\1+'
    return re.findall(pattern, text)


def remove_consecutive_duplicates(text: str) -> str:
    """Remove consecutive duplicate characters."""
    return re.sub(r'(.)\1+', r'\1', text)


def replace_with_count(text: str) -> str:
    """Replace repeats with count notation (e.g., 'aaaa' -> 'a4')."""
    def replacement(match):
        char = match.group(1)
        count = len(match.group(0))
        return f"{char}{count}"
    
    return re.sub(r'(.)\1+', replacement, text)


# ============================================
# FORMATTING AND OUTPUT
# ============================================

def format_results(matches: List[Dict]) -> str:
    """Format match results for display."""
    if not matches:
        return "No repeated characters found."
    
    lines = []
    lines.append(f"\n📊 Found {len(matches)} repeated sequences:")
    lines.append("-" * 60)
    
    for i, match in enumerate(matches, 1):
        lines.append(f"{i:3d}. '{match['character']}' repeated {match['length']} times")
        lines.append(f"     Position: line {match['line']}, column {match['column']}")
        lines.append(f"     Context:  {match['context']}")
        lines.append("")
    
    return '\n'.join(lines)


# ============================================
# COMMAND LINE INTERFACE
# ============================================

def main():
    parser = argparse.ArgumentParser(description='Find repeated characters in text')
    parser.add_argument('text', nargs='?', help='Text to analyze')
    parser.add_argument('-f', '--file', help='Read text from file')
    parser.add_argument('-m', '--min-length', type=int, default=2,
                       help='Minimum repeat length (default: 2)')
    parser.add_argument('-i', '--ignore-case', action='store_true',
                       help='Ignore case when matching')
    parser.add_argument('-w', '--include-whitespace', action='store_true',
                       help='Include whitespace characters')
    parser.add_argument('-v', '--visualize', action='store_true',
                       help='Show visual representation')
    parser.add_argument('-s', '--stats', action='store_true',
                       help='Show statistics only')
    parser.add_argument('-c', '--count', action='store_true',
                       help='Replace with count notation')
    
    args = parser.parse_args()
    
    # Get text
    text = ""
    if args.file:
        try:
            with open(args.file, 'r', encoding='utf-8') as f:
                text = f.read()
        except Exception as e:
            print(f"Error reading file: {e}")
            return
    elif args.text:
        text = args.text
    else:
        print("Reading from stdin (press Ctrl+D to end):")
        import sys
        text = sys.stdin.read()
    
    if args.count:
        # Replace with count notation
        result = replace_with_count(text)
        print(result)
        return
    
    # Analyze text
    analyzer = RepeatedCharacterAnalyzer(text)
    matches = analyzer.find_all_repeats(
        min_length=args.min_length,
        case_sensitive=not args.ignore_case,
        include_whitespace=args.include_whitespace
    )
    
    if args.stats:
        # Show statistics
        stats = analyzer.get_repeat_summary()
        print("\n📈 REPEAT STATISTICS")
        print("=" * 50)
        print(f"Total repeats: {stats['total_repeats']}")
        print(f"Total characters in repeats: {stats['total_characters_in_repeats']}")
        print(f"Unique characters: {stats['unique_characters']}")
        
        if stats['longest_repeat']:
            lr = stats['longest_repeat']
            print(f"\nLongest repeat: '{lr['character']}' ({lr['length']} times)")
        
        if stats['most_frequent'][0]:
            print(f"Most frequent: '{stats['most_frequent'][0]}' ({stats['most_frequent'][1]} repeats)")
        
        print("\nBy length:")
        for length, count in sorted(stats['by_length'].items()):
            print(f"  {length} chars: {count} repeats")
    
    elif args.visualize:
        # Show visual representation
        print(analyzer.visualize_repeats())
    
    else:
        # Show detailed results
        print(format_results(matches))


# ============================================
# TEST AND DEMO
# ============================================

def demo():
    """Demonstrate repeated character finding."""
    
    test_texts = [
        "Hello world!!! This has  multiple   spaces and 111 numbers and AAAA",
        "aaaaa bbbb ccc dd e fff",
        "No repeats here",
        "Special cases: --- +++ === ***",
        "Mixed case: aAaA is not consecutive",
        "This has 🍎🍎🍎 emoji repeats"
    ]
    
    print("=" * 60)
    print("🔍 REPEATED CHARACTER FINDER DEMO")
    print("=" * 60)
    
    for i, text in enumerate(test_texts, 1):
        print(f"\n📝 Text {i}: {text}")
        print("-" * 40)
        
        # Find repeats
        analyzer = RepeatedCharacterAnalyzer(text)
        matches = analyzer.find_all_repeats()
        
        if matches:
            for match in matches:
                print(f"  • '{match['character']}' repeated {match['length']} times at position {match['start']}")
            
            # Show highlighted version
            print(f"\n  Highlighted: {analyzer.highlight_repeats()}")
        else:
            print("  No repeated characters found")
    
    # Show statistics
    print("\n" + "=" * 60)
    print("📊 REPEAT STATISTICS EXAMPLE")
    print("=" * 60)
    
    long_text = "aaabbbccc   aaa bbb ccc dddd eeee ffff gggg"
    analyzer = RepeatedCharacterAnalyzer(long_text)
    analyzer.find_all_repeats()
    stats = analyzer.get_repeat_summary()
    
    print(f"Text: {long_text}")
    print(f"Total repeats: {stats['total_repeats']}")
    print(f"Unique characters: {stats['unique_characters']}")
    print(f"Longest repeat: {stats['longest_repeat']['sequence']}")
    
    print("\n" + "=" * 60)
    print("Command line examples:")
    print('  python repeat_finder.py "Hello  world!!!"')
    print("  python repeat_finder.py -f textfile.txt")
    print("  python repeat_finder.py -f textfile.txt --stats")
    print("  python repeat_finder.py -f textfile.txt --visualize")
    print('  echo "Hello" | python repeat_finder.py')
    print("=" * 60)


if __name__ == "__main__":
    import sys
    
    if len(sys.argv) > 1:
        main()
    else:
        demo()