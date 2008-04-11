/* 文字列表示制御コードの定義 */

#define JP_AGB_EOM_			0xff		/* 終了コード */
#define JP_AGB_CR_				0xfe		/* 改行コード */
#define JP_AGB_I_MSG_			0xfd		/* 指定バッファのメッセージ展開(ID)	*/
#define JP_AGB_CTRL_			0xfc		/* フォント制御コマンド */
#define JP_AGB_NORMAL_WAIT_	0xfb		/* "▼"		トリガー待ち→MSGエリアクリア	*/
#define JP_AGB_SCROLL_WAIT_	0xfa		/* "↓"		トリガー待ち→一行スクロール */
#define JP_AGB_EXFONT1_		0xf9		/* 拡張フォント(fontnum = 0x100〜0x1ff) */
#define JP_AGB_EXICON_			0xf8		/* 拡張アイコン(キーアイコン) */
#define JP_AGB_S_MSG_			0xf7		/* 指定バッファのメッセージ展開(任意)*/

#define JP_AGB_C_FCOL_		0x01		/* フォント制御サブコマンド:文字色変更 */
#define JP_AGB_C_BCOL_		0x02		/* フォント制御サブコマンド:背景色変更 */
#define JP_AGB_C_SCOL_		0x03		/* フォント制御サブコマンド:影色変更 */
#define JP_AGB_C_ACOL_		0x04		/* フォント制御サブコマンド:全色指定変更(パラメータはFBSの順) */
#define JP_AGB_C_PAL_		0x05		/* フォント制御サブコマンド:パレット変更 */
#define JP_AGB_C_FCHG_		0x06		/* フォント制御サブコマンド:フォント変更 */
#define JP_AGB_C_FNML_		0x07		/* フォント制御サブコマンド:フォント復帰 */
#define JP_AGB_C_WAIT_		0x08		/* フォント制御サブコマンド:ウェイトセット */
#define JP_AGB_C_TRGWAIT_	0x09		/* フォント制御サブコマンド:キーウェイト */
#define JP_AGB_C_SEWAIT_	0x0a		/* フォント制御サブコマンド:SEの鳴り終わりまで待つ */
#define JP_AGB_C_MUSPLAY_	0x0b		/* フォント制御サブコマンド:BGMをならす */
#define JP_AGB_C_EXFONT_	0x0c		/* フォント制御サブコマンド:拡張フォント指定 */
#define JP_AGB_C_XCHG_		0x0d		/* フォント制御サブコマンド:表示X位置移動 */
#define JP_AGB_C_YCHG_		0x0e		/* フォント制御サブコマンド:表示Y位置移動 */
#define JP_AGB_C_CLR_		0x0f		/* フォント制御サブコマンド:MSGエリアクリア */
#define JP_AGB_C_SEPLAY_	0x10		/* フォント制御サブコマンド:SEをならす */

//サブコード0x11〜0x16	は海外版サブコード
#define JP_AGB_C_STRUT_	0x11		/* 引数１ */
#define JP_AGB_C_OFSX_		0x12		/* 引数１ */
#define JP_AGB_C_BLANK_	0x13		/* 引数１ */
#define JP_AGB_C_WIDTH_	0x14		/* 引数１ */
#define JP_AGB_C_LANG_JP	0x15		/* 引数なし */
#define JP_AGB_C_LANG_DEF	0x16		/* 引数なし */

#define JP_AGB_C_MUSPAUSE_	0x17		/* フォント制御サブコマンド:BGMをPAUSE */
#define JP_AGB_C_MUSCONTINUE_	0x18	/* フォント制御サブコマンド:BGMをCONTINUE */


/* 文字コードの定義 */

