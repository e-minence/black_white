echo "�A�[�J�C�u���X�g����"
perl wflby_gradata_conv.pl


echo "�S�t�@�C���R���p�C��"
call convlist.bat



nnsarc -c -i -n wifi_lobby -S list.txt

copy wflby_3dmapobj_data.c ..\..\prog\src\net_app\wifi_lobby\map_conv\

del wflby_3dmapobj_data.c
del convlist.bat
