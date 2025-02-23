#!/usr/bin/env bats

# File: student_tests.sh

@test "cd to home directory" {
    run "./dsh" <<EOF
cd
pwd
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"$HOME"* ]]
}

@test "cd to a non-existent directory fails" {
    run "./dsh" <<EOF
cd /thisdoesnotexist
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"No such file or directory"* ]]
}

@test "mkdir creates a directory" {
    run "./dsh" <<EOF
mkdir testdir
ls
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"testdir"* ]]
    rm -r testdir
}

@test "rm removes a file" {
    touch tempfile.txt
    run "./dsh" <<EOF
rm tempfile.txt
ls
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" != *"tempfile.txt"* ]]
}

@test "Multiple spaces between commands should be ignored" {
    run "./dsh" <<EOF
echo      "spaced out"
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"spaced out"* ]]
}

@test "String with spaces" {
    run "./dsh" <<EOF
echo This is a test
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"This is a test"* ]]
}


@test "String with single quotes" {
    run "./dsh" <<EOF
echo 'Hello World'
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"Hello World"* ]]
}




@test "change to a directory and verify test" {
    run "./dsh" <<EOF
cd bats
pwd
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"/home/mm5622/CS283/CS283/4-ShellP2/bats"* ]]
    cd ..
}


@test "running a nonexistent command should fail" {
    run "./dsh" <<EOF
nonexistentcommand
exit
EOF
    [ "$status" -ne 1 ]
    [[ "$output" == *"execvp failed: No such file or directory"* ]]
}


@test "dragon command runs" {
    run "./dsh" <<EOF
dragon
exit
EOF
    [ "$status" -eq 0 ]
}
