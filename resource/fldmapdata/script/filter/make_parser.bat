@echo off
if "%1" == "diff" goto diff
if "%1" == "install" goto install

echo %0 sp3.y�����ɃX�N���v�g�p�p�[�T���쐬���܂�
echo �R�}���h�F
echo �@�@%0 diff  ���݂̃p�[�T�Ɣ�r���܂�
echo �@�@%0 install  ���݂̃p�[�T�ƒu�������܂�
goto end

:install
REM ruby c:\cygwin\bin\racc -o parser.rb sp3.y
"C:\Program Files\ruby-1.8\bin\racc.bat" -o parser.rb sp4.y
goto end

:diff
REM ruby c:\cygwin\bin\racc -o sample.rb sp3.y
"C:\Program Files\ruby-1.8\bin\racc.bat" -o sample.rb sp4.y
diff sample.rb parser.rb
:end
