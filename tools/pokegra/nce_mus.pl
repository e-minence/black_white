#====================================================================================
#
#	nceファイルから板ポリ１枚で描画するパラメータを抽出する
#	＋ミュージカル用データ抽出
#
#====================================================================================
use File::Basename;
use Class::Struct;

use constant FORMAT_ID		=>	4;
use constant BYTE_ORDER		=>	2;
use constant FORMAT_VERSION	=>	2;
use constant FILE_SIZE		=>	4;
use constant HEADER_SIZE	=>	2;
use constant DATA_BLOCKS	=>	2;

use constant NCE_HEADER_SIZE=>	FORMAT_ID + BYTE_ORDER + FORMAT_VERSION + FILE_SIZE + HEADER_SIZE + DATA_BLOCKS;

use constant BLOCK_TYPE		=>	4;
use constant BLOCK_SIZE		=>	4;
use constant CELLS			=>	4;
use constant OBJS			=>	4;

use constant CELL_HEADER_SIZE=>	BLOCK_TYPE + BLOCK_SIZE + CELLS;

use constant OAM_POS_X		=>	2;
use constant OAM_POS_Y		=>	2;
use constant OAM_RS_MODE	=>	1;
use constant OAM_RS_PARAM	=>	1;
use constant OAM_HFLIP		=>	1;
use constant OAM_VFLIP		=>	1;
use constant OAM_RSD_ENABLE	=>	1;
use constant OAM_OBJ_MODE	=>	1;
use constant OAM_MOSAIC		=>	1;
use constant OAM_COLOR_MODE	=>	1;
use constant OAM_SHAPE		=>	1;
use constant OAM_SIZE		=>	1;
use constant OAM_PRIORITY	=>	1;
use constant OAM_COLOR_PARAM=>	1;
use constant OAM_CHAR_NAME	=>	2;
use constant PADDING		=>	2;

#ミュージカル用
use constant MUSICAL_POINT_MAX		=>	12;
use constant OBJ_GROUPE_NUMBER_SIZE		=>	4;

use constant OBJ_SIZE		=>	OAM_POS_X +		OAM_POS_Y +			 OAM_RS_MODE +		OAM_RS_PARAM + 
								OAM_HFLIP +		OAM_VFLIP +			 OAM_RSD_ENABLE +	OAM_OBJ_MODE +
								OAM_MOSAIC +	OAM_COLOR_MODE +	 OAM_SHAPE +		OAM_SIZE +
								OAM_PRIORITY +	OAM_COLOR_PARAM +	OAM_CHAR_NAME +		PADDING;

use constant MCSS_SHIFT		=>	8;			#ポリゴン1辺の重み（FX32_SHIFTと同値）

struct MusicalData => {
	objNo	=> '$',
	pltNo	=> '$',
	ofsX	=> '$',
	ofsY	=> '$',
	grpNo	=> '$',
};

#====================================================================================
#
#	OBJ形状とOBJサイズから導く縦横サイズテーブル
#
#====================================================================================

	@obj_size_x = ( [ 8, 16, 32, 64],
					[16, 32, 32, 64],
					[ 8,  8, 16, 32] );

	@obj_size_y = ( [ 8, 16, 32, 64],
					[ 8,  8, 16, 32],
					[16, 32, 32, 64] );

#====================================================================================
#
#	処理開始
#
#====================================================================================

	$argc = @ARGV;

	if( $argc < 2 ){
		print "error:perl perl_file read_file write_dir\n";
		die;
	}

	#---------------------------------------------------------------------
	#ミュージカル用変数定義
	@musicalCells;
	@musicalDataIdx;
	$musicalCellIdx = 0;
	$totalObjNo = 0;
	#---------------------------------------------------------------------

	$write_file = basename( @ARGV[0], '.nce' );
	$write_file = @ARGV[1] . $write_file . '.NCEC';

	open( READ_NCE, @ARGV[0] ) or die "[@ARGV[0]]", $!;
	open( WRITE_NCE, "> $write_file") or die "[$write_file]", $!;

	binmode READ_NCE;
	binmode WRITE_NCE;

	#ヘッダーデータを読み込み
	read READ_NCE, $header, NCE_HEADER_SIZE; 

	#読み込んだデータをセパレート
	($format_id, $byte_order, $format_version, $file_size, $header_size, $data_blocks) = unpack "a4 S S L S S", $header;

	if( $format_id ne "NCOB" ){
		print "NitroCharacterのnceファイルではありません\n";
		die;
	}
	
