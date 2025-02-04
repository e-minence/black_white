//============================================================================
/**
 *@file		tr_ai_expert.s
 *@brief	トレーナーAI
 *@author	HisashiSogabe
 *@data		2009.12.15
 */
//============================================================================

	.text
	.include	"tr_ai_seq_def.h"

//========================================================
//	エキスパートAI
//========================================================

ExpertAISeq:
	IF_MIKATA_ATTACK	ExpertAISeq_end   //対象が味方なら終了

  TABLE_JUMP TABLE_JUMP_WAZASEQNO,WAZASEQ_TABLE,337
  AIEND

  TABLE_ADRS  WAZASEQ_TABLE
	ADRS  ExpertAI_Dummy	//	  0ダミー
	ADRS  ExpertAI_1		// 眠らせる			2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_3		// すいとる			2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_7		// じばく			2006.6.14
	ADRS  ExpertAI_8		// ゆめくい			2006.6.14バグ修正
	ADRS  ExpertAI_9		// オウム			2006.6.14テーブル追加
	ADRS  ExpertAI_10		// 攻撃力アップ		2006.6.14たんじゅん見るかも

	ADRS  ExpertAI_11		// 防御力アップ		2006.6.14*分類
	ADRS  ExpertAI_12		// 素早さアップ		2006.6.14
	ADRS  ExpertAI_13		// 特攻アップ		2006.6.14
	ADRS  ExpertAI_14		// 特防アップ		2006.6.14*分類
	ADRS  ExpertAI_15		// 命中率アップ		2006.6.14
	ADRS  ExpertAI_16		// 回避率アップ		2006.6.14アクアリング追加
	ADRS  ExpertAI_17		// かならずあたる	2006.6.14
	ADRS  ExpertAI_18		// 攻撃力ダウン	0	2006.6.14*分類
	ADRS  ExpertAI_19		// 防御力ダウン		2006.6.14
	ADRS  ExpertAI_20		// 素早さダウン		2006.6.14	

	ADRS  ExpertAI_21		// 特攻ダウン		2006.6.14*分類
	ADRS  ExpertAI_22		// 特防ダウン		2006.6.14
	ADRS  ExpertAI_23		// 命中率ダウン		2006.6.14アクアリング追加
	ADRS  ExpertAI_24		// 回避率ダウン		2006.6.14
	ADRS  ExpertAI_25		// くろいきり	*たんじゅんチェック
	ADRS  ExpertAI_26		// がまん			2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_28		// ふきとばし	まきびしチェック余力があれば
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_30		// テクスチャー		2006.6.14

	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_32		// じこさいせい		2006.6.14
	ADRS  ExpertAI_33		// どくどく			2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_35		// ひかりのかべ		2006.6.14要チェック
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_37		// ねむる			2006.6.14
	ADRS  ExpertAI_38		// いちげきひっさつ	2006.6.14
	ADRS  ExpertAI_39		// ため				2006.6.14
	ADRS  ExpertAI_40		// いかりのまえば	2006.6.14

	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_42		// しめつけ			2006.6.14
	ADRS  ExpertAI_43		// クリティカルでやすい		*特性と装備道具
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_48		// とっしん			2006.6.14ＤＰ追加
	ADRS  ExpertAI_49		// こんらん				
	ADRS  ExpertAI_50		// 攻撃２アップ		2006.6.14

	ADRS  ExpertAI_51		// 防御２アップ		2006.6.14
	ADRS  ExpertAI_52		// 素早さ２アップ	2006.6.14
	ADRS  ExpertAI_53		// 特攻２アップ		2006.6.14
	ADRS  ExpertAI_54		// 特防２アップ		2006.6.14
	ADRS  ExpertAI_55		// 命中２アップ		2006.6.14
	ADRS  ExpertAI_56		// 回避２アップ		2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_58		// 攻撃２ダウン		2006.6.14
	ADRS  ExpertAI_59		// 防御２ダウン		2006.6.14
	ADRS  ExpertAI_60		// 素早さ２ダウン	2006.6.14

	ADRS  ExpertAI_61		// 特攻２ダウン		2006.6.14
	ADRS  ExpertAI_62		// 特防２ダウン		2006.6.14
	ADRS  ExpertAI_63		// 命中２ダウン		2006.6.14
	ADRS  ExpertAI_64		// 回避２ダウン		2006.6.14
	ADRS  ExpertAI_65		// リフレクター		2006.6.14要チェック
	ADRS  ExpertAI_66		// どく				2006.6.14
	ADRS  ExpertAI_67		// まひ				2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_70		// 追加素早さダウン	2006.6.14

	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_75		// ゴッドバード		2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_78		// あてみなげ		2006.6.14
	ADRS  ExpertAI_79		// みがわり			2006.6.14
	ADRS  ExpertAI_80		// はかいこうせん	2006.6.14

	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_84		// やどりぎ			2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_86		// かなしばり		2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_89		// カウンター
	ADRS  ExpertAI_90		// アンコール※テーブル作成

	ADRS  ExpertAI_91		// いたみわけ		2006.6.14
	ADRS  ExpertAI_92		// いびき			2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_94		// ロックオン		2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_97		// ねごと			2006.6.14
	ADRS  ExpertAI_98		// みちずれ			2006.6.14
	ADRS  ExpertAI_99		// きしかいせい		2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	

	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_102	// いやしのすず		2006.6.14修正
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_105	// どろぼう			2006.6.14テーブル追加
	ADRS  ExpertAI_106	// くろいまなざし	2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_108		// 小さくなる		2006.6.14
	ADRS  ExpertAI_109	// のろい			2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	

	ADRS  ExpertAI_111	// まもる			2006.6.14フェイントチェック追加
	ADRS  ExpertAI_112	// まきびし			2006.6.14新規追加
	ADRS  ExpertAI_113	// みやぶる			2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_116	// こらえる			2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_118	// いばる
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	

	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_127	// バトンタッチ		2006.6.14
	ADRS  ExpertAI_128	// おいうち			2006.6.14敵とんぼがえりチェック追加
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	

	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_132	// あさのひざし		2006.6.14
	ADRS  ExpertAI_133	// こうごうせい		2006.6.14シーケンス削除
	ADRS  ExpertAI_134	// つきのひかり		2006.6.14シーケンス削除
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_136	// あまごい			2006.6.14
	ADRS  ExpertAI_137	// にほんばれ		2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	

	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_142	// はらだいこ		2006.6.14
	ADRS  ExpertAI_143	// じこあんじ		2006.6.14
	ADRS  ExpertAI_144	// ミラーコート		分類チェック
	ADRS  ExpertAI_145	// ロケットずつき	2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	

	ADRS  ExpertAI_151	// ソーラービーム	2006.6.14パワフルハーブ
	ADRS  ExpertAI_152	// かみなり			2006.6.14追加
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_155	// そらをとぶ		2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_157	// たまごうみ		2006.6.14
	ADRS  ExpertAI_158	// ねこだまし		2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_160		// たくわえる	

	ADRS  ExpertAI_161	// はきだす			2006.6.14
	ADRS  ExpertAI_162	// のみこむ			2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_164	// あられ			2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_166	// おだてる			2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_168	// おきみやげ		2006.6.14
	ADRS  ExpertAI_169	// からげんき		2006.6.14
	ADRS  ExpertAI_170	// きあいパンチ		2006.6.14

	ADRS  ExpertAI_171	// きつけ			2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_177	// トリック			相手に合わせてテーブル作成必要あり
	ADRS  ExpertAI_178	// なりきり			テーブル要調整
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	

	ADRS  ExpertAI_181	// ねをはる			2006.6.14
	ADRS  ExpertAI_182	// ばかぢから		しろいハーブチェック入れるかも
	ADRS  ExpertAI_183	// マジックコート	2006.6.14
	ADRS  ExpertAI_184	// リサイクル		2006.6.14
	ADRS  ExpertAI_185	// リベンジ			2006.6.14
	ADRS  ExpertAI_186	// かわらわり		2006.6.14ひかりのかべも追加
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_188	// はたきおとす		2006.6.14
	ADRS  ExpertAI_189	// がむしゃら		2006.6.14
	ADRS  ExpertAI_190	// ふんか			2006.6.14

	ADRS  ExpertAI_191	// スキルスワップ	2006.6.14
	ADRS  ExpertAI_192	// ふういん			2006.6.14何か考えるかも
	ADRS  ExpertAI_193	// リフレッシュ		2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_195	// よこどり			2006.6.14要解析
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_198	// すてみタックル	2006.6.14ＤＰ追加
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_200	// ブレイズキック	急所特性と装備道具

	ADRS  ExpertAI_201	// どろあそび		2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_204	// オーバーヒート	2006.6.14しろいハーブチェックいる？
	ADRS  ExpertAI_205	// くすぐる			2006.6.14
	ADRS  ExpertAI_206	// コスモパワー		2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_208	// ビルドアップ		2006.6.14
	ADRS  ExpertAI_209	// ポイズンテール	急所特性と装備道具
	ADRS  ExpertAI_210	// みずあそび		2006.6.14

	ADRS  ExpertAI_211	// めいそう			2006.6.14
	ADRS  ExpertAI_212	// りゅうのまい		2006.6.14
	ADRS  ExpertAI_Dummy		// ダミー	
	//追加2006.6.14
	ADRS  ExpertAI_214	// はねやすめ		2006.6.14 
	ADRS  ExpertAI_215	// じゅうりょく		2006.6.14		 
	ADRS  ExpertAI_216	// ミラクルアイ		2006.6.14 
	ADRS  ExpertAI_217	// めざましビンタ	2006.6.14 
	ADRS  ExpertAI_218	// アームハンマー	2006.6.14 
	ADRS  ExpertAI_219	// ジャイロボール	2006.6.14 
	ADRS  ExpertAI_220	// いやしのねがい	2006.6.14*控えをだすときのチェック 

	ADRS  ExpertAI_221	// しおみず		2006.6.14 
  ADRS  ExpertAI_Dummy// しぜんのめぐみ* ダメージチェックを入れるのでなし
	ADRS  ExpertAI_223	// フェイント		2006.6.14 
	ADRS  ExpertAI_224	// ついばむ			2006.6.14 
	ADRS  ExpertAI_225	// おいかぜ			2006.6.14
	ADRS  ExpertAI_226	// つぼをつく		2006.6.14 
	ADRS  ExpertAI_227	// メタルバースト	2006.6.14 
	ADRS  ExpertAI_228	// とんぼがえり		2006.6.14
	ADRS  ExpertAI_229	// インファイト		2006.6.14しろいハーブチェック 
	ADRS  ExpertAI_230	// しっぺがえし		2006.6.14 

	ADRS  ExpertAI_231	// ダメおし			2006.6.14 
	ADRS  ExpertAI_232	// さしおさえ		2006.6.14 
	ADRS  ExpertAI_233	// なげつける		2006.6.14 
	ADRS  ExpertAI_234	// サイコシフト		2006.6.14 
	ADRS  ExpertAI_235	// きりふだ			2006.6.14 
	ADRS  ExpertAI_236	// かいふくふうじ	2006.6.14 
	ADRS  ExpertAI_237	// しぼりとる		2006.6.14
	ADRS  ExpertAI_238	// パワートリック	2006.6.14
	ADRS  ExpertAI_239	// いえき			2006.6.14
	ADRS  ExpertAI_240	// おまじない		2006.6.14 

	ADRS  ExpertAI_241	// さきどり			2006.6.14	
	ADRS  ExpertAI_242	// まねっこ：テーブル2006.6.14 
	ADRS  ExpertAI_243	// パワースワップ	2006.6.14
	ADRS  ExpertAI_244	// ガードスワップ	2006.6.14
	ADRS  ExpertAI_245	// おしおき			2006.6.14 
	ADRS  ExpertAI_246	// とっておき		2006.6.14 
	ADRS  ExpertAI_247	// なやみのタネ		2006.6.14
	ADRS  ExpertAI_248	// ふいうち			2006.6.14	 
	ADRS  ExpertAI_249	// どくびし			2006.6.14 
	ADRS  ExpertAI_250	// ハートスワップ	2006.6.14 

	ADRS  ExpertAI_251	// アクアリング		2006.6.14 
	ADRS  ExpertAI_252	// でんじふゆう		2006.6.14 
	ADRS  ExpertAI_253	// フレアドライブ	2006.6.14 
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_255	// ダイビング		2006.6.14 
	ADRS  ExpertAI_256	// あなをほる		2006.6.14 
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_258	// きりばらい		2006.6.14	よくみる必要あり
	ADRS  ExpertAI_259	// トリックルーム	2006.6.14 
	ADRS  ExpertAI_260	// ふぶき			2006.6.14

	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_262	// ボルテッカー		2006.6.14ＤＰ追加
	ADRS  ExpertAI_263	// とびはねる		2006.6.14 
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_265	// ゆうわく			2006.6.14		 
	ADRS  ExpertAI_266	// ステルスロック	2006.6.14 
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy 	//さばきのつぶて*ダメージチェックを入れるので問題なし 
	ADRS  ExpertAI_269	// もろはのずつき	2006.6.14 
	ADRS  ExpertAI_270	// みかづきのまい	2006.6.14 

	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_272	// シャドーダイブ	2006.6.14 
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	ADRS  ExpertAI_Dummy		// ダミー	
	//追加2010.2.9
  ADRS  ExpertAI_277	// 	つめとぎ
	ADRS  ExpertAI_278	// 	ワイドガード
	ADRS  ExpertAI_279	// 	ガードシェア
	ADRS  ExpertAI_280	// 	パワーシェア

	ADRS  ExpertAI_281	// 	ワンダールーム
	ADRS  ExpertAI_282	// 	サイコショック
	ADRS  ExpertAI_283	// 	ベノムショック
	ADRS  ExpertAI_284	//	ボディパージ 	
	ADRS  ExpertAI_285	// 	テレキネシス
	ADRS  ExpertAI_286	// 	マジックルーム
	ADRS  ExpertAI_287	// 	うちおとす
	ADRS  ExpertAI_288	// 	やまあらし
	ADRS  ExpertAI_289	// 	はじけるほのお
	ADRS  ExpertAI_290	// 	ちょうのまい

	ADRS  ExpertAI_291	// 	ヘビーボンバー
	ADRS  ExpertAI_292	// 	シンクロノイズ
	ADRS  ExpertAI_293	// 	エレクトボール
	ADRS  ExpertAI_294	// 	みずびたし
	ADRS  ExpertAI_295	// 	ニトロチャージ
	ADRS  ExpertAI_296	// 	アシッドボム
	ADRS  ExpertAI_297	// 	イカサマ
	ADRS  ExpertAI_298	// 	シンプルビーム
	ADRS  ExpertAI_299	// 	なかまづくり
	ADRS  ExpertAI_300	// 	おさきにどうぞ

	ADRS  ExpertAI_301	// 	りんしょう
	ADRS  ExpertAI_302	// 	エコーボイス
	ADRS  ExpertAI_303	// 	なしくずし
	ADRS  ExpertAI_304	// 	クリアスモッグ
	ADRS  ExpertAI_305	// 	アシストパワー
	ADRS  ExpertAI_306	// 	ファストガード
	ADRS  ExpertAI_307	// 	サイドチェンジ
	ADRS  ExpertAI_308	// 	からをやぶる
	ADRS  ExpertAI_309	// 	いやしのはどう
	ADRS  ExpertAI_310	// 	たたりめ

	ADRS  ExpertAI_311	// 	フリーフォール
	ADRS  ExpertAI_312	// 	ギアチェンジ
	ADRS  ExpertAI_313	// 	ともえなげ
	ADRS  ExpertAI_314	// 	やきつくす
	ADRS  ExpertAI_315	// 	さきおくり
	ADRS  ExpertAI_316	// 	せいちょう
	ADRS  ExpertAI_317	// 	アクロバット
	ADRS  ExpertAI_318	// 	ミラータイプ
	ADRS  ExpertAI_319	// 	かたきうち
	ADRS  ExpertAI_320	// 	いのちがけ

	ADRS  ExpertAI_321	// 	ほたるび
	ADRS  ExpertAI_322	// 	とぐろをまく
	ADRS  ExpertAI_323	// 	ギフトパス
	ADRS  ExpertAI_324	// 	みずのちかい
	ADRS  ExpertAI_325	// 	ほのおのちかい
	ADRS  ExpertAI_326	// 	くさのちかい
	ADRS  ExpertAI_327	// 	ふるいたてる
	ADRS  ExpertAI_328	// 	コットンガード
	ADRS  ExpertAI_329	// 	いにしえのうた
	ADRS  ExpertAI_330	// 	こごえるせかい

	ADRS  ExpertAI_331	// 	フリーズボルト
	ADRS  ExpertAI_332	// 	コールドフレア
	ADRS  ExpertAI_333	// 	どなりつける
	ADRS  ExpertAI_Dummy	// 	Ｖジェネレート
	ADRS  ExpertAI_Dummy	// 	フレイムソウル
	ADRS  ExpertAI_Dummy	// 	サンダーソウル
	ADRS  ExpertAI_337	// 	ぼうふう  2010.5.20

ExpertAI_Dummy:		// ダミー	
ExpertAISeq_end:
	AIEND

//---------------------------------------------------------------------------


ExpertAI_1:	//眠らせる
	//アタック側の誤りと思われるので変更：DEFENCE→ATTACKへ2006.6.6
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,8,ExpertAI_1_4	//ゆめくい
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,107,ExpertAI_1_4	//あくむ
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,97,ExpertAI_1_2	//ねごと
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,183,ExpertAI_1_3	//マジックコート
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,92,ExpertAI_1_3	//いびき

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_NOOGAADO,ExpertAI_1_4	  	// ノーガード
	IF_EQUAL	TOKUSYU_HAYAOKI,ExpertAI_1_3	  	// はやおき

	JUMP		ExpertAI_1_end

ExpertAI_1_2:
	INCDEC		-1
ExpertAI_1_3:
	IF_RND_UNDER	128,ExpertAI_1_end
	INCDEC		-1
	JUMP		ExpertAI_1_end

ExpertAI_1_4:
	IF_RND_UNDER	128,ExpertAI_1_end
	INCDEC		+1

ExpertAI_1_end:
	AIEND

//---------------------------------------------------------------------------

ExpertAI_3:	//すいとる
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_3_1
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_3_1
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_3_1

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_HEDOROEKI,ExpertAI_3_1      //相手の特性がヘドロえき

	JUMP		ExpertAI_3_2
	
ExpertAI_3_1:
	IF_RND_UNDER	50,ExpertAI_3_2
	INCDEC		-3

ExpertAI_3_2:
	AIEND

//---------------------------------------------------------------------------
	
ExpertAI_168:	//おきみやげ
ExpertAI_7:	//じばく

	//１）守備側の回避率が７以上ならば-1して２へ
	//１’）さらに守備側の回避率が10以上〜ならば128/255で-1
	//２）攻撃側が80以上で相手より攻撃が早い場合は50/255の確率で-3で終了
	//３）攻撃側のＨＰが50以下なら128/255で-1
	IF_PARA_UNDER	CHECK_DEFENCE,PARA_AVOID,7,ExpertAI_7_1
	INCDEC		-1
	IF_PARA_UNDER	CHECK_DEFENCE,PARA_AVOID,10,ExpertAI_7_1
	IF_RND_UNDER	128,ExpertAI_7_1
	INCDEC		-1

ExpertAI_7_1:
	IF_HP_UNDER	CHECK_ATTACK,80,ExpertAI_7_2
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_7_2

	IF_COMMONRND_UNDER	50,ExpertAI_7_5
	JUMP		AI_DEC3

ExpertAI_7_2:
	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_7_4
	IF_COMMONRND_OVER	128,ExpertAI_7_3

	INCDEC		+1
ExpertAI_7_3:
	IF_HP_OVER	CHECK_ATTACK,30,ExpertAI_7_5
	IF_COMMONRND_OVER	128,ExpertAI_7_5
	INCDEC		+1
	JUMP		ExpertAI_7_5

ExpertAI_7_4:
	IF_COMMONRND_UNDER	50,ExpertAI_7_5
	INCDEC		-1
ExpertAI_7_5:
	AIEND


ExpertAI_7_poke_table:


//---------------------------------------------------------------------------

ExpertAI_8:	//ゆめくい
	//技相性が1/4、1/2なら-1
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_8_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_8_ng1
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_8_ng1

  CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_HEDOROEKI,ExpertAI_8_ng             //相手の特性がヘドロえき

	IF_WAZASICK		CHECK_DEFENCE,WAZASICK_NEMURI,ExpertAI_8_ok
	JUMP		ExpertAI_8_end

ExpertAI_8_ok:
	IF_RND_UNDER	51,ExpertAI_8_end
	INCDEC		+3
	JUMP		ExpertAI_8_end

ExpertAI_8_ng:
	IF_RND_UNDER	50,ExpertAI_8_ng1
	INCDEC		-2
ExpertAI_8_ng1:
	IF_RND_UNDER	50,ExpertAI_8_end
	INCDEC		-1
ExpertAI_8_end:
	AIEND

//---------------------------------------------------------------------------

ExpertAI_9:	//オウム
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_9_1

	CHECK_LAST_WAZA	CHECK_DEFENCE
	IFN_TABLE_JUMP	ExpertAI_9_Table,ExpertAI_9_1
	
	IF_RND_UNDER	128,ExpertAI_9_3
	INCDEC		2	//強い技は128/255オウムがえしする
	JUMP		ExpertAI_9_3

ExpertAI_9_1://相手の方が早い場合強い技でなければ減算する
	CHECK_LAST_WAZA	CHECK_DEFENCE
	IF_TABLE_JUMP	ExpertAI_9_Table,ExpertAI_9_3

	IF_RND_UNDER	80,ExpertAI_9_3
	INCDEC		-1
ExpertAI_9_3:
	AIEND

//	.ALIGN
ExpertAI_9_Table://変化して強そうな技群
	.long	79,142,147,95,47,320,325,28,108,92
	.long	12,32,90,329,238,177,109,186,103,178
	.long	184,313,319,86,137,77,247,223,63,245
	.long	168,343,213,207,259,260,271,276,285	
	//以降追加2006.6.14
	//2006.6.14
	.long	375,384,385,389,391,415,445,464	
	//2010.2.20
	.long	549,555
	.long	0xffffffff

//---------------------------------------------------------------------------
ExpertAI_50:	// 攻撃力2アップ
ExpertAI_10:	// 攻撃力アップ
	IF_PARA_UNDER	CHECK_ATTACK,PARA_POW,9,ExpertAI_10_1
	IF_RND_UNDER	100,ExpertAI_10_3
	INCDEC		-1
	JUMP		ExpertAI_10_3
ExpertAI_10_1:
	IFN_HP_EQUAL	CHECK_ATTACK,100,ExpertAI_10_3

	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,127,ExpertAI_10_2		// バトンタッチ系

	IF_RND_UNDER	128,ExpertAI_10_3
ExpertAI_10_2:
	INCDEC		2
ExpertAI_10_3:
	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_10_end
	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_10_4

	IF_RND_UNDER	40,ExpertAI_10_end
ExpertAI_10_4:
	INCDEC		-2
ExpertAI_10_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_322:			// 	とぐろをまく
ExpertAI_208:	// ビルドアップ
ExpertAI_51:	// 防御力2アップ
ExpertAI_11:	// 防御力アップ
	IF_PARA_UNDER	CHECK_ATTACK,PARA_DEF,9,ExpertAI_11_1
	IF_RND_UNDER	100,ExpertAI_11_2
	INCDEC		-1
	JUMP		ExpertAI_11_2
ExpertAI_11_1:
	IFN_HP_EQUAL	CHECK_ATTACK,100,ExpertAI_11_2_2

	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,127,ExpertAI_11_2		// バトンタッチ系

	IF_RND_UNDER	128,ExpertAI_11_2_2
ExpertAI_11_2:
	INCDEC		2
ExpertAI_11_2_2:
	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_11_3

	IF_RND_UNDER	200,ExpertAI_11_end

