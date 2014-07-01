-- function Test()
-- 	--log:msg("Event "..args.name)
-- 	--local test = eventArgs.mbutton
-- 	--log:msg("Mouse "..test)
-- 	local ma = to_mouseargs(eventArgs)
-- 	log:msg("Mouse state "..tostring(ma.mstate))
-- 	if ma.mstate == EventArgs.Up then
-- 		local static = this:find("TestSubStaticWindow")
-- 		local edit = to_editbox(this:find("TestSubEditWindow"))
-- 		edit.password = not edit.password
-- 		to_framewindow(this.parent).movable = not to_framewindow(this.parent).movable
-- 		log:msg("Password mode is "..tostring(edit.password))
-- 		to_statictext(static).text = "After click"
-- 		--to_statictext(static):setText ("After click")
-- 	end
-- end

-- function Hide()
-- 	this.parent.parent.visible = false
-- end

log:msg("Password mode is 5555552777777777");
-- log:msg(this.type);
function Test()
	local rect = Rect(1,1,2,2);
	rect.x111 = 5;
	rect.width = 412;
	-- print (rect.x111);
	log:msg("Password mode is 55555521   " .. rect.x111 .. " --- " .. rect.width);
end

Test();

local imageset = gui:getWindowManager():getImageset("skin");
log:msg('imageset name is ' .. imageset:getName());

local img = imageset:getImage("Background");
log:msg('image name is ' .. img:getName() );


local state = ImageButton.Normal;

--gui:loadXml('guitest/debug_menu.xml')