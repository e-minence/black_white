rem ev_def.bat
rem スクリプトファイル *_def.h生成
ls -1 *.ev > ev_def_tmp
awk -f ./scr_tools/ev_def2.awk ev_def_tmp > ev_def_tmp.bat
call ev_def_tmp.bat
del ev_def_tmp ev_def_tmp.bat
