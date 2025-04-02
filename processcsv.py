# wireshark filter: mptcp && frame.len > 1000 && !tcp.analysis.ack_lost_segment && !tcp.analysis.lost_segment && !tcp.analysis.out_of_order && !tcp.analysis.retransmission
import csv

lteThroughput = {}
wifiThroughput = {}
wimaxThroughput = {}
csvOut = [["time", "lte", "wifi", "wimax", "total"]]
keys = {}

with open("link_1.csv") as file:
    file = csv.reader(file)
    for lines in file:
        time = lines[0].split(".")[0]
        if time == 'frame':
            continue
        keys[int(time)] = False # true or false does not matter, just putting a value for the key to exist and be used later
        if time in lteThroughput:
            lteThroughput[time] += 11424
        else:
            lteThroughput[time] = 11424
            
with open("link_2.csv") as file:
    file = csv.reader(file)
    for lines in file:
        time = lines[0].split(".")[0]
        if time == 'frame':
            continue
        keys[int(time)] = False # true or false does not matter, just putting a value for the key to exist and be used later
        if time in wifiThroughput:
            wifiThroughput[time] += 11424 
        else:
            wifiThroughput[time] = 11424

# with open("wimax.csv") as file:
#     file = csv.reader(file)
#     for lines in file:
#         time = lines[0].split(".")[0]
#         if time == 'frame':
#             continue
#         keys[int(time)] = False # true or false does not matter, just putting a value for the key to exist and be used later
#         if time in wimaxThroughput:
#             wimaxThroughput[time] += 11424
#         else:
#             wimaxThroughput[time] = 11424
#
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
    # if time in wimaxThroughput:
    #     wimax = wimaxThroughput[time]
    total =  int(lte) + int(wifi) # + int(wimax)
    # csvOut.append([time, int(lte)/1000, int(wifi)/1000, int(wimax)/1000, total/1000])
    csvOut.append([time, int(lte)/1000, int(wifi)/1000,  total/1000])


with open("bw.csv", mode = 'w') as file:
    file = csv.writer(file)
    file.writerows(csvOut)

