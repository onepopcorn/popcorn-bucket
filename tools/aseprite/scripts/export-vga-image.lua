-- export-vga-image.lua
-- Exports an indexed (palette-based) sprite sheet to a C array.
-- Tile width and height can be set independently (non-square tiles supported).

local spr = app.activeSprite
if not spr then
  app.alert("No active sprite!")
  return
end

-- ✅ Check indexed mode
if spr.colorMode ~= ColorMode.INDEXED then
  app.alert("❌ The sprite must be in Indexed color mode.")
  return
end

-- 🧮 Ask user for tilesheet layout
local dlg = Dialog("Export Indexed Sheet to C")
dlg:number{ id="tileWidth", label="Tile Width (px)", text="16" }
dlg:number{ id="tileHeight", label="Tile Height (px)", text="16" }
dlg:number{ id="rows", label="Rows", text="1" }
dlg:number{ id="cols", label="Columns", text="1" }
dlg:file{ id="path", label="Save As", save=true, filetypes={"c", "h"} }
dlg:button{ id="ok", text="Export" }
dlg:button{ id="cancel", text="Cancel" }
dlg:show()

local data = dlg.data
if not data or data.cancel then return end

local tileWidth = data.tileWidth
local tileHeight = data.tileHeight
local rows = data.rows
local cols = data.cols
local filepath = data.path
local basename = filepath:match("([^/\\]+)%.%w+$") or "tileset"

if not filepath then return end

app.alert("ℹ️ Ensure all tiles have the same dimensions.")

-- 🧱 Start reading pixels
local img = spr.cels[1].image
local w, h = img.width, img.height

local lines = {}
table.insert(lines, string.format("// Exported from %s (Indexed)", (spr.filename and spr.filename:match("([^/\\]+)$")) or "Untitled"))
table.insert(lines, string.format("// %d x %d pixels, %d×%d tiles, tile size: %d×%d", w, h, cols, rows, tileWidth, tileHeight))
table.insert(lines, string.format("const unsigned char %s[] = {", basename))

local tileIndex = 0
for ty = 0, rows - 1 do
  for tx = 0, cols - 1 do
    tileIndex = tileIndex + 1
    table.insert(lines, string.format("  // --- Tile %d (%d,%d) ---", tileIndex, tx, ty))

    local startY = ty * tileHeight
    local startX = tx * tileWidth
    for y = 0, tileHeight - 1 do
      local rowPixels = {}
      for x = 0, tileWidth - 1 do
        local px, py = startX + x, startY + y
        local colorIndex = 0
        if px >= 0 and px < w and py >= 0 and py < h then
          colorIndex = img:getPixel(px, py)
        end
        table.insert(rowPixels, string.format("0x%02X", colorIndex))
      end
      table.insert(lines, "  " .. table.concat(rowPixels, ", ") .. ",")
    end

    table.insert(lines, string.format("  // --- End of Tile %d ---", tileIndex))
    table.insert(lines, "")
  end
end

table.insert(lines, "};")
table.insert(lines, string.format("const int %s_tile_width = %d;", basename, tileWidth))
table.insert(lines, string.format("const int %s_tile_height = %d;", basename, tileHeight))
table.insert(lines, string.format("const int %s_rows = %d;", basename, rows))
table.insert(lines, string.format("const int %s_cols = %d;", basename, cols))

-- 💾 Write to file
local f = io.open(filepath, "w")
if not f then
  app.alert("Failed to write file:\n" .. filepath)
  return
end
f:write(table.concat(lines, "\n"))
f:close()

app.alert("✅ Export complete:\n" .. filepath)