ExpertAI_11_3:
	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_11_5

	CHECK_LAST_WAZA	CHECK_DEFENCE
	CHECK_WORKWAZA_POW
	IF_EQUAL	0,ExpertAI_11_4

	//追加：相手が最後に出した技が特殊ならば出しにくくする
	//2006.6.14
	CHECK_LAST_WAZA_KIND
	IF_EQUAL	WAZADATA_DMG_SPECIAL,ExpertAI_11_5//特殊
	

	IF_RND_UNDER	60,ExpertAI_11_end
ExpertAI_11_4:
	IF_RND_UNDER	60,ExpertAI_11_end
ExpertAI_11_5:
	INCDEC		-2
ExpertAI_11_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_284:	//	ボディパージ 	

	IFN_HAVE_WAZA_SEQNO	CHECK_DEFENCE,196,ExpertAI_12	    	// くさむすび系

	IF_RND_UNDER	60,ExpertAI_12

	INCDEC		1

ExpertAI_52:	// 素早さ2アップ
ExpertAI_12:	// 素早さアップ
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_12_1
	INCDEC		-3
	JUMP		ExpertAI_12_end

ExpertAI_12_1:
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,31,ExpertAI_12_2		// ひるみ系
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,32,ExpertAI_12_2		// 回復系
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,37,ExpertAI_12_2		// ねむる
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,132,ExpertAI_12_2		// こうごうせい
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,214,ExpertAI_12_2		// はねやすめ
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,79,ExpertAI_12_2		// みがわり
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,91,ExpertAI_12_2		// いたみわけ
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,150,ExpertAI_12_2		// ひるみ系（ふみつけ系）
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,98,ExpertAI_12_2		// みちづれ系
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,127,ExpertAI_12_2		// バトンタッチ系
	JUMP		ExpertAI_12_3

ExpertAI_12_2:
	IF_RND_UNDER	70,ExpertAI_12_3
	INCDEC		2
ExpertAI_12_3:
	IF_RND_UNDER	70,ExpertAI_12_end
	INCDEC		2
ExpertAI_12_end:
	AIEND
	
//---------------------------------------------------------------------------
ExpertAI_328:			// 	コットンガード
ExpertAI_321:			// 	ほたるび
ExpertAI_53:	// 特攻2アップ
ExpertAI_13:	// 特攻アップ
	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEPOW,9,ExpertAI_13_1
	IF_RND_UNDER	100,ExpertAI_13_2
	INCDEC		-1
	JUMP		ExpertAI_13_2
ExpertAI_13_1:
	IFN_HP_EQUAL	CHECK_ATTACK,100,ExpertAI_13_2

	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,127,ExpertAI_13_1_2		// バトンタッチ系

	IF_RND_UNDER	128,ExpertAI_13_2
ExpertAI_13_1_2:
	INCDEC		2
ExpertAI_13_2:
	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_13_end
	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_13_3

	IF_RND_UNDER	70,ExpertAI_13_end
ExpertAI_13_3:
	INCDEC		-2
ExpertAI_13_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_211:	// めいそう
ExpertAI_206:	// コスモパワー
ExpertAI_54:	// 特防2アップ
ExpertAI_14:	// 特防アップ
	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEDEF,9,ExpertAI_14_1
	IF_RND_UNDER	100,ExpertAI_14_2
	INCDEC		-1
	JUMP		ExpertAI_14_2
ExpertAI_14_1:
	IFN_HP_EQUAL	CHECK_ATTACK,100,ExpertAI_14_2

	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,127,ExpertAI_14_1_2		// バトンタッチ系

	IF_RND_UNDER	128,ExpertAI_14_2
ExpertAI_14_1_2:
	INCDEC		2
ExpertAI_14_2:
	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_14_3

	IF_RND_UNDER	200,ExpertAI_14_end

ExpertAI_14_3:
	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_14_5

	CHECK_LAST_WAZA	CHECK_DEFENCE
	CHECK_WORKWAZA_POW
	IF_EQUAL	0,ExpertAI_14_4

	//追加：相手が最後に出した技が物理ならば出しにくくする
	//2006.6.14
	CHECK_LAST_WAZA_KIND
	IF_EQUAL	WAZADATA_DMG_PHYSIC,ExpertAI_14_5//物理
	
	IF_RND_UNDER	60,ExpertAI_14_end

ExpertAI_14_4:
	IF_RND_UNDER	60,ExpertAI_14_end
ExpertAI_14_5:
	INCDEC		-2
ExpertAI_14_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_55:	// 命中率2アップ	（未使用）
ExpertAI_15:	// 命中率アップ	（未使用）
	IF_PARA_UNDER	CHECK_ATTACK,PARA_HIT,9,ExpertAI_15_1
	IF_RND_UNDER	50,ExpertAI_15_1
	INCDEC		-2
ExpertAI_15_1:
	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_15_end

	INCDEC		-2
ExpertAI_15_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_108:	// 小さくなる
ExpertAI_56:	// 回避率2アップ
ExpertAI_16:	// 回避率アップ
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_NOOGAADO,ExpertAI_16_ng	  	// ノーガード
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_NOOGAADO,ExpertAI_16_ng	  	// ノーガード

	CHECK_LAST_WAZA	CHECK_DEFENCE
	CHECK_WORKWAZA_SEQNO
	IF_EQUAL	17,ExpertAI_16_ng		// 必中技
	IF_EQUAL	235,ExpertAI_16_ng		// きりふだ
	IF_EQUAL	272,ExpertAI_16_ng		// シャドーダイブ

	IF_WAZASICK	CHECK_ATTACK,WAZASICK_NOROI,ExpertAI_16_ng  	// のろい中
	IF_WAZASICK	CHECK_ATTACK,WAZASICK_MIYABURU,ExpertAI_16_ng	//みやぶる中

	CHECK_WEATHER
	IFN_EQUAL	WEATHER_AME,ExpertAI_16_arare		//　あめ中

	CHECK_LAST_WAZA	CHECK_DEFENCE
	CHECK_WORKWAZA_SEQNO            //　←抜けてた 2010/4/21
	IF_EQUAL	152,ExpertAI_16_ng		// かみなり必中

ExpertAI_16_arare:
	CHECK_WEATHER
	IFN_EQUAL	WEATHER_ARARE,ExpertAI_16_weather_end		//　あられ中

	CHECK_LAST_WAZA	CHECK_DEFENCE
	CHECK_WORKWAZA_SEQNO            //　←抜けてた 2010/4/21
	IF_EQUAL	260,ExpertAI_16_ng		// ふぶき必中

ExpertAI_16_weather_end:

	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,32,ExpertAI_16_1		// じこさいせい
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,132,ExpertAI_16_1		// あさのひざし
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,156,ExpertAI_16_1		// ねむる
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,162,ExpertAI_16_1		// のみこむ
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,214,ExpertAI_16_1		// はねやすめ

	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,ExpertAI_16_1	// マジックガード

	IF_WAZASICK	CHECK_ATTACK,WAZASICK_YADORIGI,ExpertAI_16_atk_ng	//やどりぎ

	IF_DOKUDOKU	CHECK_ATTACK,ExpertAI_16_atk_doku	                 // どくどく
	IF_WAZASICK	CHECK_ATTACK,WAZASICK_DOKU,ExpertAI_16_atk_doku  	  // どく
	IF_WAZASICK	CHECK_ATTACK,WAZASICK_YAKEDO,ExpertAI_16_atk_ng  	// やけど

	JUMP	ExpertAI_16_1

ExpertAI_16_atk_doku:
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_POIZUNHIIRU,ExpertAI_16_atk_doku_ok		// ポイズンヒール	

	JUMP	ExpertAI_16_atk_ng

ExpertAI_16_atk_doku_ok:
	IF_RND_UNDER	50,ExpertAI_16_1
	INCDEC		1
	JUMP	ExpertAI_16_1

ExpertAI_16_atk_ng:
	IF_RND_UNDER	50,ExpertAI_16_1
	INCDEC		-1
ExpertAI_16_1:

	IF_WAZASICK	CHECK_ATTACK,WAZASICK_NEWOHARU,ExpertAI_16_atk_ok  //ねをはる
	IFN_WAZASICK	CHECK_ATTACK,WAZASICK_AQUARING,ExpertAI_16_atk_ok  //アクアリング
	IF_HAVE_ITEM	CHECK_ATTACK, ITEM_TABENOKOSI,ExpertAI_16_atk_ok  //　たべのこし

	IFN_EQUAL	WEATHER_AME,ExpertAI_16_2		//　あめ中

	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_KANSOUHADA,ExpertAI_16_atk_ok		// かんそうはだ	
	IF_EQUAL	TOKUSYU_AMEUKEZARA,ExpertAI_16_atk_ok		// あめうけざら
	JUMP	ExpertAI_16_2

ExpertAI_16_atk_ok:
	IF_RND_UNDER	50,ExpertAI_16_2
	INCDEC		1
ExpertAI_16_2:

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,ExpertAI_16_3	// マジックガード

	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_NOROI,ExpertAI_16_def_ok	// のろい

	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,32,ExpertAI_16_3		// じこさいせい
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,132,ExpertAI_16_3		// あさのひざし
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,156,ExpertAI_16_3		// ねむる
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,162,ExpertAI_16_3		// のみこむ
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,214,ExpertAI_16_3		// はねやすめ

	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_YADORIGI,ExpertAI_16_def_ok	//やどりぎ

	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_YAKEDO,ExpertAI_16_def_ok  	// やけど
	IF_DOKUDOKU	CHECK_DEFENCE,ExpertAI_16_def_doku	                 // どくどく
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_DOKU,ExpertAI_16_def_doku  	  // どく

	IFN_EQUAL	WEATHER_HARE,ExpertAI_16_3		//　はれ中

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_KANSOUHADA,ExpertAI_16_def_ok		// かんそうはだ	

	JUMP	ExpertAI_16_3

ExpertAI_16_def_doku:
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_POIZUNHIIRU,ExpertAI_16_3		// ポイズンヒール	

	JUMP	ExpertAI_16_def_ok

ExpertAI_16_def_ok:
	IF_RND_UNDER	50,ExpertAI_16_3
	INCDEC		1
	JUMP	ExpertAI_16_3

ExpertAI_16_3:
	IF_PARA_UNDER	CHECK_ATTACK,PARA_AVOID,9,ExpertAI_16_4

	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_16_4
	IF_RND_UNDER	50,ExpertAI_16_4
	INCDEC		-1

ExpertAI_16_4:
	IFN_HAVE_WAZA_SEQNO	CHECK_ATTACK,127,ExpertAI_16_5		// バトンタッチ系

	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_16_5
	IF_RND_UNDER	50,ExpertAI_16_5
	INCDEC		1
ExpertAI_16_5:
	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_16_end

	IF_RND_UNDER	70,ExpertAI_16_end
ExpertAI_16_ng:
	INCDEC		-2
ExpertAI_16_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_17:	//かならずあたるok
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,10,ExpertAI_17_1
	IF_PARA_UNDER	CHECK_ATTACK,PARA_HIT,2,ExpertAI_17_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,8,ExpertAI_17_2
	IF_PARA_UNDER	CHECK_ATTACK,PARA_HIT,4,ExpertAI_17_2
	JUMP		ExpertAI_17_end

ExpertAI_17_1:
	INCDEC		1
ExpertAI_17_2:
	IF_RND_UNDER	100,ExpertAI_17_end
	INCDEC		1

ExpertAI_17_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_58:	// 攻撃力2ダウン
ExpertAI_18:	// 攻撃力ダウン
	IF_PARA_EQUAL	CHECK_DEFENCE,PARA_POW,6,ExpertAI_18_2

	INCDEC		-1

	IF_HP_OVER	CHECK_ATTACK,90,ExpertAI_18_1

	INCDEC		-1

ExpertAI_18_1:
	IF_PARA_OVER	CHECK_DEFENCE,PARA_POW,3,ExpertAI_18_2
	IF_RND_UNDER	50,ExpertAI_18_2
	INCDEC		-2
ExpertAI_18_2:
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_18_3

	INCDEC		-2

ExpertAI_18_3:

	//タイプチェックやらない2006.6.14
	//-----------------------------------------------
	//CHECK_TYPE	CHECK_DEFENCE_TYPE1
	//IF_TABLE_JUMP	ExpertAI_18_Table,ExpertAI_18_end
	//CHECK_TYPE	CHECK_DEFENCE_TYPE2
	//IF_TABLE_JUMP	ExpertAI_18_Table,ExpertAI_18_end
	//-----------------------------------------------
	CHECK_LAST_WAZA_KIND
	IFN_EQUAL	WAZADATA_DMG_SPECIAL,ExpertAI_18_end//特殊でない
	IF_RND_UNDER	128,ExpertAI_18_end
	INCDEC		-2

ExpertAI_18_end:
	AIEND

//相手のポケモンのタイプで物理、特殊の判別は厳しい
ExpertAI_18_Table:
	.long	POKETYPE_NORMAL
	.long	POKETYPE_KAKUTOU
	.long	POKETYPE_JIMEN
	.long	POKETYPE_IWA
	.long	POKETYPE_MUSHI
	.long	POKETYPE_HAGANE
	.long	0xffffffff

//---------------------------------------------------------------------------
ExpertAI_205:	// くすぐる
ExpertAI_59:	// 防御力2ダウン
ExpertAI_19:	// 防御力ダウン
	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_19_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_DEF,3,ExpertAI_19_2
ExpertAI_19_1:
	IF_RND_UNDER	50,ExpertAI_19_2
	INCDEC		-2
ExpertAI_19_2:
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_19_end

	INCDEC		-2
ExpertAI_19_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_70:	// 追加素早さダウン
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_70_end		//2008.6.21追加
	IF_WAZANO	WAZANO_KOGOERUKAZE,ExpertAI_20	// 100%追加
	IF_WAZANO	WAZANO_GANSEKIHUUZI,ExpertAI_20	// 100%追加
	IF_WAZANO	WAZANO_MADDOSYOTTO,ExpertAI_20	// 100%追加
	IF_WAZANO	WAZANO_ROOKIKKU,ExpertAI_20	// 100%追加
	IF_WAZANO	WAZANO_EREKINETTO,ExpertAI_20	// 100%追加
	IF_WAZANO	WAZANO_ZINARASI,ExpertAI_20	// 100%追加
	IF_WAZANO	WAZANO_KOGOERUSEKAI,ExpertAI_20	// 100%追加
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_70_end	//2008.6.21追加
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_70_end	//2008.6.21追加
	AIEND

ExpertAI_70_end:
	AIEND

	
//---------------------------------------------------------------------------

ExpertAI_60:	// 追加素早さ2ダウン
ExpertAI_20:	// 素早さダウン
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_20_1
	INCDEC		-3
	JUMP		ExpertAI_20_end

ExpertAI_20_1:
	IF_RND_UNDER	70,ExpertAI_20_end

	INCDEC		2
ExpertAI_20_end:
	AIEND
//---------------------------------------------------------------------------

ExpertAI_61:	// 特攻2ダウン
ExpertAI_21:	// 特攻ダウン
	IF_PARA_EQUAL	CHECK_DEFENCE,PARA_SPEPOW,6,ExpertAI_21_2

	INCDEC		-1
	IF_HP_OVER	CHECK_ATTACK,90,ExpertAI_21_1

	INCDEC		-1
ExpertAI_21_1:
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEPOW,3,ExpertAI_21_2
	IF_RND_UNDER	50,ExpertAI_21_2
	INCDEC		-2
ExpertAI_21_2:
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_21_3

	INCDEC		-2
ExpertAI_21_3:
	
	//タイプチェックをやらない2006.6.14
	//-----------------------------------------------
	//CHECK_TYPE	CHECK_DEFENCE_TYPE1
	//IF_TABLE_JUMP	ExpertAI_21_Table,ExpertAI_21_end
	//CHECK_TYPE	CHECK_DEFENCE_TYPE2
	//IF_TABLE_JUMP	ExpertAI_21_Table,ExpertAI_21_end
	//-----------------------------------------------
	
	CHECK_LAST_WAZA_KIND
	IFN_EQUAL	WAZADATA_DMG_PHYSIC,ExpertAI_21_end//物理でない
	IF_RND_UNDER	128,ExpertAI_21_end
	INCDEC		-2

ExpertAI_21_end:
	AIEND

ExpertAI_21_Table:
	.long	POKETYPE_HONOO
	.long	POKETYPE_MIZU
	.long	POKETYPE_KUSA
	.long	POKETYPE_DENKI
	.long	POKETYPE_ESPER
	.long	POKETYPE_KOORI
	.long	POKETYPE_DRAGON
	.long	POKETYPE_AKU
	.long	0xffffffff

//---------------------------------------------------------------------------
ExpertAI_62:	// 特防2ダウン
ExpertAI_22:	// 特防ダウン
	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_22_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEDEF,3,ExpertAI_22_2
ExpertAI_22_1:
	IF_RND_UNDER	50,ExpertAI_22_2
	INCDEC		-2
ExpertAI_22_2:
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_22_end

	INCDEC		-2
ExpertAI_22_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_63:	//命中2下げる
ExpertAI_23:	//命中下げる
	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_23_2
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_23_3

ExpertAI_23_2:
	IF_RND_UNDER	100,ExpertAI_23_3
	INCDEC		-1
ExpertAI_23_3:
	IF_PARA_OVER	CHECK_ATTACK,PARA_HIT,4,ExpertAI_23_4

	IF_RND_UNDER	80,ExpertAI_23_4
	INCDEC		-2

ExpertAI_23_4:
	IFN_DOKUDOKU	CHECK_DEFENCE,ExpertAI_23_5
	IF_RND_UNDER	70,ExpertAI_23_5
	INCDEC		2

ExpertAI_23_5:
	IFN_WAZASICK	CHECK_DEFENCE,WAZASICK_YADORIGI,ExpertAI_23_6
	IF_RND_UNDER	70,ExpertAI_23_6
	INCDEC		2

ExpertAI_23_6:
	IFN_WAZASICK	CHECK_ATTACK,WAZASICK_NEWOHARU,ExpertAI_23_6_1
	IF_RND_UNDER	128,ExpertAI_23_7
	INCDEC		1
	JUMP	ExpertAI_23_7

//アクアリング追加2006.6.14
ExpertAI_23_6_1:
	IFN_WAZASICK	CHECK_ATTACK,WAZASICK_AQUARING,ExpertAI_23_7
	IF_RND_UNDER	128,ExpertAI_23_7
	INCDEC		1
	
ExpertAI_23_7:
	IFN_WAZASICK	CHECK_DEFENCE,WAZASICK_NOROI,ExpertAI_23_8	// のろい
	IF_RND_UNDER	70,ExpertAI_23_8
	INCDEC		2
ExpertAI_23_8:
	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_23_end

	IF_PARA_EQUAL	CHECK_DEFENCE,PARA_HIT,6,ExpertAI_23_end

	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_23_9
	IF_HP_UNDER	CHECK_DEFENCE,40,ExpertAI_23_9
	
	IF_RND_UNDER	70,ExpertAI_23_end

ExpertAI_23_9:
	INCDEC		-2
ExpertAI_23_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_64:	// 回避率2ダウン
ExpertAI_24:	// 回避率ダウン
	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_24_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,3,ExpertAI_24_2
ExpertAI_24_1:
	IF_RND_UNDER	50,ExpertAI_24_2
	INCDEC		-2
ExpertAI_24_2:
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_24_end

	INCDEC		-2
ExpertAI_24_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_25:	//くろいきり
	IF_PARA_OVER	CHECK_ATTACK,PARA_POW,8,ExpertAI_25_1
	IF_PARA_OVER	CHECK_ATTACK,PARA_DEF,8,ExpertAI_25_1
	IF_PARA_OVER	CHECK_ATTACK,PARA_SPEPOW,8,ExpertAI_25_1
	IF_PARA_OVER	CHECK_ATTACK,PARA_SPEDEF,8,ExpertAI_25_1
	IF_PARA_OVER	CHECK_ATTACK,PARA_AVOID,8,ExpertAI_25_1

	IF_PARA_UNDER	CHECK_DEFENCE,PARA_POW,4,ExpertAI_25_1
	IF_PARA_UNDER	CHECK_DEFENCE,PARA_DEF,4,ExpertAI_25_1
	IF_PARA_UNDER	CHECK_DEFENCE,PARA_SPEPOW,4,ExpertAI_25_1
	IF_PARA_UNDER	CHECK_DEFENCE,PARA_SPEDEF,4,ExpertAI_25_1
	IF_PARA_UNDER	CHECK_DEFENCE,PARA_HIT,4,ExpertAI_25_1
	JUMP		ExpertAI_25_2	

ExpertAI_25_1:	
	IF_RND_UNDER	50,ExpertAI_25_2

	INCDEC		-3

ExpertAI_25_2:	
	IF_PARA_OVER	CHECK_DEFENCE,PARA_POW,8,ExpertAI_25_3
	IF_PARA_OVER	CHECK_DEFENCE,PARA_DEF,8,ExpertAI_25_3
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEPOW,8,ExpertAI_25_3
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEDEF,8,ExpertAI_25_3
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,8,ExpertAI_25_3

	IF_PARA_UNDER	CHECK_ATTACK,PARA_POW,4,ExpertAI_25_3
	IF_PARA_UNDER	CHECK_ATTACK,PARA_DEF,4,ExpertAI_25_3
	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEPOW,4,ExpertAI_25_3
	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEDEF,4,ExpertAI_25_3
	IF_PARA_UNDER	CHECK_ATTACK,PARA_HIT,4,ExpertAI_25_3

	IF_RND_UNDER	50,ExpertAI_25_4
	INCDEC		-1
	JUMP		ExpertAI_25_4	

ExpertAI_25_3:	
	IF_RND_UNDER	50,ExpertAI_25_4

	INCDEC		3
ExpertAI_25_4:	

	AIEND

//---------------------------------------------------------------------------

ExpertAI_26:	//がまん
	IF_HP_OVER	CHECK_ATTACK,90,ExpertAI_26_1

	INCDEC		-2
ExpertAI_26_1:	
	AIEND

//---------------------------------------------------------------------------
ExpertAI_313:			// 	ともえなげ
ExpertAI_28:	//ふきとばし

	CHECK_SLOWSTART_TURN	CHECK_DEFENCE	//スロースタート
	IF_OVER	3,	ExpertAI_28_ok1
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_MAKIBISI,ExpertAI_28_1	// まきびし中
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_STEALTHROCK,ExpertAI_28_1	// ステルスロック中
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_DOKUBISI,ExpertAI_28_1	// どくびし中

	IF_PARA_OVER	CHECK_DEFENCE,PARA_POW,8,ExpertAI_28_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_DEF,8,ExpertAI_28_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEPOW,8,ExpertAI_28_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEDEF,8,ExpertAI_28_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,8,ExpertAI_28_1

	INCDEC		-3
	JUMP		ExpertAI_28_end

ExpertAI_28_ok1:
	IF_RND_UNDER	64,ExpertAI_28_1
	INCDEC		2

ExpertAI_28_1:	
	IF_RND_UNDER	128,ExpertAI_28_end
	
	INCDEC		2
ExpertAI_28_end:	

	AIEND
//---------------------------------------------------------------------------
ExpertAI_30:	//テクスチャー
	IF_HP_OVER	CHECK_ATTACK,90,ExpertAI_30_1

	INCDEC		-2
ExpertAI_30_1:	
	CHECK_TURN
	IF_EQUAL	0,ExpertAI_30_end
	
	IF_RND_UNDER	200,AI_DEC2
ExpertAI_30_end:	
	AIEND
//---------------------------------------------------------------------------
ExpertAI_132:	//あさのひざし
ExpertAI_133:	//こうごうせい
ExpertAI_134:	//つきのひかり
	CHECK_WEATHER
	IF_EQUAL	WEATHER_ARARE,ExpertAI_134_1		//　あられ中
	IF_EQUAL	WEATHER_AME,ExpertAI_134_1		//　あめ中
	IF_EQUAL	WEATHER_SUNAARASHI,ExpertAI_134_1	//　すなあらし中
	JUMP		ExpertAI_32

ExpertAI_134_1:
	INCDEC		-2

ExpertAI_214:	// はねやすめ
ExpertAI_157:	// たまごうみ
ExpertAI_162:	// のみこむ
ExpertAI_32:	//じこさいせい
	IF_HP_EQUAL	CHECK_ATTACK,100,ExpertAI_32_2

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_32_3

	INCDEC		-8
	JUMP		ExpertAI_32_end

