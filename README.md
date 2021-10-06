# aula-interception

[Interception](https://github.com/oblitum/Interception) module for [Aula Script Engine](https://github.com/amenoyoya/aula)

## Environment

- OS:
    - Windows 10
- Build tools:
    - Microsoft Visual C++ 2019 Community Edition
- Editor:
    - VSCode

### Setup: Interception
- **Interception**:
    - マウスやキーボード等の Human Interface Device (HID) の入力を傍受する仮想ドライバ
    - ドライバは、フリー版ではソースコード非公開のため、ドライバが安全かどうかは各自判断する必要がある
- Setup:
    - https://github.com/oblitum/Interception/releases/download/v1.0.1/Interception.zip をダウンロードして解凍する
    - 解凍したディレクトリ内でエクスプローラのアドレスバーに `powershell start-process '.\command line installer\install-interception.exe' -verb runas -argumentlist '/install'` と入力して実行
        - ![install-interception.png](./img/install-interception.png)
    - ドライバインストールを完了するために、PCを再起動
- アンインストール方法:
    - `powershell start-process '.\interception\command line installer\install-interception.exe' -verb runas -argumentlist '/uninstall'`

***

## Execution

```powershell
> aula.exe .\samples\interception.lua
```

***

## Tips

### Aula.Interception.sendKeyAction について
Rainbow Six Siege 等の FPS ゲームでは、チート対策のためか `Aula.Interception.sendKeyAction` (`Win32API.SendInput` のラッパー関数) のようなキー送信関数を使ってキーマッピングするとバグってしまう

そのため、こういったゲームで独自のキーマッピングを行いたい場合は `Aula.Interception.Context:sendKeyStroke` (`interception_send` のラッパーメソッド) のような低レベル API を利用する必要がある

- 参考: [samples/g600.lua](./samples/g600.lua)