#define JP_AGB_spc_       0x00         /* "　" */
#define JP_AGB_a_         0x01         /* "あ" */
#define JP_AGB_i_         0x02         /* "い" */
#define JP_AGB_u_         0x03         /* "う" */
#define JP_AGB_e_         0x04         /* "え" */
#define JP_AGB_o_         0x05         /* "お" */
#define JP_AGB_ka_        0x06         /* "か" */
#define JP_AGB_ki_        0x07         /* "き" */
#define JP_AGB_ku_        0x08         /* "く" */
#define JP_AGB_ke_        0x09         /* "け" */
#define JP_AGB_ko_        0x0a         /* "こ" */
#define JP_AGB_sa_        0x0b         /* "さ" */
#define JP_AGB_si_        0x0c         /* "し" */
#define JP_AGB_su_        0x0d         /* "す" */
#define JP_AGB_se_        0x0e         /* "せ" */
#define JP_AGB_so_        0x0f         /* "そ" */
#define JP_AGB_ta_        0x10         /* "た" */
#define JP_AGB_ti_        0x11         /* "ち" */
#define JP_AGB_tu_        0x12         /* "つ" */
#define JP_AGB_te_        0x13         /* "て" */
#define JP_AGB_to_        0x14         /* "と" */
#define JP_AGB_na_        0x15         /* "な" */
#define JP_AGB_ni_        0x16         /* "に" */
#define JP_AGB_nu_        0x17         /* "ぬ" */
#define JP_AGB_ne_        0x18         /* "ね" */
#define JP_AGB_no_        0x19         /* "の" */
#define JP_AGB_ha_        0x1a         /* "は" */
#define JP_AGB_hi_        0x1b         /* "ひ" */
#define JP_AGB_hu_        0x1c         /* "ふ" */
#define JP_AGB_he_        0x1d         /* "へ" */
#define JP_AGB_ho_        0x1e         /* "ほ" */
#define JP_AGB_ma_        0x1f         /* "ま" */
#define JP_AGB_mi_        0x20         /* "み" */
#define JP_AGB_mu_        0x21         /* "む" */
#define JP_AGB_me_        0x22         /* "め" */
#define JP_AGB_mo_        0x23         /* "も" */
#define JP_AGB_ya_        0x24         /* "や" */
#define JP_AGB_yu_        0x25         /* "ゆ" */
#define JP_AGB_yo_        0x26         /* "よ" */
#define JP_AGB_ra_        0x27         /* "ら" */
#define JP_AGB_ri_        0x28         /* "り" */
#define JP_AGB_ru_        0x29         /* "る" */
#define JP_AGB_re_        0x2a         /* "れ" */
#define JP_AGB_ro_        0x2b         /* "ろ" */
#define JP_AGB_wa_        0x2c         /* "わ" */
#define JP_AGB_wo_        0x2d         /* "を" */
#define JP_AGB_n_         0x2e         /* "ん" */
#define JP_AGB_aa_        0x2f         /* "ぁ" */
#define JP_AGB_ii_        0x30         /* "ぃ" */
#define JP_AGB_uu_        0x31         /* "ぅ" */
#define JP_AGB_ee_        0x32         /* "ぇ" */
#define JP_AGB_oo_        0x33         /* "ぉ" */
#define JP_AGB_yya_       0x34         /* "ゃ" */
#define JP_AGB_yyu_       0x35         /* "ゅ" */
#define JP_AGB_yyo_       0x36         /* "ょ" */
#define JP_AGB_ga_        0x37         /* "が" */
#define JP_AGB_gi_        0x38         /* "ぎ" */
#define JP_AGB_gu_        0x39         /* "ぐ" */
#define JP_AGB_ge_        0x3a         /* "げ" */
#define JP_AGB_go_        0x3b         /* "ご" */
#define JP_AGB_za_        0x3c         /* "ざ" */
#define JP_AGB_zi_        0x3d         /* "じ" */
#define JP_AGB_zu_        0x3e         /* "ず" */
#define JP_AGB_ze_        0x3f         /* "ぜ" */
#define JP_AGB_zo_        0x40         /* "ぞ" */
#define JP_AGB_da_        0x41         /* "だ" */
#define JP_AGB_di_        0x42         /* "ぢ" */
#define JP_AGB_du_        0x43         /* "づ" */
#define JP_AGB_de_        0x44         /* "で" */
#define JP_AGB_do_        0x45         /* "ど" */
#define JP_AGB_ba_        0x46         /* "ば" */
#define JP_AGB_bi_        0x47         /* "び" */
#define JP_AGB_bu_        0x48         /* "ぶ" */
#define JP_AGB_be_        0x49         /* "べ" */
#define JP_AGB_bo_        0x4a         /* "ぼ" */
#define JP_AGB_pa_        0x4b         /* "ぱ" */
#define JP_AGB_pi_        0x4c         /* "ぴ" */
#define JP_AGB_pu_        0x4d         /* "ぷ" */
#define JP_AGB_pe_        0x4e         /* "ぺ" */
#define JP_AGB_po_        0x4f         /* "ぽ" */
#define JP_AGB_ttu_       0x50         /* "っ" */
#define JP_AGB_A_         0x51         /* "ア" */
#define JP_AGB_I_         0x52         /* "イ" */
#define JP_AGB_U_         0x53         /* "ウ" */
#define JP_AGB_E_         0x54         /* "エ" */
#define JP_AGB_O_         0x55         /* "オ" */
#define JP_AGB_KA_        0x56         /* "カ" */
#define JP_AGB_KI_        0x57         /* "キ" */
#define JP_AGB_KU_        0x58         /* "ク" */
#define JP_AGB_KE_        0x59         /* "ケ" */
#define JP_AGB_KO_        0x5a         /* "コ" */
#define JP_AGB_SA_        0x5b         /* "サ" */
#define JP_AGB_SI_        0x5c         /* "シ" */
#define JP_AGB_SU_        0x5d         /* "ス" */
#define JP_AGB_SE_        0x5e         /* "セ" */
#define JP_AGB_SO_        0x5f         /* "ソ" */
#define JP_AGB_TA_        0x60         /* "タ" */
#define JP_AGB_TI_        0x61         /* "チ" */
#define JP_AGB_TU_        0x62         /* "ツ" */
#define JP_AGB_TE_        0x63         /* "テ" */
#define JP_AGB_TO_        0x64         /* "ト" */
#define JP_AGB_NA_        0x65         /* "ナ" */
#define JP_AGB_NI_        0x66         /* "ニ" */
#define JP_AGB_NU_        0x67         /* "ヌ" */
#define JP_AGB_NE_        0x68         /* "ネ" */
#define JP_AGB_NO_        0x69         /* "ノ" */
#define JP_AGB_HA_        0x6a         /* "ハ" */
#define JP_AGB_HI_        0x6b         /* "ヒ" */
#define JP_AGB_HU_        0x6c         /* "フ" */
#define JP_AGB_HE_        0x6d         /* "ヘ" */
#define JP_AGB_HO_        0x6e         /* "ホ" */
#define JP_AGB_MA_        0x6f         /* "マ" */
#define JP_AGB_MI_        0x70         /* "ミ" */
#define JP_AGB_MU_        0x71         /* "ム" */
#define JP_AGB_ME_        0x72         /* "メ" */
#define JP_AGB_MO_        0x73         /* "モ" */
#define JP_AGB_YA_        0x74         /* "ヤ" */
#define JP_AGB_YU_        0x75         /* "ユ" */
#define JP_AGB_YO_        0x76         /* "ヨ" */
#define JP_AGB_RA_        0x77         /* "ラ" */
#define JP_AGB_RI_        0x78         /* "リ" */
#define JP_AGB_RU_        0x79         /* "ル" */
#define JP_AGB_RE_        0x7a         /* "レ" */
#define JP_AGB_RO_        0x7b         /* "ロ" */
#define JP_AGB_WA_        0x7c         /* "ワ" */
#define JP_AGB_WO_        0x7d         /* "ヲ" */
#define JP_AGB_N_         0x7e         /* "ン" */
#define JP_AGB_AA_        0x7f         /* "ァ" */
#define JP_AGB_II_        0x80         /* "ィ" */
#define JP_AGB_UU_        0x81         /* "ゥ" */
#define JP_AGB_EE_        0x82         /* "ェ" */
#define JP_AGB_OO_        0x83         /* "ォ" */
#define JP_AGB_YYA_       0x84         /* "ャ" */
#define JP_AGB_YYU_       0x85         /* "ュ" */
#define JP_AGB_YYO_       0x86         /* "ョ" */
#define JP_AGB_GA_        0x87         /* "ガ" */
#define JP_AGB_GI_        0x88         /* "ギ" */
#define JP_AGB_GU_        0x89         /* "グ" */
#define JP_AGB_GE_        0x8a         /* "ゲ" */
#define JP_AGB_GO_        0x8b         /* "ゴ" */
#define JP_AGB_ZA_        0x8c         /* "ザ" */
#define JP_AGB_ZI_        0x8d         /* "ジ" */
#define JP_AGB_ZU_        0x8e         /* "ズ" */
#define JP_AGB_ZE_        0x8f         /* "ゼ" */
#define JP_AGB_ZO_        0x90         /* "ゾ" */
#define JP_AGB_DA_        0x91         /* "ダ" */
#define JP_AGB_DI_        0x92         /* "ヂ" */
#define JP_AGB_DU_        0x93         /* "ヅ" */
#define JP_AGB_DE_        0x94         /* "デ" */
#define JP_AGB_DO_        0x95         /* "ド" */
#define JP_AGB_BA_        0x96         /* "バ" */
#define JP_AGB_BI_        0x97         /* "ビ" */
#define JP_AGB_BU_        0x98         /* "ブ" */
#define JP_AGB_BE_        0x99         /* "ベ" */
#define JP_AGB_BO_        0x9a         /* "ボ" */
#define JP_AGB_PA_        0x9b         /* "パ" */
#define JP_AGB_PI_        0x9c         /* "ピ" */
#define JP_AGB_PU_        0x9d         /* "プ" */
#define JP_AGB_PE_        0x9e         /* "ペ" */
#define JP_AGB_PO_        0x9f         /* "ポ" */
#define JP_AGB_TTU_       0xa0         /* "ッ" */
#define JP_AGB_n0_        0xa1         /* "０" */
#define JP_AGB_n1_        0xa2         /* "１" */
#define JP_AGB_n2_        0xa3         /* "２" */
#define JP_AGB_n3_        0xa4         /* "３" */
#define JP_AGB_n4_        0xa5         /* "４" */
#define JP_AGB_n5_        0xa6         /* "５" */
#define JP_AGB_n6_        0xa7         /* "６" */
#define JP_AGB_n7_        0xa8         /* "７" */
#define JP_AGB_n8_        0xa9         /* "８" */
#define JP_AGB_n9_        0xaa         /* "９" */
#define JP_AGB_gyoe_     0xab         /* "！" */
#define JP_AGB_hate_     0xac         /* "？" */
#define JP_AGB_kten_     0xad         /* "。" */
#define JP_AGB_bou_      0xae         /* "ー" */
#define JP_AGB_nakag_    0xaf         /* "・" */
#define JP_AGB_tenten_   0xb0         /* "…" */
#define JP_AGB_kako2_    0xb1         /* "『" */
#define JP_AGB_kakot2_   0xb2         /* "』" */
#define JP_AGB_kako_     0xb3         /* "「" */
#define JP_AGB_kakot_    0xb4         /* "」" */
#define JP_AGB_osu_      0xb5         /* "♂" */
#define JP_AGB_mesu_     0xb6         /* "♀" */
#define JP_AGB_yen_      0xb7         /* "円" */
#define JP_AGB_ten_      0xb8         /* "．" */
#define JP_AGB_batu_     0xb9         /* "×" */
#define JP_AGB_sura_     0xba         /* "／" */
#define JP_AGB_A__        0xbb         /* "Ａ" */
#define JP_AGB_B__        0xbc         /* "Ｂ" */
#define JP_AGB_C__        0xbd         /* "Ｃ" */
#define JP_AGB_D__        0xbe         /* "Ｄ" */
#define JP_AGB_E__        0xbf         /* "Ｅ" */
#define JP_AGB_F__        0xc0         /* "Ｆ" */
#define JP_AGB_G__        0xc1         /* "Ｇ" */
#define JP_AGB_H__        0xc2         /* "Ｈ" */
#define JP_AGB_I__        0xc3         /* "Ｉ" */
#define JP_AGB_J__        0xc4         /* "Ｊ" */
#define JP_AGB_K__        0xc5         /* "Ｋ" */
#define JP_AGB_L__        0xc6         /* "Ｌ" */
#define JP_AGB_M__        0xc7         /* "Ｍ" */
#define JP_AGB_N__        0xc8         /* "Ｎ" */
#define JP_AGB_O__        0xc9         /* "Ｏ" */
#define JP_AGB_P__        0xca         /* "Ｐ" */
#define JP_AGB_Q__        0xcb         /* "Ｑ" */
#define JP_AGB_R__        0xcc         /* "Ｒ" */
#define JP_AGB_S__        0xcd         /* "Ｓ" */
#define JP_AGB_T__        0xce         /* "Ｔ" */
#define JP_AGB_U__        0xcf         /* "Ｕ" */
#define JP_AGB_V__        0xd0         /* "Ｖ" */
#define JP_AGB_W__        0xd1         /* "Ｗ" */
#define JP_AGB_X__        0xd2         /* "Ｘ" */
#define JP_AGB_Y__        0xd3         /* "Ｙ" */
#define JP_AGB_Z__        0xd4         /* "Ｚ" */
#define JP_AGB_a__        0xd5         /* "ａ" */
#define JP_AGB_b__        0xd6         /* "ｂ" */
#define JP_AGB_c__        0xd7         /* "ｃ" */
#define JP_AGB_d__        0xd8         /* "ｄ" */
#define JP_AGB_e__        0xd9         /* "ｅ" */
#define JP_AGB_f__        0xda         /* "ｆ" */
#define JP_AGB_g__        0xdb         /* "ｇ" */
#define JP_AGB_h__        0xdc         /* "ｈ" */
#define JP_AGB_i__        0xdd         /* "ｉ" */
#define JP_AGB_j__        0xde         /* "ｊ" */
#define JP_AGB_k__        0xdf         /* "ｋ" */
#define JP_AGB_l__        0xe0         /* "ｌ" */
#define JP_AGB_m__        0xe1         /* "ｍ" */
#define JP_AGB_n__        0xe2         /* "ｎ" */
#define JP_AGB_o__        0xe3         /* "ｏ" */
#define JP_AGB_p__        0xe4         /* "ｐ" */
#define JP_AGB_q__        0xe5         /* "ｑ" */
#define JP_AGB_r__        0xe6         /* "ｒ" */
#define JP_AGB_s__        0xe7         /* "ｓ" */
#define JP_AGB_t__        0xe8         /* "ｔ" */
#define JP_AGB_u__        0xe9         /* "ｕ" */
#define JP_AGB_v__        0xea         /* "ｖ" */
#define JP_AGB_w__        0xeb         /* "ｗ" */
#define JP_AGB_x__        0xec         /* "ｘ" */
#define JP_AGB_y__        0xed         /* "ｙ" */
#define JP_AGB_z__        0xee         /* "ｚ" */
#define JP_AGB_cursor_    0xef			/* 右カーソル */
#define JP_AGB_colon_     0xf0			/* "：" */
#define JP_AGB_Auml_      0xf1			/* ウムラウト大文字A */
#define JP_AGB_Ouml_      0xf2			/* ウムラウト大文字O */
#define JP_AGB_Uuml_      0xf3			/* ウムラウト大文字U */
#define JP_AGB_auml_      0xf4			/* ウムラウト小文字a */
#define JP_AGB_ouml_      0xf5			/* ウムラウト小文字o */
#define JP_AGB_uuml_      0xf6			/* ウムラウト小文字u */

