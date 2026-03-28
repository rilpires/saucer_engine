-- Snake Demo for Saucer Engine
-- Controls: Arrow keys to move | R to restart after game over

-- Grid configuration
CELL_SIZE = 20
COLS = 32   -- 640 / 20
ROWS = 24   -- 480 / 20

CELL_TEXTURE = "res/sprites/cell.png"
MOVE_INTERVAL = 0.15  -- seconds between moves

-- Game state (reset on each run because these live in the node's virtual env)
snake      = {}          -- list of {x,y} grid positions; snake[1] is the head
food_node  = nil
food_x     = 0
food_y     = 0
dir_x      = 1
dir_y      = 0
next_dir_x = 1
next_dir_y = 0
timer      = 0
score      = 0
game_over  = false
needs_restart = false

-- ────────────────────────────────────────────
-- Helper: create a new SceneNode with a Sprite
-- ────────────────────────────────────────────
function make_cell(gx, gy, r, g, b)
    local node = SceneNode.new()
    node:set_position(Vector2(gx * CELL_SIZE, gy * CELL_SIZE))
    local spr = node:create_sprite()
    spr:set_texture(load(CELL_TEXTURE))
    spr:set_centralized(false)
    node:set_self_modulate(Color(r, g, b, 1))
    return node
end

-- ────────────────────────────────────────────
-- Helper: pick a random free grid cell for food
-- ────────────────────────────────────────────
function place_food()
    local fx, fy, ok
    repeat
        fx = math.floor(math.random(0, COLS - 1))
        fy = math.floor(math.random(0, ROWS - 1))
        ok = true
        for i = 1, table.getn(snake) do
            if snake[i].x == fx and snake[i].y == fy then
                ok = false
                break
            end
        end
    until ok
    food_x = fx
    food_y = fy
    if food_node then
        food_node:set_position(Vector2(fx * CELL_SIZE, fy * CELL_SIZE))
    end
end

-- ────────────────────────────────────────────
-- Lifecycle callbacks
-- ────────────────────────────────────────────
function _entered_tree()
    math.randomseed(math.floor(Engine.get_uptime() * 1000))

    -- Spawn initial 3-segment snake in the centre
    local cx = math.floor(COLS / 2)
    local cy = math.floor(ROWS / 2)
    for i = 1, 3 do
        table.insert(snake, {x = cx - i + 1, y = cy})
        this:add_child(make_cell(cx - i + 1, cy, 0, 0.8, 0))
    end

    -- Create food node (persistent, just repositioned)
    food_node = make_cell(0, 0, 1, 0.2, 0.2)
    food_node:set_name("food")
    this:add_child(food_node)
    place_food()

    print("Snake Demo  |  Arrow keys: move  |  R: restart")
end

function _input(event)
    if event:get_type() ~= InputEventType.KEY then return end
    if not event:is_pressed() then return end

    local k = event:get_key()

    if     k == KEY.UP    and dir_y == 0 then next_dir_x =  0; next_dir_y = -1
    elseif k == KEY.DOWN  and dir_y == 0 then next_dir_x =  0; next_dir_y =  1
    elseif k == KEY.LEFT  and dir_x == 0 then next_dir_x = -1; next_dir_y =  0
    elseif k == KEY.RIGHT and dir_x == 0 then next_dir_x =  1; next_dir_y =  0
    elseif k == KEY.W     and dir_y == 0 then next_dir_x =  0; next_dir_y = -1
    elseif k == KEY.S     and dir_y == 0 then next_dir_x =  0; next_dir_y =  1
    elseif k == KEY.A     and dir_x == 0 then next_dir_x = -1; next_dir_y =  0
    elseif k == KEY.D     and dir_x == 0 then next_dir_x =  1; next_dir_y =  0
    elseif k == KEY.R and game_over then
        needs_restart = true
    end
end

function _frame_start(delta)
    -- Restart requested from previous frame's _input
    if needs_restart then
        local saved_script = this:get_script()
        local new_root = SceneNode.new()
        new_root:set_script(saved_script)
        Engine.get_current_scene():set_root_node(new_root)
        return  -- 'this' is being replaced; return immediately
    end

    if game_over then return end

    timer = timer + delta
    if timer < MOVE_INTERVAL then return end
    timer = timer - MOVE_INTERVAL

    -- Apply buffered direction
    dir_x = next_dir_x
    dir_y = next_dir_y

    -- Compute new head
    local head = snake[1]
    local nx = head.x + dir_x
    local ny = head.y + dir_y

    -- Wall collision
    if nx < 0 or nx >= COLS or ny < 0 or ny >= ROWS then
        game_over = true
        print("Game Over! Final score: " .. score .. "  |  Press R to restart")
        return
    end

    -- Self collision
    for i = 1, table.getn(snake) do
        if snake[i].x == nx and snake[i].y == ny then
            game_over = true
            print("Game Over! Final score: " .. score .. "  |  Press R to restart")
            return
        end
    end

    -- Eat food?
    local ate = (nx == food_x and ny == food_y)

    -- Advance snake array
    table.insert(snake, 1, {x = nx, y = ny})
    if not ate then
        table.remove(snake)
    else
        score = score + 1
        print("Score: " .. score)
        place_food()
    end

    -- Sync visual nodes to snake array
    -- Collect non-food children in order
    local children = this:get_children()
    local segs = {}
    for i = 1, table.getn(children) do
        if children[i]:get_name() ~= "food" then
            table.insert(segs, children[i])
        end
    end

    local snake_len = table.getn(snake)

    -- Grow: add a new tail node when the snake got longer
    if snake_len > table.getn(segs) then
        local tail = snake[snake_len]
        local new_seg = make_cell(tail.x, tail.y, 0, 0.6, 0)
        this:add_child(new_seg)
        table.insert(segs, new_seg)
    end

    -- Update all segment positions and colours
    for i = 1, snake_len do
        local seg = segs[i]
        if seg then
            seg:set_position(Vector2(snake[i].x * CELL_SIZE, snake[i].y * CELL_SIZE))
            if i == 1 then
                seg:set_self_modulate(Color(0.4, 1, 0.4, 1))  -- bright head
            else
                seg:set_self_modulate(Color(0, 0.7, 0, 1))    -- body
            end
        end
    end
end
