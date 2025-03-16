#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file










@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}


@test "Client exits gracefully with 'exit' command" {
    # Start the server in the background
    ./dsh -s -i 127.0.0.1 -p 12345 &
    server_pid=$!
    sleep 1

    # Run the client and send the 'exit' command
    run ./dsh -c -i 127.0.0.1 -p 12345 <<EOF
exit
EOF

    # Assertions
    [ "$status" -eq 0 ]

    # Clean up
    kill $server_pid
}


@test "Server changes directory with 'cd' command" {
    # Start the server in the background
    ./dsh -s -i 127.0.0.1 -p 12345 &
    server_pid=$!
    sleep 1

    # Run the client and send the 'cd' command
    run ./dsh -c -i 127.0.0.1 -p 12345 <<EOF
cd /tmp
pwd
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [[ "$output" == *"/tmp"* ]]

    # Clean up
    kill $server_pid
}


@test "Server stops with 'stop-server' command" {
    # Start the server in the background
    ./dsh -s -i 127.0.0.1 -p 12345 &
    server_pid=$!
    sleep 1

    # Run the client and send the 'stop-server' command
    run ./dsh -c -i 127.0.0.1 -p 12345 <<EOF
stop-server
EOF

    # Assertions
    [ "$status" -eq 0 ]

    # Check if the server process is no longer running
    run ps -p $server_pid
    [ "$status" -ne 0 ]
}


@test "Server executes pipeline commands" {
    # Start the server in the background
    ./dsh -s -i 127.0.0.1 -p 12345 &
    server_pid=$!
    sleep 1

    # Run the client and send a pipeline command
    run ./dsh -c -i 127.0.0.1 -p 12345 <<EOF
ls | grep "dsh"
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [[ "$output" == *"dsh"* ]]

    # Clean up
    kill $server_pid
}



@test "Server starts and accepts client connections" {
    # Start the server in the background
    ./dsh -s -i 127.0.0.1 -p 12345 &
    server_pid=$!
    sleep 1 # Give the server time to start

    # Run the client and send a simple command
    run ./dsh -c -i 127.0.0.1 -p 12345 <<EOF
echo "Hello, World!"
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [[ "$output" == *"Hello, World!"* ]]

    # Clean up
    kill $server_pid
}


@test "Server executes simple command (ls)" {
    # Start the server in the background
    ./dsh -s -i 127.0.0.1 -p 12345 &
    server_pid=$!
    sleep 1

    # Run the client and send the 'ls' command
    run ./dsh -c -i 127.0.0.1 -p 12345 <<EOF
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [[ "$output" == *"dsh"* ]] # Ensure the output contains expected files

    # Clean up
    kill $server_pid
}



@test "Server executes command with arguments (ls -l)" {
    # Start the server in the background
    ./dsh -s -i 127.0.0.1 -p 12345 &
    server_pid=$!
    sleep 1

    # Run the client and send the 'ls -l' command
    run ./dsh -c -i 127.0.0.1 -p 12345 <<EOF
ls -l
EOF

    # Assertions
    [ "$status" -eq 0 ]
    [[ "$output" == *"rw"* ]] # Ensure the output contains permission details

    # Clean up
    kill $server_pid
}
