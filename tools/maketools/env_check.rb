#=====================================================================================
#
# make���Ď��p�X�N���v�g
#
# 2010.01.06  tamada GAMEFREAK inc.
# 2010.02.04  Java�̑��݃`�F�b�N��ǉ�
#
#
# �����Ɋ��ϐ��Ŏw�肳��Ă��铮�샋�[�g�f�B���N�g����n���A���݂̃f�B���N�g���Ɣ�r����B
# ��v���Ȃ��ꍇ�G���[�ɂ���
#
# Java�̑��݃`�F�b�N���s��
# ���݂��Ȃ��ꍇ�G���[�ɂ���
#
#
#=====================================================================================

#------------------------------------------------------
# �������`�F�b�N
#------------------------------------------------------
arg_dir = ARGV.shift
if arg_dir == nil || arg_dir == "" || !( File.exists?(arg_dir) ) then
  puts "enviroment check:"
  puts "�Ώۃf�B���N�g���i#{arg_dir}�j��������܂���I"
  exit -1
end

#------------------------------------------------------
# ���݂̃f�B���N�g���ƈ����Ŏ󂯎�����f�B���N�g���̔�r
#------------------------------------------------------
target_dir = arg_dir.gsub( /\\/, "/" ).sub( /\/$/, "" ).downcase
nowdir = `cygpath -m #{Dir.pwd}`.chomp.downcase
if nowdir != target_dir then
  puts "enviroment check:"
  puts "�Ώۃf�B���N�g���i#{target_dir}�j�ƌ��݂̃f�B���N�g���i#{nowdir}�j����v���܂���I"
  exit -1
end

#------------------------------------------------------
# Java�̑��݃`�F�b�N
#------------------------------------------------------
javapath = `which java`.chomp + ".exe"

if javapath =~ /^which: no java in / || File.exists?( javapath ) == false then
  puts "enviroment check:"
  puts javapath
  puts "Java��������܂���"
  exit -1
end


