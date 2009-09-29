#==========================================================
#	�~���[�W�J���E���ڕϊ�Ruby
#	����Ruby���eRuby���ĂсA�F�X�R���o�[�g���Ă���܂��B
#===========================================================
require 'fileutils'
require 'program_conv'

#�o�͌�
PROGRAM_NUM = 5
#���ڃf�[�^�G�N�Z��
PRAGRAM_DATA_EXCEL = "program_data.xls"
#���ڃf�[�^�o�͖�
PRAGRAM_BIN_NAME = "program_data.bin"
#���ڃA�[�J�C�u��
NARC_NAME_BASE = "mus_prog_data"

#�X�g���[�~���O�R���o�[�^
STREAMING_CONVERT = "waveconv -c "
#gmm�R���o�[�^
MESSAGE_CONVERT = "perl -I ../../message ../../message/msgconv.pl "
MESSAGE_CONVERT_LANG = "JPN JPN_KANJI"
#�A�[�J�C�o�[
NARC_CONVERT = "nnsarc -c -l -n "


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


index = 1
progBinName = ""
strmDataName = ""
gmmDataName = ""

while index <= PROGRAM_NUM

  isRefreshFile = 0
  pathName = sprintf("p%02d",index)

  #�t�H���_�������ꍇ�t�H���_�����
  unless FileTest.exist?(pathName)
    FileUtils.mkdir_p(pathName) 
  end

  progBinName = pathName + "/" + PRAGRAM_BIN_NAME
  
  #���ڃf�[�^�ϊ�
  if( CompFileDate( PRAGRAM_DATA_EXCEL , progBinName ) == 1 )
    ProgramExcelConv.conv(index,PRAGRAM_DATA_EXCEL,progBinName)
    isRefreshFile = 1
  end
  
  #�X�g���[�~���O�ϊ�
  cnt = 0
  searchName = pathName + "/*.wav"
  Dir::glob(searchName).each{|fileName|
    cnt = cnt + 1
    strmDataName = fileName.sub(/.wav/,".swav")
    if( CompFileDate( fileName , strmDataName ) == 1 )
      system(STREAMING_CONVERT + fileName )
      isRefreshFile = 1
    end
  }

  if( cnt != 1 )
    print("�X�g���[�~���O�f�[�^�̃R���o�[�g�ŃG���[���������܂����B\n")
    print("wav���������Awav��2�ȏ゠��܂��B\n")
    print("�f�B���N�g��[" + pathName + "]\n")
    exit
  end

  #gmm�ϊ�
  cnt = 0
  searchName = pathName + "/*.gmm"
  Dir::glob(searchName).each{|fileName|
    cnt = cnt + 1
    gmmDataName = fileName.sub(/.gmm/,".dat")
    if( CompFileDate( fileName , gmmDataName ) == 1 )
      system(MESSAGE_CONVERT + fileName + " " + pathName + "/ " + MESSAGE_CONVERT_LANG )
      isRefreshFile = 1
    end
  }

  if( cnt != 1 )
    print("���b�Z�[�W�f�[�^�̃R���o�[�g�ŃG���[���������܂����B\n")
    print("gmm���������Agmm��2�ȏ゠��܂��B\n")
    print("�f�B���N�g��[" + pathName + "]\n")
    exit
  end

  #narc�쐬
  if( isRefreshFile == 1 )
    narcFileName = pathName + "/" + NARC_NAME_BASE
    system(NARC_CONVERT + narcFileName + " " + progBinName + " " + gmmDataName + " " + strmDataName )

    #narc�T�C�Y�`�F�b�N
    state = File::stat( narcFileName + ".narc")
    print( "�A�[�J�C�u���쐬���܂����B\n" )
    print( "Name[" + narcFileName + "]\n" )
    print( "Size[" + state.size.to_s + "byte(" + ((state.size/1024).to_i).to_s + "kb)]\n" )
    if( state.size > 512*1024 )
      print( "------------------------\n" )
      print( "�T�C�Y�I�[�o�[�ł��I�I�I\n" )
      print( "------------------------\n" )
    end

  end

  index = index+1
end