ExpertAI_32_1:	
	IF_HP_UNDER	CHECK_ATTACK,50,ExpertAI_32_4
	IF_HP_OVER	CHECK_ATTACK,80,ExpertAI_32_2

	IF_RND_UNDER	70,ExpertAI_32_4
ExpertAI_32_2:	
	INCDEC		-3
	JUMP		ExpertAI_32_end

ExpertAI_32_3:	
	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_32_4
	IF_RND_UNDER	30,ExpertAI_32_4
	INCDEC		-3
	JUMP		ExpertAI_32_end

ExpertAI_32_4:	
	IFN_HAVE_WAZA_SEQNO	CHECK_DEFENCE,195,ExpertAI_32_5		// よこどり

	IF_RND_UNDER	100,ExpertAI_32_end
ExpertAI_32_5:	
	IF_RND_UNDER	20,ExpertAI_32_end

	INCDEC		2

ExpertAI_32_end:	
	AIEND
//---------------------------------------------------------------------------
ExpertAI_84:	// やどりぎ
ExpertAI_33:	//どくどく
	IFN_HAVE_DAMAGE_WAZA	ExpertAI_33_2

	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_33_1		// HPが1/2以上

	IF_RND_UNDER	50,ExpertAI_33_1
	INCDEC		-3
ExpertAI_33_1:	
	IF_HP_OVER	CHECK_DEFENCE,50,ExpertAI_33_2		// HPが1/2以上

	IF_RND_UNDER	50,ExpertAI_33_2
	INCDEC		-3
ExpertAI_33_2:	
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,14,ExpertAI_33_3	// 回避率アップ
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,111,ExpertAI_33_3	// まもる
	JUMP		ExpertAI_33_end

ExpertAI_33_3:	
	IF_RND_UNDER	60,ExpertAI_33_end
	INCDEC		2

ExpertAI_33_end:	

	AIEND
//---------------------------------------------------------------------------
ExpertAI_35:	//ひかりのかべ 2010/4/21
	
	IF_HP_UNDER	CHECK_ATTACK,50,ExpertAI_35_2		// HPが1/2以下
	
	IF_HP_UNDER	CHECK_ATTACK,90,ExpertAI_35_1		// HP90未満
	IF_RND_UNDER	30,ExpertAI_35_1
	INCDEC		1

ExpertAI_35_1:
  IF_DMG_PHYSIC_OVER CHECK_DEFENCE,ExpertAI_35_2           //攻撃の方が高い場合はやらない

	IF_RND_UNDER	64,ExpertAI_35_end
	INCDEC		1
	JUMP	ExpertAI_35_end
	
ExpertAI_35_2:
	IF_RND_UNDER	30,ExpertAI_35_end
	INCDEC		-2
ExpertAI_35_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_37:	//ねむる
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_37_3
	IFN_HP_EQUAL	CHECK_ATTACK,100,ExpertAI_37_1

	INCDEC		-8
	JUMP		ExpertAI_37_end

ExpertAI_37_1:	
	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_37_5
	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_37_2

	IF_RND_UNDER	70,ExpertAI_37_5
ExpertAI_37_2:	
	INCDEC		-3
	JUMP		ExpertAI_37_end

ExpertAI_37_3:	
	IF_HP_UNDER	CHECK_ATTACK,60,ExpertAI_37_5
	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_37_4
	IF_RND_UNDER	50,ExpertAI_37_5
ExpertAI_37_4:	
	INCDEC		-3
	JUMP		ExpertAI_37_end

ExpertAI_37_5:	
	IFN_HAVE_WAZA_SEQNO	CHECK_DEFENCE,195,ExpertAI_37_6		// よこどり

	IF_RND_UNDER	50,ExpertAI_37_end
ExpertAI_37_6:	
	IF_RND_UNDER	10,ExpertAI_37_end

	INCDEC		3

ExpertAI_37_end:	
	AIEND
//---------------------------------------------------------------------------
ExpertAI_38:	//いちげきひっさつ
	IF_RND_UNDER	192,ExpertAI_38_end
	INCDEC		+1
ExpertAI_38_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_40:	//いかりのまえば
	IF_HP_OVER	CHECK_DEFENCE,60,ExpertAI_40_end

	INCDEC		-1
ExpertAI_40_end:	
	AIEND
//---------------------------------------------------------------------------
ExpertAI_106:	// くろいまなざし
ExpertAI_42:	//しめつけ
	IF_DOKUDOKU	CHECK_DEFENCE,                      ExpertAI_42_inc
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_NOROI,       ExpertAI_42_inc	// のろい
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_HOROBINOUTA, ExpertAI_42_inc	// ほろび
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_MEROMERO,    ExpertAI_42_inc	//メロメロ
	JUMP		ExpertAI_42_end
ExpertAI_42_inc:	

	IF_RND_UNDER	128,ExpertAI_42_end
	INCDEC		1
ExpertAI_42_end:	
	AIEND
//---------------------------------------------------------------------------
ExpertAI_43:	//クリティカルでやすい
ExpertAI_200:	//クリティカルでやすい
ExpertAI_209:	//クリティカルでやすい
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_43_end
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_43_end
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_43_end

	CHECK_WAZA_AISYOU	AISYOU_2BAI,ExpertAI_43_1
	CHECK_WAZA_AISYOU	AISYOU_4BAI,ExpertAI_43_1

	IF_RND_UNDER		128,ExpertAI_43_end
ExpertAI_43_1:	
	IF_RND_UNDER		128,ExpertAI_43_end
	INCDEC		1
ExpertAI_43_end:	
	AIEND
//---------------------------------------------------------------------------
ExpertAI_118:	//いばる
	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_ZIKOANZI, ExpertAI_IbaruAnziCombo

ExpertAI_120:	//メロメロ
ExpertAI_166:	// おだてる
	IF_RND_UNDER		128,ExpertAI_49
	INCDEC		1
ExpertAI_49:	//こんらん
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_49_end

	IF_RND_UNDER		128,ExpertAI_49_1
	INCDEC		-1
ExpertAI_49_1:
	IF_HP_OVER	CHECK_DEFENCE,50,ExpertAI_49_end
	INCDEC		-1
	IF_HP_OVER	CHECK_DEFENCE,30,ExpertAI_49_end
	INCDEC		-1
ExpertAI_49_end:	
	AIEND

ExpertAI_IbaruAnziCombo:
	IF_PARA_OVER	CHECK_DEFENCE,PARA_POW,3,ExpertAI_IbaruAnziComboNg
	INCDEC			3
	CHECK_TURN
	IFN_EQUAL	0,ExpertAI_IbaruAnziComboEnd
	INCDEC			2
	JUMP		ExpertAI_IbaruAnziComboEnd

ExpertAI_IbaruAnziComboNg:
	INCDEC	-5
ExpertAI_IbaruAnziComboEnd:
	AIEND

//---------------------------------------------------------------------------

ExpertAI_65:	// リフレクター 2010/4/21
		
	IF_HP_UNDER	CHECK_ATTACK,50,ExpertAI_65_2		// HPが1/2以下
	
	IF_HP_UNDER	CHECK_ATTACK,90,ExpertAI_65_1		// HP90未満
	IF_RND_UNDER	30,ExpertAI_65_1
	INCDEC		1

ExpertAI_65_1:
  IF_DMG_PHYSIC_UNDER CHECK_DEFENCE,ExpertAI_65_2           //特攻の方が高い場合はやらない

	IF_RND_UNDER	64,ExpertAI_65_end
	INCDEC		1
	JUMP	ExpertAI_65_end
	
ExpertAI_65_2:
	IF_RND_UNDER	30,ExpertAI_35_end
	INCDEC		-2
ExpertAI_65_end:
	AIEND
	

//---------------------------------------------------------------------------
ExpertAI_66:	// どく
	IF_HP_UNDER	CHECK_ATTACK,50,ExpertAI_66_1		// HPが1/2以下
	IF_HP_OVER	CHECK_DEFENCE,50,ExpertAI_66_end	// HPが1/2以上
ExpertAI_66_1:
	INCDEC	-1
ExpertAI_66_end:
	AIEND
//---------------------------------------------------------------------------

ExpertAI_67:	// まひ
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_67_1

	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_67_end	

	INCDEC	-1
	JUMP		ExpertAI_67_end	

ExpertAI_67_1:
	IF_RND_UNDER	20,ExpertAI_67_end
	INCDEC	3

ExpertAI_67_end:
	AIEND
//相手より遅い場合は20/255の確率で+3
//相手よりも早くＨＰ70％以下なら-1

//---------------------------------------------------------------------------

ExpertAI_78:	// あてみなげ
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_78_end
	
	IF_HP_OVER	CHECK_ATTACK,60,ExpertAI_78_end	
	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_78_2	

	IF_RND_UNDER	180,ExpertAI_78_end

ExpertAI_78_2:
	IF_RND_UNDER	50,ExpertAI_78_end
	INCDEC	-1
ExpertAI_78_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_79:	// みがわり
	IFN_HAVE_WAZA	CHECK_ATTACK, WAZANO_KIAIPANTI, ExpertAI_79_hp		//みがわり
	IF_RND_UNDER	96,ExpertAI_79_hp
	INCDEC	+1

ExpertAI_79_hp:
	IF_HP_OVER	CHECK_ATTACK,90,ExpertAI_79_3	
	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_79_2	
	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_79_1	

	IF_RND_UNDER	100,ExpertAI_79_1
	INCDEC	-1
ExpertAI_79_1:
	IF_RND_UNDER	100,ExpertAI_79_2
	INCDEC	-1
ExpertAI_79_2:
	IF_RND_UNDER	100,ExpertAI_79_3
	INCDEC	-1
ExpertAI_79_3:
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_79_end

	CHECK_LAST_WAZA	CHECK_DEFENCE
	CHECK_WORKWAZA_SEQNO
	IF_EQUAL	1,ExpertAI_79_6		// ねむり
	IF_EQUAL	33,ExpertAI_79_6	// どくどく
	IF_EQUAL	66,ExpertAI_79_6	// どく
	IF_EQUAL	67,ExpertAI_79_6	// まひ
	IF_EQUAL	167,ExpertAI_79_6	// やけど
	IF_EQUAL	49,ExpertAI_79_7	// こんらん
	IF_EQUAL	84,ExpertAI_79_8	// やどりぎ
	JUMP		ExpertAI_79_end

ExpertAI_79_6:
	IFN_POKESICK	CHECK_DEFENCE,ExpertAI_79_ok	
	JUMP		ExpertAI_79_end

ExpertAI_79_7:
	IFN_WAZASICK	CHECK_DEFENCE,WAZASICK_KONRAN,ExpertAI_79_ok	
	JUMP		ExpertAI_79_end

ExpertAI_79_8:
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_YADORIGI,ExpertAI_79_end

ExpertAI_79_ok:
	IF_RND_UNDER	100,ExpertAI_79_end
	INCDEC		1

ExpertAI_79_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_80:	// はかいこうせん
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_80_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_80_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_80_ng
	
	CHECK_TOKUSEI	CHECK_ATTACK	//2006.8.7調整追加
	IF_EQUAL	TOKUSYU_NAMAKE,ExpertAI_80_ok			// なまけ

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_80_1

	IF_HP_OVER	CHECK_ATTACK,40,ExpertAI_80_ng	
	JUMP		ExpertAI_80_end	

ExpertAI_80_ok:
	IF_RND_UNDER	80,ExpertAI_80_end
	INCDEC	+1
	JUMP		ExpertAI_80_end	

ExpertAI_80_1:
	IF_HP_UNDER	CHECK_ATTACK,60,ExpertAI_80_end	

ExpertAI_80_ng:
	INCDEC	-1

ExpertAI_80_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_86:	// かなしばり
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_86_end

	CHECK_LAST_WAZA	CHECK_DEFENCE
	CHECK_WORKWAZA_POW
	IF_EQUAL	0,ExpertAI_86_1

	INCDEC	1
	JUMP		ExpertAI_86_end	

ExpertAI_86_1:
	IF_RND_UNDER	100,ExpertAI_86_end

	INCDEC	-1

ExpertAI_86_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_89:	// カウンター
	IF_WAZASICK		CHECK_DEFENCE,WAZASICK_NEMURI,ExpertAI_89_ng
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_MEROMERO,ExpertAI_89_ng	// メロメロ
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_KONRAN,ExpertAI_89_ng		// こんらん	

	IF_HP_OVER	CHECK_ATTACK,30,ExpertAI_89_1	

	IF_RND_UNDER	10,ExpertAI_89_1
	INCDEC	-1
ExpertAI_89_1:
	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_89_2	

	IF_RND_UNDER	100,ExpertAI_89_2
	INCDEC	-1

ExpertAI_89_2:
	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_MIRAAKOOTO, ExpertAI_89_6

	CHECK_LAST_WAZA	CHECK_DEFENCE
	CHECK_WORKWAZA_POW
	IF_EQUAL	0,ExpertAI_89_4

	IFN_CHOUHATSU	ExpertAI_89_3	

	IF_RND_UNDER	100,ExpertAI_89_3
	INCDEC	1

ExpertAI_89_3:
	//最後に出した技が物理でないかに変更2006.6.14
	CHECK_LAST_WAZA_KIND
	IFN_EQUAL	WAZADATA_DMG_PHYSIC,ExpertAI_89_ng

	//エメラルドまでは最後の技タイプで判断2006.6.14
	//CHECK_LAST_WAZA	CHECK_DEFENCE
	//CHECK_WORKWAZA_TYPE
	//IFN_TABLE_JUMP	ExpertAI_89_Table,ExpertAI_89_ng

ExpertAI_89_3_2:
	IF_RND_UNDER	100,ExpertAI_89_end
	INCDEC	1
	JUMP	ExpertAI_89_end

ExpertAI_89_4://威力なし
	IFN_CHOUHATSU	ExpertAI_89_5	

	IF_RND_UNDER	100,ExpertAI_89_5
	INCDEC	1

ExpertAI_89_5://相手のタイプをみて判別
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_TABLE_JUMP	ExpertAI_89_Table,ExpertAI_89_end
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_TABLE_JUMP	ExpertAI_89_Table,ExpertAI_89_end

	IF_RND_UNDER	50,ExpertAI_89_end

ExpertAI_89_6:	// ミラーコートと両方持ってる
	IF_RND_UNDER	100,ExpertAI_89_6_end
	INCDEC			4
ExpertAI_89_6_end:
	AIEND


ExpertAI_89_ng:
	INCDEC	-1
ExpertAI_89_end:
	AIEND

ExpertAI_89_Table:
	.long	POKETYPE_NORMAL
	.long	POKETYPE_KAKUTOU
	.long	POKETYPE_HIKOU
	.long	POKETYPE_DOKU
	.long	POKETYPE_JIMEN
	.long	POKETYPE_IWA
	.long	POKETYPE_MUSHI
	.long	POKETYPE_GHOST
	.long	POKETYPE_HAGANE
	.long	0xffffffff

//---------------------------------------------------------------------------
ExpertAI_90:	// アンコール
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_KANASIBARI,ExpertAI_90_1

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_90_6

	CHECK_LAST_WAZA	CHECK_DEFENCE
	CHECK_WORKWAZA_SEQNO
	IFN_TABLE_JUMP	ExpertAI_90_Table,ExpertAI_90_6

ExpertAI_90_1:
	IF_RND_UNDER		30,ExpertAI_90_end
	INCDEC	3
	JUMP			ExpertAI_90_end

ExpertAI_90_6:
	INCDEC		-2
ExpertAI_90_end:
	AIEND

ExpertAI_90_Table:
	.long	8,10,11,12,13,25,28,30,33,35
	.long	37,40,54,49,66,67,84,85,50,90
	.long	93,94,102,106,107,111,191,113,114,115
	.long	116,118,120,124,136,137,142,143,148,158
	.long	160,161,162,164,165,167,172,174,177,178
	.long	181,184,188,191,192,193,194,199,201,210
	.long	212,213
	//ＤＰ追加
	.long	215,216,220,222,223,225,226,233,234,236
	.long	238,239,240,243,244,247,250,251,252,259
	.long	0xffffffff

//---------------------------------------------------------------------------
ExpertAI_91:	// いたみわけ
	IF_HP_UNDER	CHECK_DEFENCE,80,ExpertAI_91_2	

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_91_1

	IF_HP_OVER	CHECK_ATTACK,40,ExpertAI_91_2	
	INCDEC		1
	JUMP		ExpertAI_91_end	

ExpertAI_91_1:
	IF_HP_OVER	CHECK_ATTACK,60,ExpertAI_91_2	
	INCDEC		1
	JUMP		ExpertAI_91_end	
	
ExpertAI_91_2:
	INCDEC		-1
ExpertAI_91_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_92:	// いびき
	INCDEC	2
	AIEND
//---------------------------------------------------------------------------
ExpertAI_94:	// ロックオン
	IF_RND_UNDER	128,ExpertAI_94_end

	INCDEC		2
ExpertAI_94_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_97:	// ねごと
	IF_WAZASICK	CHECK_ATTACK,WAZASICK_NEMURI,AI_INC10
	INCDEC	-5
	AIEND
//---------------------------------------------------------------------------
ExpertAI_98:	// みちずれ
	INCDEC		-1
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_98_end

	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_98_end	

	IF_RND_UNDER	128,ExpertAI_98_1
	INCDEC		1
ExpertAI_98_1:
	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_98_end	

	IF_RND_UNDER	128,ExpertAI_98_2
	INCDEC		1
ExpertAI_98_2:
	IF_HP_OVER	CHECK_ATTACK,30,ExpertAI_98_end	

	IF_RND_UNDER	100,ExpertAI_98_end
	INCDEC		2

ExpertAI_98_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_99:	// きしかいせい
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_99_2

	IF_HP_OVER	CHECK_ATTACK,33,ExpertAI_99_ng	
	IF_HP_OVER	CHECK_ATTACK,20,ExpertAI_99_end	
	IF_HP_UNDER	CHECK_ATTACK,8,ExpertAI_99_ok1	
	JUMP		ExpertAI_99_ok2	
	
ExpertAI_99_2:
	IF_HP_OVER	CHECK_ATTACK,60,ExpertAI_99_ng	
	IF_HP_OVER	CHECK_ATTACK,40,ExpertAI_99_end	
	JUMP		ExpertAI_99_ok2	
	
ExpertAI_99_ok1:
	INCDEC		1
ExpertAI_99_ok2:
	IF_RND_UNDER	100,ExpertAI_99_end
	INCDEC		1
	JUMP		ExpertAI_99_end	

ExpertAI_99_ng:
	INCDEC		-1

ExpertAI_99_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_102:	// いやしのすず
//逆だとおもわれうので修正2006.6.14	
	//IF_POKESICK		CHECK_DEFENCE,ExpertAI_102_end
	//IF_BENCH_COND	CHECK_DEFENCE,ExpertAI_102_end
	IF_POKESICK		CHECK_ATTACK,ExpertAI_102_end
	IF_BENCH_COND	CHECK_ATTACK,ExpertAI_102_end
	INCDEC		-5

ExpertAI_102_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_105:	// どろぼう
	CHECK_SOUBI_EQUIP	CHECK_DEFENCE
	IFN_TABLE_JUMP	ExpertAI_105_Table,ExpertAI_105_ng

	IF_RND_UNDER	50,ExpertAI_105_end
	INCDEC		1
	JUMP		ExpertAI_105_end	

ExpertAI_105_ng:
	INCDEC		-2
ExpertAI_105_end:
	AIEND

ExpertAI_105_Table:
	.long	SOUBI_NEMUKEZAMASI
	.long	SOUBI_ZYOUTAIIZYOUNAOSI
	.long	SOUBI_HPKAIHUKU
	.long	SOUBI_MEITYUURITUDOWN
	.long	SOUBI_HPKAIHUKUMAITURN
	.long	SOUBI_PIKATYUUTOKUKOUNIBAI
	.long	SOUBI_KARAGARADATOKOUGEKINIBAI
	//以降追加2006.6.14
	.long	SOUBI_HONOWOBATUGUNGUARD
	.long	SOUBI_MIZUBATUGUNGUARD
	.long	SOUBI_DENKIBATUGUNGUARD
	.long	SOUBI_KUSABATUGUNGUARD
	.long	SOUBI_KOORIBATUGUNGUARD
	.long	SOUBI_KAKUTOUBATUGUNGUARD
	.long	SOUBI_DOKUBATUGUNGUARD
	.long	SOUBI_ZIMENBATUGUNGUARD
	.long	SOUBI_HIKOUBATUGUNGUARD
	.long	SOUBI_ESUPAABATUGUNGUARD
	.long	SOUBI_MUSIGUARD
	.long	SOUBI_IWAGUARD
	.long	SOUBI_GOOSUTOGUARD
	.long	SOUBI_DORAGONGUARD
	.long	SOUBI_AKUGUARD
	.long	SOUBI_HAGANEGUARD
	.long	SOUBI_NOOMARUHANGEN
	.long	SOUBI_DOKUKAIHUKU
	.long	0xffffffff

//---------------------------------------------------------------------------
ExpertAI_109:	// のろい
	CHECK_TYPE	CHECK_ATTACK_TYPE1
	IF_EQUAL	POKETYPE_GHOST,ExpertAI_107_4	// ゴーストタイプ
	CHECK_TYPE	CHECK_ATTACK_TYPE2
	IF_EQUAL	POKETYPE_GHOST,ExpertAI_107_4	// ゴーストタイプ

	IF_PARA_OVER	CHECK_ATTACK,PARA_DEF,9,ExpertAI_107_end

	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_ZYAIROBOORU, ExpertAI_107_zyairo		//ジャイロボール
	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_TORIKKURUUMU, ExpertAI_107_zyairo		//トリックルーム
	JUMP	ExpertAI_107_nozyairo

ExpertAI_107_zyairo:
	IF_RND_UNDER	32,ExpertAI_107_1
	INCDEC		1
ExpertAI_107_nozyairo:
	IF_RND_UNDER	128,ExpertAI_107_1
	INCDEC		1
ExpertAI_107_1:
	IF_PARA_OVER	CHECK_ATTACK,PARA_DEF,7,ExpertAI_107_end
	
	IF_RND_UNDER	128,ExpertAI_107_2
	INCDEC		1
ExpertAI_107_2:
	IF_PARA_OVER	CHECK_ATTACK,PARA_DEF,6,ExpertAI_107_end

	IF_RND_UNDER	128,ExpertAI_107_end
	INCDEC		1
	JUMP		ExpertAI_107_end

ExpertAI_107_4:
	IF_HP_OVER	CHECK_ATTACK,80,ExpertAI_107_end	

	INCDEC		-1
ExpertAI_107_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_111:	// まもる
	//フェイント＆シャドーダイブ
	IF_HAVE_WAZA	CHECK_DEFENCE, WAZANO_FEINTO, ExpertAI_111_mamoriyaburi
	IF_HAVE_WAZA	CHECK_DEFENCE, WAZANO_SYADOODAIBU, ExpertAI_111_mamoriyaburi
	JUMP	ExpertAI_111_mamoriyaburanai

ExpertAI_111_mamoriyaburi:
	IF_COMMONRND_UNDER	128,ExpertAI_111_mamoriyaburanai
	INCDEC		-2

ExpertAI_111_mamoriyaburanai:
	CHECK_MAMORU_COUNT	CHECK_ATTACK
	IF_OVER		1,ExpertAI_111_ng2
	IF_DOKUDOKU		CHECK_ATTACK,                       ExpertAI_111_ng1	// どくどく
	IF_WAZASICK	  CHECK_ATTACK,WAZASICK_NOROI,        ExpertAI_111_ng1	// のろい
	IF_WAZASICK	  CHECK_ATTACK,WAZASICK_HOROBINOUTA,  ExpertAI_111_ng1	// ほろび
	IF_WAZASICK	  CHECK_ATTACK,WAZASICK_MEROMERO,     ExpertAI_111_ng1	//メロメロ
	IF_WAZASICK	  CHECK_ATTACK,WAZASICK_YADORIGI,     ExpertAI_111_ng1	// やどりぎ
	IF_WAZASICK	  CHECK_ATTACK,WAZASICK_AKUBI,        ExpertAI_111_ng1	// あくび

	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_NOROI,       ExpertAI_111_ok		// のろい
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_HOROBINOUTA, ExpertAI_111_ok   // ほろび

	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,32,ExpertAI_111_ng1		// じこさいせい
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,156,ExpertAI_111_ng1		// ねむる

	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,132,ExpertAI_111_ng1		// あさのひざし
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,162,ExpertAI_111_ng1		// のみこむ
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,214,ExpertAI_111_ng1		// はねやすめ

	IF_DOKUDOKU		CHECK_DEFENCE,                      ExpertAI_111_ok   // どくどく
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_MEROMERO,    ExpertAI_111_ok   //メロメロ
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_YADORIGI,    ExpertAI_111_ok   // やどりぎ
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_AKUBI,       ExpertAI_111_ok		// あくび
	
