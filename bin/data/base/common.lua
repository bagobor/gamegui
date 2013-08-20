--- Useful constants. 
OK = 1
CANCEL = 2
NO_STRING = "!!!NO STRING!!!"
SMALL_ANIM_TIME = 1.5

--- GameState
GameState = {}
GameState.LoginScreen = 0
GameState.Options = 1
GameState.CharSelection = 2
GameState.EnteringGame = 3
GameState.Game = 4
local currentGameState = GameState.LoginScreen

function GameState.GetState()
	return currentGameState
end

function GameState.SetState(st)
	currentGameState = st
end

-- debug mode
DebugMode = {}
DebugMode.Filter = {}
DebugMode.Filter.Ticks = "[tT]ick"
DebugMode.Filter.Activities = "Activity"

local debugFilters = {}
local filtering = 0

local function debug_hook(event, lineNumber)
	local info = debug.getinfo(2)
	if not info then
		Debug("debug.getinfo() failed!")
		return
	end

	if event == "call" then
		if filtering > 0 then
			return
		end
		
		if info.what and (info.what == "Lua") then
			local str = ""
			
			if info.name then
				-- filter out unneccessary stuff
				for i, v in ipairs(debugFilters) do
					if string.find(info.name, v) then
						filtering = filtering + 1 -- entering unwanted function
						return
					end
				end

				str = str..info.name
			else
				str = str.."unknown function"
			end
			if info.source and info.linedefined and (info.linedefined >= 0) then
				str = str.." ("..info.source..":"..info.linedefined..")"
			end
			Debug(str)
		end
	else -- event == "return"
		if filtering > 0 then
			if info.what and (info.what == "Lua") then
				if info.name then
					-- filter out unneccessary stuff
					for i, v in ipairs(debugFilters) do
						if string.find(info.name, v) then
							filtering = filtering - 1 -- leaving unwanted function
							if filtering < 0 then
								filtering = 0
							end
							return
						end
					end
				end
			end
		end
	end
end

function DebugMode.SetDefaultFilters()
	Debug("DebugMode.SetDefaultFilters()")
	
	DebugMode.AddFilter(DebugMode.Filter.Ticks)
end

function DebugMode.ResetFilters()
	Debug("DebugMode.ResetFilters()")
	
	debugFilters = {}
end

function DebugMode.AddFilter(filter)
	Debug("DebugMode.AddFilter("..tostring(filter)..")")

	table.insert(debugFilters, filter)
end

function DebugMode.RemoveFilter(idx)
	Debug("DebugMode.RemoveFilter("..tostring(idx)..")")

	table.remove(debugFilters, idx)
end

function DebugMode.Enable()
	Debug("DebugMode.Enable()")
	
	debug.sethook(debug_hook, "cr")
end

function DebugMode.Disable()
	Debug("DebugMode.Disable()")
	
	debug.sethook()
end

function DebugMode.IsEnabled()
	return debug.gethook() ~= nil
end


--- Log message.
-- @param msg message
function Message(msg)
    log:msg(msg)
end

--- Log warning.
-- @param msg message
function Warning(msg)
    log:warn(msg)
end

--- Log error.
-- @param msg message
function Error(msg)
    log:err(msg)
end

--- Log critical error.
-- @param msg message
function Critical(msg)
    log:crit(msg)
end

--- Print debug message.
-- @param msg message
function Debug(msg)
    log:msg(msg)
end

--- Valid empty callback.
-- logs warning
function EmptyCallback()
    Warning("!!!EmptyCallback called!!!")
end

--- Check callback.
-- @param callback callback to check.
function IsValidCallback(callback)
    if not callback or type(callback) ~= "function" then
        return false
    else
        return true
    end
end

--- Validate callback.
-- @param callback callback to validate.
-- @return valid callback (passed in or empty).
function ValidateCallback(callback)
    if not IsValidCallback(callback) then
        return EmptyCallback
    else
        return callback
    end
end

--- Convert multiple args to formatted string.
-- @param ... args to convert.
function Args2String(...)
    local str = ""
    for n = 1, select('#', ...) do
        local el = select(n, ...)
        if n ~= 1 then str = str..", " end
        str = str..tostring(el)
    end
    return str
end

