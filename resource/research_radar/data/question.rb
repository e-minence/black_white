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
    @ID                     = 0          # ����ID
    @ID_lavel               = nil        # ����ID (���x����)
    @titleStringID_lavel    = nil        # gmm�����x���� (���╶����)
    @titleStringJPN         = nil        # ���╶���� (����)
    @titleStringJPN_KANJI   = nil        # ���╶���� (����)
    @captionStringID_lavel  = nil        # gmm�����x���� (�⑫������)
    @captionStringJPN       = nil        # �⑫������ (����)
    @captionStringJPN_KANJI = nil        # �⑫������ (����)
    @answerNum              = 0          # �񓚑I�����̐�
    @bitCount               = 0          # �񓚂�\�����邽�߂ɕK�v�ȃr�b�g��
    @answers                = Array.new  # �񓚔z��
  end

  attr_reader :ID, :ID_lavel, 
              :titleStringID_lavel, :titleStringJPN, :titleStringJPN_KANJI, 
              :captionStringID_lavel, :captionStringJPN, :captionStringJPN_KANJI, 
              :answerNum, :bitCount, :answers

  #----------------------------------
  # ��brief: ����ID��ݒ肷��
  # ��param: id    ����ID
  # ��param: lavel ����ID (���x����)
  def SetID( id, lavel )
    @ID       = id
    @ID_lavel = lavel
  end

  #----------------------------------
  # ��brief: gmm�����x������ݒ肷��
  # ��param: title
  # ��param: caption
  def SetStringIDLavel( title, caption )
    @titleStringID_lavel = title
    @captionStringID_lavel = caption
  end

  #-------------------------------------------------
  # ��brief: ���╶�����ݒ肷��
  # ��param: titleStringJPN       ���╶���� (����)
  # ��param: titleStringJPN_KANJI ���╶���� (����)
  def SetTitleString( titleStringJPN, titleStringJPN_KANJI )
    @titleStringJPN       = titleStringJPN
    @titleStringJPN_KANJI = titleStringJPN_KANJI
  end

  #-------------------------------------------------
  # ��brief: �⑫�������ݒ肷��
  # ��param: captionStringJPN       �⑫������ (����)
  # ��param: captionStringJPN_KANJI �⑫������ (����)
  def SetCaptionString( captionStringJPN, captionStringJPN_KANJI )
    @captionStringJPN       = captionStringJPN
    @captionStringJPN_KANJI = captionStringJPN_KANJI
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
    if answer.ID != 0 then @answers << answer end # ���񓚂͕ێ����Ȃ�
  end

  #---------------------------------------
  # ��brief: �f�o�b�O�����o�͂���
  # ��param: directory �o�͐�f�B���N�g��
  def OutputDebug( directory )
    # �o�̓f�[�^�쐬
    outData = Array.new
    outData << "ID                     = #{@ID}"
    outData << "ID_lavel               = #{@ID_lavel}"
    outData << "titleStringID_lavel    = #{@titleStringID_lavel}"
    outData << "titleStringJPN         = #{@titleStringJPN}"
    outData << "titleStringJPN_KANJI   = #{@titleStringJPN_KANJI}"
    outData << "captionStringID_lavel  = #{@captionStringID_lavel}"
    outData << "captionStringJPN       = #{@captionStringJPN}"
    outData << "captionStringJPN_KANJI = #{@captionStringJPN_KANJI}"
    outData << "answerNum              = #{@answerNum}"
    outData << "bitCount               = #{@bitCount}"
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
