Drag = {}

Drag.QuantitySelected = false
Drag.JustStarted = false
Drag.Dropped = false

-- drag and drop vars
local dragInProgress = false -- dragging smth right now?
local dragData = {} -- anything we drag
local dragSender = nil -- who started the drag
local dragReceiver = nil -- who receives the drag
local dragIcon = nil
local dragText = nil

function Drag.On_Load()
    dragIcon = to_staticimage(this)
    gui.dragcontainer:addChild(dragIcon)
    gui.dragcontainer:setArea(Rect(Point(0,0), Size(50,50)))
    dragIcon.visible = true
    dragText = to_statictext(dragIcon:find("BottomText"))
end

-- set sender
function Drag.SetSender(sender)
    dragSender = sender
end

-- get sender
function Drag.GetSender()
    return dragSender
end

-- has valid sender
function Drag.HasSender()
    if dragSender ~= nil then
        return true
    else
        return false
    end
end

-- set receiver
function Drag.SetReceiver(receiver)
    dragReceiver = receiver
end

-- get receiver
function Drag.GetReceiver()
    return dragReceiver
end

-- has valid receiver
function Drag.HasReceiver()
    if dragReceiver ~= nil then
        return true
    else
        return false
    end
end

-- start drag process
function Drag.Begin()
    if dragInProgress then
        Error("Drag.Begin(): Drag already started!")
        return false
    end

	Drag.Dropped = false
    dragInProgress = true

    return true
end

-- end drag process
function Drag.End()
    if not dragInProgress then
        Error("Drag.End(): Drag not started!")
        return false
    end

    Drag.Reset()

    return true
end

function Drag.Reset()
	dragInProgress = false
    Drag.QuantitySelected = false
    dragData = {}
    dragSender = nil
    dragReceiver = nil
    Drag.DischargeCursor()
    Drag.JustStarted = Drag.Dropped
end

-- set drag data parameter
function Drag.SetParam(key, val)
    Debug("Drag.SetParam("..Args2String(key, val)..")")

    dragData[key] = val
end

-- get drag data parameter
function Drag.GetParam(key, def)
	if Drag.HasParam(key) then
		return dragData[key]
	else
		return def
	end
end

-- has drag data parameter
function Drag.HasParam(key)
    if not dragData[key] then
        return false
    else
        return true
    end
end

-- is drag and drop started and not yet ended
function Drag.IsStarted()
    return dragInProgress
end

-- charge cursor
function Drag.ChargeCursor(set, image, size)
	if set and image then
    	dragIcon.imageset = set
    	dragIcon.image = image
    	Quantity.SetImage(set, image)
	end
    if size then
        dragIcon.area = Rect(Point(0,0), size)
    end
    dragIcon.visible = true

    gui.cursor.type = "CursorDrag"
    
    -- update drag icon
    local count = Drag.GetParam("Quantity", 1)
    if count > 1 then
        dragText.visible = true
        dragText.text = tostring(count)
    else
    	dragText.visible = false
    end
end

-- discharge cursor
function Drag.DischargeCursor()
    dragIcon.imageset = ""
    dragIcon.image = ""
    dragIcon.visible = false
    dragText.visible = false
    Quantity.SetImage("", "")

    gui.cursor.type = "CursorNormalOver"
end

--------------------------------------
ActiveCursor = {}

-- drag types
Drag.Type = {}
Drag.Type.Invalid = 0
Drag.Type.InventoryItem = 1
Drag.Type.EquipmentItem = 2
Drag.Type.ContainerItem = 3
--Drag.Type.ContainerObject = 4
--Drag.Type.Purse = 5
Drag.Type.ExchangeItem = 6
Drag.Type.BankItem = 7
Drag.Type.LostOfficeItem = 8
Drag.Type.Action = 100
Drag.Type.Cash = 101
Drag.Type.Ability = 102
Drag.Type.ItemSelection = 103
Drag.Type.ShopSellListItem = 104
Drag.Type.ExchangeMoney = 105 

-- active cursor actions
ActiveCursor.Action = {}
ActiveCursor.Action.None = 0
ActiveCursor.Action.Drag = 1

-- container definitions
Cont = {}
Cont.Bag1 = 1
Cont.Bag2 = 2
Cont.Bag3 = 3
Cont.Bag4 = 4
Cont.Loot = -1
Cont.ShopBuyList = -2
Cont.ItemSelectionAvailable = -3
Cont.ItemSelectionEquipped = -4
Cont.ShopSellList = -5

local action = ActiveCursor.Action.None
local type = Drag.Type.Invalid
local srcContainer = nil
local srcCellIndex = nil

--[[function ActiveCursor.GetAction()
	return action
end

function ActiveCursor.GetType()
	return type, srcContainer
end]]

function ActiveCursor.On_Escape()
	if Drag.IsStarted() then
		Drag.SetParam("Cancel", true)
		ActiveCursor.Reset()
	    gui:unfreezeDrag()
	    gui:stopDrag()
		Drag.Reset()
		Drag.JustStarted = false
		return true
	else
		return false
	end
end

-- reset active cursor
function ActiveCursor.Reset()
	Debug("ActiveCursor.Reset()")
	
	if action == ActiveCursor.Action.Drag then
		local senderSlot = Drag.GetSender()
		if senderSlot and Drag.GetParam("Return") then
			Slot.SetState(senderSlot, Slot.State.Default)
		end
	end

	Drag.DischargeCursor()
	action = ActiveCursor.Action.None
	type = Drag.Type.Invalid
	srcContainer = nil
	srcCellIndex = nil
	Drag.JustStarted = false
	
	Quantity.Cancel()
	
	local gameState = GameState.GetState()
	if gameState == GameState.CharSelection then
		ItemSelection.HighlightSlots(nil)
	elseif (gameState == GameState.Game) or (gameState == GameState.EnteringGame) then
		Inventory.HighlightSlots(nil)
    	CharEquip.HighlightSlots(nil)
    	ActionBar.HighlightSlots(nil)
    	Bag.HighlightSlots(nil)
    	Exchange.HighlightSlots(nil)
    end
end

