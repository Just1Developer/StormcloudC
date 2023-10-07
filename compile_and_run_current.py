import os
from time import sleep as wait
import subprocess
import random
import atexit

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
outputFile = "StormcloudC_v1_temp_" + str(random.randint(100, 99999))
# The -Wall option is added to enable most of the commonly used warnings in gcc.
command = f"gcc -fdiagnostics-color=always {'-Wall ' if ShowAllWarnings else ''}{files_to_compile} -o {outputFile}"

print(f"Compiling to {outputFile}...")


# For deleting the exe file later
def cleanup():
    try:
        os.remove(f"{outputFile}.exe")
    except Exception as e:
        print(f"Error while deleting file {outputFile}.exe: {e}")
        print("Press enter to close")
        input()
atexit.register(cleanup)


# Execute command and generate output
result = subprocess.run(command, text=True, capture_output=True)
if result.returncode == 0:
    print(f"{TerminalColors.OKGREEN}Compiled: {TerminalColors.OKBLUE}{outputFile}.exe{TerminalColors.ENDC}")
    if result.stderr:
        print(result.stderr)
        print(f"{TerminalColors.WARNING}Compiled with warnings, see above for more information{TerminalColors.ENDC}")
        print(f"{TerminalColors.WARNING}Press enter to continue, close window to abort{TerminalColors.ENDC}")
        input()
else:
    print(result.stderr)
    print(f"{TerminalColors.FAIL}Compiling failed, see above for more information{TerminalColors.ENDC}")
    print("Press enter to close window")
    input()
    exit(1)

# Run compiled code

print(f"Running {outputFile}.exe...")
result = os.system(f"cmd /c {outputFile}")
print(f"Program exited with code {result}")
os.system("pause")