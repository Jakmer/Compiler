import os
import subprocess
import difflib

program_inputs = {
    "example1.imp": "13\n3\n",
    "example2.imp": "0\n1\n",
    "example3.imp": "1\n",
    "example4.imp": "20\n9\n",
    "example5.imp": "1234567890\n1234567890987654321\n987654321\n",
    "example6.imp": "20\n",
    "example7.imp": "0\n0\n0\n",
    "example8.imp": "",
    "example9.imp": "20\n9\n",
    "exampleA.imp": "",
    "program0.imp": "124",
    "program1.imp": "30\n25\n20\n15\n",
    "program2.imp": "",
    "program3.imp": "12",
}

unhandled_tests = [
    "example7.imp",
    "example8.imp",
    "exampleA.imp",
    "program2.imp",
]


def run_virtual_machine(test_file):
    try:
        test_name = os.path.splitext(os.path.basename(test_file))[0]
        vm_path = os.path.join(os.getcwd(), "maszyna-wirtualna")
        input_data = program_inputs.get(test_name, "10\n")

        result = subprocess.run(
            [vm_path, test_file],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            input=input_data,
            text=True,
            check=True,
        )
        # remove first 3 lines of output
        result.stdout = (
            "\n".join(result.stdout.split("\n")[3:]) if result.stdout else None
        )
        # remove last 2 lines of output
        result.stdout = (
            "\n".join(result.stdout.split("\n")[:-2]) if result.stdout else None
        )
        return result.stdout
    except subprocess.CalledProcessError as e:
        print(f"Error while running virtual machine for file {test_file}: {e}")
        return None


def read_expected_output(test_file):
    expected_file = test_file.replace(".imp.mr", ".expected")
    if os.path.exists(expected_file):
        with open(expected_file, "r") as f:
            return f.read()
    else:
        print(f"Expected output file not found: {expected_file}")
        return None


def compare_outputs(actual_output, expected_output):
    diff = list(
        difflib.unified_diff(
            expected_output.splitlines(),
            actual_output.splitlines(),
            lineterm="",
            fromfile="expected",
            tofile="actual",
        )
    )
    return diff


def run_tests(test_directory, expected_output_directory):
    test_files = [
        f
        for f in os.listdir(test_directory)
        if f.startswith("test") and f.endswith(".imp.mr")
    ]
    example_files = [
        f
        for f in os.listdir(test_directory)
        if f.startswith("example") and f.endswith(".imp.mr")
    ]
    program_files = [
        f
        for f in os.listdir(test_directory)
        if f.startswith("program") and f.endswith(".imp.mr")
    ]
    all_test = test_files + example_files + program_files
    test_results = []
    all_tests_passed = True

    for test_file in all_test:
        test_name = os.path.splitext(os.path.basename(test_file))[0]

        if test_name in unhandled_tests:
            print("Skipping test: " + test_name)
            continue
        test_path = os.path.join(test_directory, test_file)

        print(f"Running test: {test_path}")

        actual_output = run_virtual_machine(test_path)

        if actual_output is None:
            test_results.append((test_file, "Error during execution"))
            all_tests_passed = False
            continue

        expected_output_file = test_file.replace(".imp.mr", ".expected")
        expected_output_path = os.path.join(
            expected_output_directory, expected_output_file
        )
        expected_output = read_expected_output(expected_output_path)

        if expected_output is None:
            test_results.append((test_file, "Expected output file missing"))
            all_tests_passed = False
            continue

        diff = compare_outputs(actual_output, expected_output)

        if diff:
            test_results.append((test_file, "Test failed"))
            print("\n".join(diff))
            all_tests_passed = False
        else:
            test_results.append((test_file, "Test passed successfully!"))
            print(f"Test {test_file} passed successfully!")

    return -1 if not all_tests_passed else 0


def print_test_results(test_results):
    print("\nTest results:")
    for test_file, result in test_results:
        print(f"{test_file}: {result}")


if __name__ == "__main__":
    test_directory = "../test/output"
    expected_output_directory = "../test/expected_vm_output"
    exit_code = run_tests(test_directory, expected_output_directory)
    exit(exit_code)
