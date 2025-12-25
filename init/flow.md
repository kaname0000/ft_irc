## 全体の呼び出しフロー（main から順に）
- `src/main.cpp`: `Server server(port, password);` → `server.run();`
- `Server::Server` → `initSocket()` (src/class_cpp/Server.cpp:15-20, 36-86)
  - ソケット生成、`SO_REUSEADDR` 設定、ノンブロッキング化、bind/listen、リッスンFDを `POLLIN` で `_pfds` に登録。
- `Server::run()` (Server.cpp:209-254) 無限ループ
  - 毎周回 `refreshPollEvents()` で各FDに `POLLIN/POLLOUT` を再セット。
  - `poll()` → イベントFDを走査。
    - リッスンFDかつ `POLLIN`: `acceptNewClient()` (Server.cpp:132-166)
      - `accept` を非ブロッキングで回し、新FDをノンブロッキング化 → `Client` を生成し `_clients` に追加 → `_pfds` に登録。
    - それ以外: `handleClientData(i)` (Server.cpp:168-207)
      - `recv` で `_receive_buffer` に貯め、`\\r\\n` で区切って `extractCommand()` → `handleClientMessage()` へ。
      - `recv` が 0 なら切断フラグ、`EAGAIN/EWOULDBLOCK` は終了扱い、`POLLHUP/POLLERR` も切断処理。
- `handleClientMessage()` (Server.cpp:256-265)
  - 1行コマンド文字列を `Operation` に変換（src/class_cpp/Operation.cpp）。
  - `Operation::getCommandFunc()` で取得した関数ポインタを実行（`nick`/`user`/`join` など）。各コマンド実装は `src/commad/*.cpp` に配置予定（未整備）。

この順で読むと「main → Server ctor → run → poll ループ → accept → handleClientData → handleClientMessage → 各コマンド」の一本道が追える。
