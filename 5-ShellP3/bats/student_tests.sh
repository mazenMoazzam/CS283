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



@test "Pipe command: ls | grep .c" {
    run ./dsh <<EOF
ls | grep .c
EOF
    [[ "$output" == *".c"* ]]

    [ "$status" -eq 0 ]
}



@test "Multiple pipes: ls | grep .c | sort" {
    run ./dsh <<EOF
ls | grep .c | sort
EOF

    [[ "$output" == *".c"* ]]

    [ "$status" -eq 0 ]
}



@test "Pipe command with word count: ls | wc -l" {
    run ./dsh <<EOF
ls | wc -l
EOF
    expected_output="9"
    expected_message="cmd loop returned 0"
    cleaned_output=$(echo "$output" | grep -o '^[0-9]\+')

    
    [ "$cleaned_output" -eq "$expected_output" ]
    [ "$status" -eq 0 ]
   
    echo "$output" | grep -q "$expected_message"
}




@test "Chained pipe command: echo test | grep test | wc -l" {
    run ./dsh <<EOF
echo test | grep test | wc -l
EOF

    expected_output="1
dsh3> dsh3> cmd loop returned 0" 

    [ "$output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}




@test "Pipe command with cat: echo 'hello' | cat" {
    run ./dsh <<EOF
echo "hello" | cat
EOF
    expected_output="hello"
    expected_message="dsh3> dsh3> cmd loop returned 0"
    cleaned_output=$(echo "$output" | sed -n '1p')  
    
    [ "$cleaned_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}




@test "Command test: echo start | grep start | wc -c | sort -n" {
    run ./dsh <<EOF
echo start | grep start | wc -c | sort -n
EOF

    expected_output="6"
    expected_message="dsh3> dsh3> cmd loop returned 0"
    cleaned_output=$(echo "$output" | grep -o '^[0-9]\+')

    [ "$cleaned_output" -eq "$expected_output" ]
    [ "$status" -eq 0 ]
   
}

