#include "../src/interception.hpp"
#include <deque>

extern "C" {
    #include "../interception/library/interception.c"
}

enum KeyCode {
    CTRL = 0x1D,
    ALT = 0x38,
};

/*
    @MEMO Aula::Encoding::initialize() + Aula::IO::Stdout->write(...) は遅い
        ただしそれは UTF-8 => wstring 変換処理の分やや遅いだけで
        std::cout を使っても速度的にそこまで変わらなそうなので気にしなくても良さそう
        - 比較: ./interception_test.cpp
*/
__main() {
    /// Interception context 生成
    Aula::Interception::Context context;

    if (context.getState() == 0) {
        Aula::IO::Stderr->write(context.getMessage());
        return 1;
    }

    /// プロセス優先度：高
    Aula::Interception::setCurrentProcessPriority(Aula::Interception::ProcessPriority::HIGH);

    /// 入力デバイス列挙
    auto devices = context.enumerateDevices();

    for (auto device = devices.begin(); device != devices.end(); ++device) {
        Aula::IO::Stdout->write(
            Aula::Encoding::toString(device->index) + ": " + device->hardwareId
            + (device->isKeyboard ? " (Keyboard)" : (device->isMouse ? " (Mouse)" : ""))
        );
    }

    /// 入力フィルタ設定
    context.setKeyboardFilter(INTERCEPTION_FILTER_KEY_ALL);
    context.setMouseFilter(INTERCEPTION_FILTER_MOUSE_ALL - INTERCEPTION_FILTER_MOUSE_MOVE);

    /// キー入力履歴（同時押し用）
    std::deque<InterceptionKeyStroke> keyStrokes;
    InterceptionKeyStroke lastKeyStroke{0, 0};

    // 2つ同時押しに対応
    for (int i = 0; i < 2; ++i) keyStrokes.push_back({0, 0});

    while (context.recieve()) {
        auto keyStroke = context.getCurrentKeyStroke();
        auto mouseStroke = context.getCurrentMouseStroke();

        if (keyStroke && (keyStroke->code != lastKeyStroke.code || keyStroke->state != lastKeyStroke.state)) {
            keyStrokes.pop_front();
            keyStrokes.push_back({ keyStroke->code, keyStroke->state });
            for (auto it = keyStrokes.begin(); it != keyStrokes.end(); ++it) {
                Aula::IO::Stdout->write(Aula::Encoding::toString(it->code) + "," + Aula::Encoding::toString(it->state) + " ", false);
            }
            Aula::IO::Stdout->write(""); // \n

            lastKeyStroke.code = keyStroke->code;
            lastKeyStroke.state = keyStroke->state;
        }

        if (keyStrokes[0].code == CTRL && keyStrokes[0].state == INTERCEPTION_KEY_DOWN && keyStrokes[1].code == ALT && keyStrokes[1].state == INTERCEPTION_KEY_DOWN) {
            Aula::IO::Stdout->write("Ctrl + Alt pressed");
        }

        if (keyStroke) {
            // Aula::IO::Stdout->write(
            //     "Keyboard: " + context.getHardwareId() +
            //     "\n\tcode: " + Aula::Encoding::toString(keyStroke->code) +
            //     "\n\tstate: " + Aula::Encoding::toString(keyStroke->state) +
            //     "\n\tinformation: " + Aula::Encoding::toString(keyStroke->information)
            // );
            if (keyStroke->code == 1) break; // Escape で終了
        } else if (mouseStroke) {
            Aula::IO::Stdout->write(
                "Mouse: " + context.getHardwareId() +
                "\n\tstate: " + Aula::Encoding::toString(mouseStroke->state) +
                "\n\tflags: " + Aula::Encoding::toString(mouseStroke->flags) +
                "\n\trolling: " + Aula::Encoding::toString(mouseStroke->rolling) +
                "\n\tx: " + Aula::Encoding::toString(mouseStroke->x) +
                "\n\ty: " + Aula::Encoding::toString(mouseStroke->y) +
                "\n\tinformation: " + Aula::Encoding::toString(mouseStroke->information)
            );
        }
        context.pass(); // デバイス入力をそのまま送信してデフォルトの動作を行わせる
                        // ※ これを行わないとキーボード入力もマウス入力もできなくなってしまう
    }
    return 0;
}