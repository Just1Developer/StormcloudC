import os
from time import sleep as wait
import subprocess

# For cross platform coloring for those last console output, consider: from colorama import init
# If You Want All Warnings:
ShowAllWarnings = False

class TerminalColors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

# Set working directory
abspath = os.path.abspath(__file__)
dname = os.path.dirname(abspath)
os.chdir(dname)

files_to_compile = ' '.join([f for f in os.listdir() if f.endswith(".c")])

# Change if we want to
outputFile = "StormcloudC_v1"
# The -Wall option is added to enable most of the commonly used warnings in gcc.
command = f"gcc -fdiagnostics-color=always {'-Wall ' if ShowAllWarnings else ''}{files_to_compile} -o {outputFile}"

print(f"Compiling to {outputFile}...")

# Execute command and generate output
result = subprocess.run(command, text=True, capture_output=True)
if result.returncode == 0:
    print(f"{TerminalColors.OKGREEN}Compiled: {TerminalColors.OKBLUE}{outputFile}.exe{TerminalColors.ENDC}")
    if not result.stderr:
        wait(0.8)
        exit(0)

if(result.stderr):
    print(result.stderr)

if result.returncode == 0:
    print(f"{TerminalColors.WARNING}Compiled with warnings, see above for more information{TerminalColors.ENDC}")
else:
    print(f"{TerminalColors.FAIL}Compiling failed, see above for more information{TerminalColors.ENDC}")

os.system("pause")
#print("Press enter to close window")
#input()