-- drop 
function ActiveCursor.Drop()
	Debug("ActiveCursor.Drop()...")
	
	if action ~= ActiveCursor.Action.Drag then
		return
	end
	
	Debug("...we're in drag&drop...")
		
	if Drag.GetParam("Cancel") then
		return
	end
	
	Drag.SetReceiver(gui.root)
	
	-- Inventory
	if type == Drag.Type.InventoryItem then
		Debug("..."..tostring(Drag.GetParam("Quantity", 1)).." inventory item(s)")
		
		local function shortcut_checker(slot)
			if not Slot.IsEmpty(slot) then
				local intItem = ItemStorage.GetByID(slot.id)
				for qbSlotID, refCount in pairs(intItem.qbSlots) do
					if refCount then
						Debug("quick bar slot id: "..tostring(qbSlotID))
						Debug("quick bar slot refcount: "..tostring(intItem.qbSlots[qbSlotID]))
						
						local qbSlot = Slot.GetByID(qbSlotID)
						Slot.SetItem(qbSlot, nil)
						ActionBar.SetSlotState(qbSlot.areaSubType, qbSlot.cellID, {container = 0, id = 0})
						intItem.qbSlots[qbSlotID] = nil
					end
				end
			end
		end
		
		local senderSlot = Drag.GetSender()
	    if senderSlot.type == Slot.Type.Item then
		    Quantity.TryToShow(Quantity.Drop)
	    end
		
		Drag.SetParam("Return", false)
		local senderSlot = Drag.GetSender()
		Slot.SetState(senderSlot, Slot.State.Locked, Slot.SubState.InDrop)
		
		shortcut_checker(senderSlot)
		
		if (senderSlot.areaType == Slot.Area.Inventory) and (senderSlot.areaSubType == 1) then -- bag slot
			local bagIdx = senderSlot.cellID - 15
			Bag.ForEachItemSlot(bagIdx, shortcut_checker)
		end
		
		local int = act_man:GetInterface()
	    int:DropItem(srcContainer, srcCellIndex, Drag.GetParam("Quantity", 1))
	
	-- Action
    elseif (type == Drag.Type.Action) then
    	Debug("...action")
    	local senderSlot = Drag.GetSender()
    	if senderSlot.type == Slot.Type.Ability then
    		local abil = AbilityStorage.GetByID(senderSlot.abilID)
    		abil.ranks[senderSlot.rank].qbSlots[senderSlot.slotID] = nil
    	elseif senderSlot.type == Slot.Type.Item then
    		local item = ItemStorage.GetByID(senderSlot.id)
    		item.qbSlots[senderSlot.slotID] = nil
    	end
    	Slot.SetAbility(senderSlot, nil) -- type doesn't matter here
    	ActionBar.SetSlotState(senderSlot.areaSubType, senderSlot.cellID, {container = 0, id = 0})
    	
    -- Sell list item
    elseif type == Drag.Type.ShopSellListItem then
    	Debug("...shop sell list item #"..tostring(srcCellIndex))
    	
    	--[[local shopData = act_man:GetPlayer():GetShopData()
		local sellList = shopData:GetSellList()
		sellList:RemoveItem(srcCellIndex)
		Shop_SellTab.UpdateWindow()]]
    	
    -- Exchange
    elseif (type == Drag.Type.ExchangeItem) then
    	Debug("...exchange item")
    	--local controller = PlayerStatus.GetPlayer():GetExchangeController()
    	--controller:RemoveItem(srcCellIndex)
    	
    -- Money
    elseif (type == Drag.Type.Cash) then
    	Debug("...cash")
    	
	    Quantity.TryToShow(Quantity.Drop)
    	
    	local int = act_man:GetInterface()
		int:DropCoins(srcContainer, Drag.GetParam("Quantity", 1))
		
	-- Item selection
    elseif type == Drag.Type.ItemSelection then
    	Debug("..item selection")
    	
    	local senderSlot = Drag.GetSender()
    	
    	if srcContainer == Cont.ItemSelectionEquipped then
    		Debug("equipped")
    		
    		local linkSlot = ItemSelection.GetAvailableSlot(senderSlot.idx)
    		Slot.SetSelected(linkSlot, false)
    		
    		charselect:RemoveItem(senderSlot.cellID)
    		Slot.SetItemPrototype(senderSlot, nil)
    		
    		local selectedSlot = ItemSelection.GetSelectedSlot()
    		if selectedSlot and (selectedSlot.slotID == senderSlot.slotID) then
    			ItemSelection.SetSelectedSlot(nil)
    		end
    	end    	
    end

	-- reset active cursor
	ActiveCursor.Reset()
end

function ActiveCursor.CanSplitStack(quantity)
	Debug("ActiveCursor.CanSplitStack("..tostring(quantity)..")")
	
	if action == ActiveCursor.Action.Drag then
		if (type == Drag.Type.InventoryItem) or (type == Drag.Type.ContainerItem) then
			local item = nil
			if srcContainer.BuyItemByIndex then -- try to buy item
            	item = srcContainer:GetItemImplByIndex(srcCellIndex)
            	if item:IsInfinite() then
            		return true
            	end
            else
            	local slot = Drag.GetSender()
            	item = Slot.GetItem(slot)
            end
			if item and item:IsValid() then
				local itemQuantity = item:GetQuantity()
				Debug("itemQuantity = "..tostring(itemQuantity))
				if quantity then
					if (itemQuantity > 1) and (quantity < itemQuantity) then
						return true
					else
						Debug("Invalid stack size!")
					end
				else
					return true
				end
			else
				Debug("Invalid item!")
			end
		elseif type == Drag.Type.Cash then
			local char = PlayerStatus.GetPlayer():GetCharacter()
			local stats = char:GetStatisticsActivity()
			local wealth = stats:GetWealth()
		
			if quantity then
				if (wealth > 1) and (quantity < wealth) then
					return true
				else
					Debug("Invalid stack size!")
				end
			else
				return true
			end
		end
	end
	
	return false
end

function ActiveCursor.SplitStack(quantity)
	Debug("ActiveCursor.SplitStack("..tostring(quantity)..")")
	
	if ActiveCursor.CanSplitStack(quantity) then
		Drag.SetParam("Quantity", quantity)
	end
end

