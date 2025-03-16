#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file



start_server() {
    ./dsh -s -p 4000 &
    server_pid=$!
    sleep 1 # Give the server time to start
}

# Helper function to stop the server
stop_server() {
    kill -9 $server_pid
}











@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}


@test "Test 7: Client-Server Pipeline with Multiple Commands (ls | grep dsh | wc -l)" {
    start_server

    run ./dsh -c -p 5678 <<EOF
ls | grep dsh | wc -l
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [[ "$output" =~ ^[0-9]+$ ]] # Ensure the output is a number (count of files)

    stop_server
}
