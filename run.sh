#!/bin/bash

 ./clean.sh
 ./waf --run "thesis --cc=$1"
 # mv thesis-anim.xml myscripts/mptcp_testing
 cd myscripts/mptcp_testing
 ./process_pcap.sh
 ./move_results.sh $2
 cd ../..