function ActiveCursor.Use()
	Debug("ActiveCursor.Use()")
	
	if action == ActiveCursor.Action.Drag then
		if type == Drag.Type.Action then
			local slot = Drag.GetSender()
		
			if Slot.IsEmpty(slot) then
        		Debug("No shortcut to use!")
        		ActiveCursor.Reset()
        		return
    		end
        
        	ActiveCursor.Reset()
        
        	if slot.type == Slot.Type.Ability then
        		Debug("Use Ability!")

        		AbilityStorage.Use(slot.id, slot.rank)
    		elseif slot.type == Slot.Type.Item then
    			Debug("Use Item!")
				
        		local sourceSlot = ItemStorage.GetByID(slot.id).slot
        		Slot.UseClick(sourceSlot)
    		else
    			Debug("Use: Invalid Shortcut Type!")
			end
		elseif type == Drag.Type.Ability then
			
		elseif type == Drag.Type.InventoryItem then
			Debug("Inventory item")
		
			local slot = Drag.GetSender()
		
			if Shop.IsVisible() and Shop_SellTab.IsVisible() then
        		local sellMethod = Shop_SellTab.GetSellMethod()
        		if sellMethod == Shop.SellMethod.Tab then
   					return ActiveCursor.PickupContainerItem(Cont.ShopSellList, -1)
        		elseif sellMethod == Shop.SellMethod.RMB then
        			local sellList = Shop_SellTab.GetSellList()
        			Shop_SellTab.DisableUpdate()
        			sellList:PutItem(Inventory.GetContainer(), slot.cellID, 1)
        			sellList:SellItems()
        			Shop_SellTab.EnableUpdate()
        			Shop_SellTab.ClearSellList()
        			--Shop_SellTab.UpdateWindow()
        		else
        			Warning("Invalid sell method!")
        		end
        	else
        		local intItem = ItemStorage.GetByID(slot.id)
        		local itemProto = intItem.item:GetPrototype()
        		if not (itemProto:GetItemType() == ItemPrototype.ITEM_CLICKABLE) then
					for qbSlotID, refCount in pairs(intItem.qbSlots) do
						Debug("quick bar slot id: "..tostring(qbSlotID))
						Debug("quick bar slot refcount: "..tostring(intItem.qbSlots[qbSlotID]))
						
						local qbSlot = Slot.GetByID(qbSlotID)
						if not intItem.qbSlots[qbSlotID] then
							intItem.qbSlots[qbSlotID] = 1
						else
							intItem.qbSlots[qbSlotID] = intItem.qbSlots[qbSlotID] + 1
						end
					end
				end
        	
        		if itemProto:GetID() == 2 then -- system_money
        			Debug("money item")
        			local char = PlayerStatus.GetPlayer():GetCharacter()
					local purse = char:GetPurse()
        			act_man:GetInterface():SwapCoins(Inventory.GetContainer(), slot.cellID, purse, 0, Drag.GetParam("Quantity", 1))
        		elseif itemProto:GetItemType() == ItemPrototype.ITEM_CONTAINER then
        			Debug("container item")
        			local cellID = Inventory.GetFreeBagCell()
        			if cellID > 0 then
        				act_man:GetInterface():SwapItems(Inventory.GetContainer(), slot.cellID, Inventory.GetContainer(), cellID, Drag.GetParam("Quantity", 1))
        			else
        				OSD.Show("NOT_ENOUGH_SPACE")
        			end
        		else
        			Debug("item")
        			   			
        			if not IsItemBroken(intItem.item) then
        				Debug("valid, not broken item")
        				act_man:GetInterface():UseItem(srcContainer, slot.cellID)
        			else
        				Debug("broken item")
        				OSD.Show("BROKEN_ITEM")
        			end
        		end
        		Slot.SetState(slot, Slot.State.Locked, Slot.SubState.InProgress)
        	end
		elseif type == Drag.Type.EquipmentItem then
			local slot = Drag.GetSender()
		
			if Inventory.GetFirstFreeCell() == -1 then
        		return
        	end
        	
        	local intItem = ItemStorage.GetByID(slot.id)
			for qbSlotID, refCount in pairs(intItem.qbSlots) do
				Debug("quick bar slot id: "..tostring(qbSlotID))
				Debug("quick bar slot refcount: "..tostring(intItem.qbSlots[qbSlotID]))
				
				local qbSlot = Slot.GetByID(qbSlotID)
				if not intItem.qbSlots[qbSlotID] then
					intItem.qbSlots[qbSlotID] = 1
				else
					intItem.qbSlots[qbSlotID] = intItem.qbSlots[qbSlotID] + 1
				end
			end
        	
        	act_man:GetInterface():UseItem(CharEquip.GetContainer(), slot.cellID)
        	Slot.SetState(slot, Slot.State.Locked, Slot.SubState.InProgress)
		elseif type == Drag.Type.ContainerItem then
			if srcContainer.BuyItemByIndex then -- try to buy item
            	local item = srcContainer:GetItemImplByIndex(srcCellIndex)
            	
            	--if Shop_BuyTab.GetPayMethod() == 0 then
	            --	if item:GetPrice() > PlayerStats.GetMoney() then
	            --		OSD.Show("NOT_ENOUGH_MONEY")
	            --		ActiveCursor.Reset()
	            --		return
	        	--	end
        		--end
        		
        		if Inventory.GetFirstFreeCell() == -1 then
        			OSD.Show("INVENTORY_FULL")
        			ActiveCursor.Reset()
            		return
        		end
        		
        		srcContainer:BuyItemByIndex(srcCellIndex, Drag.GetParam("Quantity", 1), Shop_BuyTab.GetPayMethod())
			end
		end
		
		ActiveCursor.Reset()
	end
end

-- pickup ability
function ActiveCursor.PickupAbility(abilID, rank)
	Debug("ActiveCursor.PickupAbility("..Args2String(abilID, rank)..")")

	if action == ActiveCursor.Action.None then
		local abil = AbilityStorage.GetByID(abilID)
		if not rank then
			rank = AbilityStorage.GetMaxRank(abil)
		end
	
		local gameAbil = abil.ranks[rank].ability
		local image = gameAbil:GetTextureName()
	
		Drag.SetSender(gameAbil)
		Drag.SetParam("AbilID", abilID)
		Drag.SetParam("Rank", rank)
	
		local imageset, image = GetAbilityImage(gameAbil)
		Drag.ChargeCursor(imageset, image, Size(32,32))
		action = ActiveCursor.Action.Drag
		type = Drag.Type.Ability
		
		ActionBar.HighlightSlots(gameAbil)
		return true
	else
		return false
	end
end