//	CHECK_BTL_RULE
//	IF_EQUAL		BTL_RULE_DOUBLE,ExpertAI_111_ok							//2VS2
	
	//自分がロックオンされていたら守る
	IF_WAZASICK	  CHECK_ATTACK,WAZASICK_MUSTHIT_TARGET,ExpertAI_111_ok	//ロックオンされ中
	IF_RND_UNDER	85,ExpertAI_111_ok	//33%
	JUMP		ExpertAI_111_1

ExpertAI_111_ok:
	INCDEC		2

ExpertAI_111_1:
	IF_COMMONRND_UNDER	128,ExpertAI_111_1_2             //　連動ランダム
	INCDEC		-2
ExpertAI_111_1_2:
	CHECK_MAMORU_COUNT	CHECK_ATTACK
	IF_EQUAL	0,ExpertAI_111_end
	INCDEC		-1
	IF_RND_UNDER	128,ExpertAI_111_end
	INCDEC		-1
	JUMP		ExpertAI_111_end

ExpertAI_111_ng1:
	//自分がロックオンをされていたら
	IF_WAZASICK	CHECK_ATTACK,WAZASICK_MUSTHIT_TARGET,ExpertAI_111_end	//ロックオンされ中

ExpertAI_111_ng2:
	INCDEC		-2
ExpertAI_111_end:
	AIEND

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_112:	// まきびし
	IF_RND_UNDER	128,ExpertAI_112_end
	INCDEC	+1
	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_HOERU, ExpertAI_112_ok
	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_HUKITOBASI, ExpertAI_112_ok
	JUMP	ExpertAI_112_end
	
ExpertAI_112_ok:
	IF_RND_UNDER	64,ExpertAI_112_end
	INCDEC	+1

ExpertAI_112_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_113:	// みやぶる:回避率が攻撃側になっているのを守備側に修正2006.6.14
	CHECK_TYPE	CHECK_ATTACK_TYPE1
	IF_EQUAL	POKETYPE_GHOST,ExpertAI_113_ok1	// ゴーストタイプ
	CHECK_TYPE	CHECK_ATTACK_TYPE2
	IF_EQUAL	POKETYPE_GHOST,ExpertAI_113_ok1	// ゴーストタイプ

	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,8,ExpertAI_113_ok2

	INCDEC		-2
	JUMP		ExpertAI_113_end


ExpertAI_113_ok1:
	IF_RND_UNDER	80,ExpertAI_113_end
ExpertAI_113_ok2:
	IF_RND_UNDER	80,ExpertAI_113_end

	INCDEC		2
ExpertAI_113_end:

	AIEND
//---------------------------------------------------------------------------
ExpertAI_116:	// こらえる
	IF_HP_UNDER	CHECK_ATTACK,4,ExpertAI_116_ng	
	IF_HP_UNDER	CHECK_ATTACK,35,ExpertAI_116_ok	

ExpertAI_116_ng:
	INCDEC		-1
	JUMP		ExpertAI_116_end

ExpertAI_116_ok:
	IF_RND_UNDER	70,ExpertAI_116_end
	INCDEC		1
ExpertAI_116_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_127:	// バトンタッチ
	IF_PARA_OVER	CHECK_ATTACK,PARA_POW,8,ExpertAI_127_1
	IF_PARA_OVER	CHECK_ATTACK,PARA_DEF,8,ExpertAI_127_1
	IF_PARA_OVER	CHECK_ATTACK,PARA_SPEPOW,8,ExpertAI_127_1
	IF_PARA_OVER	CHECK_ATTACK,PARA_SPEDEF,8,ExpertAI_127_1
	IF_PARA_OVER	CHECK_ATTACK,PARA_AVOID,8,ExpertAI_127_1
	JUMP		ExpertAI_127_5

ExpertAI_127_1:
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_127_2
	IF_HP_OVER	CHECK_ATTACK,60,ExpertAI_127_end	
	JUMP		ExpertAI_127_3

ExpertAI_127_2:
	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_127_end	

ExpertAI_127_3:
	IF_RND_UNDER	80,ExpertAI_127_end
	INCDEC		2
	JUMP		ExpertAI_127_end

ExpertAI_127_5:
	IF_PARA_OVER	CHECK_ATTACK,PARA_POW,7,ExpertAI_127_6
	IF_PARA_OVER	CHECK_ATTACK,PARA_DEF,7,ExpertAI_127_6
	IF_PARA_OVER	CHECK_ATTACK,PARA_SPEPOW,7,ExpertAI_127_6
	IF_PARA_OVER	CHECK_ATTACK,PARA_SPEDEF,7,ExpertAI_127_6
	IF_PARA_OVER	CHECK_ATTACK,PARA_AVOID,7,ExpertAI_127_6
	JUMP		ExpertAI_127_ng

ExpertAI_127_6:
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_127_7
	IF_HP_OVER	CHECK_ATTACK,60,ExpertAI_127_ng	
	JUMP		ExpertAI_127_end

ExpertAI_127_7:
	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_127_end	

ExpertAI_127_ng:
	INCDEC		-2

ExpertAI_127_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_128:	// おいうち
	CHECK_NEKODAMASI	CHECK_ATTACK
	//IFN_EQUAL	0,ExpertAI_128_ok2
	IFN_EQUAL	0,ExpertAI_128_ok1

	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_GHOST,ExpertAI_128_ok1
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_ESPER,ExpertAI_128_ok1

	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_GHOST,ExpertAI_128_ok1
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_ESPER,ExpertAI_128_ok1
	
	JUMP		ExpertAI_128_ok2

ExpertAI_128_ok1:
	IF_RND_UNDER	128,ExpertAI_128_ok2
	INCDEC		1

ExpertAI_128_ok2:
	IFN_HAVE_WAZA	CHECK_DEFENCE, WAZANO_TONBOGAERI, ExpertAI_128_end
	IF_RND_UNDER	128,ExpertAI_128_end
	INCDEC		1

ExpertAI_128_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_136:	// あまごい
	IF_FIRST	IF_FIRST_ATTACK,ExpertAI_136_1

	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_SUISUI,ExpertAI_136_ok			// すいすい

ExpertAI_136_1:
	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_136_ng	

	CHECK_WEATHER
	IF_EQUAL	WEATHER_ARARE,ExpertAI_136_ok		//　あられ中
	IF_EQUAL	WEATHER_HARE,ExpertAI_136_ok		//　はれ中
	IF_EQUAL	WEATHER_SUNAARASHI,ExpertAI_136_ok	//　すなあらし中

	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_AMEUKEZARA,ExpertAI_136_ok		// あめうけざら
	IF_EQUAL	TOKUSYU_KANSOUHADA,ExpertAI_136_ok		// かんそうはだ
	IFN_EQUAL	TOKUSYU_URUOIBODHI,ExpertAI_136_end		// うるおいボディでない
	IF_POKESICK	CHECK_ATTACK,ExpertAI_136_ok		// 状態異常時
	JUMP		ExpertAI_136_end

ExpertAI_136_ok:
	INCDEC		1
	JUMP		ExpertAI_136_end

ExpertAI_136_ng:
	INCDEC		-1

ExpertAI_136_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_137:	// にほんばれ
	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_137_ng	

	CHECK_WEATHER
	IF_EQUAL	WEATHER_ARARE,ExpertAI_137_ok		//　あられ中
	IF_EQUAL	WEATHER_AME,ExpertAI_137_ok		//　あめ中
	IF_EQUAL	WEATHER_SUNAARASHI,ExpertAI_137_ok	//　すなあらし中
	
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_HURAWAAGIHUTO,ExpertAI_137_ok		// フラワーギフト
	IFN_EQUAL	TOKUSYU_RIIHUGAADO,ExpertAI_137_end		// リーフガードでない
	IF_POKESICK	CHECK_ATTACK,ExpertAI_137_ok		// 状態異常時
	
	JUMP		ExpertAI_137_end

ExpertAI_137_ok:
	INCDEC		1
	JUMP		ExpertAI_137_end

ExpertAI_137_ng:
	INCDEC		-1

ExpertAI_137_end:
	AIEND



//---------------------------------------------------------------------------
ExpertAI_142:	// はらだいこ
	IF_HP_UNDER	CHECK_ATTACK,90,ExpertAI_142_ng	
	JUMP	ExpertAI_142_end

ExpertAI_142_ng:
	INCDEC		-2
ExpertAI_142_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_143:	// じこあんじ
	IF_PARA_OVER	CHECK_DEFENCE,PARA_POW,8,ExpertAI_143_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_DEF,8,ExpertAI_143_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEPOW,8,ExpertAI_143_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEDEF,8,ExpertAI_143_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,8,ExpertAI_143_1
	JUMP	ExpertAI_143_ng

ExpertAI_143_1:
	IF_PARA_UNDER	CHECK_ATTACK,PARA_POW,7,ExpertAI_143_ok2
	IF_PARA_UNDER	CHECK_ATTACK,PARA_DEF,7,ExpertAI_143_ok2
	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEPOW,7,ExpertAI_143_ok2
	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEDEF,7,ExpertAI_143_ok2
	IF_PARA_UNDER	CHECK_ATTACK,PARA_AVOID,7,ExpertAI_143_ok1

	IF_RND_UNDER	50,ExpertAI_143_end
	JUMP	ExpertAI_143_ng

ExpertAI_143_ok1:
	INCDEC		1
ExpertAI_143_ok2:
	INCDEC		1
	AIEND
ExpertAI_143_ng:
	INCDEC		-2
ExpertAI_143_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_144:	// ミラーコート
	IF_WAZASICK		CHECK_DEFENCE,WAZASICK_NEMURI,ExpertAI_144_ng
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_MEROMERO,ExpertAI_144_ng	// メロメロ
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_KONRAN,ExpertAI_144_ng		// こんらん	

	IF_HP_OVER	CHECK_ATTACK,30,ExpertAI_144_1	

	IF_RND_UNDER	10,ExpertAI_144_1
	INCDEC	-1
ExpertAI_144_1:
	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_144_2	

	IF_RND_UNDER	100,ExpertAI_144_2
	INCDEC	-1
ExpertAI_144_2:
	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_KAUNTAA, ExpertAI_144_6

	CHECK_LAST_WAZA	CHECK_DEFENCE
	CHECK_WORKWAZA_POW
	IF_EQUAL	0,ExpertAI_144_4

	IFN_CHOUHATSU	ExpertAI_144_3	
	IF_RND_UNDER	100,ExpertAI_144_3
	INCDEC	1
ExpertAI_144_3:
	//最後に出した技が特殊でないかに変更2006.6.14
	CHECK_LAST_WAZA_KIND
	IFN_EQUAL	WAZADATA_DMG_SPECIAL,ExpertAI_144_ng

	//エメラルドまでは最後の技タイプで判断2006.6.14
	//CHECK_LAST_WAZA	CHECK_DEFENCE
	//CHECK_WORKWAZA_TYPE
	//IFN_TABLE_JUMP	ExpertAI_144_Table,ExpertAI_144_ng

ExpertAI_144_3_2:
	IF_RND_UNDER	100,ExpertAI_144_end
	INCDEC	1
	JUMP	ExpertAI_144_end

ExpertAI_144_4:
	IFN_CHOUHATSU	ExpertAI_144_5	
	IF_RND_UNDER	100,ExpertAI_144_5
	INCDEC	1
ExpertAI_144_5:
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_TABLE_JUMP	ExpertAI_144_Table,ExpertAI_144_end
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_TABLE_JUMP	ExpertAI_144_Table,ExpertAI_144_end

	IF_RND_UNDER	50,ExpertAI_144_end

ExpertAI_144_6:	// カウンターと両方持ってる
	IF_RND_UNDER	100,ExpertAI_144_6_end
	INCDEC			4
ExpertAI_144_6_end:
	AIEND


ExpertAI_144_ng:
	INCDEC	-1
ExpertAI_144_end:
	AIEND

ExpertAI_144_Table:
	.long	POKETYPE_HONOO
	.long	POKETYPE_MIZU
	.long	POKETYPE_KUSA
	.long	POKETYPE_DENKI
	.long	POKETYPE_ESPER
	.long	POKETYPE_KOORI
	.long	POKETYPE_DRAGON
	.long	POKETYPE_AKU
	.long	0xffffffff

//---------------------------------------------------------------------------
ExpertAI_39:	// かまいたち
ExpertAI_75:	// ゴッドバード
ExpertAI_145:	// ロケットずつき
ExpertAI_151:	// ソーラービーム
ExpertAI_331:	// 	フリーズボルト
ExpertAI_332:	// 	コールドフレア
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_151_ng1
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_151_ng1
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_151_ng1

	//にほんばれチェック：ソーラービーム
	IF_WAZA_SEQNO_JUMP	151,ExpertAI_151_hare		// ソーラービームのみ	2006.7.20
	JUMP	ExpertAI_151_itemcheck

ExpertAI_151_hare:	
	CHECK_WEATHER
	IFN_EQUAL	WEATHER_HARE,ExpertAI_151_itemcheck		// 日本晴れ中
	INCDEC	+2
	JUMP		ExpertAI_151_end

ExpertAI_151_itemcheck:
	//パワフルハーブチェック
	IF_HAVE_ITEM	CHECK_ATTACK, ITEM_PAWAHURUHAABU,ExpertAI_151_1_item//2006.6.23
	JUMP	ExpertAI_151_1_noitem
ExpertAI_151_1_item:
	INCDEC	+2
	JUMP		ExpertAI_151_end
	
ExpertAI_151_1_noitem:
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,111,ExpertAI_151_ng1		// まもる

	IF_HP_OVER	CHECK_ATTACK,38,ExpertAI_151_end	
	INCDEC	-1
	JUMP		ExpertAI_151_end

ExpertAI_151_ng1:
	INCDEC	-2
ExpertAI_151_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

ExpertAI_337:	//ぼうふう  2010.5.20
ExpertAI_152:	//かみなり
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_152_1
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_152_1
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_152_1
	CHECK_WEATHER
	IF_EQUAL	WEATHER_HARE,ExpertAI_152_1
	IFN_EQUAL	WEATHER_AME,ExpertAI_152_2
	INCDEC		+1
	JUMP		ExpertAI_152_2
	
ExpertAI_152_1:
	IF_RND_UNDER	50,ExpertAI_152_2
	INCDEC		-3

ExpertAI_152_2:
	AIEND

//---------------------------------------------------------------------------


//パワフルハーブチェックを入れる予定
ExpertAI_263:	//とびはねる2006.6.14
ExpertAI_255:	//ダイビング2006.6.14
ExpertAI_256:	//あなをほる2006.6.14
ExpertAI_155:	// そらをとぶ　         2010.3.14 イバンのみ追加
	
	IF_HAVE_ITEM	CHECK_ATTACK, ITEM_PAWAHURUHAABU,ExpertAI_151_1_item//2006.6.23
	IFN_HAVE_WAZA_SEQNO	CHECK_DEFENCE,111,ExpertAI_155_1		// まもる

	INCDEC	-1
	JUMP		ExpertAI_155_end

ExpertAI_272:	//シャドーダイブ（まもるチェック無視）2006.6.14
ExpertAI_155_1:
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_155_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_155_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_155_ng
	IF_HAVE_ITEM	CHECK_ATTACK, ITEM_PAWAHURUHAABU,ExpertAI_155_1_item//2006.6.23
	JUMP	ExpertAI_155_1_noitem

ExpertAI_155_1_item:
	INCDEC	+1
	JUMP		ExpertAI_155_end

ExpertAI_311:			// 	フリーフォール
ExpertAI_155_1_noitem:
	IF_FIRST	IF_FIRST_ATTACK,ExpertAI_155_1_IBAN_PASS
	IF_HP_OVER	CHECK_ATTACK,24,ExpertAI_155_1_IBAN_PASS

	IF_HAVE_ITEM	CHECK_ATTACK,ITEM_IBANNOMI,ExpertAI_155_ok

ExpertAI_155_1_IBAN_PASS:
	IF_DOKUDOKU		CHECK_DEFENCE,                    ExpertAI_155_ok	  // どくどく
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_NOROI,     ExpertAI_155_ok		// のろい
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_YADORIGI,  ExpertAI_155_ok	  // やどりぎ
	
	CHECK_WEATHER
	IF_EQUAL	WEATHER_SUNAARASHI,ExpertAI_155_2	//　すなあらし中
	IF_EQUAL	WEATHER_ARARE,ExpertAI_155_3		//　あられ中
	JUMP		ExpertAI_155_4

ExpertAI_155_2:
	CHECK_TYPE	CHECK_ATTACK_TYPE1
	IF_TABLE_JUMP	ExpertAI_155_Table,ExpertAI_155_ok
	CHECK_TYPE	CHECK_ATTACK_TYPE2
	IF_TABLE_JUMP	ExpertAI_155_Table,ExpertAI_155_ok
	JUMP		ExpertAI_155_4

ExpertAI_155_3:
	CHECK_TYPE	CHECK_ATTACK_TYPE1
	IF_EQUAL	POKETYPE_KOORI,ExpertAI_155_ok
	CHECK_TYPE	CHECK_ATTACK_TYPE2
	IF_EQUAL	POKETYPE_KOORI,ExpertAI_155_ok
	JUMP		ExpertAI_155_4

ExpertAI_155_4:
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_155_end

	CHECK_LAST_WAZA	CHECK_DEFENCE
	CHECK_WORKWAZA_SEQNO
	IFN_EQUAL	94,ExpertAI_155_ok					// ロックオン

	JUMP		ExpertAI_155_end

ExpertAI_155_ok:
	IF_RND_UNDER	80,ExpertAI_155_end
	INCDEC		1
ExpertAI_155_end:
	AIEND

ExpertAI_155_ng:
	INCDEC		1
	AIEND

ExpertAI_155_Table:
	.long	POKETYPE_JIMEN
	.long	POKETYPE_IWA
	.long	POKETYPE_HAGANE
	.long	0xffffffff


//---------------------------------------------------------------------------
ExpertAI_158:	// ねこだましok
	INCDEC		2
ExpertAI_158_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_160:		// たくわえる	2010/5/11

	IFN_HP_EQUAL	CHECK_ATTACK,100,ExpertAI_160_2_2

	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,127,ExpertAI_160_2		// バトンタッチ系

	IF_RND_UNDER	128,ExpertAI_160_2_2
ExpertAI_160_2:
	INCDEC		2
ExpertAI_160_2_2:
	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_160_3

	IF_RND_UNDER	200,ExpertAI_160_end

ExpertAI_160_3:
	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_160_5

	IF_RND_UNDER	60,ExpertAI_160_end
ExpertAI_160_5:
	INCDEC		-2
ExpertAI_160_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_161:	// はきだす
	CHECK_TAKUWAERU		CHECK_ATTACK
	IF_UNDER	2,ExpertAI_161_end

	IF_RND_UNDER	80,ExpertAI_161_end
	INCDEC		2
ExpertAI_161_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_164:	// あられ
	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_164_ng	

	CHECK_WEATHER
	IF_EQUAL	WEATHER_HARE,ExpertAI_164_ok		//　はれ中
	IF_EQUAL	WEATHER_AME,ExpertAI_164_ok			//　あめ中
	IF_EQUAL	WEATHER_SUNAARASHI,ExpertAI_164_ok	//　すなあらし中
	JUMP		ExpertAI_164_end

ExpertAI_164_ok:
	INCDEC		1

//ふぶき
ExpertAI_164_hubuki:
	IFN_HAVE_WAZA	CHECK_ATTACK, WAZANO_HUBUKI, ExpertAI_164_aisubodhi
	INCDEC		2

//アイスボディ
ExpertAI_164_aisubodhi:
	CHECK_TOKUSEI	CHECK_ATTACK
	IFN_EQUAL	TOKUSYU_AISUBODHI,ExpertAI_164_end
	INCDEC		2
	JUMP		ExpertAI_164_end

ExpertAI_164_ng:
	INCDEC		-1

ExpertAI_164_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_169:	// からげんき どく、やけど、まひ、どくどく
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_DOKU,    ExpertAI_169_ok	
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_YAKEDO,  ExpertAI_169_ok	
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_MAHI,    ExpertAI_169_ok	
	IFN_DOKUDOKU	CHECK_DEFENCE,                  ExpertAI_169_end	

ExpertAI_169_ok:
	INCDEC		1
ExpertAI_169_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_170:	// きあいパンチ 2010.3.9
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_170_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_170_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_170_ng

	IF_MIGAWARI	  CHECK_ATTACK,AI_INC2		// みがわり中
	IF_WAZASICK		CHECK_DEFENCE,WAZASICK_NEMURI,AI_INC2
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_AKUBI,AI_INC2        
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_MEROMERO,ExpertAI_170_ok1	// メロメロ
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_KONRAN,ExpertAI_170_ok1	// こんらん	

	CHECK_NEKODAMASI	CHECK_ATTACK
	IFN_EQUAL	0,ExpertAI_170_end	//でてきて１ターン目でないなら終了

	IF_RND_UNDER	200,ExpertAI_170_end
	INCDEC		1
	JUMP		ExpertAI_170_end

ExpertAI_170_ng:
	INCDEC		-1
	JUMP		ExpertAI_170_end

ExpertAI_170_ok1:
	IF_RND_UNDER	100,ExpertAI_170_end
ExpertAI_170_ok2:
	INCDEC		1
ExpertAI_170_end:
	AIEND
//---------------------------------------------------------------------------

ExpertAI_171:	// きつけ
	IF_WAZASICK		CHECK_DEFENCE,WAZASICK_MAHI,ExpertAI_171_ok
	JUMP		ExpertAI_171_end

ExpertAI_171_ok:
	INCDEC		1
ExpertAI_171_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_177:	// トリック
//テーブル構成を細かく設定する必要あり

	CHECK_SOUBI_EQUIP	CHECK_ATTACK
	IF_TABLE_JUMP	ExpertAI_177_Table2,ExpertAI_177_hachimaki
	IF_TABLE_JUMP	ExpertAI_177_Table3,ExpertAI_177_doku
	IF_TABLE_JUMP	ExpertAI_177_Table4,ExpertAI_177_yakedo
	IF_TABLE_JUMP	ExpertAI_177_Table5,ExpertAI_177_hedoro
	IF_TABLE_JUMP	ExpertAI_177_Table,ExpertAI_177_ok

ExpertAI_177_ng:
	INCDEC		-3
	JUMP		ExpertAI_177_end

ExpertAI_177_hachimaki:
	CHECK_SOUBI_EQUIP	CHECK_DEFENCE
	IF_TABLE_JUMP	ExpertAI_177_TableNG2,ExpertAI_177_ng
	INCDEC		5
	JUMP		ExpertAI_177_end

