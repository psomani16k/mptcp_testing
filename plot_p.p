# Set output format and file
set terminal pngcairo enhanced font "Arial,12" size 2560, 1440
set output "throughput_plot.png"

# Set title and labels
set title "Throughput Over Time"
set xlabel "Time (seconds)"
set ylabel "Throughput (Kb/s)"
set grid
set xtics 10
set ytics 1000

# Use comma as CSV separator
set datafile separator ","

# Define region backgrounds (optional shading)
set object 1 rect from 20, graph 0 to 50, graph 1 fc rgb "#ff0000" fs solid 0.1 behind
set object 2 rect from 50, graph 0 to 80, graph 1 fc rgb "#00ff00" fs solid 0.1 behind
set object 3 rect from 80, graph 0 to 110, graph 1 fc rgb "#0000ff" fs solid 0.1 behind
set object 4 rect from 110, graph 0 to 140, graph 1 fc rgb "#ffff00" fs solid 0.1 behind

# Define region labels
set label 1 "No Traffic" at 35, graph 1.02 center
set label 2 "Forward" at 65, graph 1.02 center
set label 3 "Reverse" at 95, graph 1.02 center
set label 4 "Reverse + Forward" at 125, graph 1.02 center

# Plot multiple throughput sources
plot "./bw.csv" using 1:2 with lines title "link_1" lw 2 lc rgb "blue", \
     "./bw.csv" using 1:3 with lines title "link_2" lw 2 lc rgb "red", \
     "./bw.csv" using 1:4 with lines title "Total" lw 2 lc rgb "green"