=pod
	#データブロック解釈のテスト
	$writeFileDataTop = tell READ_NCE;
	for( $dataIdx = 0 ; $dataIdx < $data_blocks ; $dataIdx++ )
	{
		#データのヘッダーを読み込み
		read READ_NCE, $header, CELL_HEADER_SIZE; 
		#読み込んだデータをセパレート
		($block_type, $block_size, $cells) = unpack "a4 L L", $header;
		printf "[%2d]%s\n",$dataIdx,$block_type;
		seek READ_NCE , $block_size - CELL_HEADER_SIZE, 1;
	}
	seek READ_NCE , $writeFileDataTop , 0;
=cut
	#---------------------------------------------------------------------
=pod
	#↓必要なかった
	#後にファイルのオフセットを得るため、総OBJ数を計算しておく
	$writeFileDataTop = tell READ_NCE;
	
	#セルデータのヘッダーを読み込み・セパレート
	read READ_NCE, $header, CELL_HEADER_SIZE; 
	($block_type, $block_size, $cells) = unpack "a4 L L", $header;
	for( $cell = 0 ; $cell < $cells ; $cell++ )
	{
		#セルを構成するOBJの数を読み込み
		read READ_NCE, $objs, OBJS;
		$objs = unpack "L", $objs;
		for( $obj = 0 ; $obj < $objs ; $obj++ )
		{
			#OBJデータをサイズ分seek
			seek READ_NCE , OBJ_SIZE , 1;
			$totalObjNo++;
		}
	}

	
	seek READ_NCE , $writeFileDataTop , 0;
	#OBJの個数を書き出す
	$write = pack "L",$totalObjNo;
	print WRITE_NCE $write;
	$totalObjNo = 0;
