REM ����0�𐬌��E����ȊO�����s�Ƃ݂Ȃ�
if %1 == 0 goto _OK:

:_NG
"C:\Program Files\Ipmsg\ipmsg.exe" /MSG W00159 "���C�N���s�I"
goto _END:

:_OK
"C:\Program Files\Ipmsg\ipmsg.exe" /MSG W00159 "���C�N�����I"
goto _END:

:_END
