num_threads = 8

def parse_file(filename):
    vals = [[] for _ in range(num_threads)]
    times = [[] for _ in range(num_threads)]
    index = -1;
    with open(filename, "r") as file:
        for line in file:
            if line.startswith("THREAD COUNT"):
                index += 1
                if index >= num_threads:
                    break
            elif line.startswith("Final"):
                split = line.split(' ')
                vals[index] += [float(split[len(split)-1])]
            elif line.startswith("Method"):
                split = line.split(' ')
                times[index] += [float(split[len(split)-2])]
    minval = [min(i) for i in vals if len(i) != 0]
    mintime = [min(i) for i in times if len(i) != 0]
    maxval = [max(i) for i in vals if len(i) != 0]
    maxtime = [max(i) for i in times if len(i) != 0]
    avgval = [sum(i)/len(i) for i in vals if len(i) != 0]
    avgtime = [sum(i)/len(i) for i in times if len(i) != 0]
    return (vals, times, minval, mintime, maxval, maxtime, avgval, avgtime)


files = ["omp.txt", "pthread.txt", "mpi.txt"]
csv = ["omp.csv", "pthread.csv", "mpi.csv"]
for i in range(len(files)):
    (val, time, minv, mint, maxv, maxt, avgv, avgt) = parse_file(files[i])
    with open(csv[i], "w") as f:
        #Insert header
        f.write("NUM_THREADS,MIN_VAL,MAX_VAL,AVERAGE_VAL,MIN_TIME,MAX_TIME,AVERAGE_TIME\n")
        #Insert Values
        for j in range(len(val)):
            f.write(f"{j+1},{minv[j]},{maxv[j]},{avgv[j]},{mint[j]},{maxt[j]},{avgt[j]}\n")

def parse_hybrid(filename):
    vals = []
    times = []
    with open(filename, "r") as file:
        for line in file:
            if line.startswith("Final"):
                split = line.split(' ')
                vals += [float(split[len(split)-1])]
            elif line.startswith("Method"):
                split = line.split(' ')
                times += [float(split[len(split)-2])]
    minval = min(vals)
    mintime = min(times)
    maxval = max(vals)
    maxtime = max(times)
    avgval = sum(vals)/len(vals)
    avgtime = sum(times)/len(times)
    return (vals, times, minval, mintime, maxval, maxtime, avgval, avgtime)
hfiles = ["mpi_omp.txt", "mpi_pthread.txt"]

with open("hybrid.csv", "w") as f:
    #Insert header
    f.write("0:OMP/1:PTHREAD,MIN_VAL,MAX_VAL,AVERAGE_VAL,MIN_TIME,MAX_TIME,AVERAGE_TIME\n")
    for i in range(len(hfiles)):
        (val, time, minv, mint, maxv, maxt, avgv, avgt) = parse_hybrid(hfiles[i])
        #Insert Values
        f.write(f"{i},{minv},{maxv},{avgv},{mint},{maxt},{avgt}\n")
