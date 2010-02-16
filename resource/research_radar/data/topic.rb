####################################################################################
#
# ��brief:  �������ڃI�u�W�F�N�g��`
# ��file:   topic.rb
# ��author: obata
# ��date:   2010.02.12
#
####################################################################################


#===================================================================================
# ����������
#===================================================================================
class Topic
  
  def initialize
    @ID               = 0          # ����ID
    @ID_lavel         = 0          # ����ID (���x����)
    @titleJPN         = nil        # ���ږ� (����)
    @titleJPN_KANJI   = nil        # ���ږ� (����)
    @captionJPN       = nil        # �⑫�� (����)
    @captionJPN_KANJI = nil        # �⑫�� (����)
    @questions        = Array.new  # ����z��
  end

  attr_reader :ID, :ID_lavel, 
              :titleJPN, :titleJPN_KANJI, :captionJPN, :captionJPN_KANJI, 
              :questions

  #-------------------------------
  # ��brief: ����ID��ݒ肷��
  # ��param: id    ����ID
  # ��param: lavel ����ID (���x����)
  def SetID( id, lavel )
    @ID       = id
    @ID_lavel = lavel
  end

  #--------------------------------------
  # ��brief: ���ږ���ݒ肷��
  # ��param: titleJPN       ���ږ� (����)
  # ��param: titleJPN_KANJI ���ږ� (����)
  def SetTitle( titleJPN, titleJPN_KANJI )
    @titleJPN       = titleJPN
    @titleJPN_KANJI = titleJPN_KANJI
  end

  #----------------------------------------
  # ��brief: �⑫����ݒ肷��
  # ��param: captionJPN       �⑫�� (����)
  # ��param: captionJPN_KANJI �⑫�� (����)
  def SetCaption( captionJPN, captionJPN_KANJI )
    @captionJPN       = captionJPN
    @captionJPN_KANJI = captionJPN_KANJI
  end

  #-------------------------
  # ��brief: �����ǉ�����
  # ��param: question ����
  def AddQuestion( question )
    @questions << question
  end

  #---------------------------------------
  # ��brief: �f�o�b�O�����o�͂���
  # ��param: directory �o�͐�f�B���N�g��
  def OutputDebug( directory )
    # �o�̓f�[�^�쐬
    outData = Array.new
    outData << "ID               = #{@ID}"
    outData << "ID_lavel         = #{@ID_lavel}"
    outData << "titleJPN         = #{@titleJPN}"
    outData << "titleJPN_KANJI   = #{@titleJPN_KANJI}"
    outData << "captionJPN       = #{@captionJPN}"
    outData << "captionJPN_KANJI = #{@captionJPN_KANJI}"
    @questions.each do |question|
      outData << "Q.#{question.stringJPN} (#{question.stringJPN_KANJI})"
    end
    # �o��
    fileName = directory + "/topic_#{@ID}.txt"
    file = File.open( fileName, "w" )
    file.puts( outData )
    file.close
  end

end 
