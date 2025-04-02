#!/bin/bash
ls | grep ".pcap" | xargs rm -r
ls | grep "files" | xargs rm -r

rm thesis-anim.xml
