#! ruby -Ks

  require 'kconv'

#=====================================================================================
#
# gmm�����X�N���v�g
#
# ex)
# gmm = GMM::new
# gmm.open_gmm( "template.gmm", "monsname.gmm" )
# gmm.make_row_index( "MONSNAME_", 1, "�t�V�M�_�l" )
# gmm.close_gmm
#
#=====================================================================================

class GMM
  #===========================================================================
  # ������
  #===========================================================================
  def initialize
    @fp_gmm_r
    @fp_gmm_w
    @file_open_flag = 0
  end

  #===========================================================================
  #
  # �t�@�C���̃I�[�v��
  #
  # @param[in]  read_file   gmm�̃w�b�_��t�b�^�𐶐����錳�ɂȂ�gmm�t�@�C��
  #                         gmm�t�@�C���ł��邩�m�F�͂��Ă܂���
  # @param[in]  write_file  �����o��gmm�t�@�C��
  #
  #
  #===========================================================================
  def open_gmm( read_file, write_file )
    if @file_open_flag == 1
      printf("FileOpenAlready!\n")
      exit( 1 )
    end
    begin
      @fp_gmm_r = open( read_file )
    rescue
      printf( "FileOpenError:%s\n",read_file )
      exit( 1 )
    end
    @fp_gmm_w = open( write_file, "w" )
    @file_open_flag = 1
    make_header
  end

  #===========================================================================
  #
  # gmm�̃w�b�_���쐬
  #
  #===========================================================================
  def make_header
    @fp_gmm_r.rewind
    while str = @fp_gmm_r.gets
      @fp_gmm_w.print str
      if str.index("</lock>") != nil
        @fp_gmm_w.print "\r\n"
        @fp_gmm_w.print "\r\n"
        break
      end
    end
  end

  #===========================================================================
  #
  # row�f�[�^���쐬
  #
  # @param[in]  label ���x����
  # @param[in]  str   row�f�[�^�ƂȂ镶����
  #
  #===========================================================================
  def make_row( label, str )
    @fp_gmm_w.printf( "\t<row id=\"%s\">\r\n", label )
    @fp_gmm_w.printf( "\t\t<language name=\"JPN\">%s</language>\r\n", str.toutf8 )
    @fp_gmm_w.print "\t</row>\r\n\r\n"
  end

  #===========================================================================
  #
  # row�f�[�^���쐬
  # �|�P�������Ƃ��g���[�i�[���Ƃ�ID�ɂ��A�Ԃ̐���
  #
  # @param[in]  label ID�̌��ɂȂ郉�x����
  # @param[in]  cnt   label�ɂ�������A��
  # @param[in]  str   row�f�[�^�ƂȂ镶����
  #
  # ex)����Ȍ`�ɂȂ邱�Ƃ�z��
  #   �|�P�������Ȃ�
  #   mons_no = 1
  #   row_make( "MONSNAME_", mons_no, "�t�V�M�_�l" )
  #
  #                   ��
  #
  #   <row id="MONSNAME_001">
  #     <language name="JPN">�t�V�M�_�l</language>
  #   </row>
  #
  #===========================================================================
  def make_row_index( label, cnt, str )
    label = sprintf( "%s%03d", label, cnt )
    make_row( label, str )
  end

  #===========================================================================
  #
  # gmm�̃t�b�^���쐬
  #
  #===========================================================================
  def make_footer
    find = 0
    @fp_gmm_r.rewind
    while str = @fp_gmm_r.gets
      if str.index("<flowchart-group-list>") != nil
        find = 1
      end
      if find == 1
        @fp_gmm_w.print str
      end
    end
  end

  #===========================================================================
  #
  # �t�@�C���N���[�Y
  #
  #===========================================================================
  def close_gmm
    make_footer
    @fp_gmm_r.close
    @fp_gmm_w.close
    @file_open_flag = 0
  end
end