--- Convert slot index to row and column.
-- @param idx element index (counting from 1).
-- @param maxColumn max column index.
-- @return row, column.
function Index2RowColumn(idx, maxColumn)
    local row = math.floor((idx - 1) / maxColumn)
    local column = math.mod(idx - 1, maxColumn)
    return row, column
end

--- Convert row and column to coordinates.
-- @param row row (possibly from Index2RowColumn).
-- @param col column (possibly from Index2RowColumn).
-- @param width width of elements.
-- @param height height of elements.
-- @param xSpacing distance between elements.
-- @param ySpacing distance between elements.
-- @param xOffset additional offset.
-- @param yOffset additional offset.
-- @return x, y coordinates.
function RowColumn2Coords(row, col, width, height, xSpacing, ySpacing, xOffset, yOffset)
    -- do not use nil values
    if not xSpacing then xSpacing = 0 end
    if not ySpacing then ySpacing = 0 end
    if not xOffset then xOffset = 0 end
    if not yOffset then yOffset = 0 end

    -- calculate coords
    local x = xOffset + col * (width + xSpacing)
    local y = yOffset + row * (height + ySpacing)
    return x, y
end

--- Clamp functions.
Clamp = {}

function Clamp.Line(line, maxLen)
	local maxLen = maxLen * 2
	if string.len(line) > maxLen then
		return string.sub(line, 1, maxLen).."..."
	else
		return line
	end
end

--- Clamp window.
-- @param wnd window to clamp.
-- @param clampTo window to clamp to.
-- @return clampedX, clampedY, clampedWidth, clampedHeight clamp state
function Clamp.ToWnd(wnd, clampTo)
	local parent = clampTo
	
	if (not wnd) or (not parent) then
		return
	end
	
    local area = wnd.area
	local wndSize = area.size
	local parentSize = parent.area.size
	local x, y = area.position.x, area.position.y
	local clampedX, clampedY, clampedWidth, clampedHeight = false, false, false, false
	
	if x < 0 then
		x = 0
		clampedX = true
	end
	if y < 0 then
		y = 0
		clampedY = true
	end
	if (x + wndSize.width) > parentSize.width then
		x = parentSize.width - wndSize.width
		clampedWidth = true
	end
	if (y + wndSize.height) > parentSize.height then
		y = parentSize.height - wndSize.height
		clampedHeight = true
	end

	local pos = Point(x, y)
    wnd.area = Rect(pos, area.size)
    
    return clampedX, clampedY, clampedWidth, clampedHeight
end

--- Clamp to parent.
-- @param wnd window to clamp.
function Clamp.ToParent(wnd)
	Clamp.ToWnd(wnd, wnd.parent)
end

--- Clamp to screen.
-- @param wnd window to clamp.
function Clamp.ToScreen(wnd)
	Clamp.ToWnd(wnd, gui.root)
end

--- Check whether clamping is needed.
-- @param wnd window to check.
-- @param parent window to check against.
-- @param rc borders.
-- @return clampedX, clampedY, clampedWidth, clampedHeight clamp state.
function Clamp.CheckBorders(wnd, parent, rc)
	if (not wnd) or (not parent) then
		return
	end
	
	if not rc then
		rc = Rect(0,0,0,0)
	end
	
    local area = wnd.area
	local wndSize = area.size
	local parentSize = parent.area.size
	local x, y = area.position.x + rc.position.x, area.position.y + rc.position.y
	local width, height = area.width + rc.width, area.height + rc.height
	local clampedX, clampedY, clampedWidth, clampedHeight = false, false, false, false
	
	if x < 0 then
		x = 0
		clampedX = true
	end
	if y < 0 then
		y = 0
		clampedY = true
	end
	if (x + width) > parentSize.width then
		x = parentSize.width - width
		clampedWidth = true
	end
	if (y + height) > parentSize.height then
		y = parentSize.height - height
		clampedHeight = true
	end
    
    return clampedX, clampedY, clampedWidth, clampedHeight
end

--- Center window (on the screen by default).
-- @param wnd base window.
-- @param width defaults to screen width.
-- @param height defaults to height height.
function Clamp.Center(wnd, width, height)
    local area = wnd:getArea()
    if not width then
    	width = gui.root.area.width
    end
    if not height then
    	height = gui.root.area.height
    end
    local x = width/2 - area.size.width/2
    local y = height/2 - area.size.height/2
    area.position = Point(x, y)
    wnd:setArea(area)
