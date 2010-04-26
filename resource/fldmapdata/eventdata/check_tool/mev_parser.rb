#################################################################################
#
# @brief  .mev �t�@�C����̓N���X
#
# @file   obj_event_parser.rb
# @author obata
# @date   2010.04.24
#
#################################################################################
require "obj_event_section_parser.rb"

class MEVFile

  # ������
  def initialize
    @filename     = nil                 # �t�@�C����
    @obj_section  = OBJEventSection.new # OBJ�C�x���g�Z�N�V����
  end

  # �Q�b�^�[
  attr_reader :path_to_file, :filename, :obj_section

  public
  #--------------------
  # �t�@�C����ǂݍ���
  #--------------------
  def load( filename )
    @filename = filename
    @obj_section.load( filename )
  end

end
