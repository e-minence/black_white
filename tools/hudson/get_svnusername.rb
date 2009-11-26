#=================================================================
#
#   ���r�W�����E���[�U�[�����o���X�N���v�g
#
#   2009.11.26  tamada GAMEFREAK inc.
#
#   svn info �R�}���h�̏o�͂���͂��ĕK�v�ȃp�����[�^�����o��
#
#   �I�v�V�����F
#   -n    ���[�U�[��
#   -r    ���r�W����
#   -d    ���t
#   �I�v�V�������w�肵�Ȃ��ꍇ�A��L���ׂ�
#
#=================================================================

result = `svn info -r HEAD`
lines = result.split("\r\n")
author = "Not Found"
rev = "Not Found"
date = "Not Found"

lines.each{|line|
  column = line.split
  if line =~/Last Changed Author/ then
    author = column[3]
  elsif line =~/Last Changed Rev/ then
    rev = column[3]
  elsif line =~/Last Changed Date/ then
    date = column[3] + " " + column[4]
  else
  #  p line
  end
}
case ARGV[0]
when "-n" then
  puts author
when "-r" then
  puts rev
when "-d" then
  puts date
else
  puts author
  puts rev
  puts date
end