end

--- Money functions.
Money = {}

-- money types
Money.Cash = 0
Money.Bank = 1
Money.Exchange = 10

--- Convert diams and yunas to yunas only.
-- @param diams diams.
-- @param yunas yunas.
-- @return total yunas.
function Money.Pack(diams, yunas)
    return diams * 100 + yunas
end

--- Convert yunas to diams and yunas.
-- @param yunas.
-- @return diams, yunas. 
function Money.Unpack(yunas)
    local newDiams = math.floor(yunas / 100)
    local newYunas = math.mod(yunas, 100)
    return newDiams, newYunas
end

--- Get money image using specified quantity. 
-- @param quantity
-- @return image name, quantity to show
function Money.GetImage(quantity)
	if quantity == 1 then
        return "yuna", 1
    elseif quantity == 100 then
        return "diam", 1
    elseif quantity < 100 then
        return "yunas", quantity
    elseif (math.floor(quantity / 100) * 100 == quantity) then
        return "diams", quantity / 100
    else
        return "money", quantity
    end
end

--- some colors
Cl = {}
Cl.White = Color(1, 1, 1, 1)
Cl.Gray = Color(0.5, 0.5, 0.5, 1)
Cl.Gray70 = Color(0.7, 0.7, 0.7, 1)
Cl.Gray30 = Color(0.3, 0.3, 0.3, 1)
Cl.Black = Color(0, 0, 0, 1)
Cl.Red = Color(1, 0, 0, 1)
Cl.Red50 = Color(0.5, 0, 0, 1)
Cl.Green = Color(0, 1, 0, 1)
Cl.Green50 = Color(0, 0.5, 0, 1)
Cl.Blue = Color(0, 0, 1, 1)
Cl.Blue50 = Color(0, 0, 0.5, 1)
Cl.Yellow = Color(1, 1, 0, 1)
Cl.Yellow50 = Color(0.5, 0.5, 0, 1)
Cl.Orange = Color(1, 0.5, 0, 1)
Cl.Cyan = Color(0, 1, 1, 1)
Cl.Purple = Color(0.5, 0, 0.5, 1)

Cl.AllowDrop = Cl.Green
Cl.DenyDrop = Cl.Red

function Cl.Random()
	return Color(math.random(), math.random(), math.random(), 1)
end

--- Get color by item rarity.
-- @param rar item rarity
-- @return color.
function Cl.FromRarity(rar)
	--Debug("Cl.FromRarity("..tostring(rar)..")")

	if rar == 1 then -- assume it's crude
		return Cl.Gray
	elseif rar == 2 then -- common
		return Cl.White
	elseif rar == 3 then -- uncommon
		return Cl.Cyan
	elseif rar == 4 then -- rare
		return Cl.Green
	elseif rar == 5 then -- unique
		return Cl.Yellow
	elseif rar == 6 then -- legendary
		return Cl.Orange
	elseif rar == 7 then -- artifact
		return Cl.Purple
	else
		Warning("Cl.FromRarity("..tostring(rar).."): Invalid item's rarity!")
		return Cl.White
	end
end

--- Get color by char alignment.
-- @param al char alignment.
-- @return color.
function Cl.FromAlignment(al)
	--Debug("Cl.FromAlignment("..tostring(al)..")")

	if al == CharacterWrapper.ALM_FRIEND then
		return Cl.Green
	elseif al == CharacterWrapper.ALM_CITIZEN then
		return Cl.Yellow
	elseif al == CharacterWrapper.ALM_CRIMINAL then
		return Cl.Red50
	elseif al == CharacterWrapper.ALM_NEUTRAL then
		return Cl.White
	elseif al == CharacterWrapper.ALM_ENEMY then
		return Cl.Red
	else
		Warning("Cl.FromAlignment("..tostring(al).."): Invalid char alignment!")
		return Cl.White
	end
end

--- some string functions.
String = {}

--- Format price string.
-- @param coins coins in yunas.
-- @return formatted string.
function String.FormatPrice(coins)
    local diams, yunas = Money.UnpackMoney(coins)
	return tostring(diams).."."..tostring(yunas)
end

