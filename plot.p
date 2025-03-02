# Set output format and file
set terminal pngcairo enhanced font "Arial,12" size 1200,800
set output "throughput_plot.png"

# Set title and labels
set title "Throughput Over Time"
set xlabel "Time (seconds)"
set ylabel "Throughput (KB/s)"
set grid

# Use comma as CSV separator
set datafile separator ","

# Plot multiple throughput sources
plot "./myscripts/mptcp_testing/bw.csv" using 1:2 with lines title "LTE" lw 2 lc rgb "blue", \
     "./myscripts/mptcp_testing/bw.csv" using 1:3 with lines title "WiFi" lw 2 lc rgb "red", \
     "./myscripts/mptcp_testing/bw.csv" using 1:4 with lines title "WiMAX" lw 2 lc rgb "green", \
     "./myscripts/mptcp_testing/bw.csv" using 1:5 with lines title "Total" lw 3 lc rgb "black"

