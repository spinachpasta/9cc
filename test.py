
import os
class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def check(input: str, expected: int):
    os.system(f'./9cc "{input}" > tmp.s')
    os.system("cc -o tmp tmp.s")
    returned_value = (os.system("./tmp") >> 8) & 0xff

    if expected == returned_value:
        print(f"{bcolors.OKGREEN}passed:{input=} {expected=} {bcolors.ENDC}")
        return True

    print(f"{bcolors.FAIL}FAIL:check:{input=} {expected=} {returned_value=}{bcolors.ENDC}")
    return False


assert check("0", 0)

assert check("42", 42)

assert check("0+10+3", 0+10+3)

assert check("111+10-42", 111+10-42)

assert check("   111   + 10 -     42", 111+10-42)

assert check("   0 +    10+    3",  0 +    10+    3)

assert check("10*2", 10*2)

assert check("10+1*2", 10+1*2)
assert check("10+3*2+10-5", 10+3*2+10-5)

print("OK")