REM �܂��o�[�W�����t�@�C���̃R�s�[
cd prog/spec/
copy version_br ../version
cd ../
move version_br version
cd ../

REM �}���`�u�[�g�̃N���[��
cd multiboot
make clean
cd ../multiboot2
make clean
cd ../resource
make
if not %ERRORLEVEL%==0 GOTO ERROR
cd ..

REM MultiBoot1�̎��s
cd multiboot
cd arc
make
cd ..
make
if not %ERRORLEVEL%==0 GOTO ERROR
ECHO multiboot��make���������܂����B
ECHO �F�؃R�[�h�𔭍s���Ă��������B
ECHO �ۑ����multiboot/spec/main_r.sgn
pause
call attach_rom.bat
cd ..

REM MultiBoot2�̎��s
cd multiboot2
cd arc
make
cd ..
make
if not %ERRORLEVEL%==0 GOTO ERROR
ECHO multiboot2��make���������܂����B
ECHO �F�؃R�[�h�𔭍s���Ă��������B
ECHO �ۑ����multiboot2/spec/main_r.sgn
pause
call attach_rom.bat
cd ..

REM version��߂�
cd prog/spec/
copy version ../version
cd ../../

if %ERRORLEVEL%==0 GOTO END
ECHO �}���`�u�[�gROM�̍쐬���������܂����B

:ERROR
echo �G���[���������܂���

:END
