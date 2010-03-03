=begin
 * @file    tab_out_direct.rb
 * @brief   �G�N�Z������^�u��؂�t�@�C�����o�͂���G�N�Z��
 * @author  ariizumi
 * @data    09/09/22

  tab_out.rb�ł͌v�Z�����Z���ɓ��͂���Ă����ꍇ�A���̎����o�͂��Ă��܂������ߍ쐬�B
  Ruby1.8����W���ł��Ă���win32ole�ƌ������C�u�������g���āA�G�N�Z���ɒ��ڃA�N�Z�X���A
�@�t�@�C���̏o�͂��s���Ă���B

�@�g���Ă���͈͂����ŏo�͂��Ă���̂ŁAA���1�s�ڂ���s���ƁAB�s��2��ڂ���o�͂����̂Œ��ӁB
�@
�@tab_out_direct.rb <�t�@�C����> : TAB��؂�
�@tab_out_direct.rb <�t�@�C����> -c : �J���}��؂�
�@tab_out_direct.rb <�t�@�C����> -s : �J���}��؂�{�󔒂��o��
=end


require 'win32ole'


module Worksheet
  def [] y,x
    self.Cells.Item(y,x).Value
  end
end

def getAbsolutePath filename
  fso = WIN32OLE.new('Scripting.FileSystemObject')
  return fso.GetAbsolutePathName(filename)
end


filename = getAbsolutePath(ARGV[0])
sep = "\t"
isOutSpace = FALSE
if ARGV[1] == "-c"
  sep = ","
end
if ARGV[1] == "-s"
  sep = ","
  isOutSpace = TRUE
end


xl = WIN32OLE.new('Excel.Application')

begin
  xl.Workbooks.Open(filename)
  sheetNum = xl.Worksheets.Count
#  print "�V�[�g��#{sheetNum}��\n"

  1.upto(sheetNum) do |sheetNo|
  
    sheet = xl.Worksheets.Item(sheetNo)
    sheet.extend Worksheet
    useRange = sheet.UsedRange
    colNum = useRange.Columns.Count
    rowNum = useRange.Rows.Count
#    print "#{colNum}��F#{rowNum}�s\n"
#    print "#{useRange.Column}��F#{useRange.Row}�s\n"
    
    1.upto(rowNum) do |rowNo|
      1.upto(colNum) do |colNo|
        if useRange.Cells(rowNo,colNo).Value != nil
          print useRange.Cells(rowNo,colNo).Value
          print sep
        else
          if isOutSpace == TRUE
          print sep
          end
        end
      end
      print "\n"
    end
    
#    print "�s#{useRange.Columns.Count}��\n"
#    print "�s#{useRange.Rows.Count}�s\n"
    
    
  end
ensure
  xl.Workbooks.Close
  xl.Quit
end
