# SPDX-License-Identifier: BSD-2-Clause

import argparse
import subprocess

parser = argparse.ArgumentParser(prog='check_format', description='Check and fix code formatting')
parser.add_argument('commit', default='')
parser.add_argument('-i', '--apply', action='store_true')
args = parser.parse_args()

apply = '-i' if args.apply else ''
cmd = 'git diff -U0 --no-color {} -- "*.c" "*.h" | clang-format-diff {} -p1'.format(args.commit, apply)
try:
    result = subprocess.run(cmd, capture_output=True, shell=True, check=True)
    if len(result.stdout) != 0:
        print('Formatting inconsistencies\nPlease, format the code\n')
        print(result.stdout.decode())
        exit(1)
    print('No formatting issues are present')
except subprocess.CalledProcessError as e:
    print('Failed to check the patch for formatting issues')
    print('Internal process returned with code', e.returncode)
    print(e.stdout.decode())
    exit(1)

exit(0)
