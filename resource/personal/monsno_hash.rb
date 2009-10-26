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
		"ピンボー"=>494,
		"モグリュー"=>495,
		"ゴリダルマ"=>496,
		"メグロコ"=>497,
		"ワルビル"=>498,
		"ワルビアル"=>499,
		"クルミル"=>500,
		"ハハコモリ"=>501,
		"オノックス"=>502,
		"カーメント"=>503,
		"バンビーナ"=>504,
		"ギアード"=>505,
		"プルンス"=>506,
		"マメパト"=>507,
		"ライブラ"=>508,
		"ムシャーナ"=>509,
		"チュリネ"=>510,
		"ギアン"=>511,
		"ドレディア"=>512,
		"コロモリ"=>513,
		"モンメン"=>514,
		"エルフーン"=>515,
		"サボッテン"=>516,
		"クルマユ"=>517,
		"ココロモリ"=>518,
		"ランプラー"=>519,
		"シャンドル"=>520,
		"ヒダルマ"=>521,
		"ゾロア"=>522,
		"ゾロアーク"=>523,
		"シロベア"=>524,
		"シャーベア"=>525,
		"チラン"=>526,
		"チララン"=>527,
		"ローソクン"=>528,
		"ノラコ"=>529,
		"ミネズミ"=>530,
		"ミルホッグ"=>531,
		"ナスカドリ"=>532,
		"ムンナ"=>533,
		"ハッチェ"=>534,
		"ドリュウズ"=>535,
		"オノドン"=>536,
		"ハトーボー"=>537,
		"カーオケン"=>538,
		"ケンホロウ"=>539,
		"シキジカ"=>540,
		"プルキング"=>541,
		"ライブラス"=>542,
		"ギアントス"=>543,
		"ノラニャー"=>544,
		"ワンラン"=>545,
		"ツーラン"=>546,
		"スリーラン"=>547,
		"クサヘビ"=>548,
		"クサネーク"=>549,
		"プランブラ"=>550,
		"ホットン"=>551,
		"ヒートン"=>552,
		"ヤキブート"=>553,
		"スイッコ"=>554,
		"ミズッコ"=>555,
		"ラッコール"=>556,
		"セイヤー"=>557,
		"チェストー"=>558,
		"ムカッテ"=>559,
		"ムカンデ"=>560,
		"ムカデンテ"=>561,
		"デンダニ"=>562,
		"デンチュラ"=>563,
		"イーグラ"=>564,
		"イーグラン"=>565,
		"バッフロー"=>566,
		"タニキング"=>567,
		"キノタマ"=>568,
		"キノボール"=>569,
		"トビッチュ"=>570,
		"カッチュ"=>571,
		"カチュード"=>572,
		"ヤドリ"=>573,
		"ヤドカルゴ"=>574,
		"カブリン"=>575,
		"カブリオン"=>576,
		"タニス"=>577,
		"タニーノ"=>578,
		"ソダイゴン"=>579,
		"オーブッツ"=>580,
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
		"ピンボー"=>18,
		"モグリュー"=>0,
		"ゴリダルマ"=>0,
		"メグロコ"=>0,
		"ワルビル"=>0,
		"ワルビアル"=>0,
		"クルミル"=>0,
		"ハハコモリ"=>0,
		"オノックス"=>0,
		"カーメント"=>0,
		"バンビーナ"=>0,
		"ギアード"=>0,
		"プルンス"=>0,
		"マメパト"=>0,
		"ライブラ"=>0,
		"ムシャーナ"=>0,
		"チュリネ"=>0,
		"ギアン"=>0,
		"ドレディア"=>0,
		"コロモリ"=>0,
		"モンメン"=>0,
		"エルフーン"=>0,
		"サボッテン"=>0,
		"クルマユ"=>0,
		"ココロモリ"=>0,
		"ランプラー"=>0,
		"シャンドル"=>0,
		"ヒダルマ"=>0,
		"ゾロア"=>0,
		"ゾロアーク"=>0,
		"シロベア"=>0,
		"シャーベア"=>0,
		"チラン"=>0,
		"チララン"=>0,
		"ローソクン"=>0,
		"ノラコ"=>0,
		"ミネズミ"=>0,
		"ミルホッグ"=>0,
		"ナスカドリ"=>0,
		"ムンナ"=>0,
		"ハッチェ"=>0,
		"ドリュウズ"=>0,
		"オノドン"=>0,
		"ハトーボー"=>0,
		"カーオケン"=>0,
		"ケンホロウ"=>0,
		"シキジカ"=>0,
		"プルキング"=>0,
		"ライブラス"=>0,
		"ギアントス"=>0,
		"ノラニャー"=>0,
		"ワンラン"=>0,
		"ツーラン"=>0,
		"スリーラン"=>0,
		"クサヘビ"=>0,
		"クサネーク"=>0,
		"プランブラ"=>0,
		"ホットン"=>0,
		"ヒートン"=>0,
		"ヤキブート"=>0,
		"スイッコ"=>0,
		"ミズッコ"=>0,
		"ラッコール"=>0,
		"セイヤー"=>0,
		"チェストー"=>0,
		"ムカッテ"=>0,
		"ムカンデ"=>0,
		"ムカデンテ"=>0,
		"デンダニ"=>0,
		"デンチュラ"=>0,
		"イーグラ"=>0,
		"イーグラン"=>0,
		"バッフロー"=>0,
		"タニキング"=>0,
		"キノタマ"=>0,
		"キノボール"=>0,
		"トビッチュ"=>0,
		"カッチュ"=>0,
		"カチュード"=>0,
		"ヤドリ"=>0,
		"ヤドカルゴ"=>0,
		"カブリン"=>0,
		"カブリオン"=>0,
		"タニス"=>0,
		"タニーノ"=>0,
		"ソダイゴン"=>0,
		"オーブッツ"=>0,
	}