--- Format time string.
-- @param secs time in seconds.
-- @return formatted string.
function String.FormatTime(secs)
	local minutes = math.floor(secs / 60)
	local seconds = secs - minutes * 60
	return string.format("%02d:%02d", minutes, seconds)
end

--- Input function.
Input = {}

--- Is shift key pressed?
function Input.IsShiftPressed()
    return gui:isSysKeyPressed(1)
end

--- Is control key pressed?
function Input.IsCtrlPressed()
    return gui:isSysKeyPressed(4)
end

--- Is alt key pressed?
function Input.IsAltPressed()
    return gui:isSysKeyPressed(16)
end

function Input.MButton2OptionString(mbutton, double)
	local str = ""
	if double then
		str = "double"
	else
		str = "mouse"
	end
	
	if mbutton == 4 then
		return str.."2"
	elseif mbutton == 2 then
		return str.."3"
	else
		return str.."1"
	end
end

function Input.MWheel2OptionString(mwheel)
	if mwheel > 0 then
		return "mousewheelup"
	else
		return "mousewheeldown"
	end
end

--- Set close callback for frame window.
-- @param callbackName callback to set.
function SetCloseCallback(callbackName)
	this:find("Close"):addScriptEventHandler("On_Clicked", callbackName)
end

--- Iterate through child windows.
-- @param wnd base window.
-- @param baseName base control name.
-- @param num number of children.
-- @param ftor function to call for each child. function(child, i).
function ForEachChild(wnd, baseName, num, ftor)
    for i=1, num do
        local child = wnd:find(baseName..tostring(i))
        ftor(child, i)
    end
end

--- Show child windows.
-- @param wnd base window.
-- @param baseName base control name.
-- @param num number of children.
function ShowChildren(wnd, baseName, num)
    ForEachChild(wnd, baseName, num, function(wnd) wnd.visible = true end)
end

--- Hide child windows.
-- @param wnd base window.
-- @param baseName base control name.
-- @param num number of children.
function HideChildren(wnd, baseName, num)
    ForEachChild(wnd, baseName, num, function(wnd) wnd.visible = false end)
end

--- Set window alpha.
function SetWindowAlpha(wnd, alpha)
	local color = wnd.backcolor
	color.a = alpha
	wnd.backcolor = color
end

--- Tab control.
Tab = {}

local tabControls = {}

--- Register tab control.
-- @param wnd base tab control window.
-- @param tabs array with tabs (windows).
-- @return tab control.
function Tab.Register(wnd, tabs)
	--Debug("Tab.Register()")

	local id = #tabControls + 1
	local tabCtrl = {
		wnd = wnd,
		ctrlName = tabCtrlName,
		tabs = tabs,
		id = id,
		currentTab = 1
	}
	
	local i = 1
	for _, wnd in ipairs(tabs) do
		wnd.tag = i
		i = i + 1 
	end
	
	wnd.tag = id
	table.insert(tabControls, tabCtrl)
	Tab.Activate(tabCtrl, 1)
	
	return tabCtrl
end

--- Get tab from tab control by index.
-- @param tabCtrl tab control.
-- @param idx index.
-- @return tab (window).
function Tab.Get(tabCtrl, idx)
	--Debug("Tab.Get()")

	return tabCtrl.wnd:find("Tab"..tostring(idx))
end

--- Activate tab.
-- @param tabCtrl tab control.
-- @param id tab index.
function Tab.Activate(tabCtrl, id)
	--Debug("Tab.Activate()")

    local tab = Tab.Get(tabCtrl, id)
    tab.backcolor = Cl.White
    local contentWnd = tabCtrl.tabs[id]
    if not contentWnd then
        return
    end
    contentWnd.visible = true
end

--- Deactivate tab.
-- @param tabCtrl tab control.
-- @param id tab index.
function Tab.Deactivate(tabCtrl, id)
	--Debug("Tab.Deactivate()")

    local tab = Tab.Get(tabCtrl, id)
    tab.backcolor = Cl.Gray70
    local contentWnd = tabCtrl.tabs[id]
    if not contentWnd then
        return
    end
    contentWnd.visible = false
end

