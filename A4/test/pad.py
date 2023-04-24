

with open("./test/out_there.txt", "r") as i:
    with open("./test/out_there_format.txt", "w") as o:
        for line in i:
            if line[len(line) - 1] == '\n':
                line = line[0:len(line) - 1]
            while(len(line) < 8*20):
                line += " "
            
            o.write(line + '\n')
        

