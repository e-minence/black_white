REM ##################################################
REM		���V�p�ݒ�̌Ăяo��
REM ##################################################

REM ##################################################
REM		�X�N���v�g�ǐݒ�̌Ăяo��
REM ##################################################
if exist "tools\personal\script_prompt.bat" call tools\personal\script_prompt.bat

REM ##################################################
REM �c�[���p�X�n
doskey vi=c:\tools\vi\gvim.exe $*
doskey golib=cd c:\home\gflib\gflib\src\$*
doskey godat=cd c:\home\wb_data3d$*

doskey chp=cd %PROJECT_ROOT:/=\%prog\$*
doskey chs=cd %PROJECT_ROOT:/=\%prog\src\$*
doskey chi=cd %PROJECT_ROOT:/=\%prog\include\$*
doskey che=cd %PROJECT_ROOT:/=\%prog\src\fielddata\$*
doskey chr=cd %PROJECT_ROOT:/=\%resource\$*
doskey chf=cd %PROJECT_ROOT:/=\%resource\fldmapdata\$*
