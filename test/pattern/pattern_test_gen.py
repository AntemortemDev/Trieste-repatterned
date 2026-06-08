import sys
import subprocess
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

# Generates a C++ file for pattern parsing time testing, builds it and runs it
# Usage: python3 pattern_test_gen.py <max_patterns> <max_depth> [-r | -b]
# Generates m patterns for every depth n for m in [1, max_patterns] and n in [1, max_depth]
# -r    Run the analysis and plot only
# -b    Generate and build only


pattern_generator = "../../build/test/pattern/pattern_gen"
pattern_test_src_file = "pattern_gen_src.cc"
output_dir = "./"
exe_file = "../../build/test/pattern/pattern_test"


def gen_pattern(max_depth):
    return subprocess.run([pattern_generator, "-d", str(max_depth)], capture_output=True, text=True).stdout


def build(max_patterns, max_depth):
    pattern_functions = ""
    function_calls = ""
    function_skeleton = "std::vector<detail::PatternTreeEffect<Node>> patterns_%i_%i() {\n\treturn {\n %s \n\t};\n}\n\n"

    for count in range(1, max_patterns + 1):
        for depth in range(1, max_depth + 1):
            pattern_str = ""
            for i in range(count):
                pattern_str += "\t\t" + gen_pattern(depth) + "\t\t\t>> effect,\n"

            pattern_functions += function_skeleton % (count, depth, pattern_str)
            function_calls += f"\tfunctions[{count-1}][{depth-1}] = patterns_{count}_{depth};\n"
    

    pattern_test_src = ""
    with open(pattern_test_src_file, "r") as f:
        pattern_test_src = f.read()

    out_str = pattern_test_src.replace("/*patterns*/", pattern_functions)
    out_str = out_str.replace("/*functions*/", function_calls)
    out_str = out_str.replace("/*counts*/", str(max_patterns))
    out_str = out_str.replace("/*depths*/", str(max_depth))

    out_file = f"{output_dir}/pattern_test.cc"

    with open(out_file, 'w') as f:
        f.write(out_str)

    subprocess.run(["ninja"], cwd="../../build")

def run(max_patterns, max_depth):
    measurements = 10
    results = np.zeros((max_patterns, max_depth, measurements))

    for count_index in range(0, max_patterns):
        for depth_index in range(0, max_depth):
            for i in range(measurements):
                proc_result = subprocess.run([exe_file, str(count_index+1), str(depth_index+1)], capture_output=True, text=True)

                results[-(count_index+1), depth_index, i] = int(proc_result.stdout[0:-3])

    data = pd.DataFrame(results[:, :].mean(axis=2), columns=range(1, max_depth + 1), index=reversed(range(1, max_patterns + 1)))

    ax = sns.heatmap(data, vmin=0, vmax=1000)
    ax.collections[0].colorbar.set_label('Run time(µs)')
    plt.xlabel("Max pattern depth")
    plt.ylabel("Number of patterns")
    plt.title("Pattern Parsing Run Time")
    
    plt.show()

max_patterns = int(sys.argv[1])
max_depth = int(sys.argv[2])

if(len(sys.argv) > 3):
    if(sys.argv[3] == '-r'):
        run(max_patterns, max_depth)
        sys.exit(0)
    elif(sys.argv[3] == '-b'):
        build(max_patterns, max_depth)
        sys.exit(0)
        
build(max_patterns, max_depth)
run(max_patterns, max_depth)