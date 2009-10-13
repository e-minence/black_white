#======================================================================
#
#
#   eventdata �p�_�~�[�f�[�^�����X�N���v�g
#
#
#   2009.10.13  tamada GAMEFREAK inc.
#
#======================================================================

THIS_DIRECTORY = File.dirname(__FILE__).gsub(/\\/,"/")
WINDOWS_SVN_FLAG = if `which svn`.chomp =~ /\/cygdrive\/c\/tools\/Subversion/ then
                     true
                   else
                     false
                   end

file_id = ARGV[0]

#------------------------------------------------------
#
#
#
#------------------------------------------------------
def make_dummy_file( file_id, dummy_file_name )
  new_file_name = "#{THIS_DIRECTORY}/../data/#{file_id}" + File.extname(dummy_file_name)
  if FileTest.exists?(new_file_name) then
    STDERR.puts "#{new_file_name} exists. do nothing."
    return
  end
  contents = File.open(THIS_DIRECTORY + dummy_file_name).read.gsub(/dummydata/, file_id)
  File.open(new_file_name,"w"){|file|
    file.write contents
  }
  system("unix2dos -D " + new_file_name)
  system("svn add " + new_file_name) if WINDOWS_SVN_FLAG == true
  
  return new_file_name
end

#------------------------------------------------------
#------------------------------------------------------
make_dummy_file( file_id, "/dummydata.mev")
make_dummy_file( file_id, "/dummydata.wms")