--- Default On_TabClicked handler.
-- @param tabCtrl tab control.
-- @param id new active tab index.
function Tab.On_TabClicked(tabCtrl, id)
	--Debug("Tab.On_TabClicked("..tostring(id)..")")

    if tabCtrl.currentTab < 1 then
        Warning("Tab.On_TabClicked("..tostring(id).."): Invalid current tab ID!")
        return
    end

    if id < 1 or id > #tabCtrl.tabs then
        Warning("Tab.On_TabClicked("..tostring(id).."): Invalid new tab ID!")
        return
    end

    if id == tabCtrl.currentTab then
        return
    end

    Tab.Deactivate(tabCtrl, tabCtrl.currentTab)
    Tab.Activate(tabCtrl, id)
    tabCtrl.currentTab = id
end

--- Is tab active.
-- @param tabCtrl tab control.
-- @param id tab index.
-- @return bool.
function Tab.IsActiveTab(tabCtrl, id)
	if tabCtrl.currentTab == id then
		return true
	else
		return false
	end
end

-- L10N stuff

--- Get localized string.
-- @param key key to lookup l10n string
-- @param group l10n group (defaults to "ClientGUITexts")
-- @return localized string.
function GetString(key, group)
	if not group then
		group = "ClientGUITexts"
	end

	return game:GetTextS(group, key)
end

--- Localize frame window caption.
-- @param key key to lookup l10n string
-- @param group l10n group (defaults to "ClientGUITexts")
function UpdateCaption(key, group)
	local wnd = to_framewindow(this)
	if not key then
		key = wnd.caption
	end
	wnd.caption = GetString(key, group)
end