ExpertAI_177_doku://どくどくだま

	CHECK_SOUBI_EQUIP	CHECK_DEFENCE	//同じものを持っていたら意味がないのでやらない
	IF_TABLE_JUMP	ExpertAI_177_TableNG2,ExpertAI_177_ng
	
	IF_POKESICK	CHECK_DEFENCE,ExpertAI_177_doku_check// 状態異常時は状態異常攻撃をしない
	IF_SIDEEFF	CHECK_DEFENCE, BTL_SIDEEFF_SINPINOMAMORI,ExpertAI_177_doku_check	// しんぴのまもり
	
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_HAGANE,ExpertAI_177_doku_check		// 相手がはがねタイプ
	IF_EQUAL	POKETYPE_DOKU,ExpertAI_177_doku_check		// 相手がどくタイプ 
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_HAGANE,ExpertAI_177_doku_check		// 相手がはがねタイプ
	IF_EQUAL	POKETYPE_DOKU,ExpertAI_177_doku_check	// 相手がどくタイプ

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MENEKI,ExpertAI_177_doku_check		// めんえき
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,ExpertAI_177_doku_check	// マジックガード	2006.6.6
	IF_EQUAL	TOKUSYU_POIZUNHIIRU,ExpertAI_177_doku_check		// ポイズンヒール	2006.6.6
	IF_EQUAL	TOKUSYU_DOKUBOUSOU,ExpertAI_177_doku_check		// どくぼうそう	2010.2.13
	
	INCDEC		5
	JUMP		ExpertAI_177_end

	
ExpertAI_177_doku_check://相手が毒に耐性があるとき自分が毒に耐性があるか？
	
	IF_POKESICK	CHECK_ATTACK,ExpertAI_177_ng// 状態異常時は待ち
	IF_SIDEEFF	CHECK_ATTACK, BTL_SIDEEFF_SINPINOMAMORI,ExpertAI_177_ng	// しんぴのまもり
	
	CHECK_TYPE	CHECK_ATTACK_TYPE1
	IF_EQUAL	POKETYPE_HAGANE,ExpertAI_177_ng		// はがねタイプ
	IF_EQUAL	POKETYPE_DOKU,ExpertAI_177_ng		// どくタイプ 
	CHECK_TYPE	CHECK_ATTACK_TYPE2
	IF_EQUAL	POKETYPE_HAGANE,ExpertAI_177_ng		// はがねタイプ
	IF_EQUAL	POKETYPE_DOKU,ExpertAI_177_ng		// どくタイプ

	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_MENEKI,ExpertAI_177_ng			// めんえき
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,ExpertAI_177_ng	// マジックガード	2006.6.6
	IF_EQUAL	TOKUSYU_POIZUNHIIRU,ExpertAI_177_ng		// ポイズンヒール	2006.6.6
	IF_EQUAL	TOKUSYU_BUKIYOU,ExpertAI_177_ng			// ぶきよう
	IF_EQUAL	TOKUSYU_DOKUBOUSOU,ExpertAI_177_ng		// どくぼうそう	2010.2.13

	INCDEC		5	//耐性がないのでトリックする
	JUMP		ExpertAI_177_end




ExpertAI_177_yakedo://かえんだま
	CHECK_SOUBI_EQUIP	CHECK_DEFENCE	//相手が持っていたら意味がないのでやらない
	IF_TABLE_JUMP	ExpertAI_177_TableNG2,ExpertAI_177_ng
	
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MIZUNOBEERU,ExpertAI_177_yakedo_check		// みずのベール
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,ExpertAI_177_yakedo_check		// マジックガード
	IF_EQUAL	TOKUSYU_NETUBOUSOU,ExpertAI_177_yakedo_check		// ねつぼうそう	2010.2.13
	IF_POKESICK	CHECK_DEFENCE,ExpertAI_177_yakedo_check		// 状態異常時は状態異常攻撃をしない
	IF_SIDEEFF	CHECK_DEFENCE, BTL_SIDEEFF_SINPINOMAMORI,ExpertAI_177_yakedo_check	// しんぴのまもり
	
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_HONOO,ExpertAI_177_yakedo_check			// ほのおタイプ
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_HONOO,ExpertAI_177_yakedo_check			// ほのおトタイプ
	
	INCDEC		5
	JUMP		ExpertAI_177_end


ExpertAI_177_yakedo_check:

	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_MIZUNOBEERU,ExpertAI_177_ng		// みずのベール
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,ExpertAI_177_ng	// マジックガード
	IF_EQUAL	TOKUSYU_BUKIYOU,AI_DEC5					// ぶきよう
	IF_EQUAL	TOKUSYU_NETUBOUSOU,ExpertAI_177_ng		// ねつぼうそう	2010.2.13
	IF_POKESICK	CHECK_ATTACK,ExpertAI_177_ng	// 状態異常時は状態異常攻撃をしない
	IF_SIDEEFF	CHECK_ATTACK, BTL_SIDEEFF_SINPINOMAMORI,ExpertAI_177_ng	// しんぴのまもり
	
	CHECK_TYPE	CHECK_ATTACK_TYPE1
	IF_EQUAL	POKETYPE_HONOO,ExpertAI_177_ng			// ほのおタイプ
	CHECK_TYPE	CHECK_ATTACK_TYPE2
	IF_EQUAL	POKETYPE_HONOO,ExpertAI_177_ng			// ほのおトタイプ
	
	INCDEC		5	//耐性がないのでトリックする
	JUMP		ExpertAI_177_end


ExpertAI_177_hedoro://くろいへドロ

	CHECK_SOUBI_EQUIP	CHECK_DEFENCE	//同じものを持っていたら意味がないのでやらない
	IF_TABLE_JUMP	ExpertAI_177_TableNG2,ExpertAI_177_ng
	
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_DOKU,ExpertAI_177_hedoro_check		// 相手がどくタイプ 
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_DOKU,ExpertAI_177_hedoro_check	// 相手がどくタイプ

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,ExpertAI_177_doku_check	// マジックガード	2006.6.6
	IF_EQUAL	TOKUSYU_DOKUBOUSOU,ExpertAI_177_doku_check		// どくぼうそう	2010.2.13
	INCDEC		5
	JUMP		ExpertAI_177_end

	
ExpertAI_177_hedoro_check://相手が毒に耐性があるとき自分が毒に耐性があるか？
	
	CHECK_TYPE	CHECK_ATTACK_TYPE1
	IF_EQUAL	POKETYPE_DOKU,ExpertAI_177_ng		// どくタイプ 
	CHECK_TYPE	CHECK_ATTACK_TYPE2
	IF_EQUAL	POKETYPE_DOKU,ExpertAI_177_ng		// どくタイプ

	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,ExpertAI_177_ng	// マジックガード	2006.6.6
	IF_EQUAL	TOKUSYU_BUKIYOU,ExpertAI_177_ng			// ぶきよう
	IF_EQUAL	TOKUSYU_DOKUBOUSOU,ExpertAI_177_ng		// どくぼうそう	2010.2.13

	INCDEC		5	//耐性がないのでトリックする
	JUMP		ExpertAI_177_end


ExpertAI_177_ok:
	CHECK_SOUBI_EQUIP	CHECK_DEFENCE
	IF_TABLE_JUMP	ExpertAI_177_TableNG1,ExpertAI_177_ng

	IF_RND_UNDER	50,ExpertAI_177_end
	INCDEC		2
ExpertAI_177_end:
	AIEND

ExpertAI_177_Table://味
	.long	SOUBI_KARAIKONRAN
	.long	SOUBI_SIBUIKONRAN
	.long	SOUBI_AMAIKONRAN
	.long	SOUBI_NIGAIKONRAN
	.long	SOUBI_SUPPAIKONRAN
	.long	0xffffffff

ExpertAI_177_Table2://嫌な道具
	.long	SOUBI_ONAZIWAZAONLY
	.long	SOUBI_ONAZIAWZAONLYTOKUSYUUP
	.long	SOUBI_ONAZIWAZAONLYSUBAYASAUP
	.long	SOUBI_SUBAYASADOWN
	.long	SOUBI_KOUKOUNINARU
	.long	SOUBI_TEKINIMOTASETEDAMEEZI
	.long	SOUBI_KOUGEKIDORYOKUTIUP
	.long	SOUBI_BOUGYODORYOKUTIUP
	.long	SOUBI_TOKUKOUDORYOKUTIUP
	.long	SOUBI_BOUGYODORYOKUTIUP
	.long	SOUBI_TOKUBOUDORYOKUTIUP
	.long	SOUBI_SUBAYASADORYOKUTIUP
	.long	SOUBI_HPDORYOKUTIUP
	.long	0xffffffff

ExpertAI_177_Table3://どく
	.long	SOUBI_TEKINIMOTASERUTOMOUDOKU
	.long	0xffffffff

ExpertAI_177_Table4://やけど
	.long	SOUBI_TTEKINIMOTASERUTOYAKEDO
	.long	0xffffffff

ExpertAI_177_Table5://ヘドロ
	.long	SOUBI_DOKUKAIHUKU
	.long	0xffffffff


ExpertAI_177_TableNG1://敵がこれを持っていたらやらない
	.long	SOUBI_KARAIKONRAN
	.long	SOUBI_SIBUIKONRAN
	.long	SOUBI_AMAIKONRAN
	.long	SOUBI_NIGAIKONRAN
	.long	SOUBI_SUPPAIKONRAN
	
	.long	SOUBI_DORYOKUTINIBAI
	.long	SOUBI_ONAZIWAZAONLY
	.long	SOUBI_ONAZIAWZAONLYTOKUSYUUP
	.long	SOUBI_ONAZIWAZAONLYSUBAYASAUP
	.long	SOUBI_SUBAYASADOWN
	.long	SOUBI_KOUKOUNINARU
	
	.long	SOUBI_TEKINIMOTASETEDAMEEZI
	
	.long	SOUBI_KOUGEKIDORYOKUTIUP
	.long	SOUBI_BOUGYODORYOKUTIUP
	.long	SOUBI_TOKUKOUDORYOKUTIUP
	.long	SOUBI_TOKUBOUDORYOKUTIUP
	.long	SOUBI_SUBAYASADORYOKUTIUP
	.long	SOUBI_HPDORYOKUTIUP

	.long	SOUBI_TEKINIMOTASERUTOMOUDOKU
	.long	SOUBI_TTEKINIMOTASERUTOYAKEDO
	.long	SOUBI_DOKUKAIHUKU
	.long	0xffffffff

ExpertAI_177_TableNG2://敵がこれを持っていたらやらない
	.long	SOUBI_DORYOKUTINIBAI
	.long	SOUBI_ONAZIWAZAONLY
	.long	SOUBI_ONAZIAWZAONLYTOKUSYUUP
	.long	SOUBI_ONAZIWAZAONLYSUBAYASAUP
	.long	SOUBI_SUBAYASADOWN
	.long	SOUBI_KOUKOUNINARU
	
	.long	SOUBI_TEKINIMOTASETEDAMEEZI
	
	.long	SOUBI_KOUGEKIDORYOKUTIUP
	.long	SOUBI_BOUGYODORYOKUTIUP
	.long	SOUBI_TOKUKOUDORYOKUTIUP
	.long	SOUBI_TOKUBOUDORYOKUTIUP
	.long	SOUBI_SUBAYASADORYOKUTIUP
	.long	SOUBI_HPDORYOKUTIUP

	.long	SOUBI_TEKINIMOTASERUTOMOUDOKU
	.long	SOUBI_TTEKINIMOTASERUTOYAKEDO
	.long	SOUBI_DOKUKAIHUKU
	.long	0xffffffff

//---------------------------------------------------------------------------
ExpertAI_178:	// なりきり
ExpertAI_191:	// スキルスワップ
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_TABLE_JUMP	ExpertAI_178_Table,ExpertAI_178_ng

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_TABLE_JUMP	ExpertAI_178_Table,ExpertAI_178_ok

	CHECK_BTL_RULE
	IF_EQUAL		BTL_RULE_SINGLE,ExpertAI_178_ng

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_TABLE_JUMP	ExpertAI_178_Double_Table,ExpertAI_178_ok

ExpertAI_178_ng:
	INCDEC		-1
	JUMP		ExpertAI_178_end

ExpertAI_178_ok:
	IF_RND_UNDER	50,ExpertAI_178_end
	INCDEC		2
ExpertAI_178_end:
	AIEND

//ＷＢより大幅変更2010.4.23
ExpertAI_178_Table:
	.long	TOKUSYU_KASOKU
	.long	TOKUSYU_MORAIBI
	.long	TOKUSYU_IKAKU
	.long	TOKUSYU_SUISUI
	.long	TOKUSYU_YOURYOKUSO
	.long	TOKUSYU_TIKARAMOTI
	.long	TOKUSYU_AMEUKEZARA
	.long	TOKUSYU_KONZYOU
	.long	TOKUSYU_YOGAPAWAA
	.long	TOKUSYU_DENKIENZIN
	.long	TOKUSYU_KANSOUHADA
	.long	TOKUSYU_POIZUNHIIRU
	.long	TOKUSYU_TEKIOURYOKU
	.long	TOKUSYU_SANPAWAA
	.long	TOKUSYU_TEKUNISYAN
	.long	TOKUSYU_AISUBODHI
	.long	TOKUSYU_AMANOZYAKU
	.long	TOKUSYU_NOROWAREBODHI
	.long	TOKUSYU_DOKUBOUSOU
	.long	TOKUSYU_NETUBOUSOU
	.long	TOKUSYU_SYUUKAKU
	.long	TOKUSYU_SUNAKAKI
	.long	TOKUSYU_MAZIKKUMIRAA
	.long	TOKUSYU_ITAZURAGOKORO
	.long	0xffffffff

ExpertAI_178_Double_Table:
	.long	TOKUSYU_KAGEHUMI
	.long	TOKUSYU_ARIZIGOKU
	.long	TOKUSYU_TEREPASII
	.long	TOKUSYU_HURENDOGAADO
	.long	TOKUSYU_MAIPEESU
	.long	0xffffffff




//---------------------------------------------------------------------------
ExpertAI_181:	// ねをはる
	AIEND
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ExpertAI_182:	// ばかぢから
	//しろいハーブチェックなどいれるかも
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_182_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_182_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_182_ng

	IF_PARA_UNDER	CHECK_ATTACK,PARA_POW,6,ExpertAI_182_ng

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_182_1
	IF_HP_OVER	CHECK_ATTACK,40,ExpertAI_182_ng
	JUMP		ExpertAI_182_end

ExpertAI_182_1:
	IF_HP_UNDER	CHECK_ATTACK,60,ExpertAI_182_end

ExpertAI_182_ng:
	INCDEC		-1
ExpertAI_182_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_183:	// マジックコート
	IF_HP_OVER	CHECK_DEFENCE,30,ExpertAI_183_0

	IF_RND_UNDER	100,ExpertAI_183_0
	INCDEC		-1
ExpertAI_183_0:
	CHECK_NEKODAMASI	CHECK_ATTACK
	IF_EQUAL	0,ExpertAI_183_ng

ExpertAI_183_1:
	IF_RND_UNDER	150,ExpertAI_183_end
	INCDEC		1
	JUMP		ExpertAI_183_end
ExpertAI_183_2:
	IF_RND_UNDER	50,ExpertAI_183_end

ExpertAI_183_ng:
	IF_RND_UNDER	30,ExpertAI_183_end
	INCDEC		-1
ExpertAI_183_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_184:	// リサイクル
	CHECK_RECYCLE_ITEM	CHECK_ATTACK
	IFN_TABLE_JUMP	ExpertAI_184_Table,ExpertAI_184_ng

	IF_RND_UNDER	50,ExpertAI_184_end
	INCDEC		1
	JUMP		ExpertAI_184_end	

ExpertAI_184_ng:
	INCDEC		-2
ExpertAI_184_end:
	AIEND

//ＤＰ用のアイテムナンバーに変更2006.6.14
ExpertAI_184_Table:
	.long	150,157,207
	//2006.6.14以下ＤＰより追加
	//現状なし
	.long	0xffffffff

//---------------------------------------------------------------------------
ExpertAI_185:	// リベンジ
	IF_WAZASICK		CHECK_DEFENCE,WAZASICK_NEMURI,ExpertAI_185_ng
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_MEROMERO,ExpertAI_185_ng	// メロメロ
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_KONRAN,ExpertAI_185_ng		// こんらん	

	IF_RND_UNDER	180,ExpertAI_185_ng

	INCDEC		2
	JUMP		ExpertAI_185_end

ExpertAI_185_ng:
	INCDEC		-2
ExpertAI_185_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_186:	// かわらわり
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_REFRECTOR,ExpertAI_186_ok // リフレクター中
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_HIKARINOKABE,ExpertAI_186_ok // ひかりのかべ2006.6.14

	JUMP		ExpertAI_186_end

ExpertAI_186_ok:
	INCDEC		1
ExpertAI_186_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_188:	// はたきおとす
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_SYUUKAKU,AI_DEC5		// しゅうかく

	IF_HP_UNDER	CHECK_DEFENCE,30,ExpertAI_188_end

	CHECK_NEKODAMASI	CHECK_ATTACK
	IF_OVER		0,ExpertAI_188_end

	IF_RND_UNDER	180,ExpertAI_188_end
ExpertAI_188_ok:
	INCDEC		1
ExpertAI_188_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_189:	// がむしゃら
	IF_HP_UNDER	CHECK_DEFENCE,70,ExpertAI_189_2	

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_189_1

	IF_HP_OVER	CHECK_ATTACK,40,ExpertAI_189_2	
	INCDEC		1
	JUMP		ExpertAI_189_end	

ExpertAI_189_1:
	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_189_2	
	INCDEC		1
	JUMP		ExpertAI_189_end	
	
ExpertAI_189_2:
	INCDEC		-1
ExpertAI_189_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_190:	// ふんか 2010.3.9
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_190_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_190_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_190_ng

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_190_1

	IF_HP_OVER	CHECK_ATTACK,90,ExpertAI_190_ok	

	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_190_end	
	JUMP		ExpertAI_190_ng	

ExpertAI_190_ok:
	IF_RND_UNDER	30,ExpertAI_190_end

	INCDEC		1

	IF_HAVE_ITEM	CHECK_ATTACK, ITEM_KODAWARISUKAAHU,ExpertAI_190_KODAWARISUKAAHU  //　こだわりスカーフ
	JUMP		ExpertAI_190_end	

ExpertAI_190_KODAWARISUKAAHU:
	INCDEC		1
	JUMP		ExpertAI_190_end	

ExpertAI_190_1:
	IF_HP_OVER	CHECK_ATTACK,80,ExpertAI_190_end	

ExpertAI_190_ng:
	INCDEC		-1
ExpertAI_190_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_192:	// ふういんok
	CHECK_NEKODAMASI	CHECK_ATTACK
	IF_OVER		0,ExpertAI_192_end

	IF_RND_UNDER	100,ExpertAI_192_end
	INCDEC		2
ExpertAI_192_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_193:	// リフレッシュ
	IF_HP_UNDER	CHECK_DEFENCE,50,ExpertAI_193_ng

	JUMP		ExpertAI_193_end	

ExpertAI_193_ng:
	INCDEC		-1
ExpertAI_193_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_195:	// よこどり
	CHECK_NEKODAMASI	CHECK_ATTACK
	IF_EQUAL	1,ExpertAI_195_ok1

	IF_RND_UNDER	30,ExpertAI_195_end

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_195_1

	IFN_HP_EQUAL	CHECK_ATTACK,100,ExpertAI_195_ng

	IF_HP_UNDER	CHECK_DEFENCE,70,ExpertAI_195_ng
	IF_RND_UNDER	60,ExpertAI_195_end
	JUMP		ExpertAI_195_ng	

ExpertAI_195_1:
	IF_HP_OVER	CHECK_DEFENCE,25,ExpertAI_195_ng

	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,32,ExpertAI_195_ok1		// じこさいせい
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,156,ExpertAI_195_ok1		// ねむる
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,214,ExpertAI_195_ok1		// はねやすめ

	JUMP		ExpertAI_195_ok2	

ExpertAI_195_ok1:
	IF_RND_UNDER	150,ExpertAI_195_end
	INCDEC		2
	JUMP		ExpertAI_195_end	

ExpertAI_195_ok2:
	IF_RND_UNDER	230,ExpertAI_195_ng
	INCDEC		1
	JUMP		ExpertAI_195_end	

ExpertAI_195_ng:
	IF_RND_UNDER	30,ExpertAI_195_end

	INCDEC		-2
ExpertAI_195_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_201:	// どろあそび
	IF_HP_UNDER	CHECK_ATTACK,50,ExpertAI_201_ng	

	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_DENKI,ExpertAI_201_ok
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_DENKI,ExpertAI_201_ok

	JUMP		ExpertAI_201_ng	

ExpertAI_201_ok:
	INCDEC		1
	JUMP		ExpertAI_201_end	

ExpertAI_201_ng:
	INCDEC		-1
ExpertAI_201_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_204:	// オーバーヒート 2010.3.14
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_204_ng

	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_AMANOZYAKU,ExpertAI_204_AMANOZYAKU

	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_204_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_204_ng

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_204_1

	IF_HP_OVER	CHECK_ATTACK,60,ExpertAI_204_end
	JUMP		ExpertAI_204_ng	

ExpertAI_204_AMANOZYAKU:
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_204_end
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_204_1

	IF_HP_UNDER	CHECK_ATTACK,50,ExpertAI_204_end

	IF_RND_UNDER	50,ExpertAI_215_1
	INCDEC		2
	JUMP		ExpertAI_204_end	

ExpertAI_204_1:
	IF_HP_OVER	CHECK_ATTACK,80,ExpertAI_204_end

ExpertAI_204_ng:
	INCDEC		-1
ExpertAI_204_end:
	AIEND
//---------------------------------------------------------------------------
ExpertAI_210:	// みずあそび
	IF_HP_UNDER	CHECK_ATTACK,50,ExpertAI_210_ng	

	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_HONOO,ExpertAI_210_ok
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_HONOO,ExpertAI_210_ok

	JUMP		ExpertAI_210_ng	

ExpertAI_210_ok:
	INCDEC		1
	JUMP		ExpertAI_210_end	

ExpertAI_210_ng:
	INCDEC		-1
ExpertAI_210_end:
	AIEND

//---------------------------------------------------------------------------
ExpertAI_212:	// りゅうのまい
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_212_ok

	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_212_end	

	IF_RND_UNDER	50,ExpertAI_212_end
	INCDEC		-1
	JUMP		ExpertAI_212_end
	
ExpertAI_212_ok:
	IF_RND_UNDER	50,ExpertAI_212_end

	INCDEC		1
ExpertAI_212_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_215:	// じゅうりょくok

	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_HUYUU,ExpertAI_215_1
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_FLYING,ExpertAI_215_1
	
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_HIKOU,ExpertAI_215_1	// ひこうタイプ
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_HIKOU,ExpertAI_215_1	// ひこうタイプ
	
	IF_HP_UNDER	CHECK_ATTACK,60,ExpertAI_215_end	
	IF_RND_UNDER	128,ExpertAI_215_1
	JUMP	ExpertAI_215_end	
	
ExpertAI_215_1:	
	IF_RND_UNDER	64,ExpertAI_215_end
	INCDEC	+1
	
ExpertAI_215_end:	
	AIEND
//---------------------------------------------------------------------------


ExpertAI_216:	// ミラクルアイok
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_AKU,ExpertAI_216_ok1	// あくタイプ
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_AKU,ExpertAI_216_ok1	// あくタイプ

	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,8,ExpertAI_216_ok2

	INCDEC		-2
	AIEND

ExpertAI_216_ok1:
	IF_RND_UNDER	80,ExpertAI_216_end
ExpertAI_216_ok2:
	IF_RND_UNDER	80,ExpertAI_216_end
	INCDEC		2
ExpertAI_216_end:
	AIEND

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_217:	// めざましビンタok

	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_217_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_217_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_217_ng
	IF_WAZASICK		CHECK_DEFENCE,WAZASICK_NEMURI,ExpertAI_217_ok
	JUMP		ExpertAI_217_end

ExpertAI_217_ng:
	INCDEC		-1
	JUMP	ExpertAI_217_end

ExpertAI_217_ok:
	INCDEC		1
ExpertAI_217_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_218:	// アームハンマーok

	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_218_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_218_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_218_ng
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_218_ok
	
	JUMP		ExpertAI_218_end

ExpertAI_218_ng:
	INCDEC		-1
	AIEND

ExpertAI_218_ok:
	INCDEC		1
ExpertAI_218_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_219:	// ジャイロボール
//ダメージ計算に入れたので削除

