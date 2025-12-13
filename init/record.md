# init memo

## 目的
- A-4/B-2/C-1: サーバー起動 → クライアント接続・認証コマンド受信 → Operation経由でClientを管理マップへ登録まで通す。

## 実装順序メモ
1. Serverにソケット初期化を追加: socket → setsockopt(SO_REUSEADDR) → fcntl(O_NONBLOCK) → bind → listen。エラー時はperror＋exit。
2. pollループをmain/Serverに実装: pollでPOLLIN監視、listen_fdならaccept、その他はrecvへ。EWOULDBLOCK/EAGAINは継続、0バイトやエラーで切断。
3. accept後の管理: Client生成し_client_mapとpfdsに追加。切断時はcloseしてpfds/マップから削除、Client破棄。
4. Clientのバッファ処理: appendReceiveBufferで蓄積、extractCommandで\r\n区切りの1行を返す。recv後はwhileでextractしてhandleClientMessageへ渡す。
5. Operation連携: handleClientMessageでOperationを生成しCommandFuncを取得、Data&（clients/channels管理）を渡す形に揃える。CommandFuncシグネチャは`void func(Client*, Operation&, Data&)`を想定。

## 現状の空実装
- Server, Client, Channel, Operation, main, nickコマンドが骨格のみ。ネットワーク層とバッファ処理を最初に埋める。

## 作業の注意
- ノンブロッキングI/O前提。部分送受信・切断処理を優先して安定化。
- バラでテスト: ネットワーク処理と文字列パースは単体で検証してから結合。

## 進捗メモ
- `Server`にlisten fd/port/password/pollfdsを保持するソケット初期化を実装。mainでポート/パスワードを受け取り初期化のみ実行可能。
- コマンド系はスタブ化（nick/user/...）。今は起動確認用の空実装。
- pollループを追加し、accept/recvでクライアントを管理。`Client`にバッファ処理と簡易`sendMessage`を実装し、mainは`server.run()`で待受する形に更新。