-- pickup action from action bar
function ActiveCursor.PickupAction(barID, cellID)
    Debug("ActiveCursor.PickupAction("..Args2String(barID, cellID)..")")

	if cellID < 0 then
		ActiveCursor.Reset()
		return false
	end

	local slot = ActionBar.GetSlot(barID, cellID)
	if action == ActiveCursor.Action.None then -- no active action - pickup action
		Debug("action == None")
		if Slot.IsEmpty(slot) then
			return false
		end
		
		-- set drag and drop params
		Drag.SetSender(slot)
		Drag.SetParam("Return", true)
		Drag.SetParam("Quantity", 1)
		-- slot and cursor feedback
		Slot.SetState(slot, Slot.State.Locked, Slot.SubState.InProgress)
		
		-- update cursor state
		Drag.ChargeCursor(slot.image.imageset, slot.image.image, Size(32,32))
		action = ActiveCursor.Action.Drag
		type = Drag.Type.Action
		
		ActionBar.HighlightSlots({IsValid = function() return true end})
		return true
		
	elseif action == ActiveCursor.Action.Drag then -- dragging smth - try to drop
		Debug("action == Drag")
		
		if type == Drag.Type.Action then
			Drag.Dropped = true
			
			local senderSlot = Drag.GetSender()
			
			-- remember item or ability
			local item = nil
			local slotType = slot.type
			local senderCurTime = senderSlot.curTime
			local senderMaxTime = senderSlot.maxTime
			local senderAbilID = senderSlot.abilID
			local senderRank = senderSlot.rank
			local curTime = slot.curTime
			local maxTime = slot.maxTime
			local abilID = slot.abilID
			local rank = slot.rank
			local senderSlotState = ActionBar.CopySlotState(senderSlot.areaSubType, senderSlot.cellID)
			local slotState = ActionBar.CopySlotState(barID, cellID)
			
			Debug("slotType = "..tostring(slotType))
			if (slot.type == Slot.Type.Item) and (not Slot.IsEmpty(slot)) then
				Debug("saving item...")
				item = ItemStorage.GetByID(slot.id)
				item.qbSlots[slot.slotID] = nil
				item = item.item
			elseif (slot.type == Slot.Type.Ability) and (not Slot.IsEmpty(slot)) then
				Debug("saving ability...")
				item = AbilityStorage.GetByID(slot.abilID)
				item.ranks[slot.rank].qbSlots[slot.slotID] = nil
				item = item.ranks[slot.rank].ability
			end
			
			-- set new item or ability
			if senderSlot.type == Slot.Type.Item then
				local item = ItemStorage.GetByID(senderSlot.id)
				item.qbSlots[senderSlot.slotID] = nil
				Slot.SetItem(slot, Slot.GetItem(senderSlot))
			elseif senderSlot.type == Slot.Type.Ability then
				local abil = AbilityStorage.GetByID(senderSlot.abilID)
				abil.ranks[senderSlot.rank].qbSlots[senderSlot.slotID] = nil
				abil = abil.ranks[senderSlot.rank].ability
				Slot.SetAbility(slot, abil)
			end
			
			-- restore old item or ability in sender slot
			Debug("slotType = "..tostring(slotType))
			if slotType == Slot.Type.Item then
				Debug("restoring item...")
				Slot.SetItem(senderSlot, item)
			elseif slotType == Slot.Type.Ability then
				Debug("restoring ability...")
				
				Slot.SetAbility(senderSlot, item)
			elseif slotType == Slot.Type.None then
				Debug("clearing slot...")
				
				Slot.SetItem(senderSlot, nil)
			end
			
			senderSlot.curTime = curTime
			senderSlot.maxTime = maxTime
			senderSlot.abilID = abilID
			senderSlot.rank = rank
			slot.curTime = senderCurTime
			slot.maxTime = senderMaxTime
			slot.abilID = senderAbilID
			slot.rank = senderRank
			
			ActionBar.SetSlotState(senderSlot.areaSubType, senderSlot.cellID, slotState)
			ActionBar.SetSlotState(barID, cellID, senderSlotState)
						
		elseif (type == Drag.Type.InventoryItem) or (type == Drag.Type.EquipmentItem) then
			Drag.Dropped = true
		
			local senderSlot = Drag.GetSender()
			Slot.SetItem(slot, Slot.GetItem(senderSlot))
			
			ActionBar.SetSlotState(barID, cellID, ActionBar.GetStateFromSlot(senderSlot))
		elseif (type == Drag.Type.Ability) then
			Drag.Dropped = true
		
			Slot.SetAbility(slot, Drag.GetSender())
			slot.rank = Drag.GetParam("Rank")
			slot.abilID = Drag.GetParam("AbilID")
			
			ActionBar.SetSlotState(barID, cellID, ActionBar.GetStateFromSlot(slot))
		end
		
		-- update active cursor
		ActiveCursor.Reset()
		return true
	end
end

