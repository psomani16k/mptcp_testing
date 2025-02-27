# How to make
1. Clone this repository, make sure you have all the [dependencies](https://ns-3-dce.readthedocs.io/en/latest/getting-started.html) installed. 
**Also install gcc-4.8**

```git clone https://github.com/psomani16k/thesis_bake.git && cd thesis_bake```


2. Configure bake

```./bake.py configure -e mptcp_thesis```

3. Download the projects.

```./bake.py download```
> add the `-v` or `-vvv` tags for verbose stdout

4. Now cd into source/net-next-nuse

```cd source/net-next-nuse```

5. Run the command to make the configuration
```make defconfig ARCH=lib```

6. Run the command to start the menuconfig
```make menuconfig ARCH=lib```

7. Enable the following options if they are not already
    - Networking support/Networking options/MPTCP protocol
    - Networking support/Networking options/TCP: advance congestion controll/ _MPTCP Congestion control options_
    - Networking support/Wireless/_everything that seems useful_

> To actually enable an option press Y while hovering on that option, read the top menu for more help

8. Save the config to bake/source/net-next-nuse/.config (default) file and exit recurrsively.

9. In the `bake/source/net-next-nuse/` directory, run:

```cp .config lib/arch/defconfig```

10. Go back to bake directory 

```cd ../..```

11. Go to `source/net-next-nuse/arch/lib`, edit the Makefile, on line 18 change the repo link from ssh link to https link
`https://github.com/libos-nuse/linux-libos-tools`

12. Build the projects.
```./bake.py build```
> add the `-v` or `-vvv` tags for verbose stdout

13. if you get a problem like this:
> usr/bin/env: ‘python’: No such file or directory
> make: *** [arch/lib/Makefile:135: arch/lib/linker.lds] Error 127
> make: *** Waiting for unfinished jobs....
>   GEN     lib/crc32table.h
> 
> >> Building net-next-nuse_mptcp - Problem

then:

```cd source/net-next-nuse/arch/lib/```

edit the generate-linker-script.py first line to 

```#!/usr/bin/env python3```

try step 10 again

