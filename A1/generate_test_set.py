
width = ["11111111111111111111111111111111111111111111111111111", "33", "9", "0", "-23", '*', 'BUI']
precision = ["11111111111111111111111111111111111111111111111111111", "33", "9", "0", "-23", "*", "", 'BUI']
length = ["h", "hh", "l", "ll", "j", "z", "t", "L", 'BUI']
specifier = ["d", "i", "u", "o", "x", "X", "f", "F", "e", "E", "g", "G", "a", "A", "c", "s", "p", "n", "%", 'BUI']

def addWidth(str):
    result = []
    for w in width:
        result.append(str + w)
    return result

def addPrecision(str):
    result = []
    for p in precision:
        result.append(str + "." + p)
    return result


def addLength(str):
    result = []
    for l in length:
        result.append(str + l)
    return result

def addSpecifier(str):
    result = []
    for s in specifier:
        result.append(str + s)
    return result

def addIllegal(str):
    return [str + "BUI"]

with open("./sample/q3input1.txt", "w") as f:
    result = ["", "%"]
    for r in result:
        result = result + addIllegal(r)
    for r in result:
        result = result + addWidth(r)
    for r in result:
        result = result + addPrecision(r)
    for r in result:
        result = result + addLength(r)
    for r in result:
        result = result + addSpecifier(r)
    for r in result:
        result = result + addIllegal(r)
    for r in result:
        f.write(r + '\n')
        