/* 拡張フォント文字コードの定義 */
//#define JP_AGB_ArrowU__	0xf7		/* 上向きやじるし */
//#define JP_AGB_ArrowD__	0xf8		/* 下向きやじるし */
//#define JP_AGB_ArrowL__	0xf9		/* 左向きやじるし */
//#define JP_AGB_ArrowR__	0xfa		/* 右向きやじるし */
//#define JP_AGB_Plus__	0xfb		/* プラス記号 */

/* 拡張フォント文字コードの定義 2003.4.10 追加*/
#define JP_AGB_ArrowU__	0x00		/* 上向きやじるし */
#define JP_AGB_ArrowD__	0x01		/* 下向きやじるし */
#define JP_AGB_ArrowL__	0x02		/* 左向きやじるし */
#define JP_AGB_ArrowR__	0x03		/* 右向きやじるし */
#define JP_AGB_Plus__		0x04		/* プラス記号 */
#define JP_AGB_Lv__		0x05		/* レベル記号 */
#define JP_AGB_PP__		0x06		/* ＰＰ記号 */
#define JP_AGB_ID__		0x07		/* ＩＤ記号 */
#define JP_AGB_No__		0x08		/* Ｎｏ記号 */
#define JP_AGB_UnderBer_	0x09		/* アンダーバー */
#define JP_AGB_MARU1__		0x0a		/* �@ */
#define JP_AGB_MARU2__		0x0b		/* �A */
#define JP_AGB_MARU3__		0x0c		/* �B */
#define JP_AGB_MARU4__		0x0d		/* �C */
#define JP_AGB_MARU5__		0x0e		/* �D */
#define JP_AGB_MARU6__		0x0f		/* �E */
#define JP_AGB_MARU7__		0x10		/* �F */
#define JP_AGB_MARU8__		0x11		/* �G */
#define JP_AGB_MARU9__		0x12		/* �H */
#define JP_AGB_MaruKako__	0x13		/* （ */
#define JP_AGB_MaruKakot__	0x14		/* ） */
#define JP_AGB_NijuMaru_	0x15		/* ◎ */
#define JP_AGB_Sankaku_	0x16		/* △ */

