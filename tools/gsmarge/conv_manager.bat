@echo off
set PATH=\cygwin\bin;%PATH%;

REM ---------------------------------------------------------
REM �R���o�[�g���t�H���_
REM ---------------------------------------------------------
set GS_SRC_DIR=C:\gshome\pokemon_gs\src\application\e_mail\

REM ---------------------------------------------------------
REM �R���o�[�g��t�H���_
REM ---------------------------------------------------------
set WB_DST_DIR=c:\home\pokemon_wb\prog\src\app\e_mail\

REM ---------------------------------------------------------
REM �R���o�[�g����t�@�C��
REM ---------------------------------------------------------
set SRC_FILES=email.c email.dat email_enter.c email_input.c email_main.c email_snd.h email_snd_def.h email_test.c email_tool.c

REM ---------------------------------------------------------
REM �R���o�[�g�{��
REM ---------------------------------------------------------

REM �J�n���b�Z�[�W
echo "�R���o�[�g���J�n���܂�"

REM �R���o�[�g
for %%I in ( %SRC_FILES% ) do  (
REM echo "%%I���R���o�[�g���܂�"
perl source_convert.pl %GS_SRC_DIR%%%I %WB_DST_DIR%%%I
)

REM	�I�����b�Z�[�W
echo "�S�ẴR���o�[�g���I�����܂���"
PAUSE
