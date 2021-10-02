-- #aula v1.0.0

include "../dist/interception.dll"

local context = Aula.Interception.Context.new()
local devices = context:enumerateDevices()

for i, device in ipairs(devices) do
    printf(
        "%d: index: %d, hardware_id: %s, device: %s\n",
        i,
        device.index,
        device.hardwareId,
        device.isKeyboard and "keyboard" or "mouse"
    )
end

context:setKeyboardFilter(
    Aula.Interception.FILTER_KEY_ALL
)
context:setMouseFilter(
    Aula.Interception.FILTER_MOUSE_ALL - Aula.Interception.FILTER_MOUSE_MOVE
)

while context:recieve() do
    local keystroke = context:getCurrentKeyStroke()
    if keystroke then
        printf(
            "Keyboard: %s\n\tstate: %d\n\tcode: %d\n",
            context:getHardwareId(), keystroke.state, keystroke.code
        )
        if keystroke.code == 1 then -- ESC キー => break
            break
        end
    end

    local mousestroke = context:getCurrentMouseStroke()
    if mousestroke then
        printf(
            "Mouse: %s\n\tstate: %d\n\tflags: %d\n\trolling: %d\n\t(x, y): (%d, %d)\n",
            context:getHardwareId(), mousestroke.state, mousestroke.flags,
            mousestroke.rolling, mousestroke.x, mousestroke.y
        )
    end
    context:pass() -- 入力をそのまま送信
end

context:close()
