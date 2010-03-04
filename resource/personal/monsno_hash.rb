#! ruby -Ks

	$monsno_hash = {
		"フシギダネ"=>1,
		"フシギソウ"=>2,
		"フシギバナ"=>3,
		"ヒトカゲ"=>4,
		"リザード"=>5,
		"リザードン"=>6,
		"ゼニガメ"=>7,
		"カメール"=>8,
		"カメックス"=>9,
		"キャタピー"=>10,
		"トランセル"=>11,
		"バタフリー"=>12,
		"ビードル"=>13,
		"コクーン"=>14,
		"スピアー"=>15,
		"ポッポ"=>16,
		"ピジョン"=>17,
		"ピジョット"=>18,
		"コラッタ"=>19,
		"ラッタ"=>20,
		"オニスズメ"=>21,
		"オニドリル"=>22,
		"アーボ"=>23,
		"アーボック"=>24,
		"ピカチュウ"=>25,
		"ライチュウ"=>26,
		"サンド"=>27,
		"サンドパン"=>28,
		"ニドラン♀"=>29,
		"ニドリーナ"=>30,
		"ニドクイン"=>31,
		"ニドラン♂"=>32,
		"ニドリーノ"=>33,
		"ニドキング"=>34,
		"ピッピ"=>35,
		"ピクシー"=>36,
		"ロコン"=>37,
		"キュウコン"=>38,
		"プリン"=>39,
		"プクリン"=>40,
		"ズバット"=>41,
		"ゴルバット"=>42,
		"ナゾノクサ"=>43,
		"クサイハナ"=>44,
		"ラフレシア"=>45,
		"パラス"=>46,
		"パラセクト"=>47,
		"コンパン"=>48,
		"モルフォン"=>49,
		"ディグダ"=>50,
		"ダグトリオ"=>51,
		"ニャース"=>52,
		"ペルシアン"=>53,
		"コダック"=>54,
		"ゴルダック"=>55,
		"マンキー"=>56,
		"オコリザル"=>57,
		"ガーディ"=>58,
		"ウインディ"=>59,
		"ニョロモ"=>60,
		"ニョロゾ"=>61,
		"ニョロボン"=>62,
		"ケーシィ"=>63,
		"ユンゲラー"=>64,
		"フーディン"=>65,
		"ワンリキー"=>66,
		"ゴーリキー"=>67,
		"カイリキー"=>68,
		"マダツボミ"=>69,
		"ウツドン"=>70,
		"ウツボット"=>71,
		"メノクラゲ"=>72,
		"ドククラゲ"=>73,
		"イシツブテ"=>74,
		"ゴローン"=>75,
		"ゴローニャ"=>76,
		"ポニータ"=>77,
		"ギャロップ"=>78,
		"ヤドン"=>79,
		"ヤドラン"=>80,
		"コイル"=>81,
		"レアコイル"=>82,
		"カモネギ"=>83,
		"ドードー"=>84,
		"ドードリオ"=>85,
		"パウワウ"=>86,
		"ジュゴン"=>87,
		"ベトベター"=>88,
		"ベトベトン"=>89,
		"シェルダー"=>90,
		"パルシェン"=>91,
		"ゴース"=>92,
		"ゴースト"=>93,
		"ゲンガー"=>94,
		"イワーク"=>95,
		"スリープ"=>96,
		"スリーパー"=>97,
		"クラブ"=>98,
		"キングラー"=>99,
		"ビリリダマ"=>100,
		"マルマイン"=>101,
		"タマタマ"=>102,
		"ナッシー"=>103,
		"カラカラ"=>104,
		"ガラガラ"=>105,
		"サワムラー"=>106,
		"エビワラー"=>107,
		"ベロリンガ"=>108,
		"ドガース"=>109,
		"マタドガス"=>110,
		"サイホーン"=>111,
		"サイドン"=>112,
		"ラッキー"=>113,
		"モンジャラ"=>114,
		"ガルーラ"=>115,
		"タッツー"=>116,
		"シードラ"=>117,
		"トサキント"=>118,
		"アズマオウ"=>119,
		"ヒトデマン"=>120,
		"スターミー"=>121,
		"バリヤード"=>122,
		"ストライク"=>123,
		"ルージュラ"=>124,
		"エレブー"=>125,
		"ブーバー"=>126,
		"カイロス"=>127,
		"ケンタロス"=>128,
		"コイキング"=>129,
		"ギャラドス"=>130,
		"ラプラス"=>131,
		"メタモン"=>132,
		"イーブイ"=>133,
		"シャワーズ"=>134,
		"サンダース"=>135,
		"ブースター"=>136,
		"ポリゴン"=>137,
		"オムナイト"=>138,
		"オムスター"=>139,
		"カブト"=>140,
		"カブトプス"=>141,
		"プテラ"=>142,
		"カビゴン"=>143,
		"フリーザー"=>144,
		"サンダー"=>145,
		"ファイヤー"=>146,
		"ミニリュウ"=>147,
		"ハクリュー"=>148,
		"カイリュー"=>149,
		"ミュウツー"=>150,
		"ミュウ"=>151,
		"チコリータ"=>152,
		"ベイリーフ"=>153,
		"メガニウム"=>154,
		"ヒノアラシ"=>155,
		"マグマラシ"=>156,
		"バクフーン"=>157,
		"ワニノコ"=>158,
		"アリゲイツ"=>159,
		"オーダイル"=>160,
		"オタチ"=>161,
		"オオタチ"=>162,
		"ホーホー"=>163,
		"ヨルノズク"=>164,
		"レディバ"=>165,
		"レディアン"=>166,
		"イトマル"=>167,
		"アリアドス"=>168,
		"クロバット"=>169,
		"チョンチー"=>170,
		"ランターン"=>171,
		"ピチュー"=>172,
		"ピィ"=>173,
		"ププリン"=>174,
		"トゲピー"=>175,
		"トゲチック"=>176,
		"ネイティ"=>177,
		"ネイティオ"=>178,
		"メリープ"=>179,
		"モココ"=>180,
		"デンリュウ"=>181,
		"キレイハナ"=>182,
		"マリル"=>183,
		"マリルリ"=>184,
		"ウソッキー"=>185,
		"ニョロトノ"=>186,
		"ハネッコ"=>187,
		"ポポッコ"=>188,
		"ワタッコ"=>189,
		"エイパム"=>190,
		"ヒマナッツ"=>191,
		"キマワリ"=>192,
		"ヤンヤンマ"=>193,
		"ウパー"=>194,
		"ヌオー"=>195,
		"エーフィ"=>196,
		"ブラッキー"=>197,
		"ヤミカラス"=>198,
		"ヤドキング"=>199,
		"ムウマ"=>200,
		"アンノーン"=>201,
		"ソーナンス"=>202,
		"キリンリキ"=>203,
		"クヌギダマ"=>204,
		"フォレトス"=>205,
		"ノコッチ"=>206,
		"グライガー"=>207,
		"ハガネール"=>208,
		"ブルー"=>209,
		"グランブル"=>210,
		"ハリーセン"=>211,
		"ハッサム"=>212,
		"ツボツボ"=>213,
		"ヘラクロス"=>214,
		"ニューラ"=>215,
		"ヒメグマ"=>216,
		"リングマ"=>217,
		"マグマッグ"=>218,
		"マグカルゴ"=>219,
		"ウリムー"=>220,
		"イノムー"=>221,
		"サニーゴ"=>222,
		"テッポウオ"=>223,
		"オクタン"=>224,
		"デリバード"=>225,
		"マンタイン"=>226,
		"エアームド"=>227,
		"デルビル"=>228,
		"ヘルガー"=>229,
		"キングドラ"=>230,
		"ゴマゾウ"=>231,
		"ドンファン"=>232,
		"ポリゴン２"=>233,
		"オドシシ"=>234,
		"ドーブル"=>235,
		"バルキー"=>236,
		"カポエラー"=>237,
		"ムチュール"=>238,
		"エレキッド"=>239,
		"ブビィ"=>240,
		"ミルタンク"=>241,
		"ハピナス"=>242,
		"ライコウ"=>243,
		"エンテイ"=>244,
		"スイクン"=>245,
		"ヨーギラス"=>246,
		"サナギラス"=>247,
		"バンギラス"=>248,
		"ルギア"=>249,
		"ホウオウ"=>250,
		"セレビィ"=>251,
		"キモリ"=>252,
		"ジュプトル"=>253,
		"ジュカイン"=>254,
		"アチャモ"=>255,
		"ワカシャモ"=>256,
		"バシャーモ"=>257,
		"ミズゴロウ"=>258,
		"ヌマクロー"=>259,
		"ラグラージ"=>260,
		"ポチエナ"=>261,
		"グラエナ"=>262,
		"ジグザグマ"=>263,
		"マッスグマ"=>264,
		"ケムッソ"=>265,
		"カラサリス"=>266,
		"アゲハント"=>267,
		"マユルド"=>268,
		"ドクケイル"=>269,
		"ハスボー"=>270,
		"ハスブレロ"=>271,
		"ルンパッパ"=>272,
		"タネボー"=>273,
		"コノハナ"=>274,
		"ダーテング"=>275,
		"スバメ"=>276,
		"オオスバメ"=>277,
		"キャモメ"=>278,
		"ペリッパー"=>279,
		"ラルトス"=>280,
		"キルリア"=>281,
		"サーナイト"=>282,
		"アメタマ"=>283,
		"アメモース"=>284,
		"キノココ"=>285,
		"キノガッサ"=>286,
		"ナマケロ"=>287,
		"ヤルキモノ"=>288,
		"ケッキング"=>289,
		"ツチニン"=>290,
		"テッカニン"=>291,
		"ヌケニン"=>292,
		"ゴニョニョ"=>293,
		"ドゴーム"=>294,
		"バクオング"=>295,
		"マクノシタ"=>296,
		"ハリテヤマ"=>297,
		"ルリリ"=>298,
		"ノズパス"=>299,
		"エネコ"=>300,
		"エネコロロ"=>301,
		"ヤミラミ"=>302,
		"クチート"=>303,
		"ココドラ"=>304,
		"コドラ"=>305,
		"ボスゴドラ"=>306,
		"アサナン"=>307,
		"チャーレム"=>308,
		"ラクライ"=>309,
		"ライボルト"=>310,
		"プラスル"=>311,
		"マイナン"=>312,
		"バルビート"=>313,
		"イルミーゼ"=>314,
		"ロゼリア"=>315,
		"ゴクリン"=>316,
		"マルノーム"=>317,
		"キバニア"=>318,
		"サメハダー"=>319,
		"ホエルコ"=>320,
		"ホエルオー"=>321,
		"ドンメル"=>322,
		"バクーダ"=>323,
		"コータス"=>324,
		"バネブー"=>325,
		"ブーピッグ"=>326,
		"パッチール"=>327,
		"ナックラー"=>328,
		"ビブラーバ"=>329,
		"フライゴン"=>330,
		"サボネア"=>331,
		"ノクタス"=>332,
		"チルット"=>333,
		"チルタリス"=>334,
		"ザングース"=>335,
		"ハブネーク"=>336,
		"ルナトーン"=>337,
		"ソルロック"=>338,
		"ドジョッチ"=>339,
		"ナマズン"=>340,
		"ヘイガニ"=>341,
		"シザリガー"=>342,
		"ヤジロン"=>343,
		"ネンドール"=>344,
		"リリーラ"=>345,
		"ユレイドル"=>346,
		"アノプス"=>347,
		"アーマルド"=>348,
		"ヒンバス"=>349,
		"ミロカロス"=>350,
		"ポワルン"=>351,
		"カクレオン"=>352,
		"カゲボウズ"=>353,
		"ジュペッタ"=>354,
		"ヨマワル"=>355,
		"サマヨール"=>356,
		"トロピウス"=>357,
		"チリーン"=>358,
		"アブソル"=>359,
		"ソーナノ"=>360,
		"ユキワラシ"=>361,
		"オニゴーリ"=>362,
		"タマザラシ"=>363,
		"トドグラー"=>364,
		"トドゼルガ"=>365,
		"パールル"=>366,
		"ハンテール"=>367,
		"サクラビス"=>368,
		"ジーランス"=>369,
		"ラブカス"=>370,
		"タツベイ"=>371,
		"コモルー"=>372,
		"ボーマンダ"=>373,
		"ダンバル"=>374,
		"メタング"=>375,
		"メタグロス"=>376,
		"レジロック"=>377,
		"レジアイス"=>378,
		"レジスチル"=>379,
		"ラティアス"=>380,
		"ラティオス"=>381,
		"カイオーガ"=>382,
		"グラードン"=>383,
		"レックウザ"=>384,
		"ジラーチ"=>385,
		"デオキシス"=>386,
		"ナエトル"=>387,
		"ハヤシガメ"=>388,
		"ドダイトス"=>389,
		"ヒコザル"=>390,
		"モウカザル"=>391,
		"ゴウカザル"=>392,
		"ポッチャマ"=>393,
		"ポッタイシ"=>394,
		"エンペルト"=>395,
		"ムックル"=>396,
		"ムクバード"=>397,
		"ムクホーク"=>398,
		"ビッパ"=>399,
		"ビーダル"=>400,
		"コロボーシ"=>401,
		"コロトック"=>402,
		"コリンク"=>403,
		"ルクシオ"=>404,
		"レントラー"=>405,
		"スボミー"=>406,
		"ロズレイド"=>407,
		"ズガイドス"=>408,
		"ラムパルド"=>409,
		"タテトプス"=>410,
		"トリデプス"=>411,
		"ミノムッチ"=>412,
		"ミノマダム"=>413,
		"ガーメイル"=>414,
		"ミツハニー"=>415,
		"ビークイン"=>416,
		"パチリス"=>417,
		"ブイゼル"=>418,
		"フローゼル"=>419,
		"チェリンボ"=>420,
		"チェリム"=>421,
		"カラナクシ"=>422,
		"トリトドン"=>423,
		"エテボース"=>424,
		"フワンテ"=>425,
		"フワライド"=>426,
		"ミミロル"=>427,
		"ミミロップ"=>428,
		"ムウマージ"=>429,
		"ドンカラス"=>430,
		"ニャルマー"=>431,
		"ブニャット"=>432,
		"リーシャン"=>433,
		"スカンプー"=>434,
		"スカタンク"=>435,
		"ドーミラー"=>436,
		"ドータクン"=>437,
		"ウソハチ"=>438,
		"マネネ"=>439,
		"ピンプク"=>440,
		"ペラップ"=>441,
		"ミカルゲ"=>442,
		"フカマル"=>443,
		"ガバイト"=>444,
		"ガブリアス"=>445,
		"ゴンベ"=>446,
		"リオル"=>447,
		"ルカリオ"=>448,
		"ヒポポタス"=>449,
		"カバルドン"=>450,
		"スコルピ"=>451,
		"ドラピオン"=>452,
		"グレッグル"=>453,
		"ドクロッグ"=>454,
		"マスキッパ"=>455,
		"ケイコウオ"=>456,
		"ネオラント"=>457,
		"タマンタ"=>458,
		"ユキカブリ"=>459,
		"ユキノオー"=>460,
		"マニューラ"=>461,
		"ジバコイル"=>462,
		"ベロベルト"=>463,
		"ドサイドン"=>464,
		"モジャンボ"=>465,
		"エレキブル"=>466,
		"ブーバーン"=>467,
		"トゲキッス"=>468,
		"メガヤンマ"=>469,
		"リーフィア"=>470,
		"グレイシア"=>471,
		"グライオン"=>472,
		"マンムー"=>473,
		"ポリゴンＺ"=>474,
		"エルレイド"=>475,
		"ダイノーズ"=>476,
		"ヨノワール"=>477,
		"ユキメノコ"=>478,
		"ロトム"=>479,
		"ユクシー"=>480,
		"エムリット"=>481,
		"アグノム"=>482,
		"ディアルガ"=>483,
		"パルキア"=>484,
		"ヒードラン"=>485,
		"レジギガス"=>486,
		"ギラティナ"=>487,
		"クレセリア"=>488,
		"フィオネ"=>489,
		"マナフィ"=>490,
		"ダークライ"=>491,
		"シェイミ"=>492,
		"アルセウス"=>493,
		"クサヘビ"=>494,
		"クサネーク"=>495,
		"プランブラ"=>496,
		"ポカブ"=>497,
		"チャオブー"=>498,
		"エンブオー"=>499,
		"スイッコ"=>500,
		"ミズッコ"=>501,
		"ラッコール"=>502,
		"ミネズミ"=>503,
		"ミルホッグ"=>504,
		"パピー"=>505,
		"パブロー"=>506,
		"パブドッグ"=>507,
		"チョロネコ"=>508,
		"レパルダス"=>509,
		"ブロッコ"=>510,
		"ブロッコル"=>511,
		"スチック"=>512,
		"スチームル"=>513,
		"ファウン"=>514,
		"ファウント"=>515,
		"ムンナ"=>516,
		"ムシャーナ"=>517,
		"マメパト"=>518,
		"ハトーボー"=>519,
		"ケンホロウ"=>520,
		"ムカッテ"=>521,
		"ムカンデ"=>522,
		"ムカデンテ"=>523,
		"シママ"=>524,
		"ゼブライカ"=>525,
		"ガンセキ"=>526,
		"ガンクツ"=>527,
		"ガンドロス"=>528,
		"コロモリ"=>529,
		"ココロモリ"=>530,
		"ニューモン"=>531,
		"ユーダーン"=>532,
		"シハンドン"=>533,
		"モグリュー"=>534,
		"ドリュウズ"=>535,
		"シンピッピ"=>536,
		"オタマロ"=>537,
		"ガマガル"=>538,
		"ガマゲロゲ"=>539,
		"ダゲキ"=>540,
		"ナゲキ"=>541,
		"クルミル"=>542,
		"クルマユ"=>543,
		"ハハコモリ"=>544,
		"モンメン"=>545,
		"エルフーン"=>546,
		"チュリネ"=>547,
		"ドレディア"=>548,
		"バスラオ"=>549,
		"ダルマッカ"=>550,
		"ヒヒダルマ"=>551,
		"メグロコ"=>552,
		"ワルビル"=>553,
		"ワルビアル"=>554,
		"マラカッチ"=>555,
		"ヤドリ"=>556,
		"ヤドカルゴ"=>557,
		"ナスカドリ"=>558,
		"デスマス"=>559,
		"デスカーン"=>560,
		"ゴビット"=>561,
		"ゴルーグ"=>562,
		"カセッキ"=>563,
		"カセキング"=>564,
		"アーケン"=>565,
		"アーケオス"=>566,
		"ヤブクロン"=>567,
		"ダストダス"=>568,
		"チラーミィ"=>569,
		"チラチーノ"=>570,
		"スンドメー"=>571,
		"フルコング"=>572,
		"ゴスロ"=>573,
		"ゴスロス"=>574,
		"ゴスローゼ"=>575,
		"ユニラン"=>576,
		"ダブラン"=>577,
		"ランクルス"=>578,
		"バチュル"=>579,
		"デンチュラ"=>580,
		"トビッチュ"=>581,
		"コアルヒー"=>582,
		"スワンナ"=>583,
		"ツルリ"=>584,
		"ツルット"=>585,
		"ツルツルダ"=>586,
		"バンビーナ"=>587,
		"シキジカ"=>588,
		"カブリン"=>589,
		"カブリオン"=>590,
		"タマゲタケ"=>591,
		"モロバレル"=>592,
		"ハッチェ"=>593,
		"オノドン"=>594,
		"オノックス"=>595,
		"プルンス"=>596,
		"プルキング"=>597,
		"ママンボウ"=>598,
		"エレス"=>599,
		"エレクス"=>600,
		"エレキテス"=>601,
		"テッパン"=>602,
		"ゴウキーン"=>603,
		"ギアル"=>604,
		"ギギアル"=>605,
		"ギギギアル"=>606,
		"テレス"=>607,
		"テレパス"=>608,
		"ローソクン"=>609,
		"ランプラー"=>610,
		"シャンドル"=>611,
		"ゾロア"=>612,
		"ゾロアーク"=>613,
		"シルクマ"=>614,
		"ツンベアー"=>615,
		"アイスゴン"=>616,
		"カッチュ"=>617,
		"カチュード"=>618,
		"オニソコ"=>619,
		"グラップ"=>620,
		"グラッパー"=>621,
		"オーロッチ"=>622,
		"ズルル"=>623,
		"ズルガシー"=>624,
		"バッフロン"=>625,
		"アウトン"=>626,
		"アウトロン"=>627,
		"イーグラ"=>628,
		"イーグラン"=>629,
		"メラメラス"=>630,
		"スチーラス"=>631,
		"ドラッコ"=>632,
		"ドラーゴ"=>633,
		"ドラゴーン"=>634,
		"ラー"=>635,
		"ドラー"=>636,
		"アトス"=>637,
		"ポルトス"=>638,
		"アラミス"=>639,
		"カザカミ"=>640,
		"ライカミ"=>641,
		"シン"=>642,
		"ム"=>643,
		"ツチノカミ"=>644,
		"ライ"=>645,
		"ダルタニス"=>646,
		"メロディア"=>647,
		"インセクタ"=>648,
		"ビクティ"=>649,
	}
	$formmax_hash = {
		"フシギダネ"=>0,
		"フシギソウ"=>0,
		"フシギバナ"=>0,
		"ヒトカゲ"=>0,
		"リザード"=>0,
		"リザードン"=>0,
		"ゼニガメ"=>0,
		"カメール"=>0,
		"カメックス"=>0,
		"キャタピー"=>0,
		"トランセル"=>0,
		"バタフリー"=>0,
		"ビードル"=>0,
		"コクーン"=>0,
		"スピアー"=>0,
		"ポッポ"=>0,
		"ピジョン"=>0,
		"ピジョット"=>0,
		"コラッタ"=>0,
		"ラッタ"=>0,
		"オニスズメ"=>0,
		"オニドリル"=>0,
		"アーボ"=>0,
		"アーボック"=>0,
		"ピカチュウ"=>0,
		"ライチュウ"=>0,
		"サンド"=>0,
		"サンドパン"=>0,
		"ニドラン♀"=>0,
		"ニドリーナ"=>0,
		"ニドクイン"=>0,
		"ニドラン♂"=>0,
		"ニドリーノ"=>0,
		"ニドキング"=>0,
		"ピッピ"=>0,
		"ピクシー"=>0,
		"ロコン"=>0,
		"キュウコン"=>0,
		"プリン"=>0,
		"プクリン"=>0,
		"ズバット"=>0,
		"ゴルバット"=>0,
		"ナゾノクサ"=>0,
		"クサイハナ"=>0,
		"ラフレシア"=>0,
		"パラス"=>0,
		"パラセクト"=>0,
		"コンパン"=>0,
		"モルフォン"=>0,
		"ディグダ"=>0,
		"ダグトリオ"=>0,
		"ニャース"=>0,
		"ペルシアン"=>0,
		"コダック"=>0,
		"ゴルダック"=>0,
		"マンキー"=>0,
		"オコリザル"=>0,
		"ガーディ"=>0,
		"ウインディ"=>0,
		"ニョロモ"=>0,
		"ニョロゾ"=>0,
		"ニョロボン"=>0,
		"ケーシィ"=>0,
		"ユンゲラー"=>0,
		"フーディン"=>0,
		"ワンリキー"=>0,
		"ゴーリキー"=>0,
		"カイリキー"=>0,
		"マダツボミ"=>0,
		"ウツドン"=>0,
		"ウツボット"=>0,
		"メノクラゲ"=>0,
		"ドククラゲ"=>0,
		"イシツブテ"=>0,
		"ゴローン"=>0,
		"ゴローニャ"=>0,
		"ポニータ"=>0,
		"ギャロップ"=>0,
		"ヤドン"=>0,
		"ヤドラン"=>0,
		"コイル"=>0,
		"レアコイル"=>0,
		"カモネギ"=>0,
		"ドードー"=>0,
		"ドードリオ"=>0,
		"パウワウ"=>0,
		"ジュゴン"=>0,
		"ベトベター"=>0,
		"ベトベトン"=>0,
		"シェルダー"=>0,
		"パルシェン"=>0,
		"ゴース"=>0,
		"ゴースト"=>0,
		"ゲンガー"=>0,
		"イワーク"=>0,
		"スリープ"=>0,
		"スリーパー"=>0,
		"クラブ"=>0,
		"キングラー"=>0,
		"ビリリダマ"=>0,
		"マルマイン"=>0,
		"タマタマ"=>0,
		"ナッシー"=>0,
		"カラカラ"=>0,
		"ガラガラ"=>0,
		"サワムラー"=>0,
		"エビワラー"=>0,
		"ベロリンガ"=>0,
		"ドガース"=>0,
		"マタドガス"=>0,
		"サイホーン"=>0,
		"サイドン"=>0,
		"ラッキー"=>0,
		"モンジャラ"=>0,
		"ガルーラ"=>0,
		"タッツー"=>0,
		"シードラ"=>0,
		"トサキント"=>0,
		"アズマオウ"=>0,
		"ヒトデマン"=>0,
		"スターミー"=>0,
		"バリヤード"=>0,
		"ストライク"=>0,
		"ルージュラ"=>0,
		"エレブー"=>0,
		"ブーバー"=>0,
		"カイロス"=>0,
		"ケンタロス"=>0,
		"コイキング"=>0,
		"ギャラドス"=>0,
		"ラプラス"=>0,
		"メタモン"=>0,
		"イーブイ"=>0,
		"シャワーズ"=>0,
		"サンダース"=>0,
		"ブースター"=>0,
		"ポリゴン"=>0,
		"オムナイト"=>0,
		"オムスター"=>0,
		"カブト"=>0,
		"カブトプス"=>0,
		"プテラ"=>0,
		"カビゴン"=>0,
		"フリーザー"=>0,
		"サンダー"=>0,
		"ファイヤー"=>0,
		"ミニリュウ"=>0,
		"ハクリュー"=>0,
		"カイリュー"=>0,
		"ミュウツー"=>0,
		"ミュウ"=>0,
		"チコリータ"=>0,
		"ベイリーフ"=>0,
		"メガニウム"=>0,
		"ヒノアラシ"=>0,
		"マグマラシ"=>0,
		"バクフーン"=>0,
		"ワニノコ"=>0,
		"アリゲイツ"=>0,
		"オーダイル"=>0,
		"オタチ"=>0,
		"オオタチ"=>0,
		"ホーホー"=>0,
		"ヨルノズク"=>0,
		"レディバ"=>0,
		"レディアン"=>0,
		"イトマル"=>0,
		"アリアドス"=>0,
		"クロバット"=>0,
		"チョンチー"=>0,
		"ランターン"=>0,
		"ピチュー"=>0,
		"ピィ"=>0,
		"ププリン"=>0,
		"トゲピー"=>0,
		"トゲチック"=>0,
		"ネイティ"=>0,
		"ネイティオ"=>0,
		"メリープ"=>0,
		"モココ"=>0,
		"デンリュウ"=>0,
		"キレイハナ"=>0,
		"マリル"=>0,
		"マリルリ"=>0,
		"ウソッキー"=>0,
		"ニョロトノ"=>0,
		"ハネッコ"=>0,
		"ポポッコ"=>0,
		"ワタッコ"=>0,
		"エイパム"=>0,
		"ヒマナッツ"=>0,
		"キマワリ"=>0,
		"ヤンヤンマ"=>0,
		"ウパー"=>0,
		"ヌオー"=>0,
		"エーフィ"=>0,
		"ブラッキー"=>0,
		"ヤミカラス"=>0,
		"ヤドキング"=>0,
		"ムウマ"=>0,
		"アンノーン"=>0,
		"ソーナンス"=>28,
		"キリンリキ"=>0,
		"クヌギダマ"=>0,
		"フォレトス"=>0,
		"ノコッチ"=>0,
		"グライガー"=>0,
		"ハガネール"=>0,
		"ブルー"=>0,
		"グランブル"=>0,
		"ハリーセン"=>0,
		"ハッサム"=>0,
		"ツボツボ"=>0,
		"ヘラクロス"=>0,
		"ニューラ"=>0,
		"ヒメグマ"=>0,
		"リングマ"=>0,
		"マグマッグ"=>0,
		"マグカルゴ"=>0,
		"ウリムー"=>0,
		"イノムー"=>0,
		"サニーゴ"=>0,
		"テッポウオ"=>0,
		"オクタン"=>0,
		"デリバード"=>0,
		"マンタイン"=>0,
		"エアームド"=>0,
		"デルビル"=>0,
		"ヘルガー"=>0,
		"キングドラ"=>0,
		"ゴマゾウ"=>0,
		"ドンファン"=>0,
		"ポリゴン２"=>0,
		"オドシシ"=>0,
		"ドーブル"=>0,
		"バルキー"=>0,
		"カポエラー"=>0,
		"ムチュール"=>0,
		"エレキッド"=>0,
		"ブビィ"=>0,
		"ミルタンク"=>0,
		"ハピナス"=>0,
		"ライコウ"=>0,
		"エンテイ"=>0,
		"スイクン"=>0,
		"ヨーギラス"=>0,
		"サナギラス"=>0,
		"バンギラス"=>0,
		"ルギア"=>0,
		"ホウオウ"=>0,
		"セレビィ"=>0,
		"キモリ"=>0,
		"ジュプトル"=>0,
		"ジュカイン"=>0,
		"アチャモ"=>0,
		"ワカシャモ"=>0,
		"バシャーモ"=>0,
		"ミズゴロウ"=>0,
		"ヌマクロー"=>0,
		"ラグラージ"=>0,
		"ポチエナ"=>0,
		"グラエナ"=>0,
		"ジグザグマ"=>0,
		"マッスグマ"=>0,
		"ケムッソ"=>0,
		"カラサリス"=>0,
		"アゲハント"=>0,
		"マユルド"=>0,
		"ドクケイル"=>0,
		"ハスボー"=>0,
		"ハスブレロ"=>0,
		"ルンパッパ"=>0,
		"タネボー"=>0,
		"コノハナ"=>0,
		"ダーテング"=>0,
		"スバメ"=>0,
		"オオスバメ"=>0,
		"キャモメ"=>0,
		"ペリッパー"=>0,
		"ラルトス"=>0,
		"キルリア"=>0,
		"サーナイト"=>0,
		"アメタマ"=>0,
		"アメモース"=>0,
		"キノココ"=>0,
		"キノガッサ"=>0,
		"ナマケロ"=>0,
		"ヤルキモノ"=>0,
		"ケッキング"=>0,
		"ツチニン"=>0,
		"テッカニン"=>0,
		"ヌケニン"=>0,
		"ゴニョニョ"=>0,
		"ドゴーム"=>0,
		"バクオング"=>0,
		"マクノシタ"=>0,
		"ハリテヤマ"=>0,
		"ルリリ"=>0,
		"ノズパス"=>0,
		"エネコ"=>0,
		"エネコロロ"=>0,
		"ヤミラミ"=>0,
		"クチート"=>0,
		"ココドラ"=>0,
		"コドラ"=>0,
		"ボスゴドラ"=>0,
		"アサナン"=>0,
		"チャーレム"=>0,
		"ラクライ"=>0,
		"ライボルト"=>0,
		"プラスル"=>0,
		"マイナン"=>0,
		"バルビート"=>0,
		"イルミーゼ"=>0,
		"ロゼリア"=>0,
		"ゴクリン"=>0,
		"マルノーム"=>0,
		"キバニア"=>0,
		"サメハダー"=>0,
		"ホエルコ"=>0,
		"ホエルオー"=>0,
		"ドンメル"=>0,
		"バクーダ"=>0,
		"コータス"=>0,
		"バネブー"=>0,
		"ブーピッグ"=>0,
		"パッチール"=>0,
		"ナックラー"=>0,
		"ビブラーバ"=>0,
		"フライゴン"=>0,
		"サボネア"=>0,
		"ノクタス"=>0,
		"チルット"=>0,
		"チルタリス"=>0,
		"ザングース"=>0,
		"ハブネーク"=>0,
		"ルナトーン"=>0,
		"ソルロック"=>0,
		"ドジョッチ"=>0,
		"ナマズン"=>0,
		"ヘイガニ"=>0,
		"シザリガー"=>0,
		"ヤジロン"=>0,
		"ネンドール"=>0,
		"リリーラ"=>0,
		"ユレイドル"=>0,
		"アノプス"=>0,
		"アーマルド"=>0,
		"ヒンバス"=>0,
		"ミロカロス"=>0,
		"ポワルン"=>0,
		"カクレオン"=>4,
		"カゲボウズ"=>0,
		"ジュペッタ"=>0,
		"ヨマワル"=>0,
		"サマヨール"=>0,
		"トロピウス"=>0,
		"チリーン"=>0,
		"アブソル"=>0,
		"ソーナノ"=>0,
		"ユキワラシ"=>0,
		"オニゴーリ"=>0,
		"タマザラシ"=>0,
		"トドグラー"=>0,
		"トドゼルガ"=>0,
		"パールル"=>0,
		"ハンテール"=>0,
		"サクラビス"=>0,
		"ジーランス"=>0,
		"ラブカス"=>0,
		"タツベイ"=>0,
		"コモルー"=>0,
		"ボーマンダ"=>0,
		"ダンバル"=>0,
		"メタング"=>0,
		"メタグロス"=>0,
		"レジロック"=>0,
		"レジアイス"=>0,
		"レジスチル"=>0,
		"ラティアス"=>0,
		"ラティオス"=>0,
		"カイオーガ"=>0,
		"グラードン"=>0,
		"レックウザ"=>0,
		"ジラーチ"=>0,
		"デオキシス"=>0,
		"ナエトル"=>4,
		"ハヤシガメ"=>0,
		"ドダイトス"=>0,
		"ヒコザル"=>0,
		"モウカザル"=>0,
		"ゴウカザル"=>0,
		"ポッチャマ"=>0,
		"ポッタイシ"=>0,
		"エンペルト"=>0,
		"ムックル"=>0,
		"ムクバード"=>0,
		"ムクホーク"=>0,
		"ビッパ"=>0,
		"ビーダル"=>0,
		"コロボーシ"=>0,
		"コロトック"=>0,
		"コリンク"=>0,
		"ルクシオ"=>0,
		"レントラー"=>0,
		"スボミー"=>0,
		"ロズレイド"=>0,
		"ズガイドス"=>0,
		"ラムパルド"=>0,
		"タテトプス"=>0,
		"トリデプス"=>0,
		"ミノムッチ"=>0,
		"ミノマダム"=>3,
		"ガーメイル"=>3,
		"ミツハニー"=>0,
		"ビークイン"=>0,
		"パチリス"=>0,
		"ブイゼル"=>0,
		"フローゼル"=>0,
		"チェリンボ"=>0,
		"チェリム"=>0,
		"カラナクシ"=>2,
		"トリトドン"=>2,
		"エテボース"=>2,
		"フワンテ"=>0,
		"フワライド"=>0,
		"ミミロル"=>0,
		"ミミロップ"=>0,
		"ムウマージ"=>0,
		"ドンカラス"=>0,
		"ニャルマー"=>0,
		"ブニャット"=>0,
		"リーシャン"=>0,
		"スカンプー"=>0,
		"スカタンク"=>0,
		"ドーミラー"=>0,
		"ドータクン"=>0,
		"ウソハチ"=>0,
		"マネネ"=>0,
		"ピンプク"=>0,
		"ペラップ"=>0,
		"ミカルゲ"=>0,
		"フカマル"=>0,
		"ガバイト"=>0,
		"ガブリアス"=>0,
		"ゴンベ"=>0,
		"リオル"=>0,
		"ルカリオ"=>0,
		"ヒポポタス"=>0,
		"カバルドン"=>0,
		"スコルピ"=>0,
		"ドラピオン"=>0,
		"グレッグル"=>0,
		"ドクロッグ"=>0,
		"マスキッパ"=>0,
		"ケイコウオ"=>0,
		"ネオラント"=>0,
		"タマンタ"=>0,
		"ユキカブリ"=>0,
		"ユキノオー"=>0,
		"マニューラ"=>0,
		"ジバコイル"=>0,
		"ベロベルト"=>0,
		"ドサイドン"=>0,
		"モジャンボ"=>0,
		"エレキブル"=>0,
		"ブーバーン"=>0,
		"トゲキッス"=>0,
		"メガヤンマ"=>0,
		"リーフィア"=>0,
		"グレイシア"=>0,
		"グライオン"=>0,
		"マンムー"=>0,
		"ポリゴンＺ"=>0,
		"エルレイド"=>0,
		"ダイノーズ"=>0,
		"ヨノワール"=>0,
		"ユキメノコ"=>0,
		"ロトム"=>0,
		"ユクシー"=>6,
		"エムリット"=>0,
		"アグノム"=>0,
		"ディアルガ"=>0,
		"パルキア"=>0,
		"ヒードラン"=>0,
		"レジギガス"=>0,
		"ギラティナ"=>0,
		"クレセリア"=>2,
		"フィオネ"=>0,
		"マナフィ"=>0,
		"ダークライ"=>0,
		"シェイミ"=>0,
		"アルセウス"=>2,
		"クサヘビ"=>17,
		"クサネーク"=>0,
		"プランブラ"=>0,
		"ポカブ"=>0,
		"チャオブー"=>0,
		"エンブオー"=>0,
		"スイッコ"=>0,
		"ミズッコ"=>0,
		"ラッコール"=>0,
		"ミネズミ"=>0,
		"ミルホッグ"=>0,
		"パピー"=>0,
		"パブロー"=>0,
		"パブドッグ"=>0,
		"チョロネコ"=>0,
		"レパルダス"=>0,
		"ブロッコ"=>0,
		"ブロッコル"=>0,
		"スチック"=>0,
		"スチームル"=>0,
		"ファウン"=>0,
		"ファウント"=>0,
		"ムンナ"=>0,
		"ムシャーナ"=>0,
		"マメパト"=>0,
		"ハトーボー"=>0,
		"ケンホロウ"=>0,
		"ムカッテ"=>0,
		"ムカンデ"=>0,
		"ムカデンテ"=>0,
		"シママ"=>0,
		"ゼブライカ"=>0,
		"ガンセキ"=>0,
		"ガンクツ"=>0,
		"ガンドロス"=>0,
		"コロモリ"=>0,
		"ココロモリ"=>0,
		"ニューモン"=>0,
		"ユーダーン"=>0,
		"シハンドン"=>0,
		"モグリュー"=>0,
		"ドリュウズ"=>0,
		"シンピッピ"=>0,
		"オタマロ"=>0,
		"ガマガル"=>0,
		"ガマゲロゲ"=>0,
		"ダゲキ"=>0,
		"ナゲキ"=>0,
		"クルミル"=>0,
		"クルマユ"=>0,
		"ハハコモリ"=>0,
		"モンメン"=>0,
		"エルフーン"=>0,
		"チュリネ"=>0,
		"ドレディア"=>0,
		"バスラオ"=>0,
		"ダルマッカ"=>2,
		"ヒヒダルマ"=>0,
		"メグロコ"=>2,
		"ワルビル"=>0,
		"ワルビアル"=>0,
		"マラカッチ"=>0,
		"ヤドリ"=>0,
		"ヤドカルゴ"=>0,
		"ナスカドリ"=>0,
		"デスマス"=>0,
		"デスカーン"=>0,
		"ゴビット"=>0,
		"ゴルーグ"=>0,
		"カセッキ"=>0,
		"カセキング"=>0,
		"アーケン"=>0,
		"アーケオス"=>0,
		"ヤブクロン"=>0,
		"ダストダス"=>0,
		"チラーミィ"=>0,
		"チラチーノ"=>0,
		"スンドメー"=>0,
		"フルコング"=>0,
		"ゴスロ"=>0,
		"ゴスロス"=>0,
		"ゴスローゼ"=>0,
		"ユニラン"=>0,
		"ダブラン"=>0,
		"ランクルス"=>0,
		"バチュル"=>0,
		"デンチュラ"=>0,
		"トビッチュ"=>0,
		"コアルヒー"=>0,
		"スワンナ"=>0,
		"ツルリ"=>0,
		"ツルット"=>0,
		"ツルツルダ"=>0,
		"バンビーナ"=>0,
		"シキジカ"=>4,
		"カブリン"=>4,
		"カブリオン"=>0,
		"タマゲタケ"=>0,
		"モロバレル"=>0,
		"ハッチェ"=>0,
		"オノドン"=>0,
		"オノックス"=>0,
		"プルンス"=>0,
		"プルキング"=>0,
		"ママンボウ"=>0,
		"エレス"=>0,
		"エレクス"=>0,
		"エレキテス"=>0,
		"テッパン"=>0,
		"ゴウキーン"=>0,
		"ギアル"=>0,
		"ギギアル"=>0,
		"ギギギアル"=>0,
		"テレス"=>0,
		"テレパス"=>0,
		"ローソクン"=>0,
		"ランプラー"=>0,
		"シャンドル"=>0,
		"ゾロア"=>0,
		"ゾロアーク"=>0,
		"シルクマ"=>0,
		"ツンベアー"=>0,
		"アイスゴン"=>0,
		"カッチュ"=>0,
		"カチュード"=>0,
		"オニソコ"=>0,
		"グラップ"=>0,
		"グラッパー"=>0,
		"オーロッチ"=>0,
		"ズルル"=>0,
		"ズルガシー"=>0,
		"バッフロン"=>0,
		"アウトン"=>0,
		"アウトロン"=>0,
		"イーグラ"=>0,
		"イーグラン"=>0,
		"メラメラス"=>0,
		"スチーラス"=>0,
		"ドラッコ"=>0,
		"ドラーゴ"=>0,
		"ドラゴーン"=>0,
		"ラー"=>0,
		"ドラー"=>0,
		"アトス"=>0,
		"ポルトス"=>0,
		"アラミス"=>0,
		"カザカミ"=>0,
		"ライカミ"=>0,
		"シン"=>0,
		"ム"=>0,
		"ツチノカミ"=>0,
		"ライ"=>0,
		"ダルタニス"=>0,
		"メロディア"=>0,
		"インセクタ"=>2,
		"ビクティ"=>5,
	}
	$gra2zukan_hash = {
		"001"=>1,
		"002"=>2,
		"003"=>3,
		"004"=>4,
		"005"=>5,
		"006"=>6,
		"007"=>7,
		"008"=>8,
		"009"=>9,
		"010"=>10,
		"011"=>11,
		"012"=>12,
		"013"=>13,
		"014"=>14,
		"015"=>15,
		"016"=>16,
		"017"=>17,
		"018"=>18,
		"019"=>19,
		"020"=>20,
		"021"=>21,
		"022"=>22,
		"023"=>23,
		"024"=>24,
		"025"=>25,
		"026"=>26,
		"027"=>27,
		"028"=>28,
		"029"=>29,
		"030"=>30,
		"031"=>31,
		"032"=>32,
		"033"=>33,
		"034"=>34,
		"035"=>35,
		"036"=>36,
		"037"=>37,
		"038"=>38,
		"039"=>39,
		"040"=>40,
		"041"=>41,
		"042"=>42,
		"043"=>43,
		"044"=>44,
		"045"=>45,
		"046"=>46,
		"047"=>47,
		"048"=>48,
		"049"=>49,
		"050"=>50,
		"051"=>51,
		"052"=>52,
		"053"=>53,
		"054"=>54,
		"055"=>55,
		"056"=>56,
		"057"=>57,
		"058"=>58,
		"059"=>59,
		"060"=>60,
		"061"=>61,
		"062"=>62,
		"063"=>63,
		"064"=>64,
		"065"=>65,
		"066"=>66,
		"067"=>67,
		"068"=>68,
		"069"=>69,
		"070"=>70,
		"071"=>71,
		"072"=>72,
		"073"=>73,
		"074"=>74,
		"075"=>75,
		"076"=>76,
		"077"=>77,
		"078"=>78,
		"079"=>79,
		"080"=>80,
		"081"=>81,
		"082"=>82,
		"083"=>83,
		"084"=>84,
		"085"=>85,
		"086"=>86,
		"087"=>87,
		"088"=>88,
		"089"=>89,
		"090"=>90,
		"091"=>91,
		"092"=>92,
		"093"=>93,
		"094"=>94,
		"095"=>95,
		"096"=>96,
		"097"=>97,
		"098"=>98,
		"099"=>99,
		"100"=>100,
		"101"=>101,
		"102"=>102,
		"103"=>103,
		"104"=>104,
		"105"=>105,
		"106"=>106,
		"107"=>107,
		"108"=>108,
		"109"=>109,
		"110"=>110,
		"111"=>111,
		"112"=>112,
		"113"=>113,
		"114"=>114,
		"115"=>115,
		"116"=>116,
		"117"=>117,
		"118"=>118,
		"119"=>119,
		"120"=>120,
		"121"=>121,
		"122"=>122,
		"123"=>123,
		"124"=>124,
		"125"=>125,
		"126"=>126,
		"127"=>127,
		"128"=>128,
		"129"=>129,
		"130"=>130,
		"131"=>131,
		"132"=>132,
		"133"=>133,
		"134"=>134,
		"135"=>135,
		"136"=>136,
		"137"=>137,
		"138"=>138,
		"139"=>139,
		"140"=>140,
		"141"=>141,
		"142"=>142,
		"143"=>143,
		"144"=>144,
		"145"=>145,
		"146"=>146,
		"147"=>147,
		"148"=>148,
		"149"=>149,
		"150"=>150,
		"151"=>151,
		"152"=>152,
		"153"=>153,
		"154"=>154,
		"155"=>155,
		"156"=>156,
		"157"=>157,
		"158"=>158,
		"159"=>159,
		"160"=>160,
		"161"=>161,
		"162"=>162,
		"163"=>163,
		"164"=>164,
		"165"=>165,
		"166"=>166,
		"167"=>167,
		"168"=>168,
		"169"=>169,
		"170"=>170,
		"171"=>171,
		"172"=>172,
		"173"=>173,
		"174"=>174,
		"175"=>175,
		"176"=>176,
		"177"=>177,
		"178"=>178,
		"179"=>179,
		"180"=>180,
		"181"=>181,
		"182"=>182,
		"183"=>183,
		"184"=>184,
		"185"=>185,
		"186"=>186,
		"187"=>187,
		"188"=>188,
		"189"=>189,
		"190"=>190,
		"191"=>191,
		"192"=>192,
		"193"=>193,
		"194"=>194,
		"195"=>195,
		"196"=>196,
		"197"=>197,
		"198"=>198,
		"199"=>199,
		"200"=>200,
		"201"=>201,
		"202"=>202,
		"203"=>203,
		"204"=>204,
		"205"=>205,
		"206"=>206,
		"207"=>207,
		"208"=>208,
		"209"=>209,
		"210"=>210,
		"211"=>211,
		"212"=>212,
		"213"=>213,
		"214"=>214,
		"215"=>215,
		"216"=>216,
		"217"=>217,
		"218"=>218,
		"219"=>219,
		"220"=>220,
		"221"=>221,
		"222"=>222,
		"223"=>223,
		"224"=>224,
		"225"=>225,
		"226"=>226,
		"227"=>227,
		"228"=>228,
		"229"=>229,
		"230"=>230,
		"231"=>231,
		"232"=>232,
		"233"=>233,
		"234"=>234,
		"235"=>235,
		"236"=>236,
		"237"=>237,
		"238"=>238,
		"239"=>239,
		"240"=>240,
		"241"=>241,
		"242"=>242,
		"243"=>243,
		"244"=>244,
		"245"=>245,
		"246"=>246,
		"247"=>247,
		"248"=>248,
		"249"=>249,
		"250"=>250,
		"251"=>251,
		"252"=>252,
		"253"=>253,
		"254"=>254,
		"255"=>255,
		"256"=>256,
		"257"=>257,
		"258"=>258,
		"259"=>259,
		"260"=>260,
		"261"=>261,
		"262"=>262,
		"263"=>263,
		"264"=>264,
		"265"=>265,
		"266"=>266,
		"267"=>267,
		"268"=>268,
		"269"=>269,
		"270"=>270,
		"271"=>271,
		"272"=>272,
		"273"=>273,
		"274"=>274,
		"275"=>275,
		"276"=>276,
		"277"=>277,
		"278"=>278,
		"279"=>279,
		"280"=>280,
		"281"=>281,
		"282"=>282,
		"283"=>283,
		"284"=>284,
		"285"=>285,
		"286"=>286,
		"287"=>287,
		"288"=>288,
		"289"=>289,
		"290"=>290,
		"291"=>291,
		"292"=>292,
		"293"=>293,
		"294"=>294,
		"295"=>295,
		"296"=>296,
		"297"=>297,
		"298"=>298,
		"299"=>299,
		"300"=>300,
		"301"=>301,
		"302"=>302,
		"303"=>303,
		"304"=>304,
		"305"=>305,
		"306"=>306,
		"307"=>307,
		"308"=>308,
		"309"=>309,
		"310"=>310,
		"311"=>311,
		"312"=>312,
		"313"=>313,
		"314"=>314,
		"315"=>315,
		"316"=>316,
		"317"=>317,
		"318"=>318,
		"319"=>319,
		"320"=>320,
		"321"=>321,
		"322"=>322,
		"323"=>323,
		"324"=>324,
		"325"=>325,
		"326"=>326,
		"327"=>327,
		"328"=>328,
		"329"=>329,
		"330"=>330,
		"331"=>331,
		"332"=>332,
		"333"=>333,
		"334"=>334,
		"335"=>335,
		"336"=>336,
		"337"=>337,
		"338"=>338,
		"339"=>339,
		"340"=>340,
		"341"=>341,
		"342"=>342,
		"343"=>343,
		"344"=>344,
		"345"=>345,
		"346"=>346,
		"347"=>347,
		"348"=>348,
		"349"=>349,
		"350"=>350,
		"351"=>351,
		"352"=>352,
		"353"=>353,
		"354"=>354,
		"355"=>355,
		"356"=>356,
		"357"=>357,
		"358"=>358,
		"359"=>359,
		"360"=>360,
		"361"=>361,
		"362"=>362,
		"363"=>363,
		"364"=>364,
		"365"=>365,
		"366"=>366,
		"367"=>367,
		"368"=>368,
		"369"=>369,
		"370"=>370,
		"371"=>371,
		"372"=>372,
		"373"=>373,
		"374"=>374,
		"375"=>375,
		"376"=>376,
		"377"=>377,
		"378"=>378,
		"379"=>379,
		"380"=>380,
		"381"=>381,
		"382"=>382,
		"383"=>383,
		"384"=>384,
		"385"=>385,
		"386"=>386,
		"387"=>387,
		"388"=>388,
		"389"=>389,
		"390"=>390,
		"391"=>391,
		"392"=>392,
		"393"=>393,
		"394"=>394,
		"395"=>395,
		"396"=>396,
		"397"=>397,
		"398"=>398,
		"399"=>399,
		"400"=>400,
		"401"=>401,
		"402"=>402,
		"403"=>403,
		"404"=>404,
		"405"=>405,
		"406"=>406,
		"407"=>407,
		"408"=>408,
		"409"=>409,
		"410"=>410,
		"411"=>411,
		"412"=>412,
		"413"=>413,
		"414"=>414,
		"415"=>415,
		"416"=>416,
		"417"=>417,
		"418"=>418,
		"419"=>419,
		"420"=>420,
		"421"=>421,
		"422"=>422,
		"423"=>423,
		"424"=>424,
		"425"=>425,
		"426"=>426,
		"427"=>427,
		"428"=>428,
		"429"=>429,
		"430"=>430,
		"431"=>431,
		"432"=>432,
		"433"=>433,
		"434"=>434,
		"435"=>435,
		"436"=>436,
		"437"=>437,
		"438"=>438,
		"439"=>439,
		"440"=>440,
		"441"=>441,
		"442"=>442,
		"443"=>443,
		"444"=>444,
		"445"=>445,
		"446"=>446,
		"447"=>447,
		"448"=>448,
		"449"=>449,
		"450"=>450,
		"451"=>451,
		"452"=>452,
		"453"=>453,
		"454"=>454,
		"455"=>455,
		"456"=>456,
		"457"=>457,
		"458"=>458,
		"459"=>459,
		"460"=>460,
		"461"=>461,
		"462"=>462,
		"463"=>463,
		"464"=>464,
		"465"=>465,
		"466"=>466,
		"467"=>467,
		"468"=>468,
		"469"=>469,
		"470"=>470,
		"471"=>471,
		"472"=>472,
		"473"=>473,
		"474"=>474,
		"475"=>475,
		"476"=>476,
		"477"=>477,
		"478"=>478,
		"479"=>479,
		"480"=>480,
		"481"=>481,
		"482"=>482,
		"483"=>483,
		"484"=>484,
		"485"=>485,
		"486"=>486,
		"487"=>487,
		"488"=>488,
		"489"=>489,
		"490"=>490,
		"491"=>491,
		"492"=>492,
		"493"=>493,
		"551"=>494,
		"552"=>495,
		"553"=>496,
		"554"=>497,
		"555"=>498,
		"556"=>499,
		"557"=>500,
		"558"=>501,
		"559"=>502,
		"537"=>503,
		"538"=>504,
		"590"=>505,
		"591"=>506,
		"592"=>507,
		"546"=>508,
		"547"=>509,
		"593"=>510,
		"594"=>511,
		"595"=>512,
		"596"=>513,
		"597"=>514,
		"598"=>515,
		"540"=>516,
		"516"=>517,
		"514"=>518,
		"518"=>519,
		"512"=>520,
		"548"=>521,
		"549"=>522,
		"550"=>523,
		"570"=>524,
		"515"=>525,
		"600"=>526,
		"601"=>527,
		"602"=>528,
		"520"=>529,
		"535"=>530,
		"626"=>531,
		"627"=>532,
		"628"=>533,
		"502"=>534,
		"543"=>535,
		"599"=>536,
		"586"=>537,
		"587"=>538,
		"588"=>539,
		"575"=>540,
		"576"=>541,
		"507"=>542,
		"524"=>543,
		"508"=>544,
		"521"=>545,
		"522"=>546,
		"517"=>547,
		"519"=>548,
		"603"=>549,
		"528"=>550,
		"503"=>551,
		"504"=>552,
		"505"=>553,
		"506"=>554,
		"523"=>555,
		"568"=>556,
		"569"=>557,
		"539"=>558,
		"526"=>559,
		"510"=>560,
		"604"=>561,
		"605"=>562,
		"622"=>563,
		"623"=>564,
		"624"=>565,
		"625"=>566,
		"573"=>567,
		"574"=>568,
		"533"=>569,
		"534"=>570,
		"637"=>571,
		"638"=>572,
		"606"=>573,
		"607"=>574,
		"608"=>575,
		"583"=>576,
		"584"=>577,
		"585"=>578,
		"577"=>579,
		"578"=>580,
		"560"=>581,
		"609"=>582,
		"610"=>583,
		"611"=>584,
		"612"=>585,
		"613"=>586,
		"511"=>587,
		"527"=>588,
		"571"=>589,
		"572"=>590,
		"580"=>591,
		"581"=>592,
		"541"=>593,
		"542"=>594,
		"509"=>595,
		"513"=>596,
		"536"=>597,
		"501"=>598,
		"614"=>599,
		"615"=>600,
		"616"=>601,
		"617"=>602,
		"618"=>603,
		"544"=>604,
		"545"=>605,
		"579"=>606,
		"619"=>607,
		"620"=>608,
		"563"=>609,
		"564"=>610,
		"565"=>611,
		"529"=>612,
		"530"=>613,
		"531"=>614,
		"532"=>615,
		"621"=>616,
		"561"=>617,
		"562"=>618,
		"589"=>619,
		"630"=>620,
		"631"=>621,
		"629"=>622,
		"634"=>623,
		"635"=>624,
		"582"=>625,
		"632"=>626,
		"633"=>627,
		"566"=>628,
		"567"=>629,
		"636"=>630,
		"639"=>631,
		"640"=>632,
		"641"=>633,
		"642"=>634,
		"643"=>635,
		"644"=>636,
		"645"=>637,
		"646"=>638,
		"647"=>639,
		"648"=>640,
		"649"=>641,
		"650"=>642,
		"651"=>643,
		"652"=>644,
		"653"=>645,
		"654"=>646,
		"655"=>647,
		"656"=>648,
		"657"=>649,
	}
