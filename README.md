# btle_finder

Usage:
```
Usage: ./btle_finder
Enumerate available BTLE devices with your ubertooth. Does not filter malformed packets.

	-h	Print help.
	-d	Debug mode. Prints full packet of advertising data.
	-p	Only show devices with printable names. Without this option, expect erratic output.
	-f	Filter out seen bluetooth addresses.
```

Build:
```
g++ -g btle_finder.cpp -o btle_finder -L/usr/local/lib -lubertooth -L/usr/include -lbtbb -L/usr/include/bluetooth -lbluetooth -std=c++17
```

Example:
```
┌──(kali㉿mike-kl)-[~/Documents/btle_finder]
└─$ ./btle_finder -f -p
Ubertooth API version 1.07 found, newer than that supported by libubertooth (1.06).
Things will still work, but you might want to update your host tools.
E1:75:89:3A:3F:59 Dell Keybd 
C0:87:B8:F1:C0:C0 Dell Mouse 
```
