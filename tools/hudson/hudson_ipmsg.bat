if %1 == 0 goto _OK:
if %1 == 1 goto _NG:

:_OK
"C:\Program Files\Ipmsg\ipmsg.exe" /MSG W00159 "���C�N�����I"
goto _END:

:_NG
"C:\Program Files\Ipmsg\ipmsg.exe" /MSG W00159 "���C�N���s�I"
goto _END:

:_END
pause