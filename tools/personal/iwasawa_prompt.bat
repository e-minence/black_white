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
doskey golib=cd c:\home\gflib\$*

doskey chs=cd %PROJECT_ROOT:/=\%prog\src\$*
doskey che=cd %PROJECT_ROOT:/=\%prog\src\fielddata\$*
