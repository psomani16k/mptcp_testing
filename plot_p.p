# Set output format and file
set terminal pngcairo enhanced font "Arial,12" size 4000, 3000
set output "throughput_plot.png"

# Set title and labels
set title "Throughput Over Time"
set xlabel "Time (seconds)"
set ylabel "Throughput (Kb/s)"
set grid
set xtics 10
set ytics 500

# Use comma as CSV separator
set datafile separator ","

# Define region backgrounds (optional shading)
set object 1 rect from 20, graph 0 to 80, graph 1 fc rgb "#ff0000" fs solid 0.1 behind
set object 2 rect from 80, graph 0 to 170, graph 1 fc rgb "#00ff00" fs solid 0.1 behind
set object 3 rect from 170, graph 0 to 260, graph 1 fc rgb "#0000ff" fs solid 0.1 behind
# set object 4 rect from 170, graph 0 to 220, graph 1 fc rgb "#ffff00" fs solid 0.1 behind

# Define region labels
set label 1 "No Traffic" at 50, graph 1.02 center
set label 2 "F+R Path 1" at 125, graph 1.02 center
set label 3 "F+R Path 2" at 215, graph 1.02 center
# set label 4 "Reverse + Forward" at 195, graph 1.02 center

# Plot multiple throughput sources
plot "./bw.csv" using 1:2 with lines title "PATH 1" lw 2 lc rgb "blue", \
     "./bw.csv" using 1:3 with lines title "PATH 2" lw 2 lc rgb "red", \
     "./bw.csv" using 1:4 with lines title "TOTAL" lw 2 lc rgb "green"