-- pickup item from inventory
function ActiveCursor.PickupInventoryItem(cellID)
	Debug("ActiveCursor.PickupInventoryItem("..tostring(cellID)..")")
	
	if (cellID == -1) and (action == ActiveCursor.Action.Drag) then
		Drag.Dropped = true
			
		local senderSlot = Drag.GetSender()
		local int = act_man:GetInterface()
		int:SwapItems(srcContainer, srcCellIndex, Inventory.GetContainer(), cellID, Drag.GetParam("Quantity"))
		Drag.SetParam("Return", false)
		ActiveCursor.Reset()
		return true
	end
	
	if cellID < 0 then	
		ActiveCursor.Reset()
		return false
	end

	local slot = Inventory.GetSlot(cellID)
	if action == ActiveCursor.Action.None then -- no active action - pickup item from inv
		if Slot.IsEmpty(slot) then
			return false
		end
		
		if Slot.GetState(slot) ~= Slot.State.Default then
			return false
		end
		
		local item = Slot.GetItem(slot)
		if (not item) or (not item:IsValid()) then
			return false
		end
		
		-- set drag and drop params
		Drag.SetSender(slot)
		Drag.SetParam("Quantity", item:GetQuantity())
		Drag.SetParam("MaxQuantity", item:GetQuantity())
		Drag.SetParam("Return", true)
		srcContainer = Inventory.GetContainer()
		srcCellIndex = cellID
		-- slot and cursor feedback
		Slot.SetState(slot, Slot.State.Locked, Slot.SubState.InProgress)
		
		-- update cursor state
		Drag.ChargeCursor(slot.image.imageset, slot.image.image, Size(32,32))
		action = ActiveCursor.Action.Drag
		type = Drag.Type.InventoryItem
		
		Inventory.HighlightSlots(item, slot)
		Bag.HighlightSlots(item, slot)
    	CharEquip.HighlightSlots(item)
    	ActionBar.HighlightSlots(item)
    	Exchange.HighlightSlots(item, slot)
    	return true
    	
	elseif action == ActiveCursor.Action.Drag then -- dragging smth - try to drop
		-- Action
		if type == Drag.Type.Action then
			-- Anything?
			
		-- Ability
		elseif type == Drag.Type.Ability then
			-- Anything?
			
		elseif type == Drag.Type.Cash then
			Drag.Dropped = true
		
			if ItemStorage.CanDropMoneyToSlot(Money.Cash, slot) then
				local int = act_man:GetInterface()
				int:SwapCoins(srcContainer, srcCellIndex, Inventory.GetContainer(), cellID, Drag.GetParam("Quantity", 1))
				Drag.SetParam("Return", false)
				if Slot.IsEmpty(slot) then
					Slot.SetImage(slot, "Items", "SYSTEM_MONEY")
					Slot.SetQuantity(slot, Drag.GetParam("Quantity", 1))
				end
				Slot.SetState(slot, Slot.State.Locked, Slot.SubState.InProgress)
			end
			
		-- Container
		elseif type == Drag.Type.ContainerItem then
			Drag.Dropped = true
			
			local senderSlot = Drag.GetSender()
			
			if srcContainer.BuyItemByIndex then -- try to buy item
				ActiveCursor.Use()
			else
				if ItemStorage.CanDropToSlot(Slot.GetItem(senderSlot), slot, senderSlot) then
					Drag.SetReceiver(slot)
					local int = act_man:GetInterface()
					int:SwapItems(srcContainer, srcCellIndex, Inventory.GetContainer(), cellID, Drag.GetParam("Quantity", 1))
					Drag.SetParam("Return", false)
					if Slot.IsEmpty(slot) then
						Slot.SetItem(slot, Slot.GetItem(senderSlot))
						Slot.SetQuantity(slot, Drag.GetParam("Quantity", 1))
					end
					Slot.SetState(slot, Slot.State.Locked, Slot.SubState.InProgress)
				end
			end
			
		-- Exchange item
		elseif type == Drag.Type.ExchangeItem then
			Drag.Dropped = true
		
			local controller = PlayerStatus.GetPlayer():GetExchangeController()
    		controller:RemoveItem(srcCellIndex)
    		local linkSlot = Exchange.PopLinkSlot(srcCellIndex)
			Slot.SetState(linkSlot, Slot.State.Default)
			
		-- Exchange money
		elseif type == Drag.Type.ExchangeMoney then
			Drag.Dropped = true
			
			local controller = PlayerStatus.GetPlayer():GetExchangeController()
    		controller:RemoveItem(srcCellIndex)
    		
    	-- Shop sell list
    	elseif type == Drag.Type.ShopSellListItem then
    		Drag.Dropped = true
    	
    		local shopData = act_man:GetPlayer():GetShopData()
			local sellList = shopData:GetSellList()
			sellList:RemoveItem(srcCellIndex)
			local linkSlot = Shop_SellTab.PopLinkSlot(srcCellIndex)
			Slot.SetState(linkSlot, Slot.State.Default)
			Shop_SellTab.UpdateWindow()
		
		-- Other stuff
		else
			Drag.Dropped = true
			
			local senderSlot = Drag.GetSender()
			if senderSlot.slotID ~= slot.slotID then -- don't drop to self
				if ItemStorage.CanDropToSlot(Slot.GetItem(senderSlot), slot, senderSlot) then
					Drag.SetReceiver(slot)
					local int = act_man:GetInterface()
					int:SwapItems(srcContainer, srcCellIndex, Inventory.GetContainer(), cellID, Drag.GetParam("Quantity"))
					Drag.SetParam("Return", false)
					if Slot.IsEmpty(slot) then
						Slot.SetItem(slot, Slot.GetItem(senderSlot))
						Slot.SetQuantity(slot, Drag.GetParam("Quantity", 1))
					end
					Slot.SetState(slot, Slot.State.Locked, Slot.SubState.InProgress)
				end
			end
		end
		
		-- update active cursor
		ActiveCursor.Reset()
		return true
	end
end

-- pickup item from equipment
function ActiveCursor.PickupEquipmentItem(cellID)
	Debug("ActiveCursor.PickupEquipmentItem("..tostring(cellID)..")")

	if cellID < 0 then
		ActiveCursor.Reset()
		return false
	end

	local slot = CharEquip.GetSlot(cellID)

	if action == ActiveCursor.Action.None then -- no active action - pickup item from equip
		if Slot.IsEmpty(slot) then
			return false
		end
		
		local item = Slot.GetItem(slot)
		if (not item) or (not item:IsValid()) then
			return false
		end
		
		-- set drag and drop params
		Drag.SetSender(slot)
		Drag.SetParam("Quantity", item:GetQuantity())
		Drag.SetParam("MaxQuantity", item:GetQuantity())
		Drag.SetParam("Return", true)
		srcContainer = CharEquip.GetContainer()
		srcCellIndex = cellID
		-- slot and cursor feedback
		Slot.SetState(slot, Slot.State.Locked, Slot.SubState.InProgress)
		
		-- update cursor state
		Drag.ChargeCursor(slot.image.imageset, slot.image.image, Size(32,32))
		action = ActiveCursor.Action.Drag
		type = Drag.Type.EquipmentItem
		
		Inventory.HighlightSlots(item, slot)
		Bag.HighlightSlots(item, slot)
    	CharEquip.HighlightSlots(item, slot)
    	ActionBar.HighlightSlots(item)
    	Exchange.HighlightSlots(item, slot)
    	return true
    	
	elseif action == ActiveCursor.Action.Drag then -- dragging smth - try to drop
		-- 
		if type == Drag.Type.Action then
			-- Anything?
		elseif type == Drag.Type.ContainerItem then
			-- Anything?
		else
			Drag.Dropped = true
		
			local senderSlot = Drag.GetSender()
			if senderSlot.slotID ~= slot.slotID then -- don't drop to self
				if ItemStorage.CanDropToSlot(Slot.GetItem(senderSlot), slot, senderSlot) then
					Drag.SetReceiver(slot)
					local int = act_man:GetInterface()
					int:SwapItems(srcContainer, srcCellIndex, CharEquip.GetContainer(), cellID, Drag.GetParam("Quantity"))
					Drag.SetParam("Return", false)
					if Slot.IsEmpty(slot) then
						Slot.SetItem(slot, Slot.GetItem(senderSlot))
						Slot.SetQuantity(slot, Drag.GetParam("Quantity", 1))
					end
					Slot.SetState(slot, Slot.State.Locked, Slot.SubState.InProgress)
				end
			end
		end
		
		-- update active cursor
		ActiveCursor.Reset()
		return true
	end
end

