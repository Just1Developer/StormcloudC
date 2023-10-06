import os

# Set working directory
abspath = os.path.abspath(__file__)
dname = os.path.dirname(abspath)
os.chdir(dname)

# Get Files to compile and generate compile command
files = "gcc "
for file in os.listdir():
    if(".c" not in file): continue
    print(f"Found file: {file}")
    files += file + " "

# Change if we want to
outputFile = "StormcloudC_v1"

print(f"Compiling to {outputFile}...")
# Execute command and generate output
files += f"-o \"{outputFile}\""
out = os.system(f'cmd /c {files}')
if(out == 0):
    print(f"Compiled: {outputFile}.exe")
else:
    print("Compiling failed. See above for error.")
    print("Press enter to close window")
    input()