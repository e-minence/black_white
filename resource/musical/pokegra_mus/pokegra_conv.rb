#==========================================================
#	�~���[�W�J���p�|�P�����摜�ϊ�
#	����Ruby���eRuby�Ƃ�exe�Ƃ����ĂсA�F�X�R���o�[�g���Ă���܂��B
#===========================================================

require 'fileutils'
load 'pokegra_conv.lst'

#�ϊ�perl�̃f�B���N�g��
PERL_TOOL_DIR = ENV['PROJECT_ROOT'] + "tools/pokegra/"
#�ϊ����f�[�^�̃f�B���N�g��
POKEGURA_DATA_DIR = ENV['PROJECT_ROOT'] + "../pokemon_wb_doc/pokegra_mus/"
#�ϊ���f�[�^�̃f�B���N�g��
OUTPUT_DIR = "./data/"
TEMP_DIR = "./temp/"

#�A�[�J�C�o�[
NARC_CONVERT = "nnsarc -r -l -n -i"
NARC_CONVERT_OPT = "-S pokegra_wb.scr"
NARC_NAME = "pokegra_mus.narc"

#�t�@�C���X�V����r
#file1�̕����V�������1��Ԃ�
def CompFileDate( file1 , file2 )
  retVal = 0
  
  unless( FileTest.exist?(file2) )
    retVal = 1
  else
    state1 = File::stat(file1)
    state2 = File::stat(file2)
    
    if( state1.mtime > state2.mtime )
      retVal = 1
    end
  end

  retVal
end

#"program files"���̋󔒂�����t�@�C�������g����悤�ɕϊ�����
#http://blade.nagaokaut.ac.jp/cgi-bin/scat.rb/ruby/ruby-list/32740
MAX_PATH_SIZE = 513
def get_short_path_name lfn
  old_name = File.basename(lfn)
  require 'Win32API'
  buff = 0.chr * MAX_PATH_SIZE
  len = Win32API.new( 'kernel32' , 'GetShortPathName' , %w(P P N) , "N" ).
        Call( lfn , buff, buff.size)
  return nil if len==0
  ret = buff.split(0.chr)[0].tr('\\','/')[0,len]
  name = File.basename(ret)
  dir  = ret[0,ret.size - name.size]
  if name.size > old_name.size
    name = name[0,old_name.size]
  end
  return dir+name
end
alias sfn get_short_path_name


#---------------------------------------------------------
#���C��
#---------------------------------------------------------

print("�~���[�W�J���p�|�P�����O���t�B�b�N�ϊ�\n")
unless FileTest.exist?(OUTPUT_DIR)
  FileUtils.mkdir_p(OUTPUT_DIR)
end
unless FileTest.exist?(TEMP_DIR)
  FileUtils.mkdir_p(TEMP_DIR)
end

data_idx = 0

isRefresh = FALSE

while POKEGURA_CONV_LIST[data_idx].to_i < 999
  
  print(POKEGURA_CONV_LIST[data_idx] + "\n")
  searchName = POKEGURA_DATA_DIR + POKEGURA_CONV_LIST[data_idx] + "/*.*"

  print(searchName + "\n")

  Dir::glob(searchName).each{|fileName|
    print("[" + fileName + "]\n")
    fileType = File::extname(fileName)
    case
    #ncl
    when fileType == '.ncl'
      convFile = OUTPUT_DIR + File::basename( fileName , '.*' ) + '.NCLR'
      if( CompFileDate( fileName , convFile ) == 1 )
        system( "perl " + PERL_TOOL_DIR + "ncl.pl " + fileName + " " + TEMP_DIR )
        system( "cp " + TEMP_DIR + "*.NCLR" + " " + OUTPUT_DIR )
        isRefresh = TRUE
      end

    #ncg
    when fileType == '.ncg'
      convFile = OUTPUT_DIR + File::basename( fileName , '.*' ) + '.NCGR'
      if( CompFileDate( fileName , convFile ) == 1 )
        system( "perl " + PERL_TOOL_DIR + "ncg.pl " + fileName + " " + TEMP_DIR )
        system( "cp " + TEMP_DIR + "*.NCGR" + " " + OUTPUT_DIR )
        isRefresh = TRUE
      end
      convFile = OUTPUT_DIR + File::basename( fileName , '.*' ) + '.NCBR'
      if( CompFileDate( fileName , convFile ) == 1 )
        system( "perl " + PERL_TOOL_DIR + "ncgc.pl " + fileName + " " + TEMP_DIR )
        system( "cp " + TEMP_DIR + "*.NCBR" + " " + OUTPUT_DIR )
        isRefresh = TRUE
      end

    #nce
    when fileType == '.nce'
      convFile = OUTPUT_DIR + File::basename( fileName , '.*' ) + '.NCEC'
      if( CompFileDate( fileName , convFile ) == 1 )
        system( "perl " + PERL_TOOL_DIR + "nce_mus.pl " + fileName + " " + TEMP_DIR )
        system( "cp " + TEMP_DIR + "*.NCEC" + " " + OUTPUT_DIR )
        isRefresh = TRUE
      end

    #nce
    when fileType == '.nmc'
      convFile = OUTPUT_DIR + File::basename( fileName , '.*' ) + '.NMCR'
      if( CompFileDate( fileName , convFile ) == 1 )
        system( "perl " + PERL_TOOL_DIR + "nmc.pl " + fileName + " " + TEMP_DIR )
        system( "cp " + TEMP_DIR + "*.NMCR" + " " + OUTPUT_DIR )
        system( "cp " + TEMP_DIR + "*.NCER" + " " + OUTPUT_DIR )
        system( "cp " + TEMP_DIR + "*.NANR" + " " + OUTPUT_DIR )
        system( "cp " + TEMP_DIR + "*.NMAR" + " " + OUTPUT_DIR )
        isRefresh = TRUE
      end

    end
    system( "rm " + TEMP_DIR + "*.*" )

  }

  
  data_idx = data_idx+1

end #while

if( isRefresh == TRUE || FileTest.exist?(NARC_NAME) == FALSE )
    system( NARC_CONVERT + " " + NARC_NAME + " " + NARC_CONVERT_OPT )
end


