#!/usr/bin/env bats

############################# STUDENT TEST CASES #####################################
# File: student_tests.sh
#
# Additional test cases to cover more edge cases and functionalities.
#####################################################################################

@test "Change directory to non-existent folder" {
    run ./dsh <<EOF
cd /this/folder/does/not/exist
EOF

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"

    # The command should fail
    [ "$status" -ne 0 ]
}

@test "Echo multiple arguments" {
    run ./dsh <<EOF
echo hello world from dsh
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="helloworldfromdshdsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Run an invalid command" {
    run ./dsh <<EOF
notarealcommand
EOF

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"

    # The command should fail
    [ "$status" -ne 0 ]
}

@test "Check exit command" {
    run ./dsh <<EOF
exit
EOF

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$status" -eq 0 ]
}

@test "Check built-in command dragon" {
    run ./dsh <<EOF
dragon
EOF

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$status" -eq 0 ]
}

@test "Check command substitution" {
    run ./dsh <<EOF
echo \$(whoami)
EOF

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$status" -eq 0 ]
}

@test "Check running background process" {
    run ./dsh <<EOF
sleep 1 &
EOF

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$status" -eq 0 ]
}