#define JP_AGB_E_RBUTTON_		0xcc		/* Ｒボタン */
#define JP_AGB_E_DAI_			0xcd		/* 大 */
#define JP_AGB_E_SYOU_			0xce		/* 小 */
#define JP_AGB_E_DAKU_			0xcf		/* 濁点 */
#define JP_AGB_E_HANDAKU_		0xd0		/* 半濁点 */
#define JP_AGB_E_VBAR_			0xd1		/* 縦棒 */
#define JP_AGB_E_HBAR_			0xd2		/* 横棒 */
#define JP_AGB_E_NYORO_		0xd3		/* 〜 */
#define JP_AGB_E_KAKO_			0xd4		/* （ */
#define JP_AGB_E_KAKOT_		0xd5		/* ） */
#define JP_AGB_E_SET_			0xd6		/* ⊂ */
#define JP_AGB_E_GREAT_		0xd7		/* ＞ */
#define JP_AGB_E_EYE_L_		0xd8		/* 左目 */
#define JP_AGB_E_EYE_R_		0xd9		/* 右目 */
#define JP_AGB_E_ATMARK_		0xda		/* ＠ */
#define JP_AGB_E_SEMICOLON_	0xdb		/* ； */
#define JP_AGB_E_PLUS_			0xdc		/* ＋ */
#define JP_AGB_E_MINUS_		0xdd		/* − */
#define JP_AGB_E_EQUAL_		0xde		/* ＝ */
#define JP_AGB_E_UZUMAKI_		0xdf		/* うずまきマーク */
#define JP_AGB_E_BERO_			0xe0		/* べろマーク */
#define JP_AGB_E_SANKAKU_		0xe1		/* △ */
#define JP_AGB_E_QUOT_			0xe2		/* クォーテーション開き */
#define JP_AGB_E_QUOTT_		0xe3		/* クォーテーション閉じ */
#define JP_AGB_E_CIRCLE_		0xe4		/* ● */
#define JP_AGB_E_TRIANGLE_		0xe5		/* ▼ */
#define JP_AGB_E_SQUARE_		0xe6		/* ■ */
#define JP_AGB_E_HEART_		0xe7		/* ハート */
#define JP_AGB_E_MOON_			0xe8		/* 三日月 */
#define JP_AGB_E_NOTE_			0xe9		/* 音符 */
#define JP_AGB_E_MBALL_		0xea		/* モンスターボール */
#define JP_AGB_E_THUNDER_		0xeb		/* 雷マーク */
#define JP_AGB_E_LEAF_			0xec		/* 葉っぱマーク */
#define JP_AGB_E_FIRE_			0xed		/* 炎マーク */
#define JP_AGB_E_WATER_		0xee		/* 水マーク */
#define JP_AGB_E_HANDR_		0xef		/* 右手 */
#define JP_AGB_E_HANDL_		0xf0		/* 左手 */
#define JP_AGB_E_FLOWER_		0xf1		/* 花マーク */
#define JP_AGB_E_EYE1_			0xf2		/* 目マーク１ */
#define JP_AGB_E_EYE2_			0xf3		/* 目マーク２ */
#define JP_AGB_E_IKARI_		0xf4		/* 怒りマーク */
#define JP_AGB_E_KAO_01_		0xf5		/* 顔１ */
#define JP_AGB_E_KAO_02_		0xf6		/* 顔２ */
#define JP_AGB_E_KAO_03_		0xf7		/* 顔３（ヘの字口）*/
#define JP_AGB_E_KAO_04_		0xf8		/* 顔４（鼻） */
#define JP_AGB_E_KAO_05_		0xf9		/* 顔５（笑顔）*/
#define JP_AGB_E_KAO_06_		0xfa		/* 顔６（悪そう）*/
#define JP_AGB_E_KAO_07_		0xfb		/* 顔７（ヘコみ）*/
#define JP_AGB_E_KAO_08_		0xfc		/* 顔８（真顔）*/
#define JP_AGB_E_KAO_09_		0xfd		/* 顔９（驚き）*/
#define JP_AGB_E_KAO_10_		0xfe		/* 顔10（怒り）*/

