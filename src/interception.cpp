#include <aula/lua.hpp>
#include "interception.hpp"

extern "C" {
    #include "../interception/library/interception.c"

    __declspec(dllexport) int luaopen_interception(lua_State *L) {
        sol::state_view lua(L);

        auto aula = lua["Aula"].get_or_create<sol::table>();
        auto interception = aula["Interception"].get_or_create<sol::table>();

        #include "types_interception.hpp"
        #include "types_winapi.hpp"

        interception.new_enum("ProcessPriority",
            "IDLE", Aula::Interception::ProcessPriority::IDLE,
            "BELOW_NORMAL", Aula::Interception::ProcessPriority::BELOW_NORMAL,
            "NORMAL", Aula::Interception::ProcessPriority::NORMAL,
            "ABOVE_NORMAL", Aula::Interception::ProcessPriority::ABOVE_NORMAL,
            "HIGH", Aula::Interception::ProcessPriority::HIGH,
            "REALTIME", Aula::Interception::ProcessPriority::REALTIME
        );
        
        interception.set_function("setCurrentProcessPriority", Aula::Interception::setCurrentProcessPriority);

        interception.new_usertype<Aula::Interception::MouseStroke>("MouseStroke",
            sol::constructors<
                Aula::Interception::MouseStroke(),
                Aula::Interception::MouseStroke(u16, u16, i16, i32, i32, u32)
            >(),
            "state", &Aula::Interception::MouseStroke::state,
            "flags", &Aula::Interception::MouseStroke::flags,
            "rolling", &Aula::Interception::MouseStroke::rolling,
            "x", &Aula::Interception::MouseStroke::x,
            "y", &Aula::Interception::MouseStroke::y,
            "information", &Aula::Interception::MouseStroke::information
        );
        interception.new_usertype<Aula::Interception::KeyStroke>("KeyStroke",
            sol::constructors<
                Aula::Interception::KeyStroke(),
                Aula::Interception::KeyStroke(u16, u16, u32)
            >(),
            "code", &Aula::Interception::KeyStroke::code,
            "state", &Aula::Interception::KeyStroke::state,
            "information", &Aula::Interception::KeyStroke::information
        );

        interception.new_usertype<Aula::Interception::Device>("Device",
            "index", &Aula::Interception::Device::index,
            "hardwareId", &Aula::Interception::Device::hardwareId,
            "isKeyboard", &Aula::Interception::Device::isKeyboard,
            "isMouse", &Aula::Interception::Device::isMouse
        );

        interception.new_usertype<Aula::Interception::Context>("Context",
            sol::constructors<
                Aula::Interception::Context()
            >(),
            "getState", &Aula::Interception::Context::getState,
            "getMessage", &Aula::Interception::Context::getMessage,
            "open", &Aula::Interception::Context::open,
            "close", &Aula::Interception::Context::close,
            "enumerateDevices", &Aula::Interception::Context::enumerateDevices,
            "setKeyboardFilter", &Aula::Interception::Context::setKeyboardFilter,
            "setMouseFilter", &Aula::Interception::Context::setMouseFilter,
            "recieve", &Aula::Interception::Context::recieve,
            "pass", &Aula::Interception::Context::pass,
            "sendKeyStroke", &Aula::Interception::Context::sendKeyStroke,
            "sendMouseStroke", &Aula::Interception::Context::sendMouseStroke,
            "getCurrentKeyStroke", &Aula::Interception::Context::getCurrentKeyStroke,
            "getCurrentMouseStroke", &Aula::Interception::Context::getCurrentMouseStroke,
            "getCurrentDeviceIndex", &Aula::Interception::Context::getCurrentDeviceIndex,
            "getHardwareId", sol::overload(
                [](Aula::Interception::Context *self, unsigned long deviceIndex) { return self->getHardwareId(deviceIndex); },
                [](Aula::Interception::Context *self) { return self->getHardwareId(); }
            )
        );

        interception.new_usertype<Aula::Interception::Point>("Point",
            "x", &Aula::Interception::Point::x,
            "y", &Aula::Interception::Point::y
        );
        interception.new_usertype<Aula::Interception::Size>("Size",
            "width", &Aula::Interception::Size::width,
            "height", &Aula::Interception::Size::height
        );

        interception.set_function("getCursorPos", Aula::Interception::getCursorPos);
        interception.set_function("getScreenSize", Aula::Interception::getScreenSize);
        interception.set_function("warpCursor", Aula::Interception::warpCursor);

        interception.new_enum("MouseAction",
            "LEFTDOWN", Aula::Interception::MouseAction::LEFTDOWN,
            "LEFTUP", Aula::Interception::MouseAction::LEFTUP,
            "RIGHTDOWN", Aula::Interception::MouseAction::RIGHTDOWN,
            "RIGHTUP", Aula::Interception::MouseAction::RIGHTUP,
            "MIDDLEDOWN", Aula::Interception::MouseAction::MIDDLEDOWN,
            "MIDDLEUP", Aula::Interception::MouseAction::MIDDLEUP,
            "XDOWN", Aula::Interception::MouseAction::XDOWN,
            "XUP", Aula::Interception::MouseAction::XUP
        );

        interception.new_enum("KeyAction",
            "DOWN", Aula::Interception::KeyAction::DOWN,
            "DOWNUP", Aula::Interception::KeyAction::DOWNUP,
            "UP", Aula::Interception::KeyAction::UP
        );

        interception.set_function("sendMouseAction", Aula::Interception::sendMouseAction);
        interception.set_function("sendMouseWheel", Aula::Interception::sendMouseWheel);
        interception.set_function("sendKeyAction", sol::overload(
            [](u8 vk, Aula::Interception::KeyAction mode) { return Aula::Interception::sendKeyAction(vk, mode); },
            [](u8 vk) { return Aula::Interception::sendKeyAction(vk); }
        ));
        interception.set_function("sendKeyString", Aula::Interception::sendKeyString);

        return 0;
    }
}