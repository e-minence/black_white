rem ev_def.bat
rem �X�N���v�g�t�@�C�� *_def.h����
ls -1 *.ev > ev_def_tmp
ruby scr_tools/ev_def_list.rb ev_def_tmp > ev_def_tmp.bat
call ev_def_tmp.bat
del ev_def_tmp ev_def_tmp.bat