--- Update control's text. Use it in On_Load handler.
-- @param key key to lookup l10n string (use control's text if nil)
-- @param group l10n group (defaults to "ClientGUITexts")
function UpdateL10N(key, group)
	local wnd = to_statictext(this)
	if not key then
		key = wnd.text
	end
	wnd.text = GetString(key, group)
end


function CrimeType2ImageName(t)
	if t == CrimeTypeHelper.CRIME_THEFT then
		return "Theft"
	elseif t == CrimeTypeHelper.CRIME_ASSAULT then
		return "Attack"
	elseif t == CrimeTypeHelper.CRIME_MURDER then
		return "Murder"
	elseif t == CrimeTypeHelper.CRIME_COMPLICITY then
		return "Complicity"
	elseif t == CrimeTypeHelper.CRIME_ILLEGAL_SEARCH_ATTEMPT then
		return "Possession_of_stolen"
	elseif t == CrimeTypeHelper.CRIME_THEFT_ATTEMPT then
		return "Attempted_Theft"
	else
		Warning("CrimeType2ImageName("..tostring(t).."): Unknown criminal case type!")
		return ""
	end
end

-- FIXME!!! Remove me when CrimeTypeHelper.CrimeType2String becomes accessible!!!
function CrimeType2String(t)
	if t == CrimeTypeHelper.CRIME_THEFT then
		return "Theft"
	elseif t == CrimeTypeHelper.CRIME_ASSAULT then
		return "Assault"
	elseif t == CrimeTypeHelper.CRIME_MURDER then
		return "Murder"
	elseif t == CrimeTypeHelper.CRIME_COMPLICITY then
		return "Complicity"
	elseif t == CrimeTypeHelper.CRIME_ILLEGAL_SEARCH_ATTEMPT then
		return "IllegalSearchAttempt"
	elseif t == CrimeTypeHelper.CRIME_THEFT_ATTEMPT then
		return "TheftAttempt"
	else
		Warning("CrimeType2String("..tostring(t).."): Unknown criminal case type!")
		return "Unknown criminal case"
	end
end

function CharAlignment2String(al)
	if al == CharacterWrapper.ALM_FRIEND then
		return "friendly"
	elseif al == CharacterWrapper.ALM_CITIZEN then
		return "citizen"
	elseif al == CharacterWrapper.ALM_CRIMINAL then
		return "criminal"
	elseif al == CharacterWrapper.ALM_NEUTRAL then
		return "neutral"
	elseif al == CharacterWrapper.ALM_ENEMY then
		return "hostile"
	end
end

function SlotID2SlotName(id)
	local invalid_slot = ""

	if id == ItemPrototype.HELMET then
		return "Head Slot"
	elseif id == ItemPrototype.EYEGLASSES then
		return invalid_slot
	elseif id == ItemPrototype.EARRING_1 then
		return "Trinket Slot"
	elseif id == ItemPrototype.EARRING_2 then
		return invalid_slot
	elseif id == ItemPrototype.AMULET then
		return "Necklace Slot"
	elseif id == ItemPrototype.SHOULDER then
		return "Shoulder Slot"
	elseif id == ItemPrototype.CLOAK then
		return "Cloak Slot"
	elseif id == ItemPrototype.UNDERWEAR then
		return "Underwear Slot"
	elseif id == ItemPrototype.SHIRT then
		return "Shirt Slot"
	elseif id == ItemPrototype.ARMOR then
		return "Armor Slot"
	elseif id == ItemPrototype.BELT then
		return "Belt Slot"
	elseif id == ItemPrototype.BRACERS then
		return "Bracers Slot"
	elseif id == ItemPrototype.GLOVES then
		return "Gloves Slot"
	elseif id == ItemPrototype.RING_1 then
		return "Ring Slot"
	elseif id == ItemPrototype.RING_2 then
		return "Ring Slot"
	elseif id == ItemPrototype.BANGLES_1 then
		return "Braclet Slot"
	elseif id == ItemPrototype.BANGLES_2 then
		return invalid_slot
	elseif id == ItemPrototype.BREECHES then
		return "Pants Slot"
	elseif id == ItemPrototype.GREAVES then
		return invalid_slot
	elseif id == ItemPrototype.SHOES then
		return "Boots Slot"
	elseif id == ItemPrototype.MAIN_HAND then
		return "Main Weapon Slot"
	elseif id == ItemPrototype.OFF_HAND then
		return "Secondary Weapon Slot"
	elseif id == ItemPrototype.THIRD_HAND then
		return "Main Weapon Slot"
	elseif id == ItemPrototype.FOURTH_HAND then
		return "Secondary Weapon Slot"
	else
		Warning("SlotID2SlotName("..tostring(id).."): invalid slot!")
		return invalid_slot
	end
end

function DetermineCrimeIDByQuestID(questID)
	local id = math.floor(questID / 8)
	local t = math.fmod(questID, 8)
	return id, t
end

function SkillType2String(i)
	if i == Skill.TYPE_STYLE then
		return "SKILL_TYPE_STYLE"
	elseif i == Skill.TYPE_PROF then
		return "SKILL_TYPE_PROF"
	elseif i == Skill.TYPE_MENTAL then
		return "SKILL_TYPE_MENTAL"
	elseif i == Skill.TYPE_PHYSIC then
		return "SKILL_TYPE_PHYSIC"
	elseif i == Skill.TYPE_MAGIC then
		return "SKILL_TYPE_MAGIC"
	elseif i == Skill.TYPE_MASTER then
		return "SKILL_TYPE_MASTER"
	elseif i == Skill.TYPE_ORG then
		return "SKILL_TYPE_ORG"
	elseif i == Skill.TYPE_RACIAL then
		return "SKILL_TYPE_RACIAL"
	else
		Warning("SkillType2String("..tostring(i).."): Invalid skill type!")
		return ""
	end
end

--- Get ability imageset and image.
-- @param abil game ability.
-- @return imageset, image 
function GetAbilityImage(abil)
	local abilID = abil:GetID()
	local imageset = "Abilities"
	local image = abil:GetTextureName()
	if abilID < 255 then -- local ability
		if abilID == 1 then
			imageset = "Gifts"
		else
			imageset = "Emotes"
		end
	end
	return imageset, image
end

--- slot id 2 name conversion FIXME!!! remove me
function CharSlotID2Name(id)
    if id == 0 then -- HELMET
        return "Left.Head"
    elseif id == 1 then -- EYEGLASSES
        return ""
    elseif id == 2 then -- EARRING_1
        return "Right.Trinket"
    elseif id == 3 then -- EARRING_2
        return ""
    elseif id == 4 then -- AMULET
        return "Left.Necklace"
    elseif id == 5 then -- SHOULDER
        return "Right.Shoulder"
    elseif id == 6 then -- CLOAK
        return "Right.Cloak"
    elseif id == 7 then -- UNDERWEAR
        return "Right.Underwear"
    elseif id == 8 then -- SHIRT
        return "Left.Shirt"
    elseif id == 9 then -- ARMOR
        return "Left.Chest"
    elseif id == 10 then -- BELT
        return "Left.Belt"
    elseif id == 11 then -- BRACERS
        return "Right.Bracers"
    elseif id == 12 then -- GLOVES
        return "Left.Gloves"
    elseif id == 13 then -- RING_1
        return "Left.Ring1"
    elseif id == 14 then -- RING_2
        return "Left.Ring2"
    elseif id == 15 then -- BANGLES_1
        return "Right.Bracelets"
    elseif id == 16 then -- BANGLES_2
        return ""
    elseif id == 17 then -- BREECHES
        return "Left.Pants"
    elseif id == 18 then -- GREAVES
        return ""
    elseif id == 19 then -- SHOES
        return "Right.Boots"
    elseif id == 20 then -- MAIN_HAND
        return "Left.Weapon1"
    elseif id == 21 then -- OFF_HAND
        return "Right.OffHand1"
    elseif id == 22 then -- THIRD_HAND
        return "Left.Weapon2"
    elseif id == 23 then -- FOURTH_HAND
        return "Right.OffHand2"
    elseif id == 25 then -- HAIR_STYLE
        return "Middle.HairStyle"
    else
        Warning("CharSlotID2Name("..tostring(id).."): Invalid slot ID!")
        return ""
    end
end

--- Convert quest task inner status to string.
function QuestTaskInnerStatus2String(status)
	if status == Task.TASK_FAILED then
		return "Failed"
	elseif status == Task.TASK_COMPLETED then
		return "Completed"
	elseif status == Task.TASK_IN_PROGRESS then
		return "In Progress"
	else
		Warning("QuestTaskInnerStatus2String("..tostring(status).."): invalid task inner status!")
		return ""
	end
end 

function DischargeAbility()
	local interface = act_man:GetInterface()
	local chargedAbil = interface:GetChargedAbility()
	if chargedAbil and chargedAbil:IsValid() then
		interface:DischargeAbility()
		return true
	else
		return false
	end
end

--- Class.
function class(base, ctor)
	local c = {} -- a new class instance
	if not ctor and type(base) == 'function' then
		ctor = base
		base = nil
	elseif type(base) == 'table' then
		-- our new class is a shallow copy of the base class!
		for i,v in pairs(base) do
			c[i] = v
		end
		c._base = base
	end
	-- the class will be the metatable for all its objects,
	-- and they will look up their methods in it.
	c.__index = c

	-- expose a ctor which can be called by <classname>(<args>)
	local mt = {}
	mt.__call = function(class_tbl, ...)
		local obj = {}
		setmetatable(obj, c)
		if ctor then
			ctor(obj, ...)
		else 
			-- make sure that any stuff from the base class is initialized!
			if base and base.init then
				base.init(obj, ...)
			end
		end
		return obj
	end
	c.init = ctor
	c.is_a = function(self, klass)
		local m = getmetatable(self)
		while m do 
			if m == klass then
				return true
			end
			m = m._base
		end
		return false
	end
	setmetatable(c, mt)
	return c
end

--- class for windows with dynamic content
-- @param baseWnd window with dynamic content
-- @param poolWnd pool window for caching
-- @param filename filename for content windows (items)
-- @param basename basename for content windows (final item name = basename+itemIndex)
DynamicContent = class(function(c, baseWnd, poolWnd, filename, basename)
	c.baseWnd = baseWnd
	c.poolWnd = poolWnd
	c.filename = filename
	c.basename = basename
	c.items = {}
	c.cache = {}
end)

--- add item
-- @param ftor
function DynamicContent:AddItem(ftor)
	local wndIdx = #self.items+1
	local wndName = self.basename..tostring(wndIdx)
	local wnd = table.remove(self.cache)
	if not wnd then
		wnd = gui:loadWindow(self.poolWnd, self.filename)
		wnd.name = wndName
	end
	if ftor then
		ftor(wnd)
	else
		self:AddWnd(wnd)
	end
	return wnd, wndIdx
end

function DynamicContent:AddWnd(wnd)
	self.baseWnd:addChild(wnd)
	table.insert(self.items, wnd)
end

function DynamicContent:GetItem(idx)
	return self.items[idx]
end

function DynamicContent:RemoveItem(idx, ftor)
	local wnd = self.items[idx]
	if not wnd then
		Warning("DynamicContent:RemoveItem("..tostring(idx).."): No item found")
		return
	end
    
	if ftor then
		ftor(wnd)
	end
	
	self.poolWnd:addChild(wnd)
	table.remove(self.items, idx)
	table.insert(self.cache, wnd)
end

--- remove all items
-- @param ftor
function DynamicContent:RemoveAllItems(ftor)
	for i=#self.items, 1, -1 do
		self:RemoveItem(i, ftor)
	end
end

--- for each item
-- @param ftor
function DynamicContent:ForEachItem(ftor)
	for i,v in ipairs(self.items) do
		if ftor(v, i) then
			return
		end
	end
end

function DynamicContent:GetItemCount()
	return #self.items
end

--- Class for generic game window. 
-- @param baseWnd window
GameWindow = class(function(c, baseWnd)
	c.baseWnd = baseWnd
end)

--- Is window visible?
function GameWindow:IsVisible()
	return self.baseWnd:getVisible()
end

--- Show game window.
function GameWindow:Show()
	self.baseWnd:setVisible(true)
    self.baseWnd:moveToFront()
end

--- Hide game window.
function GameWindow:Hide()
	self.baseWnd:setVisible(false)
end

--- Generic animation.
Anim = class(function(c, maxTime, ftor)
	c.maxTime = maxTime
	c.curTime = 0.0
	c.value = 0.0
	c.ftor = ftor
	if not c.ftor then
		c.ftor = function(val) return val end
	end
end)

function Anim:Tick(delta)
	self.curTime = self.curTime + delta
	self.value = self:Sample(self.curTime)
	
	return self.ftor(self.value)
end

function Anim:Sample(time)
	return (1 - ((self.maxTime - self.curTime) / self.maxTime))
end

function Anim:Value2Time(val)
	return -(((1 - val) * self.maxTime) - self.maxTime)
end

function Anim:Reset(time)
	self.value = 0.0
	if time then
		self.curTime = time
	else
		self.curTime = 0.0
	end
	
	return self
end

--- Fade-in animation.
FadeInAnim = class(Anim, function(c, maxTime)
	Anim.init(c, maxTime, function(val)	return val end)
end)

--- Fade-out animation.
FadeOutAnim = class(Anim, function(c, maxTime)
	Anim.init(c, maxTime, function(val) return 1-val end)
end)

--- FadeInOutAnim.
FadeInOutAnim = class(function(c, wnd, maxTime, ftor)
	c.wnd = wnd
	c.fadeInAnim = FadeInAnim(maxTime)
	c.fadeOutAnim = FadeOutAnim(maxTime)
	c.curAnim = nil
	c.ftor = ftor
end)

function FadeInOutAnim:Tick(delta)
	if not self.curAnim then
		self:Stop()
		return
	end
	
	local alpha = self.curAnim:Tick(delta)
	
	if alpha > 1 then
		alpha = 1
		self:Stop()
	elseif alpha < 0 then
		alpha = 0
		self:Stop()
	end
	
	self.ftor(alpha)
end

function FadeInOutAnim:Start(anim)
	if self.curAnim then
		if self.curAnim ~= anim then
			self.curAnim = anim:Reset(self.curAnim.maxTime - self.curAnim.curTime)
		end
	else
		self.curAnim = anim:Reset()
	end
	self.wnd:startTick()
end

function FadeInOutAnim:StartFadeIn()
	self:Start(self.fadeInAnim)
end

function FadeInOutAnim:StartFadeOut()
	self:Start(self.fadeOutAnim)
end

function FadeInOutAnim:Stop()
	self.wnd:stopTick()
	self.curAnim = nil
end

-- HACK: kill me as soon as item:IsBroken() works
function IsItemBroken(item)
	if (not item) or (not item:IsValid()) then
		return true
	end
	local tooltip = item:GetTooltip()
	if (not tooltip) or (not tooltip:IsValid()) then
		return false
	end
	local itemHeader = tooltip:GetItemHeader()
	if (not itemHeader) then
		return false
	end
	
	if (itemHeader.curDurability < 1) or (item:IsBroken()) then
		return true
	else
		return false
	end
end

--- On_Init callback manager.
local initCallbacks = {}

function AddInitCallback(ftor)
	table.insert(initCallbacks, ftor)
end

function ProcessInitCallbacks()
	for i,ftor in ipairs(initCallbacks) do
		ftor()
	end
end
