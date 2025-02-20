#!/bin/bash

### ensure that when _two_ non-existant input pathnames are given, each one results in an error output that mentions that pathname.

SCRIPT_NAME=$(basename "$0")
USAGE="Usage: $SCRIPT_NAME <path-to-driver-binary>"

show_usage() {
    echo "$USAGE"
}

if [ $# -eq 0 ]; then
    echo "Error: No path to driver binary provided."
    show_usage
    exit 1
fi


source $(dirname "`realpath "${BASH_SOURCE[0]}"`")/driver_inputs_test___shared_code.bash

check_for_inadvisable_sourcing; returned=$?
if [ $returned -ne 0 ]; then return $returned; fi ### simulating exception handling for an exception that is not caught at this level

driver_path="$1"
validate_driver_binary "$driver_path"


humanReadable_test_pathname="`resolve_symlink_only_of_basename "$0"`"


BAD_PATHNAME_BASE=/path/to/a/nonexistant/supposedly-P4/source/file
### Technically, these don`t need to be _unique_ pathnames in order to trigger the bad/confusing behavior of the driver that Abe
### saw before he started working on it, but unique pathnames will more helpful for debugging, in case that will ever be needed.
BAD_PATHNAME_1=$BAD_PATHNAME_BASE/1
BAD_PATHNAME_2=$BAD_PATHNAME_BASE/2

### Using ASCII double quotes to guard against bugs due to ASCII spaces, even though this test-script file is free of such bugs as of this writing.
check_for_pathname_error_in_P4_compiler_driver_output "$driver_path" "$BAD_PATHNAME_1" 1 2
result_1=$?
echo
check_for_pathname_error_in_P4_compiler_driver_output "$driver_path" "$BAD_PATHNAME_2" 2 2
result_2=$?
echo



num_failures=0
if [ $result_1 -ne 0 ]; then num_failures=1; fi
if [ $result_2 -ne 0 ]; then num_failures=$(($num_failures+1)); fi

report___num_failures___and_clamp_it_to_an_inclusive_maximum_of_255

exit $num_failures
