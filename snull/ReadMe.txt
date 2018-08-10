This module was tested on Ubuntu 16.04 as follows:

1. Modify snull.c(line 461, 462) as:
	((u8 *)saddr)[2] ^= 7; /* change the third octet (class C) */
	((u8 *)daddr)[2] ^= 7;
   because the subnet 192.168.0.x was used.

2. make && sudo insmod snull.ko

3. set /etc/hosts as:
	192.168.2.1	local0
	192.168.2.2	remote0
	192.168.5.2	local1
	192.168.5.1	remote1

4. sudo service network-manager stop

5. sudo ifconfig sn0 local0
   sudo ifconfig sn1 local1

6. ping remote0


# I found RS will be sent during the test, then IPv4s will be lost. So I configure IPv6 of sn0, sn1 to linklocal only, and the test was passed. So stops the service network-manager maybe a good choice in step 4.
