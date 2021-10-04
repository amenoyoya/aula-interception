-- #aula v1.0.0
-- Simultaneous push sample

include "../dist/interception.dll"

local KeyCode = {
    CTRL = 0x1D,
    ALT = 0x38,
}

local context = Aula.Interception.Context.new()

-- Input filter
context:setKeyboardFilter(Aula.Interception.FILTER_KEY_DOWN + Aula.Interception.FILTER_KEY_UP)
context:setMouseFilter(Aula.Interception.FILTER_MOUSE_ALL - Aula.Interception.FILTER_MOUSE_MOVE)

-- キー入力履歴（同時押し用）
local keyStrokes = { -- 2つ同時押しに対応
    Aula.Interception.KeyStroke.new(),
    Aula.Interception.KeyStroke.new()
}
local lastKeyStroke = Aula.Interception.KeyStroke.new()

while context:recieve() do
    local keystroke = context:getCurrentKeyStroke()

    -- Key input history
    if keystroke and (keystroke.code ~= lastKeyStroke.code or keystroke.state ~= lastKeyStroke.state) then
        table.remove(keyStrokes, 1)
        keyStrokes[#keyStrokes + 1] = Aula.Interception.KeyStroke.new(keystroke.code, keystroke.state, 0)
        
        for _, s in ipairs(keyStrokes) do
            printf("%d,%d ", s.code, s.state)
        end
        print"" -- \n

        -- Update last keyboard input
        lastKeyStroke.code, lastKeyStroke.state = keystroke.code, keystroke.state
    end

    -- Simultaneous push
    if (
        keyStrokes[1].code == KeyCode.CTRL
        and keyStrokes[1].state == Aula.Interception.KEY_DOWN
        and keyStrokes[2].code == KeyCode.ALT
        and keyStrokes[1].state == Aula.Interception.KEY_DOWN
    ) then
        print "Ctrl + Alt pressed"
    end

    if keystroke then
        -- printf(
        --     "Keyboard: %s\n\tstate: %d\n\tcode: %d\n",
        --     context:getHardwareId(), keystroke.state, keystroke.code
        -- )
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
