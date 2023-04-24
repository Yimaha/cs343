
case = ["", " -c l", " -c u"]
number = ["", " -e 0", " -e 1", " -e 100", " -e -1", " -e -100"]
capitcalize = ["", " -s"]

def addCase(str):
    result = []
    for w in case:
        result.append(str + w)
    return result

def addnumber(str):
    result = []
    for p in number:
        result.append(str + "." + p)
    return result


def addcapitcalize(str):
    result = []
    for l in capitcalize:
        result.append(str + l)
    return result

def addDest(str):
    return str + " ./test/input.txt ./test/output.txt"


with open("./test/script_support.txt", "w") as f:
    result = ["./filter"]
    new_result = []
    for r in result:
        result = addCase(r)
    for r in result:
        new_result.append(addDest(r))
    result = new_result
    for r in result:
        f.write(r + '\n')
        
