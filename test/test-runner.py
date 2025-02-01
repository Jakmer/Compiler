import os
import subprocess
import difflib

def run_virtual_machine(test_file):
    try:
        vm_path = os.path.join(os.getcwd(), 'maszyna-wirtualna')
        result = subprocess.run(
            [vm_path, test_file],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            input='10\n',  # Simulate entering the number 10 followed by pressing Enter
            text=True,
            check=True
        )
        # remove first 3 lines of output
        result.stdout = '\n'.join(result.stdout.split('\n')[3:]) if result.stdout else None
        # remove last 2 lines of output
        result.stdout = '\n'.join(result.stdout.split('\n')[:-2]) if result.stdout else None
        return result.stdout
    except subprocess.CalledProcessError as e:
        print(f"Error while running virtual machine for file {test_file}: {e}")
        return None

def read_expected_output(test_file):
    expected_file = test_file.replace('.imp.mr', '.expected')
    if os.path.exists(expected_file):
        with open(expected_file, 'r') as f:
            return f.read()
    else:
        print(f"Expected output file not found: {expected_file}")
        return None

def compare_outputs(actual_output, expected_output):
    diff = list(difflib.unified_diff(
        expected_output.splitlines(), 
        actual_output.splitlines(), 
        lineterm='', 
        fromfile='expected', 
        tofile='actual'
    ))
    return diff

def run_tests(test_directory, expected_output_directory):
    test_files = [f for f in os.listdir(test_directory) if f.startswith('test') and f.endswith('.imp.mr')]
    test_results = []

    for test_file in test_files:
        test_path = os.path.join(test_directory, test_file)

        print(f"Running test: {test_path}")

        actual_output = run_virtual_machine(test_path)

        if actual_output is None:
            test_results.append((test_file, "Error during execution"))
            continue

        expected_output_file = test_file.replace('.imp.mr', '.expected')
        expected_output_path = os.path.join(expected_output_directory, expected_output_file)
        expected_output = read_expected_output(expected_output_path)

        if expected_output is None:
            test_results.append((test_file, "Expected output file missing"))
            continue

        diff = compare_outputs(actual_output, expected_output)

        if diff:
            test_results.append((test_file, "Test failed"))
            print("\n".join(diff))
        else:
            test_results.append((test_file, "Test passed successfully!"))
            print(f"Test {test_file} passed successfully!")

    return test_results

def print_test_results(test_results):
    print("\nTest results:")
    for test_file, result in test_results:
        print(f"{test_file}: {result}")

if __name__ == "__main__":
    test_directory = '../test/output'
    expected_output_directory = '../test/expected_vm_output'
    test_results = run_tests(test_directory, expected_output_directory)
    print_test_results(test_results)
