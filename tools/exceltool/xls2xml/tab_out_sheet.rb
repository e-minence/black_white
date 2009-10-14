############################################
#
#	Excel����^�u��؂�e�L�X�g����
#
#	2009.10.14	k.ohno
#  �݊����������Ă���]�T���Ȃ��쐬 �V�[�g��ԍ��w��ŏo�͂ł���
#	2009.02.27	tamada
#	��삳�񂪎g���Ă������C�u�����𗬗p�B
#
#
#	���̃t�@�C���Ɠ���f�B���N�g���Ƀ��C�u����CXls2Xml.rb��
#	���ꂪ���p���Ă���JAVA�A�[�J�C�uxls2xml.jar���K�v�ł��B
#
#	�g�����F
#	ruby tab_out.rb	(�I�v�V�����j�@�t�@�C����
#
#	�I�v�V�����F
#	-t
#	-tab
#		�^�u��؂�i�f�t�H���g�̓���j
#	-c
#	--comma
#		�J���}��؂�
#
#   -n x
#       ���ԍ��̃V�[�g���o��(0����)
#
############################################

#���̃t�@�C�����u����Ă���ꏊ�Ƀ��C�u����������̂Ńp�X������
lib_path = File.dirname(__FILE__).gsub(/\\/,"/") + "/CXls2Xml"
require lib_path
#require 'CXls2Xml'

#�G���[���`
class CommnandLineArgumentError < Exception; end

class XlsTextReader < CXls2Xml
	def initialize
		#@xmldata=""
		@sheets ={}
    @sep = '\t'
    @sheetno = 0
	end
	def getXmlSheet(xlsName, sheetno)
		@xmldata = xls2xml(xlsName)
    @sheets = xml2sheet(@xmldata)
    @sheetno = sheetno
	end

	def setSeparater(sep)
		@sep = sep
	end

	def putpagetitle
		xml2pagetitle(@xmldata)
		#@xmldata
	end
	def output(sheet_number)
		sheet = @sheets[sheet_number]
		return output_sheet(sheet)
	end

	def output_sheet(sheet)
		contents = ""
		sheet.each{|row|
			row.each{|col|
				contents += sprintf("%s%s",col, @sep)
			}
			contents += sprintf("\n")
		}
		return contents
	end

	def output
    contents = ""
    loop = 0
    @sheets.each{|sheet|
      if @sheetno == -1
        contents += output_sheet(sheet)
      elsif @sheetno == loop
        contents += output_sheet(sheet)
      end
      loop=loop+1
		}
		#�Ȃ����ŏ��ɋ�s�������Ă��܂��̂őΏ��B
		#�{���I�ɂ͂����ƃf�o�b�O����
		return contents.sub(/^\n/,"")
	end
end

page = nil

sep=' '  ##�f�t�H���g
sheetno = -1
filename = ""
noget = false 

#�I�v�V�����̉��ߏ���
ARGV.each{ |command|
  if noget
    sheetno = command.to_i
    noget = false
  else
    case command
    when "-tab"		then sep = '	'
    when "-t"		then sep = '	'
    when "-comma"	then sep = ','
    when "-c"		then sep = ','
    when "-n" then noget = true
    else
      filename = command
      ##when /\-.+/		then raise CommnandLineArgumentError, "�����ȃI�v�V�����ł�"
    end
  end
}

#raise CommnandLineArgumentError, "�t�@�C�������w�肵�Ă�������" if ARGV[0] == nil
#raise FileNotFoundException, "�t�@�C��#{ARGV[0]}��������܂���" unless FileTest.exists?(ARGV[0])

conv = XlsTextReader.new
conv.getXmlSheet(filename, sheetno)
conv.setSeparater(sep)
puts "#{conv.output}"
#puts "#{conv.putpagetitle}"

