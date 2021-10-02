#include <windows.h>
#include <iostream>
#include <wchar.h>

#define INTERCEPTION_STATIC
#include "../interception/library/interception.h"

extern "C" {
    #include "../interception/library/interception.c"
}

using namespace std;

int main() {
    // 高優先度化
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

    //Interceptionのインスタンス生成
    auto context = interception_create_context();
    if (!context) {
        cout << "Failed to create Intercaption context" << endl;
        return 1;
    }
    interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_ALL);
    interception_set_filter(context, interception_is_mouse, INTERCEPTION_FILTER_MOUSE_ALL);

    // 入力を処理する
    InterceptionDevice device;
    InterceptionStroke stroke;
    while (interception_receive(context, device = interception_wait(context), &stroke, 1) > 0) {
        if (interception_is_keyboard(device)) {
            InterceptionKeyStroke& s = *(InterceptionKeyStroke*)&stroke;
            cout << "Keyboard Input "
                << "ScanCode=" << s.code
                << " State=" << s.state << endl;
        }
        else if (interception_is_mouse(device)) {
            InterceptionMouseStroke& s = *(InterceptionMouseStroke*)&stroke;
            cout << "Mouse Input"
                << " State=" << s.state
                << " Rolling=" << s.rolling
                << " Flags=" << s.flags
                << " (x,y)=(" << s.x << "," << s.y << ")"
                << endl;
        }
        
        // 全てのデバイスの入力を通過させる
        interception_send(context, device, &stroke, 1);
        if (interception_is_keyboard(device)) { // Escapeで終了
            InterceptionKeyStroke& s = *(InterceptionKeyStroke*)&stroke;
            if (s.code == 1) break;
        }
    }
    interception_destroy_context(context);
    return 0;
}