// ------------ 0xff は EOM_ と判定されるので使わないこと！ ---------

/* 拡張アイコン(キーアイコン)文字コードの定義 2003.7.30 追加*/
#define JP_AGB_FICN_BTN_A_		0x00		/* Ａボタン */
#define JP_AGB_FICN_BTN_B_		0x01		/* Ｂボタン */
#define JP_AGB_FICN_BTN_L_		0x02		/* Ｌボタン */
#define JP_AGB_FICN_BTN_R_		0x03		/* Ｒボタン */
#define JP_AGB_FICN_BTN_ST_	0x04		/* スタートボタン */
#define JP_AGB_FICN_BTN_SE_	0x05		/* セレクトボタン */
#define JP_AGB_FICN_KEY_U_		0x06		/* Ｕキー */
#define JP_AGB_FICN_KEY_D_		0x07		/* Ｄキー */
#define JP_AGB_FICN_KEY_L_		0x08		/* Ｌキー */
#define JP_AGB_FICN_KEY_R_		0x09		/* Ｒキー */
#define JP_AGB_FICN_KEY_UD_	0x0a		/* ＵＤキー */
#define JP_AGB_FICN_KEY_LR_	0x0b		/* ＬＲキー */
#define JP_AGB_FICN_KEY_UDLR_	0x0c		/* ＵＤＬＲキー */


