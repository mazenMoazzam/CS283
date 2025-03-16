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
    ./dsh -s -i 127.0.0.1 -p 12345 &
    server_pid=$!
    sleep 1
    run ./dsh -c -i 127.0.0.1 -p 12345 <<EOF
exit
EOF

    
    [ "$status" -eq 0 ]

 
    kill $server_pid
}


@test "Server changes directory with 'cd' command" {
    ./dsh -s -i 127.0.0.1 -p 12345 &
    server_pid=$!
    sleep 1
    run ./dsh -c -i 127.0.0.1 -p 12345 <<EOF
cd /tmp
pwd
EOF

    
    [ "$status" -eq 0 ]
    [[ "$output" == *"/tmp"* ]]

    
    kill $server_pid
}


@test "Server stops with 'stop-server' command" {

    ./dsh -s -i 127.0.0.1 -p 12345 &
    server_pid=$!
    sleep 1

    run ./dsh -c -i 127.0.0.1 -p 12345 <<EOF
stop-server
EOF

    [ "$status" -eq 0 ]

    
    run ps -p $server_pid
    [ "$status" -ne 0 ]
}


@test "Server executes pipeline commands" {
    ./dsh -s -i 127.0.0.1 -p 12345 &
    server_pid=$!
    sleep 1

    run ./dsh -c -i 127.0.0.1 -p 12345 <<EOF
ls | grep "dsh"
EOF

    [ "$status" -eq 0 ]
    [[ "$output" == *"dsh"* ]]
    kill $server_pid
}



@test "Server starts and accepts client connections" {
    ./dsh -s -i 127.0.0.1 -p 12345 &
    server_pid=$!
    sleep 1 
    run ./dsh -c -i 127.0.0.1 -p 12345 <<EOF
echo "Hello, World!"
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"Hello, World!"* ]]
    kill $server_pid
}


@test "Server executes simple command (ls)" {
    ./dsh -s -i 127.0.0.1 -p 12345 &
    server_pid=$!
    sleep 1
    run ./dsh -c -i 127.0.0.1 -p 12345 <<EOF
ls
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"dsh"* ]] 
    kill $server_pid
}



@test "Server executes command with arguments (ls -l)" {
    ./dsh -s -i 127.0.0.1 -p 12345 &
    server_pid=$!
    sleep 1
    run ./dsh -c -i 127.0.0.1 -p 12345 <<EOF
ls -l
EOF

    [ "$status" -eq 0 ]
    [[ "$output" == *"rw"* ]] 
    kill $server_pid
}
