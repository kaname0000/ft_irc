# 手動テストケースメモ

## CRLF 必須バージョン（現状）
- 目的: `extractCommand` が `\r\n` 終端のみを認識するため、CRLF 付きで送れば処理されることを確認。
- サーバ起動: `./ft_irc 6667 pass`
- テストコマンド（別ターミナルで実行、LF ではなく CRLF を使用）:
  ```
  printf 'PASS pass\r\nNICK alice\r\nUSER alice host server :Alice\r\n' | nc -v 127.0.0.1 6667 -w 1
  ```
  - `printf '...'` で PASS/NICK/USER を CRLF 区切りで生成
  - `| nc -v 127.0.0.1 6667` で 127.0.0.1:6667 に送信（-v は接続ログ表示）
  - `-w 1` で応答待ちタイムアウト 1 秒
- 期待出力:
  - 接続成功ログ
  - `:server.example.com 001 alice :Welcome ...` が返る

## LF のみバージョン（過去挙動）
- `extractCommand` を LF 許容にしていたときの確認用（現在は CRLF 専用に戻しているので参考記録）。
- コマンド:
  ```
  printf 'PASS pass\nNICK alice\nUSER alice host server :Alice\n' | nc -v 127.0.0.1 6667 -w 1
  ```
  - LF だけでも受理される実装時は 001 が返る。
  - 現在の CRLF 必須実装ではコマンドとして区切れず無応答→クライアント切断になる。
