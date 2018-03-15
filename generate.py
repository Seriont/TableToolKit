col_names = ["a" + str(i) for i in range(2)]

col = ["1"] * 500
s = " ".join(col)
output = open("input_big.csv", mode="w")
output.write(" ".join(col_names) + u"\n")
for j in range(1000000):
    for key in range(5):
        output.write(str(j) + " 1\n")
output.close()
