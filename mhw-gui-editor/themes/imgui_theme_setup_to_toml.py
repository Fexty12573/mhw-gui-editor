import sys
import re
from pathlib import Path

def convert_to_toml(path: Path):
    with open(path, "r") as f:
        lines = f.readlines()

    colors = {}
    
    with open(path.with_suffix(".toml"), "w") as f:
        # General settings
        f.write("[Style]\n")
        for line in lines:
            if not "ImGuiCol_" in line:
                match = re.search(r"style.([^ ]+) *= *([^;]+)", line)
                if match:
                    prop = match.group(1)
                    if match.group(2).startswith("ImVec2"):
                        match = re.search(r"ImVec2[\({]([^, ]+), *([^, ]+)[\)}]", match.group(2))
                        f.write(f"{prop} = [{match.group(1).replace('f', '')}, {match.group(2).replace('f', '')}]\n")
                    else:
                        f.write(f"{prop} = {match.group(2).replace('f', '')}\n")
                else:
                    print(f"Error: Could not parse line: {line}")

        # Colors
        f.write("\n[Colors]\n")
        for line in lines:
            if "ImGuiCol_" in line:
                color = re.search(r"\[ImGuiCol_([^ ]+)\]", line).group(1)
                vec4 = line.split("=")[1].strip()
                match = re.search(r"ImVec4[\({]([^, ]+), *([^, ]+), *([^, ]+), *([^, ]+)[\)}]", vec4)
                if match:
                    r = match.group(1).replace('f', '') # Remove trailing f
                    g = match.group(2).replace('f', '') 
                    b = match.group(3).replace('f', '') 
                    a = match.group(4).replace('f', '') 
                    f.write(f"{color} = [{r}, {g}, {b}, {a}]\n")
                    colors[color] = (r, g, b, a)
                else:
                    match = re.search(r"style.Colors\[ImGuiCol_([^ ]+)\]", line.split(" = ")[1].strip())
                    if match:
                        r, g, b, a = colors[match.group(1)]
                        f.write(f"{color} = [{r}, {g}, {b}, {a}]\n")
                    else:
                        print(f"Error: Could not parse line: {line}")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        convert_to_toml(Path(input("Enter path to theme: ")))
    else:
        convert_to_toml(Path(sys.argv[1]))
