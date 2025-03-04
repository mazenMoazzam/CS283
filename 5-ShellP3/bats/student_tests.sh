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
    expectedOutput=$(ls | wc -l)  

    run ./dsh <<EOF
ls | wc -l
EOF

    expected_message="cmd loop returned 0"
    cleanedOutput=$(echo "$output" | grep -o '^[0-9]\+')

    [ "$cleanedOutput" -eq "$expectedOutput" ]
    [ "$status" -eq 0 ]
}


@test "Chained pipe command: echo test | grep test | wc -l" {
    run ./dsh <<EOF
echo test | grep test | wc -l
EOF

    expectedOutput="1
dsh3> dsh3> cmd loop returned 0" 

    [ "$output" = "$expectedOutput" ]
    [ "$status" -eq 0 ]
}


@test "Pipe command with cat: echo 'hello' | cat" {
    run ./dsh <<EOF
echo "hello" | cat
EOF
    expectedOutput="hello"
    cleanedOutput=$(echo "$output" | sed -n '1p')  
    
    [ "$cleanedOutput" = "$expectedOutput" ]
    [ "$status" -eq 0 ]
}



@test "Command test: echo start | grep start | wc -c | sort -n" {
    run ./dsh <<EOF
echo start | grep start | wc -c | sort -n
EOF

    expectedOutput="6"
    cleanedOutput=$(echo "$output" | grep -o '^[0-9]\+')

    [ "$cleanedOutput" -eq "$expectedOutput" ]
    [ "$status" -eq 0 ]
   
}


@test "Piping with multiple commands test" {
    run ./dsh <<EOF
echo hello | tr a-z A-Z | rev
EOF

    expectedOutput="OLLEH"
    cleanedOutput=$(echo "$output" | sed -n '1p')

    [ "$cleanedOutput" = "$expectedOutput" ]
    [ "$status" -eq 0 ]
}



@test "Counting words with pipes" {
    run ./dsh <<EOF
echo hello world | wc -w
EOF

    expectedOutput="2"
    cleanedOutput=$(echo "$output" | sed -n '1p')

    [ "$cleanedOutput" = "$expectedOutput" ]
    [ "$status" -eq 0 ]
}


@test "Sorting a list with pipes" {
    run ./dsh <<EOF
echo -e "orange\nbanana\napple" | sort
EOF

    expectedOutput="apple
banana
orange"
    cleanedOutput=$(echo "$output" | sed -n '1,3p')

    [ "$cleanedOutput" = "$expectedOutput" ]
    [ "$status" -eq 0 ]
}


@test "Test output redirection" {
    run ./dsh <<EOF
echo "first line" > out.txt
cat out.txt
EOF
    expectedOutput="first line"    
    cleanedOutput=$(echo "$output" | sed -n '1p')
    [ "$cleanedOutput" = "$expectedOutput" ]
    [ "$status" -eq 0 ]
}


@test "Test output redirection: Overwrite existing file" {
    run ./dsh <<EOF
echo "first line" > testfile.txt
echo "second line" > testfile.txt
cat testfile.txt
EOF
    expectedOutput="second line"  
    cleanedOutput=$(echo "$output" | sed -n '1p')
    [ "$cleanedOutput" = "$expectedOutput" ]
    [ "$status" -eq 0 ]
}


@test "Test output redirection: Append to file" {
    run ./dsh <<EOF
echo "first line" > testfile.txt
echo "second line" >> testfile.txt
cat testfile.txt
EOF
    expectedOutput="first line
second line"
    cleanedOutput=$(echo "$output" | sed -n '1,2p')

    [ "$cleanedOutput" = "$expectedOutput" ]
    [ "$status" -eq 0 ]
}


@test "Test output redirection: Append empty line" {
    run ./dsh <<EOF
echo "line one" > testfile.txt
echo "" >> testfile.txt
cat testfile.txt
EOF
    expectedOutput="line one"  
    cleanedOutput=$(echo "$output" | sed -n '1p')

    [ "$cleanedOutput" = "$expectedOutput" ]
    [ "$status" -eq 0 ]
}


@test "Test input and output redirection together" {
    run ./dsh <<EOF
echo "data" > file1.txt
cat < file1.txt > file2.txt
cat file2.txt
EOF
    expectedOutput="data"
    cleanedOutput=$(echo "$output" | sed -n '1p')

    [ "$cleanedOutput" = "$expectedOutput" ]
    [ "$status" -eq 0 ]
}


@test "Test output redirection: Empty string to file" {
    run ./dsh <<EOF
echo -n "" > emptyfile.txt
cat emptyfile.txt
EOF
    cleanedOutput=$(echo "$output" | grep -v "dsh3>" | grep -v "cmd loop returned" | tr -d '[:space:]')

    [ -z "$cleanedOutput" ]
    [ "$status" -eq 0 ]
}



@test "Multiple redirections" {
    run ./dsh <<EOF
echo "line one" > file.txt
echo "line two" >> file.txt
echo "line three" >> file.txt
cat file.txt
EOF

    expectedOutput="line one
line two
line three"
    cleanedOutput=$(echo "$output" | sed -n '1,3p')

    [ "$cleanedOutput" = "$expectedOutput" ]
    [ "$status" -eq 0 ]
}



@test "Exit command" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}


@test "Command substitution inside pipe" {
    run ./dsh <<EOF
echo "The date is $(date)" | grep "date"
EOF

    expectedOutput="The date is"
    cleanedOutput=$(echo "$output" | sed -n '1p')

    [[ "$cleanedOutput" == *"$expectedOutput"* ]]
    [ "$status" -eq 0 ]
}

