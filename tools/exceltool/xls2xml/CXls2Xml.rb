# CXls2Xml.rb
# Excel(Open Office)�ŕҏW���ꂽ.xls�t�@�C�����Axml�`���ɕϊ����邽�߂̃N���X
# .xls����XML�ւ̕ϊ��́Apoi���C�u�������g�p����java�ōs���Ă���
# UTF-8����ShiftJIS�ւ̕ϊ��́ANKF���g�p���Ă���
# 

COLUMM_A = 0
COLUMM_B = 1
COLUMM_C = 2
COLUMM_D = 3
COLUMM_E = 4
COLUMM_F = 5
COLUMM_G = 6
COLUMM_H = 7
COLUMM_I = 8
COLUMM_J = 9
COLUMM_K = 10
COLUMM_L = 11
COLUMM_M = 12
COLUMM_N = 13
COLUMM_O = 14
COLUMM_P = 15
COLUMM_Q = 16
COLUMM_R = 17
COLUMM_S = 18
COLUMM_T = 19
COLUMM_U = 20
COLUMM_V = 21
COLUMM_W = 22
COLUMM_X = 23
COLUMM_Y = 24
COLUMM_Z = 25





### �萔��`
#
CMD_PATH = File.dirname($0)


# .xls �� .xml �ɕϊ����邽�߂̃R�}���h��
#CMD_XLS2XML = "java -Xmx512M -jar #{CMD_PATH}/xls2xml.jar -s -windows "
#CMD_XLS2XML = "java -Xmx512M -jar #{CMD_PATH}/poidom.jar -s -windows "
CMD_XLS2XML = "java -classpath xls2xml.jar XLS2XML "
CMD_2SJIS = ""
#CMD_2SJIS = " | #{CMD_PATH}/nkf.exe -SsLw"

# �o�͂�}������I�v�V����������
OPTION_NO_OUT = "NO_OUT"



### �������`
# ����xls�t�@�C����������Ȃ��G���[
STR_FILE_NOT_FOUND = "File not Found! (�t�@�C����������܂���) :"
# xlHtml�ł̃G���[
STR_BROKEN_XLS_FILE = "Broken .xls File! (xls�t�@�C�������Ă��܂�)"
# ���̕s���G���[
STR_UNKNOWN_ERROR = "Unknown Error! (�����炭�R���o�[�^�Ƀo�O������܂��I)"


### ��O�N���X
# �����G���[
class ArgErrorException < Exception; end
# xls�t�@�C����������Ȃ��G���[
class FileNotFoundException < Exception; end
# xls�t�@�C�������Ă������̃G���[
class BrokenFileException < Exception; end
# �Z���ɗ\��O�̒l�������Ă������̃G���[
class IllgalCellException < Exception; end
# ��̓G���[
class AnalyzeException <  Exception; end

