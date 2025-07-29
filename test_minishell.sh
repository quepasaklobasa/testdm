#!/bin/bash

# test_minishell.sh - Comprehensive test script for minishell
# Usage: Create this file, make it executable, then run it inside your minishell

echo "=== MINISHELL COMPREHENSIVE TEST SUITE ==="
echo

# Test 1: Basic Commands
echo "Test 1: Basic Commands"
echo "----------------------"
echo "Testing echo..."
echo "Hello, World!"
echo "Testing pwd..."
pwd
echo "Testing env (first 5 lines)..."
env | head -5
echo

# Test 2: Built-in Commands
echo "Test 2: Built-in Commands"
echo "-------------------------"
echo "Testing cd..."
pwd
cd /tmp
pwd
cd ~
pwd
echo

echo "Testing export and unset..."
export TEST_VAR="Hello from minishell"
echo "TEST_VAR is: $TEST_VAR"
env | grep TEST_VAR
unset TEST_VAR
echo "After unset, TEST_VAR is: $TEST_VAR"
echo

# Test 3: Variable Expansion
echo "Test 3: Variable Expansion"
echo "--------------------------"
echo "Exit status test..."
/bin/false
echo "Exit status after false: $?"
/bin/true
echo "Exit status after true: $?"
echo "HOME variable: $HOME"
echo "USER variable: $USER"
echo

# Test 4: Redirections
echo "Test 4: Redirections"
echo "--------------------"
echo "Testing output redirection..."
echo "This goes to file" > test_output.txt
echo "File contents:"
cat test_output.txt
echo

echo "Testing input redirection..."
wc -l < test_output.txt
echo

echo "Testing append redirection..."
echo "This is appended" >> test_output.txt
echo "File contents after append:"
cat test_output.txt
echo

# Test 5: Pipes
echo "Test 5: Pipes"
echo "-------------"
echo "Testing simple pipe..."
ls -la | head -3
echo

echo "Testing multiple pipes..."
echo -e "apple\nbanana\ncherry\napple\nbanana" | sort | uniq -c
echo

# Test 6: Heredoc
echo "Test 6: Heredoc"
echo "---------------"
echo "Testing heredoc with cat..."
cat << EOF
This is a heredoc test.
Multiple lines are supported.
This should work in minishell.
EOF
echo

# Test 7: Complex Command Combinations
echo "Test 7: Complex Commands"
echo "------------------------"
echo "Testing command with multiple redirections..."
echo "Line 1" > complex_test.txt
echo "Line 2" >> complex_test.txt
sort < complex_test.txt > sorted_test.txt
echo "Sorted output:"
cat sorted_test.txt
echo

# Test 8: Error Handling
echo "Test 8: Error Handling"
echo "----------------------"
echo "Testing non-existent command..."
nonexistent_command 2>/dev/null || echo "Command not found (expected)"
echo

echo "Testing file not found..."
cat nonexistent_file.txt 2>/dev/null || echo "File not found (expected)"
echo

# Test 9: Signal Handling
echo "Test 9: Signal Handling"
echo "-----------------------"
echo "Test Ctrl+C handling by running: sleep 5"
echo "Then press Ctrl+C to interrupt"
echo "(This test requires manual interaction)"
echo

# Test 10: Builtin edge cases
echo "Test 10: Builtin Edge Cases"
echo "---------------------------"
echo "Testing echo with -n flag..."
echo -n "No newline"
echo " - continuing on same line"
echo

echo "Testing cd without arguments..."
cd
pwd
echo

echo "Testing export without arguments..."
export | head -3
echo

# Cleanup
echo "Cleaning up test files..."
rm -f test_output.txt complex_test.txt sorted_test.txt

echo "=== TEST SUITE COMPLETE ==="
echo "If all tests ran without crashes, your minishell is working well!"
