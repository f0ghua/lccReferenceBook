cls
lcc xmsgbox.c
lcc sample.c
lcclnk -subsystem console sample.obj xmsgbox.obj
pause