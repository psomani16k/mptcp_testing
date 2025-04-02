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

# Plot multiple throughput sources
plot "./bw.csv" using 1:2 with lines title "link_1" lw 2 lc rgb "blue", \

     "./bw.csv" using 1:3 with lines title "link_2" lw 2 lc rgb "red", \

     "./bw.csv" using 1:4 with lines title "Total" lw 2 lc rgb "green", \ 