-- pickup item from container
function ActiveCursor.PickupContainerItem(contID, cellID)
	Debug("ActiveCursor.PickupContainerItem("..Args2String(contID, cellID)..")")
	
	local slot = nil
	if contID == Cont.Loot then
		if (action == ActiveCursor.Action.None) and (cellID == -1) then
			return false
		end
		
		if (action == ActiveCursor.Action.Drag) and (type == Drag.Type.ContainerItem) then
			return false
		end
	
		slot = Loot.GetSlot(cellID)
	elseif contID == Cont.ShopBuyList then
		slot = Shop_BuyTab.GetSlot(cellID)
	elseif contID == Cont.ItemSelectionAvailable then
		slot = ItemSelection.GetAvailableSlot(cellID)
	elseif contID == Cont.ItemSelectionEquipped then
		slot = ItemSelection.GetEquippedSlot(cellID)
	elseif contID == Cont.ShopSellList then
		if (action == ActiveCursor.Action.None) and (cellID == -1) then
			return false
		end
	
		if (action == ActiveCursor.Action.Drag) and (type ~= Drag.Type.InventoryItem) then
			return false
		end
		
		slot = Shop_SellTab.GetSlot(cellID)
	elseif (contID >= Cont.Bag1) and (contID <= Cont.Bag4) then
		slot = Bag.GetSlot(contID, cellID)
	end

	--if (not slot) or Slot.IsEmpty(slot) then
	if not slot then
		Debug("invalid slot")
		return false
	end

	if action == ActiveCursor.Action.None then -- no active action - pickup item from container
		Debug("trying to pickup item from container")
		
		local item = nil
		if contID == Cont.Loot then
			item = Slot.GetItem(slot)
			Drag.SetParam("Quantity", item:GetQuantity())
			Drag.SetParam("MaxQuantity", item:GetQuantity())
		elseif contID == Cont.ShopBuyList then
			local shopData = act_man:GetPlayer():GetShopData()
        	local buyList = shopData:GetBuyList()
        	item = buyList:GetItemImplByIndex(slot.cellID)
        	Drag.SetParam("Quantity", 1)
        	local qty = item:GetQuantity()
        	if qty == 0 then
        		Drag.SetParam("MaxQuantity", -1)
        	else
        		Drag.SetParam("MaxQuantity", item:GetQuantity())
        	end
		elseif contID == Cont.ItemSelectionAvailable then
			item = charselect:GetAvailableItemByIdx(slot.cellID)
			Drag.SetParam("Quantity", 1)
		elseif contID == Cont.ItemSelectionEquipped then
			item = charselect:GetAvailableItemByIdx(slot.idx)
			Drag.SetParam("Quantity", 1)
		elseif contID == Cont.ShopSellList then
			local shopData = act_man:GetPlayer():GetShopData()
        	local sellList = shopData:GetSellList()
        	item = sellList:GetItemImplByIndex(slot.cellID)
        	Drag.SetParam("Quantity", item:GetItemQuantity())
        	Drag.SetParam("MaxQuantity", item:GetItemQuantity())
        elseif (contID >= Cont.Bag1) and (contID <= Cont.Bag4) then
        	item = Slot.GetItem(slot)
        	if item and item:IsValid() then
				Drag.SetParam("Quantity", item:GetQuantity())
				Drag.SetParam("MaxQuantity", item:GetQuantity())
			end
		end
		
		if (not item) or (not item:IsValid()) then
			return false
		end
		
		type = Drag.Type.ContainerItem
		
		-- set drag and drop params
		Drag.SetSender(slot)
		Drag.SetParam("Return", true)
		if contID == Cont.Loot then
			srcContainer = Loot.GetContainer()
			srcCellIndex = slot.gmCellID
			Inventory.HighlightSlots(item, slot)
			Bag.HighlightSlots(item, slot)
		elseif contID == Cont.ShopBuyList then
			local shopData = act_man:GetPlayer():GetShopData()
			srcContainer = shopData:GetBuyList()
			srcCellIndex = slot.cellID
		elseif contID == Cont.ItemSelectionAvailable then
			srcContainer = Cont.ItemSelectionAvailable
			srcCellIndex = slot.cellID
			type = Drag.Type.ItemSelection
			ItemSelection.HighlightSlots(item)
		elseif contID == Cont.ItemSelectionEquipped then
			srcContainer = Cont.ItemSelectionEquipped
			srcCellIndex = slot.idx
			type = Drag.Type.ItemSelection
			ItemSelection.HighlightSlots(item)
		elseif contID == Cont.ShopSellList then
			local shopData = act_man:GetPlayer():GetShopData()
			srcContainer = shopData:GetSellList()
			srcCellIndex = slot.cellID
			type = Drag.Type.ShopSellListItem
			Inventory.HighlightSlots(item, slot)
			Bag.HighlightSlots(item, slot)
		elseif (contID >= Cont.Bag1) and (contID <= Cont.Bag4) then
			type = Drag.Type.InventoryItem
			srcContainer = Bag.GetContainer(contID)
			srcCellIndex = slot.cellID
			Inventory.HighlightSlots(item, slot)
			Bag.HighlightSlots(item, slot)
			CharEquip.HighlightSlots(item, slot)
    		ActionBar.HighlightSlots(item)
    		Exchange.HighlightSlots(item, slot)
		end
		-- slot and cursor feedback
		Slot.SetState(slot, Slot.State.Locked, Slot.SubState.InProgress)
		
		-- update cursor state
		Drag.ChargeCursor(slot.image.imageset, slot.image.image, Size(32,32))
		action = ActiveCursor.Action.Drag
		return true
		
	elseif action == ActiveCursor.Action.Drag then -- dragging smth - try to drop
		Debug("Dropping item to container")
		Debug("type:"..tostring(type))
		
		if contID == Cont.ShopBuyList then
			ActiveCursor.Reset()
			return
		end
		
		local function DropToBag(slot, senderSlot)
			if senderSlot.slotID ~= slot.slotID then -- don't drop to self
				if ItemStorage.CanDropToSlot(Slot.GetItem(senderSlot), slot, senderSlot) then
					Drag.SetReceiver(slot)
					local int = act_man:GetInterface()
					int:SwapItems(srcContainer, srcCellIndex, Bag.GetContainer(contID), cellID, Drag.GetParam("Quantity", 1))
					Drag.SetParam("Return", false)
					if Slot.IsEmpty(slot) then
						Slot.SetItem(slot, Slot.GetItem(senderSlot))
						Slot.SetQuantity(slot, Drag.GetParam("Quantity", 1))
					end
					Slot.SetState(slot, Slot.State.Locked, Slot.SubState.InProgress)
				end
			end
		end
		
		-- from inventory
		if type == Drag.Type.InventoryItem then
			Drag.Dropped = true
		
			local senderSlot = Drag.GetSender()
			Drag.SetReceiver(slot)
		
			-- to loot
			if contID == Cont.Loot then
				local int = act_man:GetInterface()
				int:SwapItems(srcContainer, srcCellIndex, Loot.GetContainer(), cellID, Drag.GetParam("Quantity", 1))
				Drag.SetParam("Return", false)
				if Slot.IsEmpty(slot) then
					Slot.SetItem(slot, Slot.GetItem(senderSlot))
					Slot.SetQuantity(slot, Drag.GetParam("Quantity", 1))
				end
				Slot.SetState(slot, Slot.State.Locked, Slot.SubState.InProgress)
				
			-- to shop sell list
			elseif contID == Cont.ShopSellList then
				if Slot.GetState(senderSlot) == Slot.State.InSellList then
					return
				end
			
				Debug("senderSlot.state: "..tostring(senderSlot.state))
				local shopData = act_man:GetPlayer():GetShopData()
				local sellList = shopData:GetSellList()
				sellList:PutItem(srcContainer, srcCellIndex, Drag.GetParam("Quantity", 1))
				Slot.SetState(senderSlot, Slot.State.Locked, Slot.SubState.InSellList)
				Drag.SetParam("Return", false)
				local item = Slot.GetItem(senderSlot)
				if item and item:IsValid() then
					local itemProto = item:GetPrototype()
					if Slot.SetItemPrototype(slot, itemProto) then
						Slot.SetBenefit(slot, 0)
						Slot.SetPrice(slot, 0, 0)
						Slot.SetQuantity(slot, Drag.GetParam("Quantity", 1))
						Slot.SetExtendedText(slot, GetString(itemProto:GetCodename(), "ItemNames"))
					end
				end
				Slot.SetState(slot, Slot.State.Locked, Slot.SubState.InProgress)
				Shop_SellTab.PushLinkSlot(senderSlot)
			elseif (contID >= Cont.Bag1) and (contID <= Cont.Bag4) then
				Debug("Dropping to bag: "..tostring(contID))
				DropToBag(slot, senderSlot)
			end
			
		elseif type == Drag.Type.EquipmentItem then
			Drag.Dropped = true
			
			local senderSlot = Drag.GetSender()
			Drag.SetReceiver(slot)
			
			if (contID >= Cont.Bag1) and (contID <= Cont.Bag4) then
				Debug("Dropping to bag: "..tostring(contID))
				DropToBag(slot, senderSlot)
			end

		elseif type == Drag.Type.ContainerItem then
			Drag.Dropped = true
			
			if (contID >= Cont.Bag1) and (contID <= Cont.Bag4) then
				local senderSlot = Drag.GetSender()
				
				if srcContainer.BuyItemByIndex then -- try to buy item
					ActiveCursor.Use()
				else
					if ItemStorage.CanDropToSlot(Slot.GetItem(senderSlot), slot, senderSlot) then
						Drag.SetReceiver(slot)
						local int = act_man:GetInterface()
						int:SwapItems(srcContainer, srcCellIndex, Bag.GetContainer(contID), cellID, Drag.GetParam("Quantity", 1))
						Drag.SetParam("Return", false)
						if Slot.IsEmpty(slot) then
							Slot.SetItem(slot, Slot.GetItem(senderSlot))
							Slot.SetQuantity(slot, Drag.GetParam("Quantity", 1))
						end
						Slot.SetState(slot, Slot.State.Locked, Slot.SubState.InProgress)
					end
				end
			end
			
		elseif type == Drag.Type.Cash then
			Debug("Drag type == cash")
			Drag.Dropped = true
			
			-- to loot
			if contID == Cont.Loot then
				Debug("Drop to loot")
				local int = act_man:GetInterface()
				--int:DropCoins(srcContainer, Drag.GetParam("Quantity", 1))
				int:SwapCoins(srcContainer, srcCellIndex, Loot.GetContainer(), cellID, Drag.GetParam("Quantity"))
				Drag.SetParam("Return", false)
			elseif (contID >= Cont.Bag1) and (contID <= Cont.Bag4) then
				Debug("Drop to bag")
				if ItemStorage.CanDropMoneyToSlot(Money.Cash, slot) then
					Debug("money->bag")
					local int = act_man:GetInterface()
					int:SwapCoins(srcContainer, srcCellIndex, Bag.GetContainer(contID), cellID, Drag.GetParam("Quantity", 1))
					Drag.SetParam("Return", false)
					if Slot.IsEmpty(slot) then
						Slot.SetImage(slot, "Items", "SYSTEM_MONEY")
						Slot.SetQuantity(slot, Drag.GetParam("Quantity", 1))
					end
					Slot.SetState(slot, Slot.State.Locked, Slot.SubState.InProgress)
				end
			end
		elseif type == Drag.Type.ItemSelection then
			Debug("item selection")
			
			Drag.Dropped = true
			
			local senderSlot = Drag.GetSender()
			if senderSlot.slotID ~= slot.slotID then -- don't drop to self
				local itemPrototype = charselect:GetAvailableItemByIdx(srcCellIndex)
				if ItemStorage.CanDropToSlot(itemPrototype, slot) then
					Drag.SetReceiver(slot)
					
					if not Slot.IsEmpty(slot) then
						local linkSlot = ItemSelection.GetAvailableSlot(slot.idx)
    					Slot.SetSelected(linkSlot, false)
					
						charselect:RemoveItem(slot.cellID)
						Slot.SetItemPrototype(slot, nil)
					end
					slot.idx = srcCellIndex
					charselect:AddItem(slot.cellID, itemPrototype)
					Drag.SetParam("Return", false)
					Slot.SetItemPrototype(slot, itemPrototype)
					Slot.SetQuantity(slot, Drag.GetParam("Quantity", 1))
					Slot.SetState(slot, Slot.State.Default)
					Slot.SetState(senderSlot, Slot.State.Default)
					Slot.SetSelected(senderSlot, true, Cl.Green)
					ItemSelection.SetSelectedSlot(slot)
				end
			end
		end
		
		-- update active cursor
		ActiveCursor.Reset()
		return true
	end
