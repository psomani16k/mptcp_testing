import csv

lteThroughput = {}
wifiThroughput = {}
wimaxThroughput = {}
csvOut = [["time", "lte", "wifi", "wimax", "total"]]
keys = {}

with open("./myscripts/mptcp_testing/lte.csv") as file:
    file = csv.reader(file)
    for lines in file:
        if lines[1] == '1502' and lines[3] == lines[4] == lines[5] == lines[6] == lines[7] == "":
            time = lines[0].split(".")[0]
            keys[int(time)] = False
            if time in lteThroughput:
                lteThroughput[time] += 1428
            else:
                lteThroughput[time] = 1428
            
with open("./myscripts/mptcp_testing/wifi.csv") as file:
    file = csv.reader(file)
    for lines in file:
        if lines[1] == '1502' and lines[3] == lines[4] == lines[5] == lines[6] == lines[7] == "":
            time = lines[0].split(".")[0]
            keys[int(time)] = False
            if time in wifiThroughput:
                wifiThroughput[time] += 1428
            else:
                wifiThroughput[time] = 1428

with open("./myscripts/mptcp_testing/wimax.csv") as file:
    file = csv.reader(file)
    for lines in file:
        if lines[1] == '1502' and lines[3] == lines[4] == lines[5] == lines[6] == lines[7] == "":
            time = lines[0].split(".")[0]
            keys[int(time)] = False
            if time in wimaxThroughput:
                wimaxThroughput[time] += 1428
            else:
                wimaxThroughput[time] = 1428

keys = list(keys.keys())
keys.sort()
for time in keys:
    time = str(time)
    lte = 0
    wifi = 0
    wimax = 0
    if time in lteThroughput:
        lte = lteThroughput[time]
    if time in wifiThroughput:
        wifi = wifiThroughput[time]
    if time in wimaxThroughput:
        wimax = wimaxThroughput[time]
    total =  int(lte) + int(wifi) + int(wimax)
    csvOut.append([time, int(lte)/1000, int(wifi)/1000, int(wimax)/1000, total/1000])


with open("./myscripts/mptcp_testing/bw.csv", mode = 'w') as file:
    file = csv.writer(file)
    file.writerows(csvOut)