//	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_219_ng
//	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_219_ng
//	FIELD_CONDITION_CHECK	FIELD_CONDITION_TRICKROOM,ExpertAI_219_trickroom	//トリックルーム中か否か	
//	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_219_ok
//	JUMP		ExpertAI_219_end
//ExpertAI_219_trickroom:
//	IF_FIRST	IF_FIRST_ATTACK,ExpertAI_219_ok
//	JUMP		ExpertAI_219_end
//ExpertAI_219_ng:
//	INCDEC		-1
//	AIEND
//ExpertAI_219_ok:
//	IF_RND_UNDER	160,ExpertAI_219_end
//	INCDEC		1
//ExpertAI_219_end:
	AIEND
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
ExpertAI_221:	// しおみずok

	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_221_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_221_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_221_ng
	
	IF_HP_OVER	CHECK_DEFENCE,50,ExpertAI_221_end
	INCDEC	1
	IF_RND_UNDER	128,ExpertAI_221_end
	INCDEC	1
	JUMP	ExpertAI_221_end

ExpertAI_221_ng:
	INCDEC		-1

ExpertAI_221_end:
	AIEND
//---------------------------------------------------------------------------
	
//---------------------------------------------------------------------------
ExpertAI_223:	// フェイント
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,111,ExpertAI_223_mamoru		//まもる
	IF_RND_UNDER	64,ExpertAI_223_mamoru
	JUMP	ExpertAI_223_end

ExpertAI_223_mamoru:
	CHECK_TOKUSEI	CHECK_ATTACK
	IFN_EQUAL	TOKUSYU_KONZYOU,ExpertAI_223_konjopass

	IF_HAVE_ITEM	CHECK_ATTACK,ITEM_KAENDAMA,ExpertAI_223_konjo
	IF_HAVE_ITEM	CHECK_ATTACK,ITEM_DOKUDOKUDAMA,ExpertAI_223_konjo

	JUMP  ExpertAI_223_konjopass

ExpertAI_223_konjo:
	CHECK_TURN
	IFN_EQUAL	0,ExpertAI_223_konjopass

	INCDEC	2                                           //　こんじょうで　かえんだま　どくどくだまを持っている。

ExpertAI_223_konjopass:
	IF_DOKUDOKU   CHECK_ATTACK,                       ExpertAI_223_ok1		// どくどく
	IF_WAZASICK		CHECK_ATTACK,WAZASICK_NOROI,        ExpertAI_223_ok1		// のろい
	IF_WAZASICK	  CHECK_ATTACK,WAZASICK_HOROBINOUTA,  ExpertAI_223_ok1		// ほろび
	IF_WAZASICK		CHECK_ATTACK,WAZASICK_MEROMERO,     ExpertAI_223_ok1		//メロメロ
	IF_WAZASICK	  CHECK_ATTACK,WAZASICK_YADORIGI,     ExpertAI_223_ok1		// やどりぎ
	IF_WAZASICK	  CHECK_ATTACK,WAZASICK_AKUBI,        ExpertAI_223_ok1		// あくび
	
	IF_HP_EQUAL		CHECK_DEFENCE,100,ExpertAI_223_count
	CHECK_SOUBI_EQUIP	CHECK_DEFENCE
	IFN_TABLE_JUMP	ExpertAI_223_Table,ExpertAI_223_count	//装備アイテム
	

ExpertAI_223_ok1:
	IF_RND_UNDER	128,ExpertAI_223_count
	INCDEC	1

ExpertAI_223_count:
	CHECK_MAMORU_COUNT	CHECK_DEFENCE
	IF_EQUAL		0,ExpertAI_223_1
	IF_EQUAL		1,ExpertAI_223_2
	IF_OVER			2,ExpertAI_223_ng

ExpertAI_223_1:
	IF_RND_UNDER	128,ExpertAI_223_end
	INCDEC	1
	JUMP	ExpertAI_223_end

ExpertAI_223_2:
	IF_RND_UNDER	192,ExpertAI_223_end
	INCDEC	1
	JUMP	ExpertAI_223_end

ExpertAI_223_ng:
	INCDEC		-2
ExpertAI_223_end:
	AIEND

ExpertAI_223_Table://装備アイテム
	.long	SOUBI_HPKAIHUKUMAITURN
	.long	SOUBI_DOKUKAIHUKU
	.long	0xffffffff


//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_224:	// ついばむok

	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_224_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_224_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_224_ng
	
	CHECK_NEKODAMASI	CHECK_ATTACK
	IF_EQUAL	0,ExpertAI_224_1
	IF_RND_UNDER	64,ExpertAI_224_1
	INCDEC	1

ExpertAI_224_1:
	IF_RND_UNDER	128,ExpertAI_224_end
	INCDEC	1
	JUMP	ExpertAI_224_end

ExpertAI_224_ng:
	INCDEC	-1
ExpertAI_224_end:
	AIEND


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ExpertAI_225:	// おいかぜok
	IF_RND_UNDER	64,ExpertAI_225_end
	IF_FIRST	IF_FIRST_ATTACK,ExpertAI_225_ng
	IF_HP_UNDER	CHECK_ATTACK,31,ExpertAI_225_ng
	IF_HP_OVER	CHECK_ATTACK,75,ExpertAI_225_ok
	IF_RND_UNDER	64,ExpertAI_225_end
ExpertAI_225_ok:
	INCDEC	1
	JUMP	ExpertAI_225_end
ExpertAI_225_ng:
	INCDEC	-1
ExpertAI_225_end:	
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_226:	// つぼをつくok
	IF_HP_UNDER	CHECK_ATTACK,51,ExpertAI_226_ng
	IF_HP_OVER	CHECK_ATTACK,90,ExpertAI_226_ok
	IF_RND_UNDER	128,ExpertAI_226_end

ExpertAI_226_ok:	// つぼをつく
	IF_RND_UNDER	64,ExpertAI_226_end
	INCDEC	1
	JUMP	ExpertAI_226_end

ExpertAI_226_ng:
	INCDEC	-1
ExpertAI_226_end:
	AIEND
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ExpertAI_227:	// メタルバースト：カウンターを流用
	IF_WAZASICK		CHECK_DEFENCE,WAZASICK_NEMURI,ExpertAI_227_ng
	IF_WAZASICK   CHECK_DEFENCE,WAZASICK_MEROMERO,ExpertAI_227_ng	// メロメロ
	IF_WAZASICK	  CHECK_DEFENCE,WAZASICK_KONRAN,ExpertAI_227_ng		// こんらん
	//相手が後攻技を持っている場合
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,185,ExpertAI_227_ng		// リベンジ
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,170,ExpertAI_227_ng		// きあいパンチ
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,78,ExpertAI_227_ng		// あてみなげ
	

	IF_HP_OVER	CHECK_ATTACK,30,ExpertAI_227_1	

	IF_RND_UNDER	10,ExpertAI_227_1
	INCDEC	-1

ExpertAI_227_1:
	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_227_koukou	

	IF_RND_UNDER	100,ExpertAI_227_koukou
	INCDEC	-1


ExpertAI_227_koukou:
	IF_RND_UNDER	192,ExpertAI_227_2
	INCDEC	1


ExpertAI_227_2:
	CHECK_LAST_WAZA	CHECK_DEFENCE
	CHECK_WORKWAZA_POW
	IF_EQUAL	0,ExpertAI_227_3

	IFN_CHOUHATSU	ExpertAI_227_3	

	IF_RND_UNDER	100,ExpertAI_227_3
	INCDEC	1

ExpertAI_227_3:
	IFN_CHOUHATSU	ExpertAI_227_end	

	IF_RND_UNDER	100,ExpertAI_227_end
	INCDEC	1
	JUMP	ExpertAI_227_end


ExpertAI_227_ng:
	INCDEC	-1
ExpertAI_227_end:
	AIEND

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
ExpertAI_228:	// とんぼがえりok(ほぼ)
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_228_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_228_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_228_ng
	
	CHECK_BENCH_COUNT	CHECK_ATTACK
	IF_EQUAL	0,ExpertAI_228_end			// 味方の控えがいない
	
	IF_HAVE_BATSUGUN	ExpertAI_228_ng1	// 抜群を持っている
	JUMP	ExpertAI_228_1


ExpertAI_228_ng:
	INCDEC	-1
	JUMP	ExpertAI_228_end	

ExpertAI_228_ng1:
	IF_RND_UNDER	64,ExpertAI_228_1
	INCDEC	-2
	
ExpertAI_228_1:
	IF_BENCH_DAMAGE_MAX		LOSS_CALC_OFF,ExpertAI_228_2

	IF_RND_UNDER	64,ExpertAI_228_2
	INCDEC	-2
	JUMP	ExpertAI_228_end	

ExpertAI_228_2:
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_228_3
	IF_HP_OVER	CHECK_DEFENCE,30,ExpertAI_228_4
	IF_RND_UNDER	128,ExpertAI_228_5
	JUMP	ExpertAI_228_4

	
ExpertAI_228_3:
	IF_RND_UNDER	64,ExpertAI_228_4
	INCDEC	+1

ExpertAI_228_4:
	IF_RND_UNDER	128,ExpertAI_228_5
	INCDEC	+1
	
ExpertAI_228_5:
	IF_FIRST	IF_FIRST_ATTACK,ExpertAI_228_ok
	IF_RND_UNDER	128,ExpertAI_228_end

ExpertAI_228_ok:
	INCDEC	1
ExpertAI_228_end:
	AIEND

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ExpertAI_229:	// インファイト：オーバーヒートを流用ok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_229_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_229_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_229_ng

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_229_1

	IF_HP_OVER	CHECK_ATTACK,60,ExpertAI_229_end
	JUMP		ExpertAI_229_ng	

ExpertAI_229_1:
	IF_HP_OVER	CHECK_ATTACK,80,ExpertAI_229_end

ExpertAI_229_ng:
	INCDEC		-1
ExpertAI_229_end:
	AIEND

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_230:	// しっぺがえしok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_230_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_230_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_230_ng
	IF_FIRST	IF_FIRST_ATTACK,ExpertAI_230_end
	IF_HP_UNDER	CHECK_ATTACK,30, ExpertAI_230_end
	IF_RND_UNDER	64,ExpertAI_230_end
	INCDEC	1
	AIEND

ExpertAI_230_ng:	
	INCDEC	-1

ExpertAI_230_end:	
	AIEND
	
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_231:	// ダメおしok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_231_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_231_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_231_ng
	IF_FIRST	IF_FIRST_ATTACK,ExpertAI_231_end
	
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_SAMEHADA, ExpertAI_231_1//さめはだ

	CHECK_SOUBI_ITEM	CHECK_ATTACK
	IF_TABLE_JUMP	ExpertAI_231_Table,ExpertAI_231_1//ダメージを与える木の実
	
	IF_RND_UNDER	128,ExpertAI_231_1
	JUMP	ExpertAI_231_end
		

ExpertAI_231_ng:	
	INCDEC	-1
	JUMP	ExpertAI_231_end

ExpertAI_231_1:	
	IF_RND_UNDER	128,ExpertAI_231_end
	INCDEC	1

ExpertAI_231_end:	
	AIEND

ExpertAI_231_Table://木の実ナンバー
	.long	ITEM_ZYAPONOMI,ITEM_RENBUNOMI
	.long	0xffffffff
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_232:	// さしおさえok
	IF_RND_UNDER	128,ExpertAI_232_end
	INCDEC	1
ExpertAI_232_end:	// さしおさえ
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_233:	// なげつけるok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_233_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_233_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_233_ng
	
	CHECK_NAGETSUKERU_IRYOKU	CHECK_ATTACK
	IF_UNDER	30,ExpertAI_233_nagenai	//30未満は攻撃系でないので投げない
	IF_OVER		90,ExpertAI_233_nageru1	//91以上は超攻撃系なので投げる
	IF_OVER		60,ExpertAI_233_nageru2	//61以上はそこそこ攻撃系なので投げる
	IF_RND_UNDER	128,ExpertAI_233_end
	INCDEC	-1
	JUMP	ExpertAI_233_end

ExpertAI_233_nagenai:
	INCDEC	-2
	JUMP	ExpertAI_233_end


ExpertAI_233_nageru1:
	CHECK_WAZA_AISYOU	AISYOU_2BAI,ExpertAI_233_nageru1_ok
	CHECK_WAZA_AISYOU	AISYOU_4BAI,ExpertAI_233_nageru1_ok
	IF_RND_UNDER	128,ExpertAI_233_nageru2
	INCDEC	1
	JUMP	ExpertAI_233_nageru2

ExpertAI_233_nageru1_ok:
	INCDEC	4

ExpertAI_233_nageru2:
	IF_RND_UNDER	64,ExpertAI_233_end
	INCDEC	1
	JUMP	ExpertAI_233_end

ExpertAI_233_ng:	
	CHECK_SOUBI_EQUIP	CHECK_ATTACK
	IF_TABLE_JUMP	ExpertAI_233_Table,ExpertAI_233_end
	INCDEC	-1

ExpertAI_233_end:	
	AIEND

ExpertAI_233_Table://相性が悪くても状態異常系ならば減算しない
	.long	SOUBI_HIRUMASERU				//ひるませる
	.long	SOUBI_DOKUBARIUP				//どくにする	
	.long	SOUBI_TEKINIMOTASERUTOMOUDOKU	//もうどくにする	
	.long	SOUBI_TTEKINIMOTASERUTOYAKEDO	//やけどににする
	.long	SOUBI_PIKATYUUTOKUKOUNIBAI		//まひにする
	.long	0xffffffff


//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_234:	// サイコシフトok
	IFN_POKESICK	CHECK_ATTACK,AI_DEC10	// 自分が状態異常でない場合やらない
	IF_RND_UNDER	128,ExpertAI_234_end
	IF_HP_UNDER	CHECK_DEFENCE,30, ExpertAI_234_end
	INCDEC	1

ExpertAI_234_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_235:	// きりふだok
	
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_235_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_235_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_235_ng
	//IFN_WAZANO	WAZANO_KIRIHUDA,	ExpertAI_235_end//きりふだでない:認識していない2006.7.20
	CHECK_PP_REMAIN
	IF_EQUAL	1,ExpertAI_235_ok	//出せる
	IF_EQUAL	2,ExpertAI_235_2	//あと１
	IF_EQUAL	3,ExpertAI_235_3	//あと２

	//相手がプレッシャーを持っていると出しやすい
	CHECK_TOKUSEI	CHECK_DEFENCE
	IFN_EQUAL	TOKUSYU_PURESSYAA, ExpertAI_235_1
	IF_RND_UNDER	30,ExpertAI_235_1
	INCDEC	1

//必ず当たるを移植--------------------------------------
ExpertAI_235_1:
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,10,ExpertAI_235_2
	IF_PARA_UNDER	CHECK_ATTACK,PARA_HIT,2,ExpertAI_235_2
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,8,ExpertAI_235_3
	IF_PARA_UNDER	CHECK_ATTACK,PARA_HIT,4,ExpertAI_235_3
	JUMP		ExpertAI_235_end

ExpertAI_235_2:
	INCDEC		1
ExpertAI_235_3:
	IF_RND_UNDER	100,ExpertAI_235_end
	INCDEC		1

	JUMP		ExpertAI_235_end
//------------------------------------------------------------

ExpertAI_235_ok:
	//IF_RND_UNDER	25,ExpertAI_235_end
	INCDEC	3
	JUMP	ExpertAI_235_end


ExpertAI_235_ng:	
	INCDEC	-1
ExpertAI_235_end:	

	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_236:	// かいふくふうじ

	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,8,ExpertAI_236_ok		//ゆめくいok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,32,ExpertAI_236_ok	//じこさいせいok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,214,ExpertAI_236_ok	//はやねすみok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,157,ExpertAI_236_ok	//たまごうみok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,132,ExpertAI_236_ok	//つきのひかりok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,37,ExpertAI_236_ok	//ねむるok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,162,ExpertAI_236_ok	//のみこむok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,3,ExpertAI_236_ok		//すいとるok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,181,ExpertAI_236_ok	//ねをはる
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,251,ExpertAI_236_ok	//アクアリングok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,84,ExpertAI_236_ok	//やどりぎのタネ
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,220,ExpertAI_236_ok	//いやしのねがいok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,270,ExpertAI_236_ok	//みかづきのまいok
	IF_WAZASICK	CHECK_ATTACK,WAZASICK_YADORIGI,ExpertAI_236_ok// 自分やどられ中ok
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_AQUARING,ExpertAI_236_ok// 相手アクアリング中ok
	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_NEWOHARU,ExpertAI_236_ok//相手ねをはる中ok
	IF_RND_UNDER	96,ExpertAI_236_ok
	JUMP	ExpertAI_236_end

ExpertAI_236_ok:
	IF_RND_UNDER	25,ExpertAI_236_end
	INCDEC	1
ExpertAI_236_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_237:	// しぼりとるok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_237_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_237_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_237_ng
	IF_HP_UNDER	CHECK_DEFENCE,50, ExpertAI_237_ng
	IF_HP_EQUAL	CHECK_DEFENCE,100, ExpertAI_237_ok1
	IF_HP_OVER	CHECK_DEFENCE,85, ExpertAI_237_ok2
	JUMP	ExpertAI_237_end
	

ExpertAI_237_ok1:	
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_237_ok1_1
	INCDEC	1
ExpertAI_237_ok1_1:	
	INCDEC	1
ExpertAI_237_ok2:	
	IF_RND_UNDER	25,ExpertAI_237_end
	INCDEC	1
	JUMP	ExpertAI_237_end

ExpertAI_237_ng:	
	INCDEC	-1
ExpertAI_237_end:	
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_238:	// パワートリックok
	//ＨＰをみて適度に繰り出す。１度行うとやらない。basicAIで対処。
	IF_HP_OVER	CHECK_ATTACK,90,ExpertAI_238_1
	IF_HP_OVER	CHECK_ATTACK,60,ExpertAI_238_2
	IF_HP_OVER	CHECK_ATTACK,30,ExpertAI_238_3
	JUMP	AI_DEC2

ExpertAI_238_1:
	IF_RND_UNDER	96,ExpertAI_238_end
	INCDEC	+1
	JUMP	ExpertAI_238_end
	
ExpertAI_238_2:
	IF_RND_UNDER	128,ExpertAI_238_end
	INCDEC	+1
	JUMP	ExpertAI_238_end

ExpertAI_238_3:
	IF_RND_UNDER	164,ExpertAI_238_end
	INCDEC	+1
	JUMP	ExpertAI_238_end

ExpertAI_238_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_298:			// 	シンプルビーム
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,10,ExpertAI_239_ng		//  アップ系
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,11,ExpertAI_239_ng		//  アップ系
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,12,ExpertAI_239_ng		//  アップ系
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,13,ExpertAI_239_ng		//  アップ系
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,14,ExpertAI_239_ng		//  アップ系
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,15,ExpertAI_239_ng		//  アップ系
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,16,ExpertAI_239_ng		//  アップ系
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,50,ExpertAI_239_ng		//  アップ系
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,51,ExpertAI_239_ng		//  アップ系
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,52,ExpertAI_239_ng		//  アップ系
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,53,ExpertAI_239_ng		//  アップ系
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,54,ExpertAI_239_ng		//  アップ系
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,55,ExpertAI_239_ng		//  アップ系
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,56,ExpertAI_239_ng		//  アップ系
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,109,ExpertAI_239_ng		//  のろい 
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,206,ExpertAI_239_ng		//  コスモパワー 
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,208,ExpertAI_239_ng		//  ビルドアップ 
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,211,ExpertAI_239_ng		//  めいそう 
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,212,ExpertAI_239_ng		//  りゅうのまい 
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,226,ExpertAI_239_ng		//  つぼをつく 
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,277,ExpertAI_239_ng		//  つめとぎ 
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,290,ExpertAI_239_ng		//  ちょうのまい
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,308,ExpertAI_239_ng		//  からをやぶる
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,316,ExpertAI_239_ng		//  せいちょう
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,322,ExpertAI_239_ng		//  とぐろをまく

	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,18,ExpertAI_239_ok		//  ダウン系
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,19,ExpertAI_239_ok		//  ダウン系
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,20,ExpertAI_239_ok		//  ダウン系
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,21,ExpertAI_239_ok		//  ダウン系
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,22,ExpertAI_239_ok		//  ダウン系
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,23,ExpertAI_239_ok		//  ダウン系
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,24,ExpertAI_239_ok		//  ダウン系
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,58,ExpertAI_239_ok		//  ダウン系
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,59,ExpertAI_239_ok		//  ダウン系
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,60,ExpertAI_239_ok		//  ダウン系
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,61,ExpertAI_239_ok		//  ダウン系
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,62,ExpertAI_239_ok		//  ダウン系
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,63,ExpertAI_239_ok		//  ダウン系
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,64,ExpertAI_239_ok		//  ダウン系

ExpertAI_239:	    // いえき
	CHECK_TOKUSEI	CHECK_ATTACK
	IF_TABLE_JUMP	ExpertAI_239_Table,ExpertAI_239_ok

	IF_RND_UNDER		64,ExpertAI_239_end
	INCDEC		1
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_239_end

	IF_RND_UNDER		128,ExpertAI_239_1
	INCDEC		-1
ExpertAI_239_1:
	IF_HP_OVER	CHECK_DEFENCE,50,ExpertAI_239_end
	INCDEC		-1
	IF_HP_OVER	CHECK_DEFENCE,30,ExpertAI_239_end
	INCDEC		-1
	JUMP		ExpertAI_239_end

ExpertAI_239_ng:
	INCDEC		-1
	JUMP		ExpertAI_239_end

ExpertAI_239_ok:
	IF_RND_UNDER		40,ExpertAI_239_end
	INCDEC		1
ExpertAI_239_end:	
	AIEND

ExpertAI_239_Table:
	.long	TOKUSYU_NOOGAADO  	      //ノーガード
	.long	TOKUSYU_ITAZURAGOKORO  	  //いたずらごころ
	.long	TOKUSYU_MIRAKURUSUKIN  	  //ミラクルスキン
	.long	TOKUSYU_SYUUKAKU  		    //しゅうかく
	.long	TOKUSYU_NETUBOUSOU		    //ねつぼうそう
	.long	TOKUSYU_DOKUBOUSOU		    //どくぼうそう
	.long	TOKUSYU_TEKUNISYAN		    //テクニシャン
	.long	TOKUSYU_MAZIKKUGAADO		  //マジックガード
	.long	TOKUSYU_NOOMARUSUKIN		  //ノーマルスキン
	.long	TOKUSYU_POIZUNHIIRU		    //ポイズンヒール
	.long	TOKUSYU_TIKARAMOTI		    //ちからもち
	.long	TOKUSYU_HUSIGINAMAMORI		//ふしぎなまもり
	.long	TOKUSYU_KAGEHUMI		      //かげふみ
	.long	0xffffffff

	IF_EQUAL	TOKUSYU_NOOGAADO,ExpertAI_1_4	  	// ノーガード

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_240:	// おまじないok

	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_240_ng
	//クリティカル技
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,43,ExpertAI_240_ok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,200,ExpertAI_240_ok
	IF_HAVE_WAZA_SEQNO	CHECK_DEFENCE,209,ExpertAI_240_ok
	IF_RND_UNDER	64,ExpertAI_240_ok
	JUMP	ExpertAI_240_end

ExpertAI_240_ok:
	INCDEC	1
	JUMP	ExpertAI_240_end

ExpertAI_240_ng:
	INCDEC	-1
ExpertAI_240_end:
	AIEND
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ExpertAI_241:	// さきどり
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_241_ng
	
	IF_LAST_WAZA_DAMAGE_CHECK	CHECK_DEFENCE,LOSS_CALC_OFF,ExpertAI_241_Ok
	JUMP	ExpertAI_241_1

ExpertAI_241_Ok:
	IF_RND_UNDER	32,ExpertAI_241_1
	INCDEC	1	

ExpertAI_241_1:
	
	//相手が最後に出した技が変化技
	CHECK_LAST_WAZA_KIND
	IF_EQUAL	WAZADATA_DMG_NONE,ExpertAI_241_2
	IF_RND_UNDER	128,ExpertAI_241_end
	INCDEC	1	

ExpertAI_241_2:
	IF_RND_UNDER	64,ExpertAI_241_end
	INCDEC	1	
	JUMP	ExpertAI_241_end

ExpertAI_241_ng:
	INCDEC	-2	