end

-- pickup exchange item
function ActiveCursor.PickupExchangeItem(cellID)
	Debug("ActiveCursor.PickupExchangeItem("..tostring(cellID)..")")

	if cellID < 0 then
		ActiveCursor.Reset()
		return
	end

	local slot = Exchange.GetPlayerSlot(cellID)

	if action == ActiveCursor.Action.None then -- no active action - pickup item from exchange
		if Slot.IsEmpty(slot) then
			return
		end
		
		local item = Slot.GetItem(slot)
		if (not item) or (not item:IsValid()) then
			return
		end
		
		-- set drag and drop params
		Drag.SetSender(slot)
		Drag.SetParam("Quantity", item:GetQuantity())
		Drag.SetParam("MaxQuantity", item:GetQuantity())
		Drag.SetParam("Return", true)
		srcContainer = Exchange.GetPlayerContainer()
		srcCellIndex = cellID
		-- slot and cursor feedback
		Slot.SetState(slot, Slot.State.Locked, Slot.SubState.InProgress)
		
		-- update cursor state
		Drag.ChargeCursor(slot.image.imageset, slot.image.image, Size(32,32))
		action = ActiveCursor.Action.Drag
		type = Drag.Type.ExchangeItem
	elseif action == ActiveCursor.Action.Drag then -- dragging smth - try to drop
		Drag.Dropped = true
		
		-- from inventory
		if type == Drag.Type.InventoryItem then
			local senderSlot = Drag.GetSender()
			if senderSlot.slotID ~= slot.slotID then
				local item = Slot.GetItem(senderSlot)
				if ItemStorage.CanDropToSlot(item, slot) then
					Drag.SetReceiver(slot)
					local controller = PlayerStatus.GetPlayer():GetExchangeController()
					controller:PutItem(srcContainer, srcCellIndex, cellID, Drag.GetParam("MaxQuantity")) -- FIXME: MaxQuantity -> Quantity
					Slot.SetState(senderSlot, Slot.State.Locked, Slot.SubState.InExchange)
					Drag.SetParam("Return", false)
					if Slot.IsEmpty(slot) then
						Slot.SetItem(slot, item)
						Slot.SetQuantity(slot, Drag.GetParam("Quantity", 1))
						Slot.SetExtendedText(slot, item:GetTooltip():GetParamName("name"))
					else
						local linkSlot = Exchange.PopLinkSlot(slot.cellID)
						Slot.SetState(linkSlot, Slot.State.Default)
					end
					Slot.SetState(slot, Slot.State.Locked, Slot.SubState.InProgress)
					Exchange.PushLinkSlot(senderSlot)
				end
			end
		elseif type == Drag.Type.Cash then
			local controller = PlayerStatus.GetPlayer():GetExchangeController()
			if controller:InExchange() then
				controller:PutItem(srcContainer, srcCellIndex, Exchange.MoneySlotID, Drag.GetParam("Quantity", 1))
				Drag.SetParam("Return", false)
				Slot.SetState(slot, Slot.State.Locked, Slot.SubState.InProgress)
			else
				Debug("Not in exchange")
			end
		end
		
		-- reset active cursor
		ActiveCursor.Reset()
	end