### .xls �� XML�ϊ��N���X
class CXls2Xml
  ### �����ݒ�
  ### @param  �Ȃ�
  ### @return �s��
  def initialize
    @numError = 0
  end

  ### .xls��XML�ɕϊ�(�o�b�t�@�ɏo��)
  ### @param  .xls�t�@�C����
  ### @return XML�e�L�X�g�z��
  def xls2xml(xlsFileName)
    begin
      raise FileNotFoundException, xlsFileName if ! FileTest::exist? xlsFileName
      out = open("|" + CMD_XLS2XML + xlsFileName)
      lines = out.readlines
      out.close
      return lines
    rescue
      raise BrokenFileException
    end
  end

  ### xml������f�[�^����V�[�g�f�[�^�ɕϊ�
  ### @return sheets[�V�[�g�ԍ�][row][col]�ŃA�N�Z�X�ł���z��
  def xml2sheet(xml)
    sheets = []
    iSheet = 0
    mode = 0
    lastRow = 0
    xml.each do |line|
      case mode
      when 0
	### �V�[�g��T��
	if line.include?("<sheet>")
	  sheets[iSheet] = []
	  mode = 1
	  lastRow = 0
	end
      when 1
	### row,col�f�[�^�𓾂�
	if line.include?("</sheet>")
	  mode = 0
          iSheet += 1
          p iSheet
	end
	row, col = getRowCol(line)
	if (row != -1) and (col != -1)
	  while (lastRow <= row)
	    if (sheets[iSheet][lastRow] == nil)
	      sheets[iSheet][lastRow] = []
	      lastRow += 1
	    end
	  end
	  sheets[iSheet][row][col] = getValue(line)
	  next
	end
      else
	raise "�R���o�[�^�Ƀo�O������ł��܂��I"
      end
    end
    return sheets
  end
  
  
  def xml2pagetitle(xml)
    tarray = []
    xml.each do |line|
      if line.include?("<pagetitle>")
        tarray.push(getValue(line))
      end
    end
    return tarray
  end
  
  
  ### �w��n���̃f�[�^�𒊏o����
  ### @param  ���o����f�[�^�̌n����\��������
  ### @param  ���o����f�[�^�̌n����\�������񂪏������܂�Ă���.xls�̗�(COL)
  ### @param  XML�e�L�X�g�z��
  ### @param  �n���ʂ̃f�[�^�𒊏o����T�u���[�`��
  ### @return ���o�����f�[�^

  def getTypeData(type, typecol, xml, subFunc)
    begin
      sheets = xml2sheet(xml)
      #p sheets
      outArray = []        # �ŏI�I�ɕԂ��z��(�z��̔z��ɂȂ�)
      objArray = []        # 1�A�C�e���̏����i�[����z��
      mode = 0
      typeRow = -1
      subFunc += "(type, value, iRow, iCol, typeRow, objArray)"

      typeSheet = nil
      sheets.each do |sheet|
	iRow = 0
	sheet.each do |row|
	  #p row[typecol]
	  if row[typecol] == type
	    # �n������
	    #puts "====�n�������I===="
	    typeRow = iRow
	    typeSheet = sheet
	    break
	  end
	  iRow += 1
	end
	if typeSheet != nil
	  break
	end
      end

      if typeSheet == nil
	# �G���[
	printError("Type not Found! (�w��^�C�v������܂���) #{type}")
	@numError += 1
      end

      if typeSheet != nil
	#puts "typeRow:#{typeRow}"
	#puts "typeSheet.size:#{typeSheet.size}"
	for iRow in typeRow ... typeSheet.size
	  row = typeSheet[iRow]
	  #puts "==row=="
	  #p row
	  if row.size == 0
	    break
	  end
	  iCol = 0
	  row.each do |cell|
	    value  = row[iCol]
	    break if eval subFunc
	    iCol += 1
	  end
	  outArray.push(objArray.clone)
	end
      end
    end
    #puts "outArray"
    #p outArray
    return outArray
  end

				     


  ### ���s������ϊ�����
  ### @param  �s������
  ### @return �s��
  def convLineFeed(line)
    if !(line.gsub!(/\r\n/, "\r\n"))
      if !(line.gsub!(/\n/, "\r\n"))
	if line.gsub!(/\r/, "\r\n")
	end
      end
    end
  end

  ### XML������̈ʒu(Row, Col)��Ԃ�
  ### @param  ���ׂ镶����
  ### @return Row��Col��2�v�f�̔z��(Row, Col�������ꍇ��-1)
  def getRowCol(str)
    row = -1
    col = -1
    if str =~ /row\s*=\s*\"(\d+)\"\s+col\s*=\s*\"(\d+)\"/i	
      row = $1.to_i + 1
      col = $2.to_i
    end
    rowcol = [row, col]
  end

  ### ���l�ȊO�̕����������Ă��Ȃ������ׂ�
  ### @param  ���ׂ镶����
  ### @param  ���̃Z����row
  ### @param  ���̃Z����col
  ### @return TRUE ���l�������Ă���
  def checkInteger(value, row, col)
    if checkNil(value, row, col)
      return FALSE
    elsif value =~ /[^0-9\+\-\.]/
      errorStr = "Only Number! (���l�����󂯕t���܂���) #{value}"
      printAnalyzeError(errorStr, row, col)
      return FALSE
    elsif value.count(".") > 1
      errorStr = "Invalid Number! (���l�\��������������܂���) #{value}"
      printAnalyzeError(errorStr, row, col)
      return FALSE
    end
    return TRUE
  end

  ### ���l�ȊO�̕����������Ă��Ȃ������ׂ�(�͈̓`�F�b�N����)
  ### @param  ���ׂ镶����
  ### @param  �ŏ��l
  ### @param  �ő�l
  ### @param  ���̃Z����row
  ### @param  ���̃Z����col
  ### @return TRUE ���l�������Ă���
  def checkIntegerLimit(value, min, max, row, col)
    if checkInteger(value, row, col)
      i = value.to_i
      if i<min || i>max
	printAnalyzeError("#{min}����#{max}�̒l�łȂ��Ă͂Ȃ�܂���", row, col)
      end
    end
  end

  ### ���l�ȊO�̕����������Ă��Ȃ������ׂ�(�͈̓`�F�b�N����A����������)
  ### @param  ���ׂ镶����
  ### @param  �ŏ��l
  ### @param  �ő�l
  ### @param  ���̃Z����row
  ### @param  ���̃Z����col
  ### @return TRUE ���l�������Ă���
  def checkFloatLimit(value, min, max, row, col)
    if checkInteger(value, row, col)
      i = value.to_f
      if i<min || i>max
	printAnalyzeError("#{min}����#{max}�̒l�łȂ��Ă͂Ȃ�܂���", row, col)
      end
    end
  end

  ### C++�̃��x���Ɏg�p�ł��镶���ȊO�������Ă��Ȃ������ׂ�
  ### @param  ���ׂ镶����
  ### @param  ���̃Z����row
  ### @param  ���̃Z����col
  ### @return �s��
  def checkCppLabel(value, row, col)
    if checkNil(value, row, col)
    elsif value =~ /^[0-9]/
      # �G���[
      errorStr = "Illegal Label! (���x�����̐擪�ɐ����͎g���܂���) #{value}"
      printAnalyzeError(errorStr, row, col)
    elsif value =~ /[^A-Za-z0-9\_]/
      # �G���[
      errorStr = "Illegal Label! (���x�����ɂ�[A-Z0-9_]�����g���܂���) #{value}"
      printAnalyzeError(errorStr, row, col)
    end
  end

  ### ���x�� = �l �Ƃ����`���ɂȂ��Ă��邩���ׂ�
  ### @param  ���ׂ镶����
  ### @param  ���̃Z����row
  ### @param  ���̃Z����col
  ### @return �s��
  def checkLabelValue(value, row, col)
    if checkNil(value, row, col)
    elsif value =~ /^[0-9]/
      # �G���[
      errorStr = "Illegal Label! (���x�����̐擪�ɐ����͎g���܂���) #{value}"
      printAnalyzeError(errorStr,row,col)
    elsif ! (value =~ /[A-Z0-9\_]+\s*=\s*\S+/)
      # �G���[
      errorStr = "Illegal Label! (���x��=�l �Ƃ��������ɂȂ��Ă��܂���) #{value}"
      printAnalyzeError(errorStr, row, col)
    end
  end

  ### �o��Q�[���^�C�v�ɂӂ��킵���Ȃ������񂪓����Ă��Ȃ������ׂ�
  ### @param  ���ׂ镶����
  ### @param  ���̃Z����row
  ### @param  ���̃Z����col
  ### @return �s��
  def checkGameTypeLabel(value, row, col)
    if checkNil(value, row, col)
    elsif value != "ONLINE" and value != "FIRST" and value != "ROOM"
      # �G���[
      errorStr = "Illegal Game Type! (ONLINE, FIRST, ROOM�ȊO�g���܂���) #{value}"
      printAnalyzeError(errorStr, row, col)
    end
  end


  ### nil����Ȃ������ׂ�
  ### @param  ���ׂ镶����
  ### @param  ���̃Z����row
  ### @param  ���̃Z����col
  ### @return nil�Ȃ�TRUE
  def checkNil(value, row, col)
    if value.nil?
      # �G���[
      errorStr = "No value! (�Z�����󔒂ł�) "
      printAnalyzeError(errorStr, row, col)
      return TRUE
    end
    return FALSE
  end

  ### �Z���̒l��Ԃ�
  ### @param  �Z���̒l�������Ă���xml������
  ### @return �Z���̒l
  def getValue(str)
    str =~ /\>\s*(.+)\s*\<\//i
    value = $1
    # �^�O�͑S���폜(���ۗ]�v�Ȉʒu��<FONT���͂����Ă������Ƃ�������)
    value.gsub!(/<[^>]+>/, "") if (! value.nil?)
    return value
  end


  ### .xls�̉�͌��ʂ�\������
  ### @param  �Ȃ�
  ### @return �G���[��
  def printAnalyzeResult
    str = ""
    if @numError == 0
      str = "#{File.basename($0)} : Converting Finished. (����ɃR���o�[�g�ł��܂���)"
    else
      str = "#{File.basename($0)} : Number of Errors #{@numError} (�G���[��#{@numError}����܂���)"
    end
    puts "+" + ("-" * (str.size+2)) + "+"
    puts "| " + str + " |"
    puts "+" + ("-" * (str.size+2)) + "+"
    raise AnalyzeException if @numError != 0
    puts ""
  end

  ### .xls�̉�̓G���[��\�����A�G���[�J�E���^��i�߂�
  ### @param  �G���[������
  ### @param  ���̃Z����row
  ### @param  ���̃Z����col
  ### @return �G���[��
  def printAnalyzeError(errorStr, row, col)
    colStr = ""
    loop {
      colStr = "A" + colStr
      colStr[0] += (col % 26)
      col = (col / 26).floor - 1     # Float�ŗ����Ƃ��̂��߂ɐ؂�̂ď��������
      if (col < 0) 
	break
      end
    }
    # colStr = "A"
    # colStr[0] += col
    str = "CELL:(" + colStr + ":" + row.to_s + ") " + errorStr
    printError(str)
    @numError += 1
  end

  ### �G���[�������W���o�͂���
  ### @param  �G���[������
  ### @return �s��
  def printError(str)
    print "Error: " + str + "\n"
  end
end


