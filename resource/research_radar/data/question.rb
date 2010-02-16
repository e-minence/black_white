####################################################################################
#
# ��brief:  ����I�u�W�F�N�g��`
# ��file:   answer.rb
# ��author: obata
# ��date:   2010.02.12
#
####################################################################################


#===================================================================================
# ������
#===================================================================================
class Question

  def initialize
    @ID              = 0          # ����ID
    @ID_lavel        = nil        # ����ID (���x����)
    @stringJPN       = nil        # ���╶���� (����)
    @stringJPN_KANJI = nil        # ���╶���� (����)
    @answerNum       = 0          # �񓚑I�����̐�
    @bitCount        = 0          # �񓚂�\�����邽�߂ɕK�v�ȃr�b�g��
    @answers         = Array.new  # �񓚔z��
  end

  attr_reader :ID, :ID_lavel, :stringJPN, :stringJPN_KANJI, :answerNum, :bitCount, :answears

  #----------------------------------
  # ��brief: ����ID��ݒ肷��
  # ��param: id    ����ID
  # ��param: lavel ����ID (���x����)
  def SetID( id, lavel )
    @ID       = id
    @ID_lavel = lavel
  end

  #-------------------------------------------
  # ��brief: ���╶�����ݒ肷��
  # ��param: stringJPN       ���╶���� (����)
  # ��param: stringJPN_KANJI ���╶���� (����)
  def SetString( stringJPN, stringJPN_KANJI )
    @stringJPN       = stringJPN
    @stringJPN_KANJI = stringJPN_KANJI
  end

  #-----------------------------------
  # ��brief: �񓚑I�����̐���ݒ肷��
  # ��param: answerNum �I�����̐�
  def SetAnswerNum( answerNum )
    @answerNum = answerNum
  end

  #-------------------------------------------------------
  # ��brief: �񓚂�\�����邽�߂ɕK�v�ȃr�b�g����ݒ肷��
  # ��param: bitCount �r�b�g��
  def SetBitCount( bitCount )
    @bitCount = bitCount
  end

  #--------------------------
  # ��brief: �񓚂�ǉ�����
  # ��param: answer ��
  def AddAnswer( answer )
    @answers << answer
  end

  #---------------------------------------
  # ��brief: �f�o�b�O�����o�͂���
  # ��param: directory �o�͐�f�B���N�g��
  def OutputDebug( directory )
    # �o�̓f�[�^�쐬
    outData = Array.new
    outData << "ID              = #{@ID}"
    outData << "ID_lavel        = #{@ID_lavel}"
    outData << "stringJPN       = #{@stringJPN}"
    outData << "stringJPN_KANJI = #{@stringJPN_KANJI}"
    outData << "answerNum       = #{@answerNum}"
    outData << "bitCount        = #{@bitCount}"
    @answers.each do |answer|
      outData << "A.#{answer.stringJPN} (#{answer.stringJPN_KANJI})"
    end
    # �o��
    fileName = directory + "/question_#{@ID}.txt"
    file = File.open( fileName, "w" )
    file.puts( outData )
    file.close
  end

end
