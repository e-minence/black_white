echo "�A�[�J�C�u���X�g����"
perl wflby_gradata_conv.pl


echo "�S�t�@�C���R���p�C��"
call convlist.bat



nnsarc -c -i -n wifi_lobby -S list.txt

rem copy wflby_3dmapobj_data.c ..\wifi_lobby_map_conv\

rem del wflby_3dmapobj_data.c
del convlist.bat
