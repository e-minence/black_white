####################################################################################
#
# ��brief:  �񓚃I�u�W�F�N�g��`
# ��file:   answer.rb
# ��author: obata
# ��date:   2010.02.12
#
####################################################################################


#===================================================================================
# ����
#===================================================================================
class Answer

  def initialize
    @ID              = 0    # ��ID
    @ID_lavel        = 0    # ��ID (���x����)
    @stringID_lavel  = nil  # gmm�����x����
    @stringJPN       = nil  # �񓚕����� (����)
    @stringJPN_KANJI = nil  # �񓚕����� (����)
    @color_lavel     = nil  # �\���F (���x�� 0x000000-0xffffff)
    @colorR          = 0    # �\���F (�� 0-31)
    @colorG          = 0    # �\���F (�� 0-31)
    @colorB          = 0    # �\���F (�� 0-31)
  end

  attr_reader :ID, :ID_lavel, :stringID_lavel, :stringJPN, :stringJPN_KANJI, 
              :color_lavel, :colorR, :colorG, :colorB

  #----------------------------
  # ��brief: ��ID��ݒ肷��
  # ��param: id    ��ID
  # ��param: lavel ��ID���x����
  def SetID( id, lavel )
    @ID = id
    @ID_lavel = lavel
  end

  #----------------------------------
  # ��brief: gmm�����x������ݒ肷��
  # ��param: lavel
  def SetStringIDLavel( lavel )
    @stringID_lavel = lavel
  end

  #--------------------------------------------
  # ��brief: �񓚕������ݒ肷��
  # ��param: stringJPN       �񓚕����� (����)
  # ��param: stringJPN_KANJI �񓚕����� (����)
  def SetString( stringJPN, stringJPN_KANJI )
    @stringJPN       = stringJPN 
    @stringJPN_KANJI = stringJPN_KANJI 
  end

  #---------------------------------------------------
  # ��brief: �\���F��ݒ肷��
  # ��param: color_lavel �\���F(���x�� 000000-ffffff)
  def SetColor( color_lavel )
    red   = ( color_lavel.hex & 0xff0000 ) >> 16
    green = ( color_lavel.hex & 0x00ff00 ) >> 8
    blue  = ( color_lavel.hex & 0x0000ff )
    red   = 0x1f * red   / 0xff
    green = 0x1f * green / 0xff
    blue  = 0x1f * blue  / 0xff
    @colorR = red
    @colorG = green
    @colorB = blue
    @color_lavel = color_lavel
  end

  #-----------------------------------------
  # ��brief: �f�o�b�O�����o�͂���
  # ��param: directory �o�͐�f�B���N�g����
  def OutputDebug( directory )
    # �o�̓f�[�^�쐬
    outData = Array.new
    outData << "ID              = #{@ID}"
    outData << "ID_lavel        = #{@ID_lavel}"
    outData << "stringID_lavel  = #{@stringID_lavel}"
    outData << "stringJPN       = #{@stringJPN}"
    outData << "stringJPN_KANJI = #{@stringJPN_KANJI}"
    outData << "color           = #{@color_lavel}(%d %d %d)" % [@colorR, @colorG, @colorB]
    # �o��
    fileName = directory + "/answer_#{@ID}.txt"
    file = File.open( fileName, "w" )
    file.puts( outData )
    file.close
  end

end
