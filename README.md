### IEEE OUI Downloader

Download IEEE OUI lookup references
- reference: https://standards-oui.ieee.org/

The following files are created:
- oui.csv
- cid.csv
- iab.csv
- mam.csv
- oui36.csv

Note: need to have libcurl installed for compilation

Compile:
```
apt install libcurl4-openssl-dev
gcc -o ieee_oui_downloader ieee_oui_downloader.c -lcurl
```