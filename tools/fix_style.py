#!/usr/bin/env python3

"""
This script runs clang-format over all files ending in .h, .c, .cpp, .proto
listed by git in the given directory.
"""

import argparse
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path

# Try to import tqdm for progress bar support
try:
    from tqdm import tqdm
    HAS_TQDM = True
except ImportError:
    HAS_TQDM = False

VERSION_REQUIRED_MAJOR = 19


def print_installation_instructions():
    """Print instructions for installing the required clang-format version."""
    print("On Ubuntu 24.04, you can install it:")
    print()
    print(f"    'sudo apt install clang-format-{VERSION_REQUIRED_MAJOR}'")
    print()
    print("Otherwise, you can use clang-format from docker:")
    print()
    print("    'tools/run-docker.sh tools/fix_style.py .'")


def find_clang_format():
    """Find the clang-format executable."""
    # Try to find the latest version of clang-format
    if shutil.which(f'clang-format-{VERSION_REQUIRED_MAJOR}'):
        return f'clang-format-{VERSION_REQUIRED_MAJOR}'
    elif shutil.which('clang-format'):
        return 'clang-format'
    else:
        print_installation_instructions()
        sys.exit(1)


def check_clang_version(clang_format):
    """Check that the clang-format version matches requirements."""
    try:
        result = subprocess.run(
            [clang_format, '--version'],
            capture_output=True,
            text=True,
            check=True
        )
        version_output = result.stdout
    except subprocess.CalledProcessError:
        print("Could not determine clang-format version")
        sys.exit(1)

    # Extract version number using regex
    semver_regex = r'(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)'
    match = re.search(semver_regex, version_output)

    if match:
        version_major = int(match.group(1))

        if VERSION_REQUIRED_MAJOR > version_major:
            print(f"Clang version {version_major} too old (required: {VERSION_REQUIRED_MAJOR})")
            print()
            print_installation_instructions()
            sys.exit(1)
        elif VERSION_REQUIRED_MAJOR < version_major:
            print(f"Clang version {version_major} too new (required: {VERSION_REQUIRED_MAJOR})")
            print()
            print_installation_instructions()
            sys.exit(1)
    else:
        print("Could not determine clang-format version")
        sys.exit(1)


def load_ignore_patterns(script_dir):
    """Load patterns from style-ignore.txt."""
    ignore_file = script_dir / 'style-ignore.txt'
    patterns = []

    if ignore_file.exists():
        with open(ignore_file, 'r') as f:
            patterns = [line.strip() for line in f if line.strip()]

    return patterns


def should_ignore(file_path, directory, ignore_patterns):
    """Check if a file should be ignored based on ignore patterns."""
    full_path = str(Path(directory) / file_path)

    for pattern in ignore_patterns:
        if pattern in full_path:
            return True

    return False


def get_git_files(directory):
    """Get list of .h, .c, .cpp, and .proto files from git."""
    try:
        result = subprocess.run(
            ['git', 'ls-files'],
            cwd=directory,
            capture_output=True,
            text=True,
            check=True
        )

        # Filter for relevant file extensions
        files = []
        for line in result.stdout.splitlines():
            if re.search(r'\.(h|c|cpp|proto)$', line):
                files.append(line)

        return files
    except subprocess.CalledProcessError:
        print("Error: Could not get git file list")
        sys.exit(1)


def format_file(clang_format, file_path, quiet_mode, diff_cmd):
    """Format a single file and check for changes."""
    orig_file = f"{file_path}.orig"

    # Create backup
    shutil.copy2(file_path, orig_file)

    try:
        # Run clang-format
        subprocess.run(
            [clang_format, '-style=file', '-i', file_path],
            check=True
        )

        # Check if file changed
        files_differ = not files_are_identical(file_path, orig_file)

        if files_differ:
            if not quiet_mode:
                print(f"Changed {file_path}:")
                subprocess.run([diff_cmd, orig_file, file_path])
            return True

        return False

    finally:
        # Clean up backup
        if os.path.exists(orig_file):
            os.remove(orig_file)


def files_are_identical(file1, file2):
    """Compare two files byte by byte."""
    try:
        with open(file1, 'rb') as f1, open(file2, 'rb') as f2:
            return f1.read() == f2.read()
    except IOError:
        return False


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description='Run clang-format over git-tracked source files'
    )
    parser.add_argument(
        'directory',
        help='Directory to process'
    )
    parser.add_argument(
        '-q', '--quiet',
        action='store_true',
        help='Quiet mode - suppress diff output'
    )

    args = parser.parse_args()

    # Validate directory
    directory = Path(args.directory)
    if not directory.exists():
        print(f"Error: Directory '{args.directory}' does not exist")
        sys.exit(1)

    # Find and validate clang-format
    clang_format = find_clang_format()
    check_clang_version(clang_format)

    # Get script directory for finding style-ignore.txt
    script_dir = Path(__file__).parent

    # Load ignore patterns
    ignore_patterns = load_ignore_patterns(script_dir)

    # Determine diff command (use colordiff if available)
    diff_cmd = 'colordiff' if shutil.which('colordiff') else 'diff'

    # Get list of files from git
    files = get_git_files(directory)

    # Filter out ignored files first to get accurate count
    files_to_process = [
        file for file in files
        if not should_ignore(file, directory, ignore_patterns)
    ]

    # Create progress bar if tqdm is available and not in quiet mode
    if HAS_TQDM and not args.quiet:
        file_iterator = tqdm(
            files_to_process,
            desc="Formatting files",
            unit="file",
            ncols=80,
            bar_format='{desc}: {percentage:3.0f}%|{bar}| {n_fmt}/{total_fmt}'
        )
    else:
        file_iterator = files_to_process

    # Process each file
    error_found = False
    for file in file_iterator:
        file_path = directory / file

        # Format file and track if changes were made
        if format_file(clang_format, str(file_path), args.quiet, diff_cmd):
            error_found = True

    # Exit with error if any files were changed
    if error_found:
        sys.exit(1)


if __name__ == '__main__':
    main()
