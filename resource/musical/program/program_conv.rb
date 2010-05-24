#==========================================================
#	�~���[�W�J���E���ڃf�[�^�ϊ�Ruby
#	
#===========================================================
require 'fileutils'
require 'win32ole'

$KCODE = "SJIS"

#============================
#	define
#============================
STR_END = "#END"
HASH_ERROR = 255

#�G�N�Z���̃f�[�^�J�n�s
EXCEL_DATA_COL = 4

#�f�[�^�C���f�b�N�X
DATA_IDX_MANAGE_NO = 1
DATA_IDX_BACK_NO = 2
DATA_IDX_CON_COOL = 3
DATA_IDX_CON_CUTE = 4
DATA_IDX_CON_ELEGANT = 5
DATA_IDX_CON_UNIQUE = 6
DATA_IDX_CON_RANDOM = 7
DATA_IDX_AWARD_TYPE = 8
DATA_IDX_AWARD_NO = 9
DATA_IDX_POKE_DATA_START = 10

DATA_IDX_VERSION = 148
DATA_IDX_LANG_CODE = 149

POKEDATA_IDX_MONSNO = 0
POKEDATA_IDX_TRAINER = 1
POKEDATA_IDX_TR_NAME = 2
POKEDATA_IDX_TR_SEX = 3
POKEDATA_IDX_POKE_SEX = 4
POKEDATA_IDX_APPEAL_TYPE = 5
POKEDATA_IDX_OPEN_POINT = 6   #�o��|�C���g
POKEDATA_IDX_GOODS_BASE = 7

GOODSDATA_IDX_NO = 0
GOODSDATA_IDX_POS = 1

#��̌��ς�����R�R���Ή����邱��
GOODS_NUM = 8
GOODSDATA_NUM = 2
POKE_NUM = 6
POKEDATA_NUM = 7 + GOODSDATA_NUM*GOODS_NUM

#============================
#	proto
#============================

#============================
#	main
#============================
module Worksheet
  def [] y,x
    self.Cells.Item(y,x).Value
  end
end

def getAbsolutePath filename
  fso = WIN32OLE.new('Scripting.FileSystemObject')
  return fso.GetAbsolutePathName(filename)
end

module ProgramExcelConv

def self.conv( no , dataFileName , outFileName )

xl = WIN32OLE.new('Excel.Application')
begin

  filename = getAbsolutePath(dataFileName)
  xl.Workbooks.Open(filename)

  outFile = File.open( outFileName, "wb" );

  sheet = xl.Worksheets.Item("output")
  sheet.extend Worksheet
  
  dataCol = EXCEL_DATA_COL + no - 1
  
  padAry = Array(170) #(0xAA)

  #�w�i�ԍ�
  ary = Array( sheet[dataCol,DATA_IDX_BACK_NO].to_i )
  outFile.write( ary.pack("C") )

  #�R���f�B�V����
  ary = Array( sheet[dataCol,DATA_IDX_CON_COOL].to_i )
  outFile.write( ary.pack("C") )
  ary = Array( sheet[dataCol,DATA_IDX_CON_CUTE].to_i )
  outFile.write( ary.pack("C") )
  ary = Array( sheet[dataCol,DATA_IDX_CON_ELEGANT].to_i )
  outFile.write( ary.pack("C") )
  ary = Array( sheet[dataCol,DATA_IDX_CON_UNIQUE].to_i )
  outFile.write( ary.pack("C") )
  ary = Array( sheet[dataCol,DATA_IDX_CON_RANDOM].to_i )
  outFile.write( ary.pack("C") )

  #���J��
  ary = Array( sheet[dataCol,DATA_IDX_AWARD_TYPE].to_i )
  outFile.write( ary.pack("C") )
  outFile.write( padAry.pack("C") ) #�p�f�B���O
  ary = Array( sheet[dataCol,DATA_IDX_AWARD_NO].to_i )
  outFile.write( ary.pack("S") )
  ary = Array( sheet[dataCol,DATA_IDX_VERSION].to_i )
  outFile.write( ary.pack("C") )
  ary = Array( sheet[dataCol,DATA_IDX_LANG_CODE].to_i )
  outFile.write( ary.pack("C") )

  #�ۂ��f�[�^
  pokeIdx = 0
  while pokeIdx < POKE_NUM

    rowBase = DATA_IDX_POKE_DATA_START + (POKEDATA_NUM*pokeIdx)

    ary = Array( sheet[dataCol,rowBase+POKEDATA_IDX_MONSNO].to_i )
    outFile.write( ary.pack("S") )
    ary = Array( sheet[dataCol,rowBase+POKEDATA_IDX_TRAINER].to_i )
    outFile.write( ary.pack("C") )
    ary = Array( sheet[dataCol,rowBase+POKEDATA_IDX_TR_NAME].to_i )
    outFile.write( ary.pack("C") )
    ary = Array( sheet[dataCol,rowBase+POKEDATA_IDX_TR_SEX].to_i )
    outFile.write( ary.pack("C") )
    ary = Array( sheet[dataCol,rowBase+POKEDATA_IDX_POKE_SEX].to_i )
    outFile.write( ary.pack("C") )
    ary = Array( sheet[dataCol,rowBase+POKEDATA_IDX_APPEAL_TYPE].to_i )
    outFile.write( ary.pack("S") )
    ary = Array( sheet[dataCol,rowBase+POKEDATA_IDX_OPEN_POINT].to_i )
    outFile.write( ary.pack("S") )
    outFile.write( padAry.pack("S") ) #�p�f�B���O
    
    goodsIdx = 0
    while goodsIdx < GOODS_NUM
      goodsRowBase = DATA_IDX_POKE_DATA_START + (POKEDATA_NUM*pokeIdx) + POKEDATA_IDX_GOODS_BASE + (GOODSDATA_NUM*goodsIdx)
      ary = Array( sheet[dataCol,goodsRowBase+GOODSDATA_IDX_NO].to_i )
      outFile.write( ary.pack("S") )
      ary = Array( sheet[dataCol,goodsRowBase+GOODSDATA_IDX_POS].to_i )
      outFile.write( ary.pack("C") )
      outFile.write( padAry.pack("C") ) #�p�f�B���O
      
      goodsIdx = goodsIdx + 1
    end

    pokeIdx = pokeIdx + 1
  end

ensure
  outFile.close
  xl.Workbooks.Close
  xl.Quit

end #begin

end #self.conv

end #ProgramExcelConv