end

function ActiveCursor.PickupMoney(moneyType)
	Debug("ActiveCursor.PickupMoney("..tostring(moneyType)..")")

	if action == ActiveCursor.Action.None then -- no active action
		Drag.SetSender({ type = Slot.Type.Money })
		
		if moneyType == Money.Cash then
			local char = PlayerStatus.GetPlayer():GetCharacter()
			local stats = char:GetStatisticsActivity()
			local purse = char:GetPurse()
			local wealth = stats:GetWealth()
		
			Drag.SetParam("Quantity", wealth)
			Drag.SetParam("MaxQuantity", wealth)
			Drag.SetParam("Return", true)
			srcContainer = purse
			srcCellIndex = 0
			
			type = Drag.Type.Cash
		elseif moneyType == Money.Exchange then
			local controller = PlayerStatus.GetPlayer():GetExchangeController()
			if controller:InExchange() then
				local cont = controller:GetPlayerState():GetContainer()
				local item = cont:GetItem(Exchange.MoneySlotID)
				if item and item:IsValid() then
					local quantity = item:GetQuantity()
					Drag.SetParam("Quantity", quantity)
					Drag.SetParam("MaxQuantity", quantity)
					Drag.SetParam("Return", true)
					srcContainer = cont
					srcCellIndex = Exchange.MoneySlotID
					
					type = Drag.Type.ExchangeMoney
				else
					Debug("Invalid item")
					return
				end
			else
				Debug("Not in exchange")
				return
			end
		end
		
		-- update cursor state
		Drag.ChargeCursor("Money", "money", Size(32,32))
		action = ActiveCursor.Action.Drag
		
	elseif action == ActiveCursor.Action.Drag then -- dragging - try to drop
		Drag.Dropped = true
	
		-- from inventory
		if type == Drag.Type.InventoryItem then
			if moneyType == Money.Cash then
				local senderSlot = Drag.GetSender()
				local item = Slot.GetItem(senderSlot)
				if item and item:IsValid() then
					local proto = item:GetPrototype()
					if proto and proto:IsValid() then
						if proto:GetID() == 2 then -- money
							local char = PlayerStatus.GetPlayer():GetCharacter()
							local int = act_man:GetInterface()
							int:SwapCoins(srcContainer, srcCellIndex, char:GetPurse(), 0, Drag.GetParam("Quantity", 1))
							Drag.SetParam("Return", false)
						end
					end
				end
			end
		-- from container
		elseif type == Drag.Type.ContainerItem then
			if moneyType == Money.Cash then
				local senderSlot = Drag.GetSender()
				local item = Slot.GetItem(senderSlot)
				if item and item:IsValid() then
					local proto = item:GetPrototype()
					if proto and proto:IsValid() then
						if proto:GetID() == 2 then -- money
							local int = act_man:GetInterface()
							int:SwapItems(srcContainer, srcCellIndex, Inventory.GetContainer(), -1, Drag.GetParam("Quantity", 1))
							Drag.SetParam("Return", false)
						end
					end
				end
			end
		elseif type == Drag.Type.Cash then
			if moneyType == Money.Exchange then
				local controller = PlayerStatus.GetPlayer():GetExchangeController()
				if controller:InExchange() then
					controller:PutItem(srcContainer, srcCellIndex, Exchange.MoneySlotID, Drag.GetParam("Quantity", 1))
					Drag.SetParam("Return", false)
				else
					Debug("Not in exchange")
				end
			end
		elseif type == Drag.Type.ExchangeMoney then
			if moneyType == Money.Cash then			
				local controller = PlayerStatus.GetPlayer():GetExchangeController()
	    		controller:RemoveItem(srcCellIndex)
	    	end
		end
		
		-- reset active cursor
		ActiveCursor.Reset()
	end
end
