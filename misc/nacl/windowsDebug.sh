CHROMEDIR=C:/Users/psi/AppData/Local/Google/Chrome/Application
CHROME_VERSION=21.0.1180.83

start python2 -m SimpleHTTPServer

export NACLVERBOSITY=3
$CHROMEDIR/chrome.exe --nacl-gdb=D:/opt/nacl_sdk/nacl-gdb/win64/nacl64-gdb.exe http://localhost:8000/

