import csv
import subprocess
from tabulate import tabulate
from matplotlib import pyplot as plt

bench = input("Enter program to benchmark: ").strip()

N = 13
my_g = [0.0] * N
my_mem = [0.0] * N
my_exec = [0.0] * N

blis_g = [0.0] * N
blis_mem = [0.0] * N
blis_exec = [0.0] * N

sz = []

benchCount = 10
for j in range(benchCount):
    with open("inp", "w") as f:
        p = subprocess.Popen(["./build/benchmark", bench, "0"], stdout=f)
        p.wait()
        f.flush()
    my_rows = []
    blis_rows = []
    flag = True
    with open("inp", "r") as f:
        csbReader = csv.reader(f)
        header = next(csbReader)
        for row in csbReader:
            if flag is True:
                my_rows.append(row)
            else:
                blis_rows.append(row)
            flag = not flag
    for i in range(len(my_rows)):
        my_g[i] += float(my_rows[i][3])
        my_mem[i] += float(my_rows[i][2])
        my_exec[i] += float(my_rows[i][1])
        if len(sz) < N:
            sz.append(int(my_rows[i][0]))
    for i in range(len(blis_rows)):
        blis_g[i] += float(blis_rows[i][3])
        blis_mem[i] += float(blis_rows[i][2])
        blis_exec[i] += float(blis_rows[i][1])
    print("Done", j)

for i in range(len(blis_g)):
    blis_g[i] /= benchCount;
    blis_mem[i] /= benchCount;
    blis_exec[i] /= benchCount;

for i in range(len(my_g)):
    my_g[i] /= benchCount;
    my_mem[i] /= benchCount;
    my_exec[i] /= benchCount;

# summarize calculations
my_avg = {1000: 0.0, 100000: 0.0, 1000000: 0.0, 10000000: 0.0, 100000000: 0.0}
blis_avg = {1000: 0.0, 100000: 0.0, 1000000: 0.0, 10000000: 0.0, 100000000: 0.0}
cnt = {1000: 0, 100000: 0, 1000000: 0, 10000000: 0, 100000000: 0}
strFormat = {1000: "1e4", 100000: "1e5", 1000000: "1e6", 10000000: "1e7", 100000000: "1e8"}
for i in range(N):
    minVal = 1000
    minAbs = 1e15
    for k in my_avg.keys():
        if abs(sz[i] - k) < minAbs:
            minAbs = abs(sz[i] - k)
            minVal = k
    my_avg[minVal] += my_g[i]
    blis_avg[minVal] += blis_g[i]
    cnt[minVal] += 1

for k in my_avg.keys():
    my_avg[k] /= cnt[k]
    blis_avg[k] /= cnt[k]

table_g = []
table_m = []
table_e = []
for i in range(N):
    table_g.append([sz[i], my_g[i], blis_g[i]])
    table_m.append([sz[i], my_mem[i], blis_mem[i]])
    table_e.append([sz[i], my_exec[i], blis_exec[i]])
table_avg = []
for k in my_avg.keys():
    table_avg.append([strFormat[k], my_avg[k], blis_avg[k]])

print(tabulate(table_m, headers=['N', 'FAST-BLAS', 'BLIS'], tablefmt='pretty'))
print(tabulate(table_g, headers=['N', 'FAST-BLAS', 'BLIS'], tablefmt='pretty'))
print(tabulate(table_e, headers=['N', 'FAST-BLAS', 'BLIS'], tablefmt='pretty'))
print(tabulate(table_avg, headers=['N', 'FAST-BLAS', 'BLIS'], tablefmt='pretty'))

plt.subplot(1, 3, 1)
plt.plot(sz, my_g)
plt.plot(sz, blis_g)
plt.xlabel("Vector size")
plt.ylabel("GFLOPS")
plt.title("GFLOPS comparison")
plt.legend(["FAST-BLAS", "BLIS"], loc="lower right")
plt.subplot(1, 3, 2)
plt.plot(sz, my_mem)
plt.plot(sz, blis_mem)
plt.xlabel("Vector size")
plt.ylabel("Memory bandwidth")
plt.title("Memory bandwidth comparison")
plt.legend(["FAST-BLAS", "BLIS"], loc="lower right")
plt.subplot(1, 3, 3)
plt.plot(sz, my_exec)
plt.plot(sz, blis_exec)
plt.xlabel("Vector size")
plt.ylabel("Execution time")
plt.title("Execution time comparison")
plt.legend(["FAST-BLAS", "BLIS"], loc="lower right")
plt.show()
