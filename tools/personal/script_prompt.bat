REM ##################################################
REM		�X�N���v�g�ǋ��L�ݒ�̌Ăяo��
REM ##################################################

PATH=%PROJECT_ROOT%tools/script/ctags;%PATH%

REM ##################################################
REM �c�[���p�X�n
doskey mks=make -j 3
doskey sa=c:\progra~1\sakura\sakura.exe $*
doskey maketag=mktags_wb_script

REM ##################################################
REM �f�B���N�g���ړ��V���[�g�J�b�g�n
doskey ch=cd %PROJECT_ROOT:/=\%$*
doskey che=cd %PROJECT_RSCDIR:/=\%fldmapdata\$*
doskey gose=cd %PROJECT_RSCDIR:/=\%fldmapdata\script\$*
doskey goev=cd %PROJECT_RSCDIR:/=\%fldmapdata\eventdata\$*

