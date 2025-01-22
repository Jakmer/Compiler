#!/bin/bash
# This script is used to start the testing process. It will run the test cases

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <project_dir>"
    exit 1
fi

project_dir=$1
input_dir="$project_dir/test/input"
output_dir="$project_dir/test/output"
log_dir="$project_dir/test/log"
file_extension=".imp"
result_file_extension=".mr"
log_file_extension=".log"

if [ ! -d "$output_dir" ]; then
    mkdir -p "$output_dir"
fi

if [ ! -d "$log_dir" ]; then
    mkdir -p "$log_dir"
fi

run_cmd="$project_dir/build/compiler"

# Find all the test files
test_files=$(find "$input_dir" -name "*$file_extension")

# List of all the test files
echo "----------------------------------------"
echo "Test Files:"
echo "----------------------------------------"
for test_file in $input_dir
do
    echo "$test_file"
done


total_tests=0
passed_tests=0
failed_tests=0

run_test() {
    local test_file="$1"
    local output_file="$2"

    echo "Running test case: $run_cmd $test_file $output_file"
    $run_cmd "$test_file" "$output_file" &> "$log_dir/$(basename "$test_file")$log_file_extension"
    
    local exit_code=$?
    total_tests=$((total_tests + 1))

    if [ $exit_code -eq 0 ]; then
        if echo "$test_file" | grep -q "error" ; then
            echo "Test FAILED: $test_file with exit code $exit_code"
            failed_tests=$((failed_tests + 1))
            exit 1
        else
            echo "Test PASSED: $test_file"
            passed_tests=$((passed_tests + 1))
        fi
    else
        if echo "$test_file" | grep -q "error" ; then
            echo "Test PASSED: $test_file"
            passed_tests=$((passed_tests + 1))
        else
            echo "Test FAILED: $test_file with exit code $exit_code"
            failed_tests=$((failed_tests + 1))
            exit 1
        fi
    fi

    echo "----------------------------------------"
}

main() {
    echo "Starting Testing Process"
    echo "----------------------------------------"

    for test_file in $test_files
    do
        output_file="$output_dir/$(basename "$test_file")$result_file_extension"
        run_test "$test_file" "$output_file" 
    done


    echo "Testing Summary"
    echo "----------------------------------------"
    echo "Total Tests: $total_tests"
    echo "Passed:      $passed_tests"
    echo "Failed:      $failed_tests"

    if [ $failed_tests -gt 0 ]; then
        echo "Some tests FAILED."
        exit 1
    else
        echo "All tests PASSED."
        exit 0
    fi
}

main