=cut
	#---------------------------------------------------------------------

	#セルデータのヘッダーを読み込み
	read READ_NCE, $header, CELL_HEADER_SIZE; 

	#読み込んだデータをセパレート
	($block_type, $block_size, $cells) = unpack "a4 L L", $header;

	if( $block_type ne "CELL" ){
		print "NitroCharacterのnceファイルではありません\n";
		die;
	}

	#セルの枚数を書き出す
	$write = pack "L",$cells;
	print WRITE_NCE $write;
	
	#セルデータ読み込み
	for( $cell = 0 ; $cell < $cells ; $cell++ ){
		#セルを構成するOBJの数を読み込み
		read READ_NCE, $objs, OBJS;
		$objs = unpack "L", $objs;
		$mepachi = 0;
		$mepachi_min_x = 0;
		$mepachi_min_y = 0;
		$mepachi_max_x = 0;
		$mepachi_max_y = 0;
		$mepachi_size_x = 0;
		$mepachi_size_y = 0;
		$mepachi_char = 0;
		$min_x		= 256;
		$max_x		= -256;
		$min_y		= 256;
		$max_y		= -256;
		$min_max_flag = 0;
		$dress_up_cell = 0;
		
		#---------------------------------------------------------------------
		#(パディングのせいで8個・・・
		@musicalDataIdx = (255,255,255,255,255,255,255,255);
		$dataArrIdx = 0;
		#---------------------------------------------------------------------
		for( $obj = 0 ; $obj < $objs ; $obj++ ){
			#OBJデータ読み込み
			read READ_NCE, $obj_data, OBJ_SIZE;
			#OBJデータをセパレート
			( $oam_pos_x,
			  $oam_pos_y,
			  $oam_rs_mode,
			  $oam_rs_param,
			  $oam_hflip,
			  $oam_vflip,
			  $oam_rsd_enable,
			  $oam_obj_mode,
			  $oam_mosaic,
			  $oam_color_mode,
			  $oam_shape,
			  $oam_size,
			  $oam_priority,
			  $oam_color_param,
			  $oam_char_name,
			  $padding ) = unpack "s s C C C C C C C C C C C C S S", $obj_data;

			#倍角フラグがONのときの補正計算をする
			if( $oam_rsd_enable ){
				$x1 = $oam_pos_x + ( $obj_size_x[$oam_shape][$oam_size] / 2 ) ;
				$x2 = $x1 + $obj_size_x[$oam_shape][$oam_size];
				$y1 = $oam_pos_y + ( $obj_size_y[$oam_shape][$oam_size] / 2 );
				$y2 = $y1 + $obj_size_y[$oam_shape][$oam_size];
			}
			else{
				$x1 = $oam_pos_x;
				$x2 = $oam_pos_x+$obj_size_x[$oam_shape][$oam_size];
				$y1 = $oam_pos_y;
				$y2 = $oam_pos_y+$obj_size_y[$oam_shape][$oam_size];
			}

			#OBJモードがONのときはメパチ用キャラなので、別コンバートする
			if( $oam_obj_mode ){
				#OBJのサイズを見て各座標の最小値と最大値を求めておく
				if( $mepachi == 0 ){
					$mepachi_min_x	= $x1;
					$mepachi_max_x	= $x2;
					$mepachi_min_y	= $y1;
					$mepachi_max_y	= $y2;
					$mepachi_char	= $oam_char_name;
				}
				else{
					if( $mepachi_min_x > $x1 ){
						$mepachi_min_x	= $x1;
						$mepachi_char	= $oam_char_name;
					}
					if( $mepachi_max_x < $x2 ){
						$mepachi_max_x = $x2;
					}
					if( $mepachi_min_y > $y1 ){
						$mepachi_min_y	= $y1;
						$mepachi_char	= $oam_char_name;
					}
					if( $mepachi_max_y < $y2 ){
						$mepachi_max_y = $y2;
					}
				}
				$mepachi++;
			}
			#---------------------------------------------------------------------
			#カラーパレットが0・１以外のときは、ドレスアップ用セル
			elsif( $oam_color_param > 1 ){
				if( $dataArrIdx >= 8 )
				{
					print "ポイントの数が多いよ！！\n";
				}

				$musicalCells[$musicalCellIdx] = MusicalData->new();
				$musicalCells[$musicalCellIdx]->objNo($totalObjNo);
				$musicalCells[$musicalCellIdx]->pltNo($oam_color_param);
				$musicalCells[$musicalCellIdx]->ofsX($oam_pos_x+($obj_size_x[$oam_shape][$oam_size]/2));
				$musicalCells[$musicalCellIdx]->ofsY($oam_pos_y+($obj_size_y[$oam_shape][$oam_size]/2));

				$musicalDataIdx[$dataArrIdx] = $musicalCellIdx;
				$dataArrIdx += 1;
				$musicalCellIdx += 1;
				
			}
			#---------------------------------------------------------------------
			else{
				#OBJのサイズを見て各座標の最小値と最大値を求めておく
				if( $min_x > $x1 ){
					$min_x		= $x1;
					$char_name	= $oam_char_name;
				}
				if( $max_x < $x2 ){
					$max_x = $x2;
				}
				if( $min_y > $y1 ){
					$min_y		= $y1;
					$char_name	= $oam_char_name;
				}
				if( $max_y < $y2 ){
					$max_y = $y2;
				}
				$min_max_flag = 1;
			}
			$totalObjNo += 1;
		}
		if( $min_max_flag ){
			$size_x = $max_x - $min_x;
			$size_y = $max_y - $min_y;
		}
		else{
			$size_x = 0;
			$size_y = 0;
		}

=pod
		print "cell:$cell min_x:$min_x min_y:$min_y max_x:$max_x max_y:$max_y\n";
		print "size_x:$size_x size_y:$size_y char_name:$char_name\n";
=cut

		$mepachi_size_x = $mepachi_max_x - $mepachi_min_x;
		$mepachi_size_y = $mepachi_max_y - $mepachi_min_y;

		$min_y *= -1;
		$mepachi_min_y *= -1;

=pod
		#セルの情報を書き出す
		$write = pack "S C C l l S C C l l", $char_name, $size_x, $size_y, $min_x, $min_y, $mepachi_char, $mepachi_size_x, $mepachi_size_y, $mepachi_min_x, $mepachi_min_y;
		print WRITE_NCE $write;
=cut

#前もって計算をしておくコンバート処理
#=pod
		$min_x = $min_x << MCSS_SHIFT;
		$min_y = $min_y << MCSS_SHIFT;
		$size_x = $size_x << 12;
		$size_y = $size_y << 12;
		$tex_s = ( ( $char_name % 32 ) * 8 ) << 12;
		$tex_t = ( ( $char_name >> 5 ) * 8 ) << 12;

#デバッグ表示
=pod
		$tex_ss = ( ( $char_name % 32 ) * 8 );
		$tex_tt = ( ( $char_name >> 5 ) * 8 );
		print "tex_s:$tex_ss\n";
		print "tex_t:$tex_tt\n\n";
=cut

		$mepachi_min_x = $mepachi_min_x << MCSS_SHIFT;
		$mepachi_min_y = $mepachi_min_y << MCSS_SHIFT;
		$mepachi_size_x = $mepachi_size_x << 12;
		$mepachi_size_y = $mepachi_size_y << 12;
		$mepachi_tex_s = ( ( $mepachi_char % 32 ) * 8 ) << 12;
		$mepachi_tex_t = ( ( $mepachi_char >> 5 ) * 8 ) << 12;

		#セルの情報を書き出す(パディングのせいで8個・・・
		$write = pack "l l l l l l l l l l l l C C C C C C C C", $min_x, $min_y, $size_x, $size_y, $tex_s, $tex_t, $mepachi_min_x, $mepachi_min_y, $mepachi_size_x, $mepachi_size_y, $mepachi_tex_s, $mepachi_tex_t,@musicalDataIdx[0..7];
		print WRITE_NCE $write;
#=cut
	}

	#---------------------------------------------------------------------
	printf "TotalObj[%d] MusCell[%d]\n",$totalObjNo,$musicalCellIdx;
	#ミュージカル用グループの解釈
	#まずヘッダーを読み、キャラ番号ヘッダだったら捨てる
	read READ_NCE, $header, CELL_HEADER_SIZE;
	($block_type, $block_size, $cells) = unpack "a4 L L", $header;
	if( $block_type == "CNUM" ){
		seek READ_NCE , $block_size - CELL_HEADER_SIZE, 1;
		#新しいヘッダ設定
		read READ_NCE, $header, CELL_HEADER_SIZE;
		($block_type, $block_size, $cells) = unpack "a4 L L", $header;
	}
	
	#グループヘッダの解読
	#グループが無かった場合は、すべて上向きとする
	if( $block_type == "GRP " ){
		$totalObjNo = 0;
		for( $cell = 0 ; $cell < $cells ; $cell++ ){
			read READ_NCE, $objs, OBJS;
			$objs = unpack "L", $objs;
			for( $obj = 0 ; $obj < $objs ; $obj++ ){
				#OBJデータ読み込み
				read READ_NCE, $groupeNo, OBJ_GROUPE_NUMBER_SIZE;
				$groupeNo = unpack "L", $groupeNo;
				
				#OBJの番号が一致したら
				for( $idx = 0 ; $idx< $musicalCellIdx ; $idx++ ){
					if( $musicalCells[$idx]->objNo == $totalObjNo ){
						$musicalCells[$idx]->grpNo( $groupeNo );
					}
				}
				$totalObjNo++;
			}
			
		}
	}
	
	if( $musicalCellIdx > 0 ){
		for( $idx = 0; $idx < $musicalCellIdx ; $idx++ ){
			printf "[%2d]%d:%d:%d:%d:%d\n",$idx,$musicalCells[$idx]->objNo ,$musicalCells[$idx]->pltNo, $musicalCells[$idx]->ofsX, $musicalCells[$idx]->ofsY ,$musicalCells[$idx]->grpNo;
			#セルの情報を書き出す
			$write = pack "l l l l l",$musicalCells[$idx]->objNo ,$musicalCells[$idx]->pltNo, $musicalCells[$idx]->ofsX, $musicalCells[$idx]->ofsY ,$musicalCells[$idx]->grpNo;
			print WRITE_NCE $write;
		}
	}
	#---------------------------------------------------------------------
	close READ_NCE;
	close WRITE_NCE;