ExpertAI_241_end:
	AIEND
	
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
ExpertAI_242:	// まねっこ
//まねっこ技が自分の持っている技のなかで一番強い技の場合点数を加算する追加
//自分の方が早くても相手の技が強力でなければ結果的に減算する。

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_242_1
	
	IF_LAST_WAZA_DAMAGE_CHECK	CHECK_DEFENCE,LOSS_CALC_OFF,ExpertAI_242_ok

	CHECK_LAST_WAZA	CHECK_DEFENCE
	IFN_TABLE_JUMP	ExpertAI_242_Table,ExpertAI_242_1
		
	IF_RND_UNDER	128,ExpertAI_242_3
	INCDEC		2				//強いわざは返す
	JUMP		ExpertAI_242_3

ExpertAI_242_ok:
	IF_RND_UNDER	32,ExpertAI_242_3
	INCDEC		2	
	JUMP		ExpertAI_242_3


ExpertAI_242_1://相手の方が早い場合強い技でなければ減算する
	IF_LAST_WAZA_DAMAGE_CHECK	CHECK_DEFENCE,LOSS_CALC_OFF,ExpertAI_242_3
	CHECK_LAST_WAZA	CHECK_DEFENCE
	IF_TABLE_JUMP	ExpertAI_242_Table,ExpertAI_242_3

	IF_RND_UNDER	80,ExpertAI_242_3
	INCDEC		-1
ExpertAI_242_3:
	AIEND

ExpertAI_242_Table://変化して強そうな技群
	.long	79,142,147,95,47,320,325,28,108,92
	.long	12,32,90,329,238,177,109,186,103,178
	.long	184,313,319,86,137,77,247,223,63,245
	.long	168,343,213,207,259,260,271,276,285
	//2006.6.14
	.long	375,384,385,389,391,415,445,464	
	.long	0xffffffff


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ExpertAI_243:	// パワースワップok
	CHECK_STATUS_DIFF	CHECK_DEFENCE,PTL_ABILITY_ATK//（対象相手（DEFENCE）ーATTACK）
	IF_OVER			3,ExpertAI_243_ok1	//4以上
	IF_OVER			1,ExpertAI_243_ok2	//2以上
	IF_OVER			0,ExpertAI_243_ok3	//1以上
	IF_EQUAL		0,ExpertAI_243_ok4	//0
	JUMP	ExpertAI_243_end

ExpertAI_243_ok1:
	CHECK_STATUS_DIFF	CHECK_DEFENCE,PTL_ABILITY_SPATK
	IF_OVER			3,ExpertAI_243_05	//4以上
	IF_OVER			1,ExpertAI_243_04	//2以上
	IF_EQUAL		0,ExpertAI_243_03	//0
	JUMP	ExpertAI_243_end
	
ExpertAI_243_ok2:
	CHECK_STATUS_DIFF	CHECK_DEFENCE,PTL_ABILITY_SPATK
	IF_OVER			3,ExpertAI_243_04	//4以上
	IF_OVER			1,ExpertAI_243_03	//2以上
	IF_EQUAL		0,ExpertAI_243_02	//0
	JUMP	ExpertAI_243_end

ExpertAI_243_ok3:
	CHECK_STATUS_DIFF	CHECK_DEFENCE,PTL_ABILITY_SPATK
	IF_OVER			3,ExpertAI_243_03	//4以上
	IF_OVER			1,ExpertAI_243_02	//2以上
	IF_EQUAL		0,ExpertAI_243_01	//0
	JUMP	ExpertAI_243_end

ExpertAI_243_ok4:
	CHECK_STATUS_DIFF	CHECK_DEFENCE,PTL_ABILITY_SPATK
	IF_OVER			3,ExpertAI_243_03	//4以上
	IF_OVER			1,ExpertAI_243_02	//2以上
	IF_OVER			0,ExpertAI_243_01	//0以上
	JUMP	ExpertAI_243_end

ExpertAI_243_05:
	IF_RND_UNDER	128,ExpertAI_243_04
	INCDEC	+5
	JUMP	ExpertAI_243_end

ExpertAI_243_04:
	IF_RND_UNDER	128,ExpertAI_243_03
	INCDEC	+4
	JUMP	ExpertAI_243_end
	
ExpertAI_243_03:
	IF_RND_UNDER	128,ExpertAI_243_02
	INCDEC	+3
	JUMP	ExpertAI_243_end
	
ExpertAI_243_02:
	IF_RND_UNDER	128,ExpertAI_243_01
	INCDEC	+2
	JUMP	ExpertAI_243_end
	
ExpertAI_243_01:
	IF_RND_UNDER	128,ExpertAI_243_end
	INCDEC	+1
	JUMP	ExpertAI_243_end

ExpertAI_243_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_244:	// ガードスワップok
	CHECK_STATUS_DIFF	CHECK_DEFENCE,PTL_ABILITY_DEF//（対象相手（DEFENCE）ーATTACK）
	IF_OVER			3,ExpertAI_244_ok1	//4以上
	IF_OVER			1,ExpertAI_244_ok2	//2以上
	IF_OVER			0,ExpertAI_244_ok3	//1以上
	IF_EQUAL		0,ExpertAI_244_ok4	//0
	JUMP	ExpertAI_244_end

ExpertAI_244_ok1:
	CHECK_STATUS_DIFF	CHECK_DEFENCE,PTL_ABILITY_SPDEF
	IF_OVER			3,ExpertAI_244_05	//4以上
	IF_OVER			1,ExpertAI_244_04	//2以上
	IF_EQUAL		0,ExpertAI_244_03	//0
	JUMP	ExpertAI_244_end
	
ExpertAI_244_ok2:
	CHECK_STATUS_DIFF	CHECK_DEFENCE,PTL_ABILITY_SPDEF
	IF_OVER			3,ExpertAI_244_04	//4以上
	IF_OVER			1,ExpertAI_244_03	//2以上
	IF_EQUAL		0,ExpertAI_244_02	//0
	JUMP	ExpertAI_244_end

ExpertAI_244_ok3:
	CHECK_STATUS_DIFF	CHECK_DEFENCE,PTL_ABILITY_SPDEF
	IF_OVER			3,ExpertAI_244_03	//4以上
	IF_OVER			1,ExpertAI_244_02	//2以上
	IF_EQUAL		0,ExpertAI_244_01	//0
	JUMP	ExpertAI_244_end

ExpertAI_244_ok4:
	CHECK_STATUS_DIFF	CHECK_DEFENCE,PTL_ABILITY_SPDEF
	IF_OVER			3,ExpertAI_244_03	//4以上
	IF_OVER			1,ExpertAI_244_02	//2以上
	IF_OVER			0,ExpertAI_244_01	//0以上
	JUMP	ExpertAI_244_end

ExpertAI_244_05:
	IF_RND_UNDER	128,ExpertAI_244_04
	INCDEC	+5
	JUMP	ExpertAI_244_end

ExpertAI_244_04:
	IF_RND_UNDER	128,ExpertAI_244_03
	INCDEC	+4
	JUMP	ExpertAI_244_end
	
ExpertAI_244_03:
	IF_RND_UNDER	128,ExpertAI_244_02
	INCDEC	+3
	JUMP	ExpertAI_244_end
	
ExpertAI_244_02:
	IF_RND_UNDER	128,ExpertAI_244_01
	INCDEC	+2
	JUMP	ExpertAI_244_end
	
ExpertAI_244_01:
	IF_RND_UNDER	128,ExpertAI_244_end
	INCDEC	+1
	JUMP	ExpertAI_244_end

ExpertAI_244_end:
	AIEND

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_245:	// おしおきok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_245_end
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_245_end
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_245_end
	CHECK_STATUS_UP	CHECK_DEFENCE
	IF_OVER		6,ExpertAI_245_ok1
	IF_OVER		5,ExpertAI_245_ok2
	IF_OVER		4,ExpertAI_245_ok3
	IF_OVER		3,ExpertAI_245_ok4
	IF_OVER		2,ExpertAI_245_ok4
	JUMP	ExpertAI_245_end

ExpertAI_245_ok1:
	IF_RND_UNDER	128,ExpertAI_245_ok2
	INCDEC	+4

ExpertAI_245_ok2:
	IF_RND_UNDER	128,ExpertAI_245_ok3
	INCDEC	+3

ExpertAI_245_ok3:
	IF_RND_UNDER	128,ExpertAI_245_ok4
	INCDEC	+2
	
ExpertAI_245_ok4:
	IF_RND_UNDER	128,ExpertAI_245_end
	INCDEC	+1

ExpertAI_245_end:	
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_246:	// とっておきok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_246_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_246_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_246_ng
	IF_TOTTEOKI	CHECK_ATTACK,ExpertAI_246_ok// とっておき出せる
	JUMP	ExpertAI_246_end

ExpertAI_246_ng:
	INCDEC	-1
	JUMP	ExpertAI_246_end
ExpertAI_246_ok:
	INCDEC	1
ExpertAI_246_end:	
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_247:	// なやみのタネ
	IFN_HAVE_WAZA	CHECK_DEFENCE, WAZANO_NEMURU, ExpertAI_247_1
	INCDEC	+1

ExpertAI_247_1:
	IF_HP_UNDER	CHECK_ATTACK,50,ExpertAI_247_2
	IF_RND_UNDER	128,ExpertAI_247_2
	INCDEC	+1
	
ExpertAI_247_2:
	IF_RND_UNDER	64,ExpertAI_247_end
	INCDEC	+1
	JUMP	ExpertAI_247_end

ExpertAI_247_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_248:	// ふいうちok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_248_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_248_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_248_ng

	IF_RND_UNDER	64,ExpertAI_248_end
	INCDEC	1
	JUMP	ExpertAI_248_end

ExpertAI_248_ng:
	INCDEC	-1
ExpertAI_248_end:	
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_249:	// どくびしok
	IF_RND_UNDER	128,ExpertAI_249_end
	INCDEC	+1
	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_HOERU, ExpertAI_249_ok
	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_HUKITOBASI, ExpertAI_249_ok
	JUMP	ExpertAI_249_end
	
ExpertAI_249_ok:
	IF_RND_UNDER	64,ExpertAI_249_end
	INCDEC	+1

ExpertAI_249_end:	
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_250:	// ハートスワップ：じこあんじを借用
	IF_PARA_OVER	CHECK_DEFENCE,PARA_POW,7,ExpertAI_250_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_DEF,7,ExpertAI_250_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEPOW,7,ExpertAI_250_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEDEF,7,ExpertAI_250_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,7,ExpertAI_250_1
	IF_CONTFLG	  CHECK_DEFENCE,BPP_CONTFLG_KIAIDAME,ExpertAI_250_1
	JUMP	ExpertAI_250_ng

ExpertAI_250_1:
	IF_PARA_UNDER	CHECK_ATTACK,PARA_POW,7,ExpertAI_250_ok2
	IF_PARA_UNDER	CHECK_ATTACK,PARA_DEF,7,ExpertAI_250_ok2
	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEPOW,7,ExpertAI_250_ok2
	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEDEF,7,ExpertAI_250_ok2
	IF_PARA_UNDER	CHECK_ATTACK,PARA_AVOID,7,ExpertAI_250_ok1
	IFN_CONTFLG	  CHECK_ATTACK,BPP_CONTFLG_KIAIDAME,ExpertAI_250_ok2

	IF_RND_UNDER	50,ExpertAI_250_end
	JUMP	ExpertAI_250_ng

ExpertAI_250_ok1:
	INCDEC		1
ExpertAI_250_ok2:
	INCDEC		1
	AIEND
ExpertAI_250_ng:
	INCDEC		-2
ExpertAI_250_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_251:	// アクアリングok
	IF_HP_UNDER	CHECK_ATTACK,50,ExpertAI_251_end
	IF_RND_UNDER	128,ExpertAI_251_end
	INCDEC	+1
ExpertAI_251_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_252:	// でんじふゆうok
	IF_HP_UNDER	CHECK_ATTACK,50,ExpertAI_252_end

	IF_HAVE_WAZA	CHECK_DEFENCE, WAZANO_ZISIN, ExpertAI_252_ok1
	IF_HAVE_WAZA	CHECK_DEFENCE, WAZANO_DAITINOTIKARA, ExpertAI_252_ok1
	IF_HAVE_WAZA	CHECK_DEFENCE, WAZANO_ZIWARE, ExpertAI_252_ok1
	JUMP	ExpertAI_252_1

ExpertAI_252_ok1:
	INCDEC	1
ExpertAI_252_1:
	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_JIMEN,ExpertAI_252_ok2	// じめんタイプ
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_JIMEN,ExpertAI_252_ok2	// じめんタイプ
	IF_RND_UNDER	128,ExpertAI_252_end

ExpertAI_252_ok2:
	INCDEC	1

ExpertAI_252_end:	// でんじふゆう
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_254:	// わるあがき
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//ExpertAI_255:	// ダイビング：そらをとぶと一緒
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//ExpertAI_256:	// あなをほる：そらをとぶと一緒
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_257:	// なみのり
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_258:	// きりばらい
	//１）壁まきびしがない場合、回避率チェックのみ
	//２）壁のみある場合、壁を消しやすくする
	//３）まきびしのみの場合、しない
	//４）壁があり、まきびしがある場合、壁を消すかどうかのチェック後に回避率チェックを行う
	
	//壁チェック
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_HIKARINOKABE,ExpertAI_258_ok1	// ひかりのかべ
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_REFRECTOR,ExpertAI_258_ok1	// リフレクター

	//まきびしチェック
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_MAKIBISI,ExpertAI_258_ng1	// まきびし中
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_STEALTHROCK,ExpertAI_258_ng1 // ステルスロック中
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_DOKUBISI,ExpertAI_258_ng1 // どくびし中
	JUMP	ExpertAI_258_avoid

ExpertAI_258_ok1:
	IF_HP_OVER	CHECK_ATTACK,30,ExpertAI_258_ok1_1
	CHECK_BENCH_COUNT	CHECK_ATTACK
	IF_EQUAL	0,ExpertAI_258_1			// 味方の控えがいない

ExpertAI_258_ok1_1:
	INCDEC		1
	CHECK_BENCH_COUNT	CHECK_DEFENCE
	IF_EQUAL	0,ExpertAI_258_end			// 敵の控えがいない
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_MAKIBISI,ExpertAI_258_ng2	// まきびし中
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_STEALTHROCK,ExpertAI_258_ng2 // ステルスロック中
	IF_SIDEEFF	CHECK_DEFENCE,BTL_SIDEEFF_DOKUBISI,ExpertAI_258_ng2 // どくびし中
	JUMP	ExpertAI_258_avoid


ExpertAI_258_ng1://壁がない状態でまきびし系がある場合
	INCDEC	-2
	JUMP	ExpertAI_258_avoid

ExpertAI_258_ng2://壁がある状態でまきびし系がある場合
	IF_RND_UNDER	128,ExpertAI_258_avoid
	INCDEC	-1
	JUMP	ExpertAI_258_avoid


ExpertAI_258_avoid:

	IF_HP_UNDER	CHECK_ATTACK,70,ExpertAI_258_1
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,3,ExpertAI_258_2

ExpertAI_258_1:
	IF_RND_UNDER	50,ExpertAI_258_2
	INCDEC		-2
ExpertAI_258_2:
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_258_end

	INCDEC		-2
ExpertAI_258_end:
	AIEND


//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

ExpertAI_259:	// トリックルームok
	
	CHECK_BTL_RULE
	IF_EQUAL		BTL_RULE_DOUBLE,ExpertAI_259_end	//2VS2ならスルー
	IF_HP_OVER	CHECK_ATTACK,30,ExpertAI_259_1
	CHECK_BENCH_COUNT	CHECK_ATTACK
	IF_EQUAL	0,ExpertAI_259_end	// 味方の控えがいない

ExpertAI_259_1:	
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_259_ok
	INCDEC		-1	//basicAIですでに-10している
	JUMP		ExpertAI_259_end

ExpertAI_259_ok:
	IF_RND_UNDER	64,ExpertAI_259_end
	INCDEC		3

ExpertAI_259_end:
	AIEND

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

ExpertAI_260:	//ふぶきok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_260_1
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_260_1
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_260_1
	CHECK_WEATHER
	IFN_EQUAL	WEATHER_ARARE,ExpertAI_260_end
	INCDEC		1
	JUMP		ExpertAI_260_end
	
ExpertAI_260_1:
	IF_RND_UNDER	50,ExpertAI_260_end
	INCDEC		-3

ExpertAI_260_end:
	AIEND

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//ExpertAI_263:	// とびはねる：そらをとぶと一緒
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_265:	// ゆうわくok

	IF_PARA_EQUAL	CHECK_DEFENCE,PARA_SPEPOW,6,ExpertAI_265_2

	INCDEC		-1
	IF_HP_OVER	CHECK_ATTACK,90,ExpertAI_265_1

	INCDEC		-1
ExpertAI_265_1:
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEPOW,3,ExpertAI_265_2
	IF_RND_UNDER	50,ExpertAI_265_2
	INCDEC		-2
ExpertAI_265_2:
	IF_HP_OVER	CHECK_DEFENCE,70,ExpertAI_265_3
	INCDEC		-2

ExpertAI_265_3:
	CHECK_LAST_WAZA_KIND
	IFN_EQUAL	WAZADATA_DMG_PHYSIC,ExpertAI_265_end//物理でない
	IF_RND_UNDER	64,ExpertAI_265_end
	INCDEC		-1

ExpertAI_265_end:
	AIEND

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_266:	// ステルスロックok
	IF_RND_UNDER	128,ExpertAI_266_end
	INCDEC	+1
	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_HOERU, ExpertAI_266_ok
	IF_HAVE_WAZA	CHECK_ATTACK, WAZANO_HUKITOBASI, ExpertAI_266_ok
	JUMP	ExpertAI_266_end
	
ExpertAI_266_ok:
	IF_RND_UNDER	64,ExpertAI_266_end
	INCDEC	+1

ExpertAI_266_end:
	AIEND

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_267:	// おしゃべり
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_268:	// さばきのつぶて
//ダメージ計算に入れた
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_48:	// とっしんok
ExpertAI_198:	// すてみタックルok
ExpertAI_253:	// フレアドライブok
ExpertAI_262:	// ボルテッカーok
ExpertAI_269:	// もろはのずつきok
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_269_end	
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_269_end
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_269_end

	CHECK_TOKUSEI	CHECK_ATTACK
	IF_EQUAL	TOKUSYU_ISIATAMA, ExpertAI_269_ok
	IF_EQUAL	TOKUSYU_MAZIKKUGAADO,ExpertAI_269_ok
	JUMP	ExpertAI_269_end
ExpertAI_269_ok:
	INCDEC	+1
ExpertAI_269_end:
	AIEND
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ExpertAI_220:	// いやしのねがい：自爆を流用
ExpertAI_270:	// みかづきのまい：自爆流用

ExpertAI_270_1:
	IF_HP_UNDER	CHECK_ATTACK,80,ExpertAI_270_2
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_270_2

	IF_RND_UNDER	192,ExpertAI_270_5
	JUMP		AI_DEC5

ExpertAI_270_2:
	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_270_4
	IF_RND_UNDER	192,ExpertAI_270_3
	INCDEC		+1

	IF_HAVE_BATSUGUN	ExpertAI_270_hikae_batugun	//抜群を持っている
	IF_RND_UNDER	192,ExpertAI_270_hikae_batugun
	INCDEC		+1
	
ExpertAI_270_hikae_batugun:	

	IF_BENCH_DAMAGE_MAX		LOSS_CALC_OFF,ExpertAI_270_hikae_nostrong	//控えの方が強い技を持っている
	JUMP	ExpertAI_270_3
	
ExpertAI_270_hikae_nostrong:	
	IF_RND_UNDER	128,ExpertAI_270_3
	INCDEC	+1

ExpertAI_270_3:
	IF_HP_OVER	CHECK_ATTACK,30,ExpertAI_270_5
	IF_RND_UNDER	128,ExpertAI_270_5
	INCDEC		+1
	JUMP		ExpertAI_270_5

ExpertAI_270_4:
	IF_RND_UNDER	50,ExpertAI_270_5
	INCDEC		-1
ExpertAI_270_5:
	AIEND

ExpertAI_277:			// 	つめとぎ
	IF_PARA_UNDER	CHECK_ATTACK,PARA_HIT,9,ExpertAI_277_1
	IF_RND_UNDER	50,ExpertAI_277_1
	INCDEC		-2
ExpertAI_277_1:
	IF_HP_OVER	CHECK_ATTACK,80,ExpertAI_277_end

	INCDEC		-2
ExpertAI_277_end:
	AIEND

ExpertAI_278:			// 	ワイドガード
	AIEND

ExpertAI_279:			// 	ガードシェア
	CHECK_NEKODAMASI	CHECK_ATTACK
	IFN_EQUAL	0,ExpertAI_279_dec	//でてきて１ターン目でないなら終了

	IF_PARA_OVER	CHECK_ATTACK,PARA_DEF,7,ExpertAI_279_dec      // 自分のパラメータが上がってたら出さない
	IF_PARA_OVER	CHECK_ATTACK,PARA_SPEDEF,7,ExpertAI_279_dec   // 自分のパラメータが上がってたら出さない

	IF_PARA_UNDER	CHECK_DEFENCE,PARA_DEF,8,ExpertAI_279_2       // 相手のパラメータが2段階ＵＰ未満
	IF_PARA_UNDER	CHECK_DEFENCE,PARA_SPEDEF,8,ExpertAI_279_2    // 相手のパラメータが2段階ＵＰ未満

	IF_RND_UNDER	50,ExpertAI_279_1
  INCDEC    1

ExpertAI_279_1:
	IF_PARA_UNDER	CHECK_DEFENCE,PARA_DEF,10,ExpertAI_279_2      // 相手のパラメータが4段階ＵＰ未満
	IF_PARA_UNDER	CHECK_DEFENCE,PARA_SPEDEF,10,ExpertAI_279_2   // 相手のパラメータが4段階ＵＰ未満

	IF_RND_UNDER	50,ExpertAI_279_2
  INCDEC    1

ExpertAI_279_2:
  CHECK_MONSNO	CHECK_ATTACK
	IF_TABLE_JUMP	ExpertAI_279_bougyo_table,ExpertAI_279_bougyo
	IF_TABLE_JUMP	ExpertAI_279_tokubou_table,ExpertAI_279_bougyo

  CHECK_MONSNO	CHECK_DEFENCE
	IF_TABLE_JUMP	ExpertAI_279_bougyo_table,ExpertAI_279_inc    // 防御の高いポケモンなら出す
	IF_TABLE_JUMP	ExpertAI_279_tokubou_table,ExpertAI_279_inc   // 特防の高いポケモンなら出す

	JUMP		ExpertAI_279_end

ExpertAI_279_bougyo:                    //　防御が高いポケモンは基本的にシングル戦では出さない
	CHECK_BTL_RULE
	IF_EQUAL		BTL_RULE_SINGLE, AI_DEC10
	JUMP		ExpertAI_279_end

ExpertAI_279_inc:
	IF_RND_UNDER	50,ExpertAI_279_end
  INCDEC    1
	JUMP		ExpertAI_279_end

ExpertAI_279_dec:
  INCDEC    -1
ExpertAI_279_end:
  AIEND

ExpertAI_279_bougyo_table:         // 防御の固いポケモン群
	.long	MONSNO_TUBOTUBO      //  230
	.long	MONSNO_REZIROKKU     //  200
  .long	MONSNO_HAGANEERU     //  200
	.long	MONSNO_BOSUGODORA    //  180
	.long	MONSNO_PARUSHEN      //  180
	.long	MONSNO_TORIDEPUSU    //  168
	.long	MONSNO_IWAAKU        //  160
	.long	MONSNO_REZISUTIRU    //  150
	.long	MONSNO_DAINOOZU	     //  145
	.long	MONSNO_GURAADON      //  140●
	.long	MONSNO_KOOTASU       //  140
	.long	MONSNO_KODORA        //  140
	.long	MONSNO_EAAMUDO       //  140
	.long	MONSNO_FORETOSU      //  140
	.long	MONSNO_YONOWAARU     //  135
	.long	MONSNO_NOZUPASU      //  135
	.long	MONSNO_YUKUSII       //  130
	.long	MONSNO_RIIFIA        //  130
	.long	MONSNO_DOSAIDON      //  130
	.long	MONSNO_METAGUROSU    //  130
	.long	MONSNO_ZIIRANSU      //  130
	.long	MONSNO_SAMAYOORU     //  130
	.long	MONSNO_RUGIA         //  130●
	.long	MONSNO_GOROONYA      //  130

	.long	MONSNO_510 // デスカーン 145
	.long	MONSNO_623 // アバゴウラ 133
	.long	MONSNO_618 // ゴウキーン 131
	.long	MONSNO_602 // ガンドロス 130
	.long	MONSNO_645 // アトス     129

	.long	0xffffffff

