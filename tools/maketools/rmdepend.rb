#============================================================================
#
# �ˑ���`�t�@�C�����폜����make�������邽�߂̃c�[��
#
# @author tamada GAMEFREAK inc.
# @date 2009.09.16  renew
#
# @note
#   prog/makefile��"echo_buildtype"�^�[�Q�b�g�Ɉˑ����Ă���B
#   "echo_buildtype"��NITRO_BUILDTYPE���擾���邽�߂����ɒǉ������B
#
#============================================================================
require "fileutils"

PROGDIR = ENV["PROJECT_PROGDIR"].sub(/C:/,"/cygdrive/c").chomp

def DebugPut arg
  puts "DEBUG:" + arg
end

#�w��t�@�C�������܂ވˑ��t�@�C�������X�g�A�b�v����
def search_depend_files( search_name )
  build_type = `cd #{PROGDIR}; make echo_buildtype`.chomp
  #DebugPut "result:#{build_type}"
  depend_path = "#{PROGDIR}depend/#{build_type}/*.d"
  command = "grep #{search_name} #{depend_path}"
  DebugPut "command:#{command}"
  dependfiles = `#{command}`
  DebugPut "\"#{dependfiles}\""
  return dependfiles
end

#�ˑ��t�@�C�������X�g����ˑ��t�@�C���ƃI�u�W�F�N�g�t�@�C�����폜����
def remove_depfile_and_objfile( dependfiles )
  dependfiles.each{|line|
    file = line.sub(/:.*$/,"").chomp
    puts "rm #{file}"
    FileUtils.rm( file )
    FileUtils.rm( file.sub(/\/depend\//,"\/obj\/").sub(/d$/,"o") )
  }
end

#����������s�{��
raise Exception, "�w�b�_�t�@�C�������w�肵�Ă�������" if ARGV.size == 0 

search_name = ARGV.shift
dependfiles = search_depend_files( search_name )
raise Exception, "�ˑ����Ă���t�@�C����������܂���" if dependfiles == ""
remove_depfile_and_objfile( dependfiles )

