####################################################################################
#
# ��brief:  �^�u��؂�t�@�C�� ���[�_�[
# ��file:   tab_separated_file_rader.rb
# ��author: obata
# ��date:   2010.02.10 # #
# ���^�u�ŋ�؂�ꂽ�G�N�Z���f�[�^��ǂݍ���, �C�ӂ̃Z���̒l���擾���邽�߂̃N���X
#
####################################################################################


#===================================================================================
# ���^�u��؂�t�@�C�� ���[�_�[
#===================================================================================
class TabSeparatedFileReader

  def initialize
    @fileName = nil  # �t�@�C����
    @fileRows = nil  # �t�@�C���̊e�s
  end

  #------------------------------
  # ��brief: �t�@�C����ǂݍ���
  # ��param: fileName �t�@�C����
  def ReadFile( fileName )
    # �t�@�C�������݂��Ȃ�
    if File.exist?( fileName ) == FALSE then abort( "#{fileName} is not exist" ) end
    # �ǂݍ���
    file      = File.open( fileName, "r" )
    @fileRows = file.readlines
    @fileName = fileName
    file.close
  end

  #------------------------------------
  # ��brief:  �l���擾����
  # ��param:  rowIdx    �s�C���f�b�N�X
  # ��param:  columnIdx ��C���f�b�N�X
  # ��return: �w�肵���Z���̒l
  def GetCell( rowIdx, columnIdx )
    # �J���Ă��Ȃ�
    if @fileName == nil then abort( "file is not opened" ) end 
    # �w��ʒu�̃f�[�^��Ԃ�
    contents = @fileRows[ rowIdx ].split(/\t/)
    return contents[ columnIdx ]
  end

  #------------------------------------
  # ��brief:  �s�����擾����
  # ��return: �ǂݍ��񂾃t�@�C���̍s��
  def GetRowNum
    return @fileRows.size
  end

end 
