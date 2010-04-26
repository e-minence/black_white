#################################################################################
#
# @brief  .mev�t�@�C����̓N���X
#
# @file   mev_analyzer.rb
# @author obata
# @date   2010.04.24
#
################################################################################# 
# -*- coding: utf-8 -*-
require "mev_parser.rb"
require "obj_event_section_parser.rb"
require "obj_event_parser.rb"


RESULT_FILENAME = "event_check_result.txt"


# �����g���[�i�[�C�x���g�̃��X�g
$event_type_trainer_list = 
[
"EV_TYPE_TR",
"EV_TYPE_TRAINER",
"EV_TYPE_TR_EYEALL",
"EV_TYPE_TRAINER_EYEALL",
"EV_TYPE_TR_KYORO",
"EV_TYPE_TRAINER_KYORO",
"EV_TYPE_TR_SPINS_L",
"EV_TYPE_TR_SPINS_R",
"EV_TYPE_TR_SPINM_L",
"EV_TYPE_TR_SPINM_R",
"EV_TYPE_TRAINER_SPIN_STOP_L",
"EV_TYPE_TRAINER_SPIN_STOP_R",
"EV_TYPE_TRAINER_SPIN_MOVE_L",
"EV_TYPE_TRAINER_SPIN_MOVE_R",
"EV_TYPE_TR_D_ALT",
"EV_TYPE_TRAINER_DASH_ALTER",
"EV_TYPE_TR_D_REA",
"EV_TYPE_TRAINER_DASH_REACT",
"EV_TYPE_TR_D_ACC",
"EV_TYPE_TRAINER_DASH_ACCEL",
"EV_TYPE_TR_D_KYORO",
"EV_TYPE_TRAINER_DASH_ACCEL_KYORO",
"EV_TYPE_TR_D_SPINS_L",
"EV_TYPE_TR_D_SPINS_R",
"EV_TYPE_TR_D_SPINM_L",
"EV_TYPE_TR_D_SPINM_R",
"EV_TYPE_TRAINER_DASH_ACCEL_SPIN_STOP_L",
"EV_TYPE_TRAINER_DASH_ACCEL_SPIN_STOP_R",
"EV_TYPE_TRAINER_DASH_ACCEL_SPIN_MOVE_L",
"EV_TYPE_TRAINER_DASH_ACCEL_SPIN_MOVE_R",
]

class MEVAnalyzer

  def initialize
    @out_data = Array.new # �o�̓f�[�^
    @analyze_flag_trainer = false # �����g���[�i�[��̓t���O
  end

  # �A�N�Z�b�T
  attr_accessor :analyze_flag_trainer

  private
  #--------------------------------------------------
  # OBJ�C�x���g�������g���[�i�[���ǂ������`�F�b�N����
  #--------------------------------------------------
  def check_obj_event_is_trainer( event )
    $event_type_trainer_list.include?( event.EVENT_TYPE_Number )
  end

  private
  #------------------------------
  # ��͌��ʂ��t�@�C���ɏo�͂���
  #------------------------------
  def output_result( filename )
    file = File.open( filename, "w" )
    file.puts( @out_data )
    file.close
  end

  private
  #---------------------------------
  # �o�̓f�[�^�Ƀt�@�C������ǉ�����
  #---------------------------------
  def output_filename( mev )
    @out_data << "------------------------\r\n"
    @out_data << File.basename( mev.filename ) + "\r\n"
    @out_data << "------------------------\r\n"
  end

  private
  #--------------------------------------------------
  # �o�̓f�[�^�Ɏ����g���[�i�[�C�x���g����ǉ�����
  #--------------------------------------------------
  def output_trainer( obj_section )
    obj_section.event.each do |event|
      if check_obj_event_is_trainer( event ) then
        puts event.event_number
        puts event.ID_name
        data = event.OBJ_CODE_Number
        data += ", "
        data += "ID = #{event.ID_name}"
        data += "\r\n"
        @out_data << data
      end
    end
  end

  public
  #----------
  # ��͂���
  #----------
  def analyze( mev_files )
    print( "< Analyze Start!! > --> " )

    # ���ׂĂ�.mev�t�@�C������͂���
    mev_files.each do |mev|

      print( "#{File.basename( mev.filename )} --> " )
      STDOUT.flush

      output_filename( mev ) # �t�@�C����
      if @analyze_flag_trainer then output_trainer( mev.obj_section ) end # �����g���[�i�[

      puts( "OK!" )
      STDOUT.flush

    end

    puts( "< Analyze Finish!! >" )

    # ��͌��ʂ��o��
    output_result( RESULT_FILENAME )
  end

end
