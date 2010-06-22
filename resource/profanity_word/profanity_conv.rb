#==========================================================
#	�~���[�W�J���E���ڃf�[�^�ϊ�Ruby
#	
#===========================================================
require 'fileutils'
require 'win32ole'
require 'kconv'
require 'nkf'

$KCODE = "UTF8"

#============================
#	define
#============================
EOMCODE = 0xFF
STR_LEN = 32
RAND_KEY = 0x72012891
#============================
#	proto
#============================
SHEET_NAME = ["Japanese",
              "UK English",
              "German",
              "French",
              "Italian",
              "Spanish",
              "Korean"]
FILE_NAME =  ["jp_data.bin",
              "us_data.bin",
              "de_data.bin",
              "fr_data.bin",
              "it_data.bin",
              "es_data.bin",
              "kr_data.bin"]

#============================
#	main
#============================
module Worksheet
  def [] y,x
    self.Cells.Item(y,x).Value2
  end
end

def getAbsolutePath filename
  fso = WIN32OLE.new('Scripting.FileSystemObject')
  return fso.GetAbsolutePathName(filename)
end

module ProfanaryExcelConv

dataFileName = ARGV[0]

#�󂯎�镶�����UTF8�Ɏw��

t = Time.now
#srand( t.sec ^ t.usec ^ Process.pid )
srand( 0x19840127 )

WIN32OLE.codepage = WIN32OLE::CP_UTF8
xl = WIN32OLE.new('Excel.Application')
begin
  
  filename = getAbsolutePath(dataFileName)
  xl.Workbooks.Open(filename)

  sheetNum = 0
  while sheetNum < 7
    print( SHEET_NAME[sheetNum] + "->" + FILE_NAME[sheetNum] + "\n" )
    outFile = File.open( FILE_NAME[sheetNum] , "wb" );

    sheet = xl.Worksheets.Item(SHEET_NAME[sheetNum])
    sheet.extend Worksheet
    
    line = 1
    isLoop = 1
    randCode = RAND_KEY
    while isLoop == 1
      
      if( sheet[line,1] != nil )
        
        #UTF8�Ŏ󂯎��
        strTemp = sheet[line,1].to_s
        #UTF8��UTF16�ɕϊ�
        strBase = strTemp.kconv(Kconv::UTF16,Kconv::UTF8)
        #1byte�P�ʂɕ���
        strArr = strBase.unpack("C*")
        strLen = strArr.size

        #print(  "[" + strTemp + "][" + strBase + "]Len = " + strArr.size.to_s + "\n")
        
        pos = 0
        #�ʏ�̕�������
        while pos < strArr.size/2
          temp = strArr[pos*2]
          strArr[pos*2] = strArr[pos*2+1]
          strArr[pos*2+1] = temp
          pos = pos+1
        end
        #EOM����
        strArr[pos*2]   = EOMCODE
        strArr[pos*2+1] = EOMCODE
        pos = pos+1
        #�S�~�f�[�^
        while pos < STR_LEN
          strArr[pos*2]   = (rand * 255).to_i
          strArr[pos*2+1] = (rand * 255).to_i
          pos = pos+1
        end
        
        #�Í���
        outputArr = Array.new
        pos = 0
        while pos < STR_LEN
          outputArr[pos] = strArr[pos*2  ] +
                           strArr[pos*2+1]*0x100
          randCode = ((randCode * 1103515245 + 24691))&0xFFFFFFFF
          #print( randCode.to_s + "," )
          outputArr[pos] = outputArr[pos]^((randCode/65536)&0xFFFF)

          pos = pos+1
        end
        
        
        #�o��
        pos = 0
        while pos < STR_LEN
          #print( outputArr[pos].to_s + "," )
          ary = Array( outputArr[pos].to_i )
          outFile.write( ary.pack("S") )
          pos = pos+1
        end
        
        #ary = Array( sheet[0,0].to_i )
        #outFile.write( ary.pack("C") )
        
        line = line + 1
      else
        isLoop = 0
      end

    end #while
    
    sheetNum = sheetNum+1
    outFile.close
  end #while sheetNum < 7
  
ensure
  #outFile.close
  xl.Workbooks.Close
  xl.Quit

end #begin

end
