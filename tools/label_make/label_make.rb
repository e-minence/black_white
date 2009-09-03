
#=====================================================================================
#
# ���[�}���ϊ����x�������X�N���v�g
#
#=====================================================================================

  $KCODE = "Shift-JIS"

class LabelMake

  DIC_SRC = 0
  DIC_DST = 1
  U_FLAG = 1
  F_FLAG = 2
  J_FLAG = 3

  def initialize
    @dic = []
    cnt = 0
    open( File.dirname(__FILE__) + "/label.dic" ) {|fp_r|
      while str = fp_r.gets
        @dic[ cnt ] = str
        cnt += 1
      end
    }
  end

  #===========================================================================
  # �^����ꂽ�����񂩂烉�x���𐶐�
  # @param[in]  head_str  ���x���̓��ɂ��镶����i�|�P�������Ȃ�MONSNO_�Ƃ��j
  # @param[in]  str       ���x���ϊ����镶����
  #
  # @retval ���x���ϊ�����������
  #===========================================================================
  def make_label( head_str, str )
    label_str = head_str
    cnt = 0
    bou_str = ""
    mini_flag = 0
    mini_flag_tmp = 0
    tu_flag = 0

    while str[ cnt ] != nil
      moji = str[ cnt + 0 .. cnt + 1 ]
      
      @dic.size.times {|dic_cnt|
        split_dic = @dic[ dic_cnt ].split(/\s+/)
        if moji == split_dic[ DIC_SRC ]
          case split_dic[ DIC_DST ][ 0 ].chr
          when "@"
            mini_flag = ( split_dic[ DIC_DST ][ 1 ].chr ).to_i
            label_str << split_dic[ DIC_DST ][ 2 .. split_dic[ DIC_DST ].size - 1 ]
          when "-"
            label_str << bou_str
          when "/"
            tu_flag = 1
          when ":"
            if split_dic[ DIC_DST ][ 1 ].chr == "H"
              p "H"
              p mini_flag
              case mini_flag
              when U_FLAG
                label_str[ label_str.size - 1 ] = "W"
                label_str << split_dic[ DIC_DST ][ 1 .. split_dic[ DIC_DST ].size - 1 ]
              when F_FLAG
                label_str[ label_str.size - 2 ] = "F"
                label_str[ label_str.size - 1 ] = split_dic[ DIC_DST ][ 2 .. split_dic[ DIC_DST ].size - 1 ]
              when J_FLAG
                label_str[ label_str.size - 2 ] = "J"
                label_str[ label_str.size - 1 ] = split_dic[ DIC_DST ][ 2 .. split_dic[ DIC_DST ].size - 1 ]
              else
                label_str[ label_str.size - 1 ] = split_dic[ DIC_DST ][ 1 .. split_dic[ DIC_DST ].size - 1 ]
              end
            else
              label_str[ label_str.size - 1 ] = split_dic[ DIC_DST ][ 1 .. split_dic[ DIC_DST ].size - 1 ]
            end
          else
            if tu_flag == 0
              label_str << split_dic[ DIC_DST ]
            else
              tu_flag = 0
              label_str << split_dic[ DIC_DST ][ 0 ].chr
              label_str << split_dic[ DIC_DST ]
            end
          end
          bou_str = split_dic[ DIC_DST ][ split_dic[ DIC_DST ].size - 1].chr
          if mini_flag_tmp == 0
            mini_flag_tmp = mini_flag
          else
            mini_flag = 0
            mini_flag_tmp = 0
          end
        end
      }
      cnt += 2
    end

    return label_str
  end

end

#�g�����T���v��
# require 'label_make'    #�p�X�w��͔C�ӂł����Ȃ��Ă�������
# include LabelMake
#
# label = LabelMake.new
# p label.make_label( "MONSNO_", "�R���b�^" )

