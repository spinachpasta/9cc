
import os

def check(input: str, expected: int):
    os.system(f"./9cc {input} > tmp.s")
    os.system("cc -o tmp tmp.s")
    returned_value = (os.system("./tmp") >> 8) & 0xff

    if expected == returned_value:
        return True

    print(f"check:{input=} {expected=} {returned_value=}")
    return False


assert check("0", 0)

assert check("42", 42)

print("OK")