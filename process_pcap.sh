#!/bin/bash

rm lte.csv wifi.csv wimax.csv bw.csv

# tshark -r ../../thesis-main-*-1-0.pcap -T fields -e frame.time_epoch -e frame.len -E header=y -E separator=, "mptcp && frame.len > 1000 && !tcp.analysis.ack_lost_segment && !tcp.analysis.lost_segment && !tcp.analysis.out_of_order && !tcp.analysis.retransmission" > lte.csv
# tshark -r ../../thesis-main-*-1-1.pcap -T fields -e frame.time_epoch -e frame.len -E header=y -E separator=, "mptcp && frame.len > 1000 && !tcp.analysis.ack_lost_segment && !tcp.analysis.lost_segment && !tcp.analysis.out_of_order && !tcp.analysis.retransmission" > wifi.csv
# tshark -r ../../thesis-main-*-1-2.pcap -T fields -e frame.time_epoch -e frame.len -E header=y -E separator=, "mptcp && frame.len > 1000 && !tcp.analysis.ack_lost_segment && !tcp.analysis.lost_segment && !tcp.analysis.out_of_order && !tcp.analysis.retransmission" > wimax.csv

tshark -r ../../thesis-ww-1-0.pcap -T fields -e frame.time_epoch -e frame.len -E header=y -E separator=, "mptcp && frame.len > 1000 && !tcp.analysis.ack_lost_segment && !tcp.analysis.lost_segment && !tcp.analysis.out_of_order && !tcp.analysis.retransmission" > link_1.csv
tshark -r ../../thesis-ww-1-1.pcap -T fields -e frame.time_epoch -e frame.len -E header=y -E separator=, "mptcp && frame.len > 1000 && !tcp.analysis.ack_lost_segment && !tcp.analysis.lost_segment && !tcp.analysis.out_of_order && !tcp.analysis.retransmission" > link_2.csv

python3 processcsv.py

# gnuplot plot_p.p
