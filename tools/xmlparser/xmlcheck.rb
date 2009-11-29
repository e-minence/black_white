#  XML���p�[�T�[�œǂ߂邩���������܂��B
#  �ǂ߂�΁Arexml�ō\���������܂�
#   k.ohno  2009.03.27

require "rexml/document"
require "rexml/parseexception"
include REXML
require "kconv"

# XML�t�@�C��������͂���t�@���N�V����
def getXMLFileName

    fname = ""
    while fname == ""
        printf("XML�t�@�C��������͂��Ă�������: ")
        fname = $stdin.gets.strip
    end

    return fname

end

##### main�����ł�

# XML�t�@�C�������擾���܂�
xml_file_name = ARGV[0]

if xml_file_name == nil
    xml_file_name = getXMLFileName
end

printf("--- xml�t�@�C����: %s\n", xml_file_name)

# XML�t�@�C����open���܂�
doc = nil

begin



    File.open(xml_file_name) {|xmlfile|
        doc = REXML::Document.new xmlfile
    }

        printf("---�G���[�͂���܂���ł���\n")

# �p�[�X�G���[�����������Ƃ�
    rescue REXML::ParseException=>error
        printf("--- �p�[�X�G���[�ł�\n")
        printf("to_s=%s", error.to_s())

# �t�@�C���̓ǂݍ��݃G���[�Ȃǂ����������Ƃ�
    rescue Exception=>error
        printf("--- �G���[ : %s", error)

end
