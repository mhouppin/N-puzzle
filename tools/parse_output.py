import sys

def main(argv):
    if len(argv) != 2:
        print("Usage: parse_output.py <output_file>")
        return 1

    lastInfoLine = ""
    info = {
        "Path length": "0",
        "Total time": "0.000 seconds",
        "Total nodes": "0",
        "Total explorations": "0",
        "Duplicate nodes": "0",
        "Paths shrunk": "0",
        "Average path length": "0.00",
    }

    solved = False

    with open(argv[1], 'r') as f:
        for line in f:
            if "INFO" in line:
                lastInfoLine = line
            if ":" in line:
                if "Path length" in line:
                    solved = True
                key, value = line.strip().split(":")
                if key in info:
                    info[key] = value.strip()

    if solved:
        print("Solved (%7d nodes, %6.3f seconds, length %3d, average %5.2f)" % (
            int(info["Total nodes"]),
            float(info["Total time"].split()[0]),
            int(info["Path length"]),
            float(info["Average path length"])))

    elif len(lastInfoLine):
        fields = lastInfoLine.split()
        outputStr = "Unsolved ("

        if fields[1] != "(":
            outputStr += fields[1][1:]
        else:
            outputStr += fields[2]
        outputStr += " seconds, "

        outputStr += ' '.join(fields[-2:])
        outputStr += ")"
        print(outputStr)

if __name__ == "__main__":
    main(sys.argv)
