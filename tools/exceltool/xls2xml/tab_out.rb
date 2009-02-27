############################################
#
#	Excel����^�u��؂�e�L�X�g����
#
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
	end
	def getXmlSheet(xlsName)
		@xmldata = xls2xml(xlsName)
		@sheets = xml2sheet(@xmldata)
	end

	def output(sep)
		contents = ""
		@sheets.each{|sheet|
			sheet.each{|row|
				row.each{|col|
					contents += sprintf("%s%s",col, sep)
				}
				contents += sprintf("\n")
			}
		}
		#�Ȃ����ŏ��ɋ�s�������Ă��܂��̂őΏ��B
		#�{���I�ɂ͂����ƃf�o�b�O����
		return contents.sub(/^\n/,"")
	end
end

#�I�v�V�����̉��ߏ���
case ARGV[0]
when "-tab"		then sep = '	'
when "-t"		then sep = '	'
when "-comma"	then sep = ','
when "-c"		then sep = ','
when /\-.+/		then raise CommnandLineArgumentError, "�����ȃI�v�V�����ł�"
end

#�I�v�V�����������Ƃ��̃f�t�H���g�ݒ�
if sep != nil then
	ARGV.shift
else
	sep = '	'
end

raise CommnandLineArgumentError, "�t�@�C�������w�肵�Ă�������" if ARGV[0] == nil
raise FileNotFoundException, "�t�@�C��#{ARGV[0]}��������܂���" unless FileTest.exists?(ARGV[0])

conv = XlsTextReader.new
conv.getXmlSheet(ARGV[0])
puts "#{conv.output(sep)}"

