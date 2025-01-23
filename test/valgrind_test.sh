#!/bin/bash
# This script is used to start valgrind testing process. It will run the test cases

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <project_dir>"
    exit 1
fi

project_dir=$1
input_dir="$project_dir/test/input"
output_dir="$project_dir/test/valgrind_output"
log_dir="$project_dir/test/valgrind_log"
file_extension=".imp"
result_file_extension=".mr"
log_file_extension=".log"
valgrind_error=5

if [ ! -d "$output_dir" ]; then
    mkdir -p "$output_dir"
fi

if [ ! -d "$log_dir" ]; then
    mkdir -p "$log_dir"
fi

run_cmd="$project_dir/build/compiler"

# Find all the test files
test_files=$(find "$input_dir" -name "*$file_extension")

main() {
    echo "Starting Testing Process"
    echo "----------------------------------------"

    for test_file in $test_files
    do
        output_file="$output_dir/$(basename "$test_file")$result_file_extension"
        log_file="$log_dir/$(basename "$test_file")$log_file_extension"
        
        # Run Valgrind with the test file and output the logs
        echo "----------------------------------------"
        echo "Running Valgrind for $test_file"
        echo "----------------------------------------"
        valgrind --leak-check=full --track-origins=yes --log-file="$log_file" --error-exitcode=$valgrind_error "$run_cmd" "$test_file" "$output_file"
        
        valgrind_exit_code=$?
        
        # Check if Valgrind found any errors
        if [ $valgrind_exit_code -eq $valgrind_error ]; then
            echo "Valgrind detected errors for $test_file. Check the log file for details."
            exit 5
        fi
    done
}

main
