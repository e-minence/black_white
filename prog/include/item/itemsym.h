//============================================================================================
/**
 * itemsym.h
 * 2009 Game Freak Inc.
 * item data cnv ver.2.00
 */
//============================================================================================
#ifndef ITEMSYM_H
#define ITEMSYM_H

#define ITEM_DUMMY_DATA		( 0 )		// ダミーデータ
#define ITEM_MASUTAABOORU		( 1 )		// マスターボール
#define ITEM_HAIPAABOORU		( 2 )		// ハイパーボール
#define ITEM_SUUPAABOORU		( 3 )		// スーパーボール
#define ITEM_MONSUTAABOORU		( 4 )		// モンスターボール
#define ITEM_SAFARIBOORU		( 5 )		// サファリボール
#define ITEM_NETTOBOORU		( 6 )		// ネットボール
#define ITEM_DAIBUBOORU		( 7 )		// ダイブボール
#define ITEM_NESUTOBOORU		( 8 )		// ネストボール
#define ITEM_RIPIITOBOORU		( 9 )		// リピートボール
#define ITEM_TAIMAABOORU		( 10 )		// タイマーボール
#define ITEM_GOOZYASUBOORU		( 11 )		// ゴージャスボール
#define ITEM_PUREMIABOORU		( 12 )		// プレミアボール
#define ITEM_DAAKUBOORU		( 13 )		// ダークボール
#define ITEM_HIIRUBOORU		( 14 )		// ヒールボール
#define ITEM_KUIKKUBOORU		( 15 )		// クイックボール
#define ITEM_PURESYASUBOORU		( 16 )		// プレシャスボール
#define ITEM_KIZUGUSURI		( 17 )		// キズぐすり
#define ITEM_DOKUKESI		( 18 )		// どくけし
#define ITEM_YAKEDONAOSI		( 19 )		// やけどなおし
#define ITEM_KOORINAOSI		( 20 )		// こおりなおし
#define ITEM_NEMUKEZAMASI		( 21 )		// ねむけざまし
#define ITEM_MAHINAOSI		( 22 )		// まひなおし
#define ITEM_KAIHUKUNOKUSURI		( 23 )		// かいふくのくすり
#define ITEM_MANTANNOKUSURI		( 24 )		// まんたんのくすり
#define ITEM_SUGOIKIZUGUSURI		( 25 )		// すごいキズぐすり
#define ITEM_IIKIZUGUSURI		( 26 )		// いいキズぐすり
#define ITEM_NANDEMONAOSI		( 27 )		// なんでもなおし
#define ITEM_GENKINOKAKERA		( 28 )		// げんきのかけら
#define ITEM_GENKINOKATAMARI		( 29 )		// げんきのかたまり
#define ITEM_OISIIMIZU		( 30 )		// おいしいみず
#define ITEM_SAIKOSOODA		( 31 )		// サイコソーダ
#define ITEM_MIKKUSUORE		( 32 )		// ミックスオレ
#define ITEM_MOOMOOMIRUKU		( 33 )		// モーモーミルク
#define ITEM_TIKARANOKONA		( 34 )		// ちからのこな
#define ITEM_TIKARANONEKKO		( 35 )		// ちからのねっこ
#define ITEM_BANNOUGONA		( 36 )		// ばんのうごな
#define ITEM_HUKKATUSOU		( 37 )		// ふっかつそう
#define ITEM_PIIPIIEIDO		( 38 )		// ピーピーエイド
#define ITEM_PIIPIIRIKABAA		( 39 )		// ピーピーリカバー
#define ITEM_PIIPIIEIDAA		( 40 )		// ピーピーエイダー
#define ITEM_PIIPIIMAKKUSU		( 41 )		// ピーピーマックス
#define ITEM_HUENSENBEI		( 42 )		// フエンせんべい
#define ITEM_KINOMIZYUUSU		( 43 )		// きのみジュース
#define ITEM_SEINARUHAI		( 44 )		// せいなるはい
#define ITEM_MAKKUSUAPPU		( 45 )		// マックスアップ
#define ITEM_TAURIN		( 46 )		// タウリン
#define ITEM_BUROMUHEKISIN		( 47 )		// ブロムヘキシン
#define ITEM_INDOMETASIN		( 48 )		// インドメタシン
#define ITEM_RIZOTIUMU		( 49 )		// リゾチウム
#define ITEM_HUSIGINAAME		( 50 )		// ふしぎなアメ
#define ITEM_POINTOAPPU		( 51 )		// ポイントアップ
#define ITEM_KITOSAN		( 52 )		// キトサン
#define ITEM_POINTOMAKKUSU		( 53 )		// ポイントマックス
#define ITEM_MORINOYOUKAN		( 54 )		// もりのヨウカン
#define ITEM_EFEKUTOGAADO		( 55 )		// エフェクトガード
#define ITEM_KURITHIKATTAA		( 56 )		// クリティカッター
#define ITEM_PURASUPAWAA		( 57 )		// プラスパワー
#define ITEM_DHIFENDAA		( 58 )		// ディフェンダー
#define ITEM_SUPIIDAA		( 59 )		// スピーダー
#define ITEM_YOKUATAARU		( 60 )		// ヨクアタール
#define ITEM_SUPESYARUAPPU		( 61 )		// スペシャルアップ
#define ITEM_SUPESYARUGAADO		( 62 )		// スペシャルガード
#define ITEM_PIPPININGYOU		( 63 )		// ピッピにんぎょう
#define ITEM_ENEKONOSIPPO		( 64 )		// エネコのシッポ
#define ITEM_AOIBIIDORO		( 65 )		// あおいビードロ
#define ITEM_KIIROBIIDORO		( 66 )		// きいろビードロ
#define ITEM_AKAIBIIDORO		( 67 )		// あかいビードロ
#define ITEM_KUROIBIIDORO		( 68 )		// くろいビードロ
#define ITEM_SIROIBIIDORO		( 69 )		// しろいビードロ
#define ITEM_ASASENOSIO		( 70 )		// あさせのしお
#define ITEM_ASASENOKAIGARA		( 71 )		// あさせのかいがら
#define ITEM_AKAIKAKERA		( 72 )		// あかいかけら
#define ITEM_AOIKAKERA		( 73 )		// あおいかけら
#define ITEM_KIIROIKAKERA		( 74 )		// きいろいかけら
#define ITEM_MIDORINOKAKERA		( 75 )		// みどりのかけら
#define ITEM_SIRUBAASUPUREE		( 76 )		// シルバースプレー
#define ITEM_GOORUDOSUPUREE		( 77 )		// ゴールドスプレー
#define ITEM_ANANUKENOHIMO		( 78 )		// あなぬけのヒモ
#define ITEM_MUSIYOKESUPUREE		( 79 )		// むしよけスプレー
#define ITEM_TAIYOUNOISI		( 80 )		// たいようのいし
#define ITEM_TUKINOISI		( 81 )		// つきのいし
#define ITEM_HONOONOISI		( 82 )		// ほのおのいし
#define ITEM_KAMINARINOISI		( 83 )		// かみなりのいし
#define ITEM_MIZUNOISI		( 84 )		// みずのいし
#define ITEM_RIIHUNOISI		( 85 )		// リーフのいし
#define ITEM_TIISANAKINOKO		( 86 )		// ちいさなキノコ
#define ITEM_OOKINAKINOKO		( 87 )		// おおきなキノコ
#define ITEM_SINZYU		( 88 )		// しんじゅ
#define ITEM_OOKINASINZYU		( 89 )		// おおきなしんじゅ
#define ITEM_HOSINOSUNA		( 90 )		// ほしのすな
#define ITEM_HOSINOKAKERA		( 91 )		// ほしのかけら
#define ITEM_KINNOTAMA		( 92 )		// きんのたま
#define ITEM_HAATONOUROKO		( 93 )		// ハートのウロコ
#define ITEM_AMAIMITU		( 94 )		// あまいミツ
#define ITEM_SUKUSUKUKOYASI		( 95 )		// すくすくこやし
#define ITEM_ZIMEZIMEKOYASI		( 96 )		// じめじめこやし
#define ITEM_NAGANAGAKOYASI		( 97 )		// ながながこやし
#define ITEM_NEBANEBAKOYASI		( 98 )		// ねばねばこやし
#define ITEM_NEKKONOKASEKI		( 99 )		// ねっこのカセキ
#define ITEM_TUMENOKASEKI		( 100 )		// ツメのカセキ
#define ITEM_KAINOKASEKI		( 101 )		// かいのカセキ
#define ITEM_KOURANOKASEKI		( 102 )		// こうらのカセキ
#define ITEM_HIMITUNOKOHAKU		( 103 )		// ひみつのコハク
#define ITEM_TATENOKASEKI		( 104 )		// たてのカセキ
#define ITEM_ZUGAINOKASEKI		( 105 )		// ずがいのカセキ
#define ITEM_KITYOUNAHONE		( 106 )		// きちょうなホネ
#define ITEM_HIKARINOISI		( 107 )		// ひかりのいし
#define ITEM_YAMINOISI		( 108 )		// やみのいし
#define ITEM_MEZAMEISI		( 109 )		// めざめいし
#define ITEM_MANMARUISI		( 110 )		// まんまるいし
#define ITEM_KANAMEISI		( 111 )		// かなめいし
#define ITEM_HAKKINDAMA		( 112 )		// はっきんだま
#define ITEM_KONGOUDAMA		( 135 )		// こんごうだま
#define ITEM_SIRATAMA		( 136 )		// しらたま
#define ITEM_GURASUMEERU		( 137 )		// グラスメール
#define ITEM_HUREIMUMEERU		( 138 )		// フレイムメール
#define ITEM_BURUUMEERU		( 139 )		// ブルーメール
#define ITEM_BURUUMUMEERU		( 140 )		// ブルームメール
#define ITEM_TONNERUMEERU		( 141 )		// トンネルメール
#define ITEM_SUTIIRUMEERU		( 142 )		// スチールメール
#define ITEM_RABURABUMEERU		( 143 )		// ラブラブメール
#define ITEM_BURIZAADOMEERU		( 144 )		// ブリザードメール
#define ITEM_SUPEESUMEERU		( 145 )		// スペースメール
#define ITEM_EAMEERU		( 146 )		// エアメール
#define ITEM_MOZAIKUMEERU		( 147 )		// モザイクメール
#define ITEM_BURIKKUMEERU		( 148 )		// ブリックメール
#define ITEM_KURABONOMI		( 149 )		// クラボのみ
#define ITEM_KAGONOMI		( 150 )		// カゴのみ
#define ITEM_MOMONNOMI		( 151 )		// モモンのみ
#define ITEM_TIIGONOMI		( 152 )		// チーゴのみ
#define ITEM_NANASINOMI		( 153 )		// ナナシのみ
#define ITEM_HIMERINOMI		( 154 )		// ヒメリのみ
#define ITEM_ORENNOMI		( 155 )		// オレンのみ
#define ITEM_KIINOMI		( 156 )		// キーのみ
#define ITEM_RAMUNOMI		( 157 )		// ラムのみ
#define ITEM_OBONNOMI		( 158 )		// オボンのみ
#define ITEM_FIRANOMI		( 159 )		// フィラのみ
#define ITEM_UINOMI		( 160 )		// ウイのみ
#define ITEM_MAGONOMI		( 161 )		// マゴのみ
#define ITEM_BANZINOMI		( 162 )		// バンジのみ
#define ITEM_IANOMI		( 163 )		// イアのみ
#define ITEM_ZURINOMI		( 164 )		// ズリのみ
#define ITEM_BURIINOMI		( 165 )		// ブリーのみ
#define ITEM_NANANOMI		( 166 )		// ナナのみ
#define ITEM_SESINANOMI		( 167 )		// セシナのみ
#define ITEM_PAIRUNOMI		( 168 )		// パイルのみ
#define ITEM_ZAROKUNOMI		( 169 )		// ザロクのみ
#define ITEM_NEKOBUNOMI		( 170 )		// ネコブのみ
#define ITEM_TAPORUNOMI		( 171 )		// タポルのみ
#define ITEM_ROMENOMI		( 172 )		// ロメのみ
#define ITEM_UBUNOMI		( 173 )		// ウブのみ
#define ITEM_MATOMANOMI		( 174 )		// マトマのみ
#define ITEM_MOKOSINOMI		( 175 )		// モコシのみ
#define ITEM_GOSUNOMI		( 176 )		// ゴスのみ
#define ITEM_RABUTANOMI		( 177 )		// ラブタのみ
#define ITEM_NOMERUNOMI		( 178 )		// ノメルのみ
#define ITEM_NOWAKINOMI		( 179 )		// ノワキのみ
#define ITEM_SIIYANOMI		( 180 )		// シーヤのみ
#define ITEM_KAISUNOMI		( 181 )		// カイスのみ
#define ITEM_DORINOMI		( 182 )		// ドリのみ
#define ITEM_BERIBUNOMI		( 183 )		// ベリブのみ
#define ITEM_OKKANOMI		( 184 )		// オッカのみ
#define ITEM_ITOKENOMI		( 185 )		// イトケのみ
#define ITEM_SOKUNONOMI		( 186 )		// ソクノのみ
#define ITEM_RINDONOMI		( 187 )		// リンドのみ
#define ITEM_YATHENOMI		( 188 )		// ヤチェのみ
#define ITEM_YOPUNOMI		( 189 )		// ヨプのみ
#define ITEM_BIAANOMI		( 190 )		// ビアーのみ
#define ITEM_SYUKANOMI		( 191 )		// シュカのみ
#define ITEM_BAKOUNOMI		( 192 )		// バコウのみ
#define ITEM_UTANNOMI		( 193 )		// ウタンのみ
#define ITEM_TANGANOMI		( 194 )		// タンガのみ
#define ITEM_YOROGINOMI		( 195 )		// ヨロギのみ
#define ITEM_KASIBUNOMI		( 196 )		// カシブのみ
#define ITEM_HABANNOMI		( 197 )		// ハバンのみ
#define ITEM_NAMONOMI		( 198 )		// ナモのみ
#define ITEM_RIRIBANOMI		( 199 )		// リリバのみ
#define ITEM_HOZUNOMI		( 200 )		// ホズのみ
#define ITEM_TIIRANOMI		( 201 )		// チイラのみ
#define ITEM_RYUGANOMI		( 202 )		// リュガのみ
#define ITEM_KAMURANOMI		( 203 )		// カムラのみ
#define ITEM_YATAPINOMI		( 204 )		// ヤタピのみ
#define ITEM_ZUANOMI		( 205 )		// ズアのみ
#define ITEM_SANNOMI		( 206 )		// サンのみ
#define ITEM_SUTAANOMI		( 207 )		// スターのみ
#define ITEM_NAZONOMI		( 208 )		// ナゾのみ
#define ITEM_MIKURUNOMI		( 209 )		// ミクルのみ
#define ITEM_IBANNOMI		( 210 )		// イバンのみ
#define ITEM_ZYAPONOMI		( 211 )		// ジャポのみ
#define ITEM_RENBUNOMI		( 212 )		// レンブのみ
#define ITEM_HIKARINOKONA		( 213 )		// ひかりのこな
#define ITEM_SIROIHAABU		( 214 )		// しろいハーブ
#define ITEM_KYOUSEIGIPUSU		( 215 )		// きょうせいギプス
#define ITEM_GAKUSYUUSOUTI		( 216 )		// がくしゅうそうち
#define ITEM_SENSEINOTUME		( 217 )		// せんせいのツメ
#define ITEM_YASURAGINOSUZU		( 218 )		// やすらぎのすず
#define ITEM_MENTARUHAABU		( 219 )		// メンタルハーブ
#define ITEM_KODAWARIHATIMAKI		( 220 )		// こだわりハチマキ
#define ITEM_OUZYANOSIRUSI		( 221 )		// おうじゃのしるし
#define ITEM_GINNOKONA		( 222 )		// ぎんのこな
#define ITEM_OMAMORIKOBAN		( 223 )		// おまもりこばん
#define ITEM_KIYOMENOOHUDA		( 224 )		// きよめのおふだ
#define ITEM_KOKORONOSIZUKU		( 225 )		// こころのしずく
#define ITEM_SINKAINOKIBA		( 226 )		// しんかいのキバ
#define ITEM_SINKAINOUROKO		( 227 )		// しんかいのウロコ
#define ITEM_KEMURIDAMA		( 228 )		// けむりだま
#define ITEM_KAWARAZUNOISI		( 229 )		// かわらずのいし
#define ITEM_KIAINOHATIMAKI		( 230 )		// きあいのハチマキ
#define ITEM_SIAWASETAMAGO		( 231 )		// しあわせタマゴ
#define ITEM_PINTORENZU		( 232 )		// ピントレンズ
#define ITEM_METARUKOOTO		( 233 )		// メタルコート
#define ITEM_TABENOKOSI		( 234 )		// たべのこし
#define ITEM_RYUUNOUROKO		( 235 )		// りゅうのウロコ
#define ITEM_DENKIDAMA		( 236 )		// でんきだま
#define ITEM_YAWARAKAISUNA		( 237 )		// やわらかいすな
#define ITEM_KATAIISI		( 238 )		// かたいいし
#define ITEM_KISEKINOTANE		( 239 )		// きせきのタネ
#define ITEM_KUROIMEGANE		( 240 )		// くろいメガネ
#define ITEM_KUROOBI		( 241 )		// くろおび
#define ITEM_ZISYAKU		( 242 )		// じしゃく
#define ITEM_SINPINOSIZUKU		( 243 )		// しんぴのしずく
#define ITEM_SURUDOIKUTIBASI		( 244 )		// するどいくちばし
#define ITEM_DOKUBARI		( 245 )		// どくバリ
#define ITEM_TOKENAIKOORI		( 246 )		// とけないこおり
#define ITEM_NOROINOOHUDA		( 247 )		// のろいのおふだ
#define ITEM_MAGATTASUPUUN		( 248 )		// まがったスプーン
#define ITEM_MOKUTAN		( 249 )		// もくたん
#define ITEM_RYUUNOKIBA		( 250 )		// りゅうのキバ
#define ITEM_SIRUKUNOSUKAAHU		( 251 )		// シルクのスカーフ
#define ITEM_APPUGUREEDO		( 252 )		// アップグレード
#define ITEM_KAIGARANOSUZU		( 253 )		// かいがらのすず
#define ITEM_USIONOOKOU		( 254 )		// うしおのおこう
#define ITEM_NONKINOOKOU		( 255 )		// のんきのおこう
#define ITEM_RAKKIIPANTI		( 256 )		// ラッキーパンチ
#define ITEM_METARUPAUDAA		( 257 )		// メタルパウダー
#define ITEM_HUTOIHONE		( 258 )		// ふといホネ
#define ITEM_NAGANEGI		( 259 )		// ながねぎ
#define ITEM_AKAIBANDANA		( 260 )		// あかいバンダナ
#define ITEM_AOIBANDANA		( 261 )		// あおいバンダナ
#define ITEM_PINKUNOBANDANA		( 262 )		// ピンクのバンダナ
#define ITEM_MIDORINOBANDANA		( 263 )		// みどりのバンダナ
#define ITEM_KIIRONOBANDANA		( 264 )		// きいろのバンダナ
#define ITEM_KOUKAKURENZU		( 265 )		// こうかくレンズ
#define ITEM_TIKARANOHATIMAKI		( 266 )		// ちからのハチマキ
#define ITEM_MONOSIRIMEGANE		( 267 )		// ものしりメガネ
#define ITEM_TATUZINNOOBI		( 268 )		// たつじんのおび
#define ITEM_HIKARINONENDO		( 269 )		// ひかりのねんど
#define ITEM_INOTINOTAMA		( 270 )		// いのちのたま
#define ITEM_PAWAHURUHAABU		( 271 )		// パワフルハーブ
#define ITEM_DOKUDOKUDAMA		( 272 )		// どくどくだま
#define ITEM_KAENDAMA		( 273 )		// かえんだま
#define ITEM_SUPIIDOPAUDAA		( 274 )		// スピードパウダー
#define ITEM_KIAINOTASUKI		( 275 )		// きあいのタスキ
#define ITEM_FOOKASURENZU		( 276 )		// フォーカスレンズ
#define ITEM_METORONOOMU		( 277 )		// メトロノーム
#define ITEM_KUROITEKKYUU		( 278 )		// くろいてっきゅう
#define ITEM_KOUKOUNOSIPPO		( 279 )		// こうこうのしっぽ
#define ITEM_AKAIITO		( 280 )		// あかいいと
#define ITEM_KUROIHEDORO		( 281 )		// くろいヘドロ
#define ITEM_TUMETAIIWA		( 282 )		// つめたいいわ
#define ITEM_SARASARAIWA		( 283 )		// さらさらいわ
#define ITEM_ATUIIWA		( 284 )		// あついいわ
#define ITEM_SIMETTAIWA		( 285 )		// しめったいわ
#define ITEM_NEBARINOKAGIDUME		( 286 )		// ねばりのかぎづめ
#define ITEM_KODAWARISUKAAHU		( 287 )		// こだわりスカーフ
#define ITEM_KUTTUKIBARI		( 288 )		// くっつきバリ
#define ITEM_PAWAARISUTO		( 289 )		// パワーリスト
#define ITEM_PAWAABERUTO		( 290 )		// パワーベルト
#define ITEM_PAWAARENZU		( 291 )		// パワーレンズ
#define ITEM_PAWAABANDO		( 292 )		// パワーバンド
#define ITEM_PAWAAANKURU		( 293 )		// パワーアンクル
#define ITEM_PAWAAUEITO		( 294 )		// パワーウエイト
#define ITEM_KIREINANUKEGARA		( 295 )		// きれいなぬけがら
#define ITEM_OOKINANEKKO		( 296 )		// おおきなねっこ
#define ITEM_KODAWARIMEGANE		( 297 )		// こだわりメガネ
#define ITEM_HINOTAMAPUREETO		( 298 )		// ひのたまプレート
#define ITEM_SIZUKUPUREETO		( 299 )		// しずくプレート
#define ITEM_IKAZUTIPUREETO		( 300 )		// いかずちプレート
#define ITEM_MIDORINOPUREETO		( 301 )		// みどりのプレート
#define ITEM_TURARANOPUREETO		( 302 )		// つららのプレート
#define ITEM_KOBUSINOPUREETO		( 303 )		// こぶしのプレート
#define ITEM_MOUDOKUPUREETO		( 304 )		// もうどくプレート
#define ITEM_DAITINOPUREETO		( 305 )		// だいちのプレート
#define ITEM_AOZORAPUREETO		( 306 )		// あおぞらプレート
#define ITEM_HUSIGINOPUREETO		( 307 )		// ふしぎのプレート
#define ITEM_TAMAMUSIPUREETO		( 308 )		// たまむしプレート
#define ITEM_GANSEKIPUREETO		( 309 )		// がんせきプレート
#define ITEM_MONONOKEPUREETO		( 310 )		// もののけプレート
#define ITEM_RYUUNOPUREETO		( 311 )		// りゅうのプレート
#define ITEM_KOWAMOTEPUREETO		( 312 )		// こわもてプレート
#define ITEM_KOUTETUPUREETO		( 313 )		// こうてつプレート
#define ITEM_AYASIIOKOU		( 314 )		// あやしいおこう
#define ITEM_GANSEKIOKOU		( 315 )		// がんせきおこう
#define ITEM_MANPUKUOKOU		( 316 )		// まんぷくおこう
#define ITEM_SAZANAMINOOKOU		( 317 )		// さざなみのおこう
#define ITEM_OHANANOOKOU		( 318 )		// おはなのおこう
#define ITEM_KOUUNNOOKOU		( 319 )		// こううんのおこう
#define ITEM_KIYOMENOOKOU		( 320 )		// きよめのおこう
#define ITEM_PUROTEKUTAA		( 321 )		// プロテクター
#define ITEM_EREKIBUUSUTAA		( 322 )		// エレキブースター
#define ITEM_MAGUMABUUSUTAA		( 323 )		// マグマブースター
#define ITEM_AYASIIPATTI		( 324 )		// あやしいパッチ
#define ITEM_REIKAINONUNO		( 325 )		// れいかいのぬの
#define ITEM_SURUDOITUME		( 326 )		// するどいツメ
#define ITEM_SURUDOIKIBA		( 327 )		// するどいキバ
#define ITEM_WAZAMASIN01		( 328 )		// わざマシン０１
#define ITEM_WAZAMASIN02		( 329 )		// わざマシン０２
#define ITEM_WAZAMASIN03		( 330 )		// わざマシン０３
#define ITEM_WAZAMASIN04		( 331 )		// わざマシン０４
#define ITEM_WAZAMASIN05		( 332 )		// わざマシン０５
#define ITEM_WAZAMASIN06		( 333 )		// わざマシン０６
#define ITEM_WAZAMASIN07		( 334 )		// わざマシン０７
#define ITEM_WAZAMASIN08		( 335 )		// わざマシン０８
#define ITEM_WAZAMASIN09		( 336 )		// わざマシン０９
#define ITEM_WAZAMASIN10		( 337 )		// わざマシン１０
#define ITEM_WAZAMASIN11		( 338 )		// わざマシン１１
#define ITEM_WAZAMASIN12		( 339 )		// わざマシン１２
#define ITEM_WAZAMASIN13		( 340 )		// わざマシン１３
#define ITEM_WAZAMASIN14		( 341 )		// わざマシン１４
#define ITEM_WAZAMASIN15		( 342 )		// わざマシン１５
#define ITEM_WAZAMASIN16		( 343 )		// わざマシン１６
#define ITEM_WAZAMASIN17		( 344 )		// わざマシン１７
#define ITEM_WAZAMASIN18		( 345 )		// わざマシン１８
#define ITEM_WAZAMASIN19		( 346 )		// わざマシン１９
#define ITEM_WAZAMASIN20		( 347 )		// わざマシン２０
#define ITEM_WAZAMASIN21		( 348 )		// わざマシン２１
#define ITEM_WAZAMASIN22		( 349 )		// わざマシン２２
#define ITEM_WAZAMASIN23		( 350 )		// わざマシン２３
#define ITEM_WAZAMASIN24		( 351 )		// わざマシン２４
#define ITEM_WAZAMASIN25		( 352 )		// わざマシン２５
#define ITEM_WAZAMASIN26		( 353 )		// わざマシン２６
#define ITEM_WAZAMASIN27		( 354 )		// わざマシン２７
#define ITEM_WAZAMASIN28		( 355 )		// わざマシン２８
#define ITEM_WAZAMASIN29		( 356 )		// わざマシン２９
#define ITEM_WAZAMASIN30		( 357 )		// わざマシン３０
#define ITEM_WAZAMASIN31		( 358 )		// わざマシン３１
#define ITEM_WAZAMASIN32		( 359 )		// わざマシン３２
#define ITEM_WAZAMASIN33		( 360 )		// わざマシン３３
#define ITEM_WAZAMASIN34		( 361 )		// わざマシン３４
#define ITEM_WAZAMASIN35		( 362 )		// わざマシン３５
#define ITEM_WAZAMASIN36		( 363 )		// わざマシン３６
#define ITEM_WAZAMASIN37		( 364 )		// わざマシン３７
#define ITEM_WAZAMASIN38		( 365 )		// わざマシン３８
#define ITEM_WAZAMASIN39		( 366 )		// わざマシン３９
#define ITEM_WAZAMASIN40		( 367 )		// わざマシン４０
#define ITEM_WAZAMASIN41		( 368 )		// わざマシン４１
#define ITEM_WAZAMASIN42		( 369 )		// わざマシン４２
#define ITEM_WAZAMASIN43		( 370 )		// わざマシン４３
#define ITEM_WAZAMASIN44		( 371 )		// わざマシン４４
#define ITEM_WAZAMASIN45		( 372 )		// わざマシン４５
#define ITEM_WAZAMASIN46		( 373 )		// わざマシン４６
#define ITEM_WAZAMASIN47		( 374 )		// わざマシン４７
#define ITEM_WAZAMASIN48		( 375 )		// わざマシン４８
#define ITEM_WAZAMASIN49		( 376 )		// わざマシン４９
#define ITEM_WAZAMASIN50		( 377 )		// わざマシン５０
#define ITEM_WAZAMASIN51		( 378 )		// わざマシン５１
#define ITEM_WAZAMASIN52		( 379 )		// わざマシン５２
#define ITEM_WAZAMASIN53		( 380 )		// わざマシン５３
#define ITEM_WAZAMASIN54		( 381 )		// わざマシン５４
#define ITEM_WAZAMASIN55		( 382 )		// わざマシン５５
#define ITEM_WAZAMASIN56		( 383 )		// わざマシン５６
#define ITEM_WAZAMASIN57		( 384 )		// わざマシン５７
#define ITEM_WAZAMASIN58		( 385 )		// わざマシン５８
#define ITEM_WAZAMASIN59		( 386 )		// わざマシン５９
#define ITEM_WAZAMASIN60		( 387 )		// わざマシン６０
#define ITEM_WAZAMASIN61		( 388 )		// わざマシン６１
#define ITEM_WAZAMASIN62		( 389 )		// わざマシン６２
#define ITEM_WAZAMASIN63		( 390 )		// わざマシン６３
#define ITEM_WAZAMASIN64		( 391 )		// わざマシン６４
#define ITEM_WAZAMASIN65		( 392 )		// わざマシン６５
#define ITEM_WAZAMASIN66		( 393 )		// わざマシン６６
#define ITEM_WAZAMASIN67		( 394 )		// わざマシン６７
#define ITEM_WAZAMASIN68		( 395 )		// わざマシン６８
#define ITEM_WAZAMASIN69		( 396 )		// わざマシン６９
#define ITEM_WAZAMASIN70		( 397 )		// わざマシン７０
#define ITEM_WAZAMASIN71		( 398 )		// わざマシン７１
#define ITEM_WAZAMASIN72		( 399 )		// わざマシン７２
#define ITEM_WAZAMASIN73		( 400 )		// わざマシン７３
#define ITEM_WAZAMASIN74		( 401 )		// わざマシン７４
#define ITEM_WAZAMASIN75		( 402 )		// わざマシン７５
#define ITEM_WAZAMASIN76		( 403 )		// わざマシン７６
#define ITEM_WAZAMASIN77		( 404 )		// わざマシン７７
#define ITEM_WAZAMASIN78		( 405 )		// わざマシン７８
#define ITEM_WAZAMASIN79		( 406 )		// わざマシン７９
#define ITEM_WAZAMASIN80		( 407 )		// わざマシン８０
#define ITEM_WAZAMASIN81		( 408 )		// わざマシン８１
#define ITEM_WAZAMASIN82		( 409 )		// わざマシン８２
#define ITEM_WAZAMASIN83		( 410 )		// わざマシン８３
#define ITEM_WAZAMASIN84		( 411 )		// わざマシン８４
#define ITEM_WAZAMASIN85		( 412 )		// わざマシン８５
#define ITEM_WAZAMASIN86		( 413 )		// わざマシン８６
#define ITEM_WAZAMASIN87		( 414 )		// わざマシン８７
#define ITEM_WAZAMASIN88		( 415 )		// わざマシン８８
#define ITEM_WAZAMASIN89		( 416 )		// わざマシン８９
#define ITEM_WAZAMASIN90		( 417 )		// わざマシン９０
#define ITEM_WAZAMASIN91		( 418 )		// わざマシン９１
#define ITEM_WAZAMASIN92		( 419 )		// わざマシン９２
#define ITEM_HIDENMASIN01		( 420 )		// ひでんマシン０１
#define ITEM_HIDENMASIN02		( 421 )		// ひでんマシン０２
#define ITEM_HIDENMASIN03		( 422 )		// ひでんマシン０３
#define ITEM_HIDENMASIN04		( 423 )		// ひでんマシン０４
#define ITEM_HIDENMASIN05		( 424 )		// ひでんマシン０５
#define ITEM_HIDENMASIN06		( 425 )		// ひでんマシン０６
#define ITEM_HIDENMASIN07		( 426 )		// ひでんマシン０７
#define ITEM_HIDENMASIN08		( 427 )		// ひでんマシン０８
#define ITEM_TANKENSETTO		( 428 )		// たんけんセット
#define ITEM_TAKARABUKURO		( 429 )		// たからぶくろ
#define ITEM_RUURUBUKKU		( 430 )		// ルールブック
#define ITEM_POKETORE		( 431 )		// ポケトレ
#define ITEM_POINTOKAADO		( 432 )		// ポイントカード
#define ITEM_BOUKENNOOTO		( 433 )		// ぼうけんノート
#define ITEM_SIIRUIRE		( 434 )		// シールいれ
#define ITEM_AKUSESARIIIRE		( 435 )		// アクセサリーいれ
#define ITEM_SIIRUBUKURO		( 436 )		// シールぶくろ
#define ITEM_TOMODATITETYOU		( 437 )		// ともだちてちょう
#define ITEM_HATUDENSYOKII		( 438 )		// はつでんしょキー
#define ITEM_KODAINOOMAMORI		( 439 )		// こだいのおまもり
#define ITEM_GINGADANNOKAGI		( 440 )		// ギンガだんのカギ
#define ITEM_AKAIKUSARI		( 441 )		// あかいくさり
#define ITEM_TAUNMAPPU		( 442 )		// タウンマップ
#define ITEM_BATORUSAATYAA		( 443 )		// バトルサーチャー
#define ITEM_KOINKEESU		( 444 )		// コインケース
#define ITEM_BORONOTURIZAO		( 445 )		// ボロのつりざお
#define ITEM_IITURIZAO		( 446 )		// いいつりざお
#define ITEM_SUGOITURIZAO		( 447 )		// すごいつりざお
#define ITEM_KODAKKUZYOURO		( 448 )		// コダックじょうろ
#define ITEM_POFINKEESU		( 449 )		// ポフィンケース
#define ITEM_ZITENSYA		( 450 )		// じてんしゃ
#define ITEM_RUUMUKII		( 451 )		// ルームキー
#define ITEM_OOKIDONOTEGAMI		( 452 )		// オーキドのてがみ
#define ITEM_MIKADUKINOHANE		( 453 )		// みかづきのはね
#define ITEM_MENBAAZUKAADO		( 454 )		// メンバーズカード
#define ITEM_TENKAINOHUE		( 455 )		// てんかいのふえ
#define ITEM_HUNENOTIKETTO		( 456 )		// ふねのチケット
#define ITEM_KONTESUTOPASU		( 457 )		// コンテストパス
#define ITEM_KAZANNOOKIISI		( 458 )		// かざんのおきいし
#define ITEM_OTODOKEMONO		( 459 )		// おとどけもの
#define ITEM_HIKIKAEKEN1		( 460 )		// ひきかえけん１
#define ITEM_HIKIKAEKEN2		( 461 )		// ひきかえけん２
#define ITEM_HIKIKAEKEN3		( 462 )		// ひきかえけん３
#define ITEM_SOUKONOKAGI		( 463 )		// そうこのカギ
#define ITEM_HIDENNOKUSURI		( 464 )		// ひでんのくすり
#define ITEM_BATORUREKOODAA		( 465 )		// バトルレコーダー
#define ITEM_GURASIDEANOHANA		( 466 )		// グラシデアのはな
#define ITEM_HIMITUNOKAGI		( 467 )		// ひみつのカギ
#define ITEM_BONGURIKEESU		( 468 )		// ぼんぐりケース
#define ITEM_ANNOONNOOTO		( 469 )		// アンノーンノート
#define ITEM_KINOMIPURANTAA		( 470 )		// きのみプランター
#define ITEM_DAUZINGUMASIN		( 471 )		// ダウジングマシン
#define ITEM_BURUUKAADO		( 472 )		// ブルーカード
#define ITEM_OISIISIPPO		( 473 )		// おいしいシッポ
#define ITEM_TOUMEINASUZU		( 474 )		// とうめいなスズ
#define ITEM_KAADOKII		( 475 )		// カードキー
#define ITEM_TIKANOKAGI		( 476 )		// ちかのかぎ
#define ITEM_ZENIGAMEZYOURO		( 477 )		// ゼニガメじょうろ
#define ITEM_AKAIUROKO		( 478 )		// あかいウロコ
#define ITEM_OTOSIMONO		( 479 )		// おとしもの
#define ITEM_RINIAPASU		( 480 )		// リニアパス
#define ITEM_KIKAINOBUHIN		( 481 )		// きかいのぶひん
#define ITEM_GINIRONOHANE		( 482 )		// ぎんいろのはね
#define ITEM_NIZIIRONOHANE		( 483 )		// にじいろのはね
#define ITEM_HUSIGINATAMAGO		( 484 )		// ふしぎなタマゴ
#define ITEM_AKABONGURI		( 485 )		// あかぼんぐり
#define ITEM_AOBONGURI		( 486 )		// あおぼんぐり
#define ITEM_KIBONGURI		( 487 )		// きぼんぐり
#define ITEM_MIDOBONGURI		( 488 )		// みどぼんぐり
#define ITEM_MOMOBONGURI		( 489 )		// ももぼんぐり
#define ITEM_SIROBONGURI		( 490 )		// しろぼんぐり
#define ITEM_KUROBONGURI		( 491 )		// くろぼんぐり
#define ITEM_SUPIIDOBOORU		( 492 )		// スピードボール
#define ITEM_REBERUBOORU		( 493 )		// レベルボール
#define ITEM_RUAABOORU		( 494 )		// ルアーボール
#define ITEM_HEBIIBOORU		( 495 )		// ヘビーボール
#define ITEM_RABURABUBOORU		( 496 )		// ラブラブボール
#define ITEM_HURENDOBOORU		( 497 )		// フレンドボール
#define ITEM_MUUNBOORU		( 498 )		// ムーンボール
#define ITEM_KONPEBOORU		( 499 )		// コンペボール
#define ITEM_PAAKUBOORU		( 500 )		// パークボール
#define ITEM_FOTOARUBAMU		( 501 )		// フォトアルバム
#define ITEM_gbPUREIYAA		( 502 )		// ＧＢプレイヤー
#define ITEM_UMINARINOSUZU		( 503 )		// うみなりのスズ
#define ITEM_IKARIMANZYUU		( 504 )		// いかりまんじゅう
#define ITEM_DEETAKAADO01		( 505 )		// データカード０１
#define ITEM_DEETAKAADO02		( 506 )		// データカード０２
#define ITEM_DEETAKAADO03		( 507 )		// データカード０３
#define ITEM_DEETAKAADO04		( 508 )		// データカード０４
#define ITEM_DEETAKAADO05		( 509 )		// データカード０５
#define ITEM_DEETAKAADO06		( 510 )		// データカード０６
#define ITEM_DEETAKAADO07		( 511 )		// データカード０７
#define ITEM_DEETAKAADO08		( 512 )		// データカード０８
#define ITEM_DEETAKAADO09		( 513 )		// データカード０９
#define ITEM_DEETAKAADO10		( 514 )		// データカード１０
#define ITEM_DEETAKAADO11		( 515 )		// データカード１１
#define ITEM_DEETAKAADO12		( 516 )		// データカード１２
#define ITEM_DEETAKAADO13		( 517 )		// データカード１３
#define ITEM_DEETAKAADO14		( 518 )		// データカード１４
#define ITEM_DEETAKAADO15		( 519 )		// データカード１５
#define ITEM_DEETAKAADO16		( 520 )		// データカード１６
#define ITEM_DEETAKAADO17		( 521 )		// データカード１７
#define ITEM_DEETAKAADO18		( 522 )		// データカード１８
#define ITEM_DEETAKAADO19		( 523 )		// データカード１９
#define ITEM_DEETAKAADO20		( 524 )		// データカード２０
#define ITEM_DEETAKAADO21		( 525 )		// データカード２１
#define ITEM_DEETAKAADO22		( 526 )		// データカード２２
#define ITEM_DEETAKAADO23		( 527 )		// データカード２３
#define ITEM_DEETAKAADO24		( 528 )		// データカード２４
#define ITEM_DEETAKAADO25		( 529 )		// データカード２５
#define ITEM_DEETAKAADO26		( 530 )		// データカード２６
#define ITEM_DEETAKAADO27		( 531 )		// データカード２７
#define ITEM_MOEGIIRONOTAMA		( 532 )		// もえぎいろのたま
#define ITEM_ROKKUKAPUSERU		( 533 )		// ロックカプセル
#define ITEM_BENIIRONOTAMA		( 534 )		// べにいろのたま
#define ITEM_AIIRONOTAMA		( 535 )		// あいいろのたま
#define ITEM_NAZONOSUISYOU		( 536 )		// なぞのすいしょう
#define ITEM_PARESUHEGOO		( 537 )		// パレスへゴー
#define ITEM_KURITHIKATTO2		( 538 )		// クリティカット２
#define ITEM_SUPIIDAA2		( 539 )		// スピーダー２
#define ITEM_spAPPU2		( 540 )		// ＳＰアップ２
#define ITEM_spGAADO2		( 541 )		// ＳＰガード２
#define ITEM_DHIFENDAA2		( 542 )		// ディフェンダー２
#define ITEM_PURASUPAWAA2		( 543 )		// プラスパワー２
#define ITEM_YOKUATAARU2		( 544 )		// ヨクアタール２
#define ITEM_SUPIIDAA3		( 545 )		// スピーダー３
#define ITEM_spAPPU3		( 546 )		// ＳＰアップ３
#define ITEM_spGAADO3		( 547 )		// ＳＰガード３
#define ITEM_DHIFENDAA3		( 548 )		// ディフェンダー３
#define ITEM_PURASUPAWAA3		( 549 )		// プラスパワー３
#define ITEM_YOKUATAARU3		( 550 )		// ヨクアタール３
#define ITEM_SUPIIDAA6		( 551 )		// スピーダー６
#define ITEM_spAPPU6		( 552 )		// ＳＰアップ６
#define ITEM_spGAADO6		( 553 )		// ＳＰガード６
#define ITEM_DHIFENDAA6		( 554 )		// ディフェンダー６
#define ITEM_PURASUPAWAA6		( 555 )		// プラスパワー６
#define ITEM_YOKUATAARU6		( 556 )		// ヨクアタール６
#define ITEM_SUKIRUKOORU		( 557 )		// スキルコール
#define ITEM_AITEMUDOROPPU		( 558 )		// アイテムドロップ
#define ITEM_AITEMUKOORU		( 559 )		// アイテムコール
#define ITEM_HURATTOKOORU		( 560 )		// フラットコール
#define ITEM_KURITHIKATTO3		( 561 )		// クリティカット３
#define ITEM_KARUISI		( 562 )		// かるいし
#define ITEM_SINKANOKISEKI		( 563 )		// しんかのきせき
#define ITEM_GOTUGOTUMETTO		( 564 )		// ゴツゴツメット
#define ITEM_HUUSEN		( 565 )		// ふうせん
#define ITEM_REDDOKAADO		( 566 )		// レッドカード
#define ITEM_NERAINOMATO		( 567 )		// ねらいのまと
#define ITEM_SIMETUKEBANDO		( 568 )		// しめつけバンド
#define ITEM_KYUUKON		( 569 )		// きゅうこん
#define ITEM_ZYUUDENTI		( 570 )		// じゅうでんち
#define ITEM_DASYUTUPODDO		( 571 )		// だしゅつポッド
#define ITEM_HONOONOZYUERU		( 572 )		// ほのおのジュエル
#define ITEM_MIZUNOZYUERU		( 573 )		// みずのジュエル
#define ITEM_DENKINOZYUERU		( 574 )		// でんきのジュエル
#define ITEM_KUSANOZYUERU		( 575 )		// くさのジュエル
#define ITEM_KOORINOZYUERU		( 576 )		// こおりのジュエル
#define ITEM_KAKUTOUZYUERU		( 577 )		// かくとうジュエル
#define ITEM_DOKUNOZYUERU		( 578 )		// どくのジュエル
#define ITEM_ZIMENNOZYUERU		( 579 )		// じめんのジュエル
#define ITEM_HIKOUNOZYUERU		( 580 )		// ひこうのジュエル
#define ITEM_ESUPAAZYUERU		( 581 )		// エスパージュエル
#define ITEM_MUSINOZYUERU		( 582 )		// むしのジュエル
#define ITEM_IWANOZYUERU		( 583 )		// いわのジュエル
#define ITEM_GOOSUTOZYUERU		( 584 )		// ゴーストジュエル
#define ITEM_DORAGONZYUERU		( 585 )		// ドラゴンジュエル
#define ITEM_AKUNOZYUERU		( 586 )		// あくのジュエル
#define ITEM_HAGANENOZYUERU		( 587 )		// はがねのジュエル
#define ITEM_NOOMARUZYUERU		( 588 )		// ノーマルジュエル
#define ITEM_TAIRYOKUNOHANE		( 589 )		// たいりょくのハネ
#define ITEM_KINRYOKUNOHANE		( 590 )		// きんりょくのハネ
#define ITEM_TEIKOUNOHANE		( 591 )		// ていこうのハネ
#define ITEM_TIRYOKUNOHANE		( 592 )		// ちりょくのハネ
#define ITEM_SEISINNOHANE		( 593 )		// せいしんのハネ
#define ITEM_SYUNPATUNOHANE		( 594 )		// しゅんぱつのハネ
#define ITEM_HAATONOHANE		( 595 )		// ハートのハネ

#define ITEM_DATA_MAX		( 595 )		// アイテム最大数

#define ITEM_MAIL_MAX	( 12 )	// メール数
#define ITEM_NUTS_MAX	( 64 )	// きのみ数

#endif // ITEMSYM_H
