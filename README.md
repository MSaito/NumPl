NumPl
=====
Number Place Problem Generator

ナンプレ問題作成プログラム
このプログラムは、<a href="http://manieth.hatenablog.com/">メニエスの名のもとに</a>
でぐだぐだと関連することなどを書きながら開発しました。
UNIX風環境下で gcc で make して動作するはずです。
開発環境は OS X + MacPorts

実行ファイル

generate 問題を生成する

* generate [-v] [-S] [-c number] [-s seed] [-hltfy]
 *	--verbose, -v 冗長モード、このモードでは1問しか問題を作成しない。
 *	--symmetric, -S 点対称の問題を作成する。
 *	--count, -c 出力する問題の数
 *	--seed, -s 使用する疑似乱数の初期化数字
 *	--hidden, -h hidden_singe の解法を含む問題を出力する。
 *	--locked, -l locked_candidates の解法を含む問題を出力する。
 *	--tuple, -t naked pair, hidden pair などの解法を含む問題を出力する。
 * 	--fish, -f x-wing, swordfishなどの解法を含む問題を出力する。
 *	--xywing, -y xy-wingの解法を含む問題を出力する。
* generate の出力の前半は解の情報、後半はナンプレの問題、
ただし（なんちゃって）正規化されているので、convert によってランダム化した方がよい。

convert 行や列、数字を入れ換え問題の見た目を変更する

* convert [-v] [-s seed] [puzzle_string]
 *	--verbose, -v 解法についての情報付きで出力する。
 *	--seed, -s 疑似乱数の初期化数値
 *	puzzle_string 変換する問題の文字列、コマンドラインで指定されなければ、
	標準入力から読み込む。

analyze　手持ちの解法で解けるか調べる

* analyze [-s] [puzzle_string]
 *	--silent -s 余分な情報を出力しない。
 *	puzzle_string 変換する問題の文字列、コマンドラインで指定されなければ、
	標準入力から読み込む。

make doc でdoxygen によるドキュメントが作成されます。
