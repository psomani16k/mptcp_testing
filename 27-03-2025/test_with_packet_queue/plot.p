# Set output format and file
set terminal pngcairo enhanced font "Arial,12" size 2560, 1440
set output "throughput_plot.png"

# Set title and labels
set title "Throughput Over Time"
set xlabel "Time (seconds)"
set ylabel "Throughput (Kb/s)"
set grid

# Use comma as CSV separator
set datafile separator ","

# Plot multiple throughput sources
plot "./test_base/bw.csv" using 1:4 with lines title "BASE" lw 2 lc rgb "black", \
     "./test_rt/bw.csv" using 1:4 with lines title "RT" lw 2 lc rgb "red", \
     "./test_ft/bw.csv" using 1:4 with lines title "FT" lw 2 lc rgb "green", \
     "./test_ft_rt/bw.csv" using 1:4 with lines title "RT-FT" lw 2 lc rgb "blue", \

