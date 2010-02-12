######################################################### 
#
# ��brief:  �G�N�Z�� �^�u��؂�R���o�[�^
# ��file:   excel_tab_out.rb
# ��author: obata
# ��date:   2010.02.12
#
######################################################### 
require "win32ole"


#--------------------------------------------------------
# ��brief:  �t�@�C���̐�΃p�X���擾����
# ��param:  filename �t�@�C����
# ��return: �w�肵���t�@�C���̐�΃p�X
#--------------------------------------------------------
def GetAbsolutePath( filename )
  fso = WIN32OLE.new('Scripting.FileSystemObject')
  return fso.GetAbsolutePathName( filename )
end


#--------------------------------------------------------
# ��brief: �G�N�Z���t�@�C�����^�u��؂�e�L�X�g�ɕϊ�����
# ��param: ARGV[0] �R���o�[�g�ΏۃG�N�Z���t�@�C����
# ��param: ARGV[1] �R���o�[�g�Ώۂ̃V�[�g�ԍ�
# ��param: ARGV[2] �R���o�[�g��̏o�̓t�@�C����
#--------------------------------------------------------
EXCEL_FILENAME  = GetAbsolutePath( ARGV[0] )
SHEET_NO        = ARGV[1].to_i
OUTPUT_FILENAME = ARGV[2]

excel = WIN32OLE.new( "Excel.Application" )
book  = excel.Workbooks.Open( EXCEL_FILENAME )
sheet = book.WorkSheets( SHEET_NO )
file  = File.open( OUTPUT_FILENAME, "w" )

begin
  sheet.UsedRange.Rows.each do |row|
    record = Array.new
    row.Columns.each do |cell|
      if cell.Value != nil then record << cell.Value end
    end
    if record.size != 0 then
      data = record.join("\t")
      file.puts( data.strip )
    end
  end
ensure
  book.Close
  excel.Quit
  file.close
end
