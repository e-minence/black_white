rem ev_def.bat
rem �X�N���v�g�t�@�C�� *_def.h����
ls -1 *.ev > ev_def_tmp
gawk -f ./scr_tools/ev_def2.awk ev_def_tmp > ev_def_tmp.bat
call ev_def_tmp.bat
del ev_def_tmp ev_def_tmp.bat