ExpertAI_279_tokubou_table:         // 特防の固いポケモン群
	.long	MONSNO_TUBOTUBO      //  230
	.long	MONSNO_REZIAISU      //  200
	.long	MONSNO_HOUOU         //  154●
	.long	MONSNO_RUGIA         //  154●
	.long	MONSNO_DAINOOZU      //  150
	.long	MONSNO_REZISUTIRU    //  150
	.long	MONSNO_KAIOOGA       //  140●
	.long	MONSNO_MANTAIN       //  140
	.long	MONSNO_TORIDEPUSU    //  138
	.long	MONSNO_YONOWAARU     //  135
	.long	MONSNO_HAPINASU      //  135
	.long	MONSNO_KURESERIA     //  130
	.long	MONSNO_YUKUSII       //  130
	.long	MONSNO_RATHIASU      //  130
	.long	MONSNO_SAMAYOORU     //  130
	.long	MONSNO_BURAKKII      //  130
	.long	MONSNO_MIROKAROSU    //  130
	.long	MONSNO_HURIIZAA      //  130

	.long	MONSNO_621 //　アイスゴン135
	.long	MONSNO_647 //　アラミス  129

	.long	0xffffffff

ExpertAI_280:			// 	パワーシェア
	CHECK_NEKODAMASI	CHECK_ATTACK
	IFN_EQUAL	0,ExpertAI_280_dec	//でてきて１ターン目でないなら終了

	IF_PARA_OVER	CHECK_ATTACK,PARA_POW,7,ExpertAI_280_dec      // 自分のパラメータが上がってたら出さない
	IF_PARA_OVER	CHECK_ATTACK,PARA_SPEPOW,7,ExpertAI_280_dec   // 自分のパラメータが上がってたら出さない

	IF_PARA_UNDER	CHECK_DEFENCE,PARA_POW,8,ExpertAI_280_2       // 相手のパラメータが2段階ＵＰ未満
	IF_PARA_UNDER	CHECK_DEFENCE,PARA_SPEPOW,8,ExpertAI_280_2    // 相手のパラメータが2段階ＵＰ未満

	IF_RND_UNDER	50,ExpertAI_280_1
  INCDEC    1

ExpertAI_280_1:
	IF_PARA_UNDER	CHECK_DEFENCE,PARA_POW,10,ExpertAI_280_2      // 相手のパラメータが4段階ＵＰ未満
	IF_PARA_UNDER	CHECK_DEFENCE,PARA_SPEPOW,10,ExpertAI_280_2   // 相手のパラメータが4段階ＵＰ未満

	IF_RND_UNDER	50,ExpertAI_280_2
  INCDEC    1

ExpertAI_280_2:
  CHECK_MONSNO	CHECK_ATTACK
	IF_TABLE_JUMP	ExpertAI_280_kougeki_table,ExpertAI_280_kougeki 
	IF_TABLE_JUMP	ExpertAI_280_tokukou_table,ExpertAI_280_kougeki

  CHECK_MONSNO	CHECK_DEFENCE
	IF_TABLE_JUMP	ExpertAI_280_kougeki_table,ExpertAI_280_inc    // 攻撃の高いポケモンなら出す
	IF_TABLE_JUMP	ExpertAI_280_tokukou_table,ExpertAI_280_inc    // 特攻の高いポケモンなら出す

	JUMP		ExpertAI_280_end

ExpertAI_280_kougeki:                    //　攻撃が高いポケモンは基本的にシングル戦では使わない
	CHECK_BTL_RULE
	IF_EQUAL		BTL_RULE_SINGLE, AI_DEC10
	JUMP		ExpertAI_280_end

ExpertAI_280_inc:
	IF_RND_UNDER	50,ExpertAI_280_end
  INCDEC    1
	JUMP		ExpertAI_280_end

ExpertAI_280_dec:
  INCDEC    -1
ExpertAI_280_end:
  AIEND

ExpertAI_280_kougeki_table:         // 攻撃の高いポケモン群
	.long	MONSNO_RAMUPARUDO     //  165
	.long	MONSNO_REZIGIGASU     //  160
	.long	MONSNO_KEKKINGU       //  160
	.long	MONSNO_DEOKISISU      //  150★    
	.long	MONSNO_REKKUUZA       //  150●
	.long	MONSNO_GURAADON       //  150●
	.long	MONSNO_DOSAIDON       //  140
	.long	MONSNO_METAGUROSU     //  135
	.long	MONSNO_BOOMANDA       //  135
	.long	MONSNO_BANGIRASU      //  134
	.long	MONSNO_KAIRYUU        //  134
	.long	MONSNO_MANMUU         //  130
	.long	MONSNO_GABURIASU      //  130
	.long	MONSNO_ABUSORU        //  130
	.long	MONSNO_KINOGASSA      //  130
	.long	MONSNO_HOUOU          //  130●
	.long	MONSNO_RINGUMA        //  130
	.long	MONSNO_HASSAMU        //  130
	.long	MONSNO_BUUSUTAA       //  130
	.long	MONSNO_SAIDON	        //  130
	.long	MONSNO_KINGURAA       //  130
	.long	MONSNO_KAIRIKII       //  130

	.long	MONSNO_650 //　ム         150●
	.long	MONSNO_547 //　オノノクス 147
	.long	MONSNO_628 //　シハンドン 140
	.long	MONSNO_503 //　ヒヒダルマ 140
	.long	MONSNO_625 //　アーケオス 140
	.long	MONSNO_543 //　ドリュウズ 135
	.long	MONSNO_572 //　カブリオン 135
	.long	MONSNO_653 //　ライ       135●
	.long	MONSNO_602 //　ガンドロス 135
	.long	MONSNO_646 //　ポルトス   129

	.long	0xffffffff

ExpertAI_280_tokukou_table:         // 特攻の高いポケモン群
	.long	MONSNO_MYUUTUU        //  130●
	.long	MONSNO_PARUKIA        //  150●
	.long	MONSNO_DHIARUGA       //  150●
	.long	MONSNO_DEOKISISU      //  150★
	.long	MONSNO_REKKUUZA       //  150●
	.long	MONSNO_KAIOOGA        //  150●
	.long	MONSNO_DAAKURAI       //  135★
	.long	MONSNO_PORIGONz       //  135
	.long	MONSNO_HUUDHIN        //  135
	.long	MONSNO_HIIDORAN       //  130
	.long	MONSNO_GUREISIA       //  130
	.long	MONSNO_ZIBAKOIRU      //  130
	.long	MONSNO_RATHIOSU       //  130
	.long	MONSNO_EEFI           //  130
	.long	MONSNO_GENGAA         //  130

	.long	MONSNO_650 //　シン       150● 
	.long	MONSNO_565 //　シャンデラ 145 
	.long	MONSNO_644 //　ドラー     135 
	.long	MONSNO_653 //　ライ       135●
	.long	MONSNO_654 //　ダルタニス 129★

	.long	0xffffffff

ExpertAI_281:			// 	ワンダールーム
  AIEND

ExpertAI_282:			// 	サイコショック
  AIEND

ExpertAI_283:			// 	ベノムショック
  AIEND

ExpertAI_285:			// 	テレキネシス
	IF_PARA_UNDER	CHECK_ATTACK,PARA_HIT,9,ExpertAI_285_1
	IF_RND_UNDER	50,ExpertAI_285_1
	INCDEC		-2
ExpertAI_285_1:
	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_285_2

	IF_RND_UNDER	50,ExpertAI_285_2
	INCDEC		-2
ExpertAI_285_2:
	IF_PARA_UNDER	CHECK_DEFENCE,PARA_AVOID,9,ExpertAI_285_end

	IF_RND_UNDER	50,ExpertAI_285_end
	INCDEC		1
ExpertAI_285_end:
	AIEND

ExpertAI_286:			// 	マジックルーム
	IF_HAVE_ITEM	CHECK_ATTACK,0,ExpertAI_286_ok  //　自分は何も持ってない
	IF_HAVE_ITEM	CHECK_DEFENCE,ITEM_TABENOKOSI,ExpertAI_286_ok  //　たべのこし
	IF_HAVE_ITEM	CHECK_DEFENCE,ITEM_KODAWARISUKAAHU,ExpertAI_286_ok  //　こだわりスカーフ

	CHECK_BTL_RULE
	IF_EQUAL		BTL_RULE_SINGLE,ExpertAI_286_end

	IF_HAVE_ITEM	CHECK_DEFENCE,ITEM_HUUSEN,ExpertAI_286_ok  //　ふうせん
	JUMP	ExpertAI_286_end

ExpertAI_286_ok:	
	IF_RND_UNDER	128,ExpertAI_286_end
	INCDEC		1

ExpertAI_286_end:
	AIEND

ExpertAI_287:			// 	うちおとす
	CHECK_TOKUSEI	CHECK_DEFENCE
	IF_EQUAL	TOKUSYU_HUYUU,ExpertAI_287_1

	IF_WAZASICK	CHECK_DEFENCE,WAZASICK_FLYING,ExpertAI_287_1

	CHECK_TYPE	CHECK_DEFENCE_TYPE1
	IF_EQUAL	POKETYPE_HIKOU,ExpertAI_287_1	// ひこうタイプ
	CHECK_TYPE	CHECK_DEFENCE_TYPE2
	IF_EQUAL	POKETYPE_HIKOU,ExpertAI_287_1	// ひこうタイプ

	JUMP	ExpertAI_287_end	
	
ExpertAI_287_1:	
	IF_RND_UNDER	64,ExpertAI_287_end
	INCDEC	+1

ExpertAI_287_end:	
	AIEND

ExpertAI_288:			// 	やまあらし
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_288_end
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_288_end
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_288_end

	IF_WAZANO	WAZANO_YAMAARASI,ExpertAI_288_buturi	// 物理

	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEDEF,8,ExpertAI_288_end
  JUMP  ExpertAI_288_ok	

ExpertAI_288_buturi:	
	IF_PARA_UNDER	CHECK_ATTACK,PARA_DEF,8,ExpertAI_288_end

ExpertAI_288_ok:	
	IF_RND_UNDER	64,ExpertAI_288_end
	INCDEC	+1

ExpertAI_288_end:	
	AIEND

ExpertAI_289:			// 	はじけるほのお
	AIEND

ExpertAI_290:			// 	ちょうのまい
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_290_ok

	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_290_end	

	IF_RND_UNDER	50,ExpertAI_290_end
	INCDEC		-1
	JUMP		ExpertAI_290_end
	
ExpertAI_290_ok:
	IF_RND_UNDER	50,ExpertAI_290_end

	INCDEC		1
ExpertAI_290_end:
	AIEND

ExpertAI_291:			// 	ヘビーボンバー
                                        //　体重が調べたい
	AIEND

ExpertAI_292:			// 	シンクロノイズ
                                        //　２〜３匹のタイプが調べたい
	AIEND

ExpertAI_293:			// 	エレクトボール
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_293_end
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_293_end
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_293_end

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_293_ng
	IF_PARA_UNDER	CHECK_ATTACK,PARA_AGI,8,ExpertAI_293_end

	IF_RND_UNDER	70,ExpertAI_293_end
	INCDEC		1
	JUMP		ExpertAI_290_end

ExpertAI_293_ng:
	INCDEC		-1
ExpertAI_293_end:
	AIEND

ExpertAI_294:			// 	みずびたし
	AIEND

ExpertAI_295:			// 	ニトロチャージ
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_295_end
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_295_end
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_295_end

	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_295_ok

	IF_RND_UNDER	50,ExpertAI_295_end
	INCDEC		-1
	JUMP		ExpertAI_295_end
	
ExpertAI_295_ok:
	IF_RND_UNDER	50,ExpertAI_295_end

	INCDEC		1
ExpertAI_295_end:
	AIEND

ExpertAI_296:			// 	アシッドボム
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEDEF,5,ExpertAI_296_end
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEDEF,3,ExpertAI_296_ng2

ExpertAI_296_ng:
	INCDEC		-1
ExpertAI_296_ng2:
	IF_RND_UNDER	128,ExpertAI_296_end
	INCDEC		-1
ExpertAI_296_end:
	AIEND

ExpertAI_297:			// 	イカサマ
	AIEND

ExpertAI_299:			// 	なかまづくり
	IF_RND_UNDER	128,ExpertAI_299_end
	INCDEC		1
ExpertAI_299_end:
	AIEND

ExpertAI_300:			// 	おさきにどうぞ
	AIEND

ExpertAI_301:			// 	りんしょう
	AIEND

ExpertAI_302:			// 	エコーボイス
	AIEND

ExpertAI_303:			// 	なしくずし
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_303_end
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_303_end
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_303_end

	IF_PARA_UNDER	CHECK_ATTACK,PARA_DEF,9,ExpertAI_303_end
	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEDEF,9,ExpertAI_303_end

	IF_RND_UNDER	128,ExpertAI_303_end
  INCDEC 1
ExpertAI_303_end:
	AIEND

ExpertAI_304:			// 	クリアスモッグ
	IF_PARA_OVER	CHECK_DEFENCE,PARA_POW,8,ExpertAI_304_ok
	IF_PARA_OVER	CHECK_DEFENCE,PARA_DEF,8,ExpertAI_304_ok
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEPOW,8,ExpertAI_304_ok
	IF_PARA_OVER	CHECK_DEFENCE,PARA_SPEDEF,8,ExpertAI_304_ok
	IF_PARA_OVER	CHECK_DEFENCE,PARA_AVOID,8,ExpertAI_304_ok

	IF_RND_UNDER	50,ExpertAI_304_end
	INCDEC		-1
	JUMP		ExpertAI_304_end	

ExpertAI_304_ok:	
	IF_RND_UNDER	50,ExpertAI_304_end

	INCDEC		1
ExpertAI_304_end:	

	AIEND

ExpertAI_305:			// 	アシストパワー
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_303_end
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_303_end
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_303_end

	IF_PARA_OVER	CHECK_ATTACK,PARA_POW,8,ExpertAI_305_next
	IF_PARA_OVER	CHECK_ATTACK,PARA_DEF,8,ExpertAI_305_next
	IF_PARA_OVER	CHECK_ATTACK,PARA_SPEPOW,8,ExpertAI_305_next
	IF_PARA_OVER	CHECK_ATTACK,PARA_SPEDEF,8,ExpertAI_305_next
	IF_PARA_OVER	CHECK_ATTACK,PARA_AVOID,8,ExpertAI_305_next
	JUMP		ExpertAI_305_end	

ExpertAI_305_next:			
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,109,ExpertAI_305_ok		//  のろい 
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,206,ExpertAI_305_ok		//  コスモパワー 
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,208,ExpertAI_305_ok		//  ビルドアップ 
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,211,ExpertAI_305_ok		//  めいそう 
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,212,ExpertAI_305_ok		//  りゅうのまい 
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,226,ExpertAI_305_ok		//  つぼをつく 
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,277,ExpertAI_305_ok		//  つめとぎ 
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,290,ExpertAI_305_ok		//  ちょうのまい
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,308,ExpertAI_305_ok		//  からをやぶる
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,316,ExpertAI_305_ok		//  せいちょう
	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,322,ExpertAI_305_ok		//  とぐろをまく

	IF_PARA_OVER	CHECK_ATTACK,PARA_POW,10,ExpertAI_305_ok
	IF_PARA_OVER	CHECK_ATTACK,PARA_DEF,10,ExpertAI_305_ok
	IF_PARA_OVER	CHECK_ATTACK,PARA_SPEPOW,10,ExpertAI_305_ok
	IF_PARA_OVER	CHECK_ATTACK,PARA_SPEDEF,10,ExpertAI_305_ok
	IF_PARA_OVER	CHECK_ATTACK,PARA_AVOID,10,ExpertAI_305_ok

	JUMP		ExpertAI_305_end	

ExpertAI_305_ok:
	IF_RND_UNDER	50,ExpertAI_305_end
	INCDEC		1
ExpertAI_305_end:
	AIEND

ExpertAI_306:			// 	ファストガード
	CHECK_NEKODAMASI	CHECK_DEFENCE
	IF_EQUAL	0,ExpertAI_306_ok

	INCDEC		-1

ExpertAI_306_ok:
	IF_RND_UNDER	128,ExpertAI_306_end
	INCDEC		1
ExpertAI_306_end:	
	AIEND

ExpertAI_307:			// 	サイドチェンジ
	AIEND

ExpertAI_308:			// 	からをやぶる
	IF_PARA_UNDER	CHECK_ATTACK,PARA_DEF,6,ExpertAI_308_ng
	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEDEF,6,ExpertAI_308_ng

	JUMP	ExpertAI_308_end

ExpertAI_308_ng:
	INCDEC		-1
ExpertAI_308_end:
	AIEND

ExpertAI_309:			// 	いやしのはどう
	AIEND

ExpertAI_310:			// 	たたりめ
	AIEND

ExpertAI_312:			// 	ギアチェンジ
	IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_312_ok

	IF_HP_OVER	CHECK_ATTACK,50,ExpertAI_312_end	

	IF_RND_UNDER	50,ExpertAI_312_end
	INCDEC		-1
	JUMP		ExpertAI_312_end
	
ExpertAI_312_ok:
	IF_RND_UNDER	50,ExpertAI_312_end

	INCDEC		1
ExpertAI_312_end:
	AIEND

ExpertAI_314:			// 	やきつくす
	CHECK_TURN
	IFN_EQUAL	0,ExpertAI_314_end

	IF_RND_UNDER	50,ExpertAI_314_end
	INCDEC		1
ExpertAI_314_end:
	AIEND

ExpertAI_315:			// 	さきおくり
	AIEND

ExpertAI_316:			// 	せいちょう
	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_316_end	

	IF_RND_UNDER	50,ExpertAI_316_end
	INCDEC		-1
ExpertAI_316_end:
	AIEND


ExpertAI_317:			// 	アクロバット
	CHECK_WAZA_AISYOU	AISYOU_0BAI,ExpertAI_317_ng
	CHECK_WAZA_AISYOU	AISYOU_1_4BAI,ExpertAI_317_ng
	CHECK_WAZA_AISYOU	AISYOU_1_2BAI,ExpertAI_317_ng

	IF_HAVE_ITEM	CHECK_ATTACK,0,ExpertAI_317_ok  //　なにも持ってない

ExpertAI_317_ng:
	IF_RND_UNDER	50,ExpertAI_317_end
	INCDEC		-1
	JUMP		ExpertAI_317_end

ExpertAI_317_ok:
	IF_RND_UNDER	50,ExpertAI_317_end
	INCDEC		1
ExpertAI_317_end:
	AIEND


ExpertAI_318:			// 	ミラータイプ
	CHECK_TYPE	CHECK_ATTACK_TYPE1
	IF_TABLE_JUMP	ExpertAI_318_ok_Table,ExpertAI_318_ok
	IF_TABLE_JUMP	ExpertAI_318_ng_Table,ExpertAI_318_ng
	CHECK_TYPE	CHECK_ATTACK_TYPE2
	IF_TABLE_JUMP	ExpertAI_318_ok_Table,ExpertAI_318_ok
	IF_TABLE_JUMP	ExpertAI_318_ng_Table,ExpertAI_318_ng

	JUMP	ExpertAI_318_end

ExpertAI_318_ng:
	IF_RND_UNDER	50,ExpertAI_318_end
	INCDEC		-1
	JUMP	ExpertAI_318_end

ExpertAI_318_ok:
	IF_RND_UNDER	50,ExpertAI_318_end
	INCDEC		1
ExpertAI_318_end:
	AIEND


ExpertAI_318_ok_Table:
	.long	POKETYPE_HONOO
	.long	POKETYPE_MIZU
	.long	POKETYPE_KUSA
	.long	POKETYPE_DENKI
	.long	POKETYPE_KOORI
	.long	POKETYPE_DOKU
	.long	POKETYPE_SP
	.long	POKETYPE_AKU
	.long	POKETYPE_HAGANE
	.long	0xffffffff

ExpertAI_318_ng_Table:
	.long	POKETYPE_GHOST
	.long	POKETYPE_DRAGON
	.long	0xffffffff

ExpertAI_319:			// 	かたきうち
	AIEND

ExpertAI_320:			// 	いのちがけ
	IF_MIGAWARI	  CHECK_DEFENCE,ExpertAI_320_ng		// みがわり中
  IF_HAVE_BATSUGUN ExpertAI_320_ng

	IF_HP_UNDER	CHECK_DEFENCE,40,ExpertAI_320_ng
	IF_HP_OVER	CHECK_ATTACK,40,ExpertAI_320_end

	IF_RND_UNDER	50,ExpertAI_320_end

ExpertAI_320_ng:
	INCDEC		-2
ExpertAI_320_end:
	AIEND

ExpertAI_323:			// 	ギフトパス
	AIEND

ExpertAI_324:			// 	みずのちかい
	AIEND
ExpertAI_325:			// 	ほのおのちかい
	AIEND
ExpertAI_326:			// 	くさのちかい
	AIEND

ExpertAI_327:			// 	ふるいたてる
	IF_PARA_UNDER	CHECK_ATTACK,PARA_POW,9,ExpertAI_327_1
	IF_RND_UNDER	100,ExpertAI_327_3
	INCDEC		-1
	JUMP		ExpertAI_327_3

ExpertAI_327_1:
	IF_PARA_UNDER	CHECK_ATTACK,PARA_SPEPOW,9,ExpertAI_327_2
	IF_RND_UNDER	100,ExpertAI_327_3
	INCDEC		-1
	JUMP		ExpertAI_327_3

ExpertAI_327_2:
	IFN_HP_EQUAL	CHECK_ATTACK,100,ExpertAI_327_4

	IF_HAVE_WAZA_SEQNO	CHECK_ATTACK,127,ExpertAI_327_3		// バトンタッチ系

	IF_RND_UNDER	128,ExpertAI_327_4

ExpertAI_327_3:
	INCDEC		2
ExpertAI_327_4:
	IF_HP_OVER	CHECK_ATTACK,70,ExpertAI_327_end
	IF_HP_UNDER	CHECK_ATTACK,40,ExpertAI_327_5

	IF_RND_UNDER	40,ExpertAI_327_end
ExpertAI_327_5:
	INCDEC		-2
ExpertAI_327_end:
	AIEND

ExpertAI_329:	// 	いにしえのうた
	AIEND

ExpertAI_330:	// 	こごえるせかい
  IF_FIRST	IF_FIRST_DEFENCE,ExpertAI_330_1
	INCDEC		-1
	JUMP		ExpertAI_330_end

ExpertAI_330_1:
	IF_RND_UNDER	70,ExpertAI_330_end

	INCDEC		2
ExpertAI_330_end:
	AIEND

ExpertAI_333:	// 	どなりつける
	AIEND


AI_DEC1:
	INCDEC		-1	
	AIEND
AI_DEC2:
	INCDEC		-2	
	AIEND
AI_DEC3:
	INCDEC		-3	
	AIEND
AI_DEC5:
	INCDEC		-5	
	AIEND
AI_DEC6:
	INCDEC		-6	
	AIEND
AI_DEC8:
	INCDEC		-8	
	AIEND
AI_DEC10:
	INCDEC		-10
	AIEND
AI_DEC12:
	INCDEC		-12
	AIEND
AI_DEC30:
	INCDEC		-30
	AIEND
AI_INC1:
	INCDEC		1	
	AIEND
AI_INC2:
	INCDEC		2	
	AIEND
AI_INC3:
	INCDEC		3	
	AIEND
AI_INC5:
	INCDEC		5	
	AIEND
AI_INC10:
	INCDEC		10	
	AIEND

