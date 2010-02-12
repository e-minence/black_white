####################################################################################
#
# ��brief:  ��I�u�W�F�N�g
# ��file:   hobby.rb
# ��author: obata
# ��date:   2010.02.12
#
####################################################################################


#===================================================================================
# ���
#===================================================================================
class Hobby

  def initialize
    @ID              = 0    # ID
    @stringJPN       = nil  # ��� (����)
    @stringJPN_KANJI = nil  # ��� (����)
  end

  attr_reader :ID, :stringJPN, :stringJPN_KANJI

  #-----------------------
  # ��brief: ID��ݒ肷��
  # ��param: id ID
  def SetID( id )
    @ID = id
  end

  #-------------------------------
  # ��brief: �����ݒ肷��
  # ��param: stringJPN       ����
  # ��param: stringJPN_KANJI ����
  def SetString( stringJPN, stringJPN_KANJI )
    @stringJPN       = stringJPN
    @stringJPN_KANJI = stringJPN_KANJI
  end

  #-----------------------------------------
  # ��brief: �f�o�b�O�����o�͂���
  # ��param: directory �o�͐�̃f�B���N�g��
  def OutputDebug( directory )
    # �o�̓f�[�^�쐬
    outData = Array.new
    outData << "stringJPN       = #{@stringJPN}"
    outData << "stringJPN_KANJI = #{@stringJPN_KANJI}"
    # �o��
    fileName = directory + "/hobby_#{@ID}.txt"
    file = File.open( fileName, "w" )
    file.puts( outData )
    file.close
  end
  
end
