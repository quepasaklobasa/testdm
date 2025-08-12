#!/bin/bash

# test_minishell.sh - Simplified test script for minishell
# Usage: Save as test_minishell.sh, chmod +x test_minishell.sh, run in minishell with ./test_minishell.sh

echo "=== MINISHELL SIMPLIFIED TEST SUITE ==="
echo

# Test 1: Basic Built-ins
echo "Test 1: Basic Built-ins"
echo "----------------------"
echo "Testing echo..."
echo "Hello, Minishell!"
echo "Testing pwd..."
pwd
echo "Testing env (check for PATH, HOME, USER)..."
env | grep -E 'PATH|HOME|USER'
echo

# Test 2: Built-in cd
echo "Test 2: cd Command"
echo "-----------------"
echo "Current directory:"
pwd
echo "Changing to /tmp..."
cd /tmp
pwd
echo "Returning to home directory..."
cd
pwd
echo

# Test 3: Export and Unset
echo "Test 3: Export and Unset"
echo "-----------------------"
echo "Setting TEST_VAR..."
export TEST_VAR="Hello from minishell"
echo "TEST_VAR is: $TEST_VAR"
env | grep TEST_VAR
echo "Unsetting TEST_VAR..."
unset TEST_VAR
echo "After unset, env should not show TEST_VAR:"
env | grep TEST_VAR || echo "TEST_VAR not found (expected)"
echo

# Test 4: Variable Expansion and Exit Status
echo "Test 4: Variable Expansion and Exit Status"
echo "-----------------------------------------"
echo "Testing exit status..."
/bin/false
echo "Exit status after false: $?"
/bin/true
echo "Exit status after true: $?"
echo "HOME variable: $HOME"
echo

# Test 5: Redirections
echo "Test 5: Redirections"
echo "-------------------"
echo "Testing output redirection..."
echo "This goes to file" > test_output.txt
echo "File contents:"
cat test_output.txt
echo "Testing append redirection..."
echo "This is appended" >> test_output.txt
echo "File contents after append:"
cat test_output.txt
echo "Testing input redirection..."
echo "Count lines in test_output.txt:"
cat < test_output.txt | wc -l
echo

# Test 6: Pipes
echo "Test 6: Pipes"
echo "------------"
echo "Testing simple pipe..."
ls | grep test_output.txt
echo

# Test 7: Heredoc
echo "Test 7: Heredoc"
echo "--------------"
echo "Testing heredoc with cat..."
cat << EOF
This is a heredoc test.
It should work in minishell.
EOF
echo

# Test 8: Error Handling
echo "Test 8: Error Handling"
echo "---------------------"
echo "Testing non-existent command..."
nonexistent_command 2>/dev/null || echo "Command not found (expected)"
echo "Testing file not found..."
cat nonexistent_file.txt 2>/dev/null || echo "File not found (expected)"
echo

# Cleanup
echo "Cleaning up test files..."
rm test_output.txt

echo "=== TEST SUITE COMPLETE ==="
echo "Check output for expected behavior."
