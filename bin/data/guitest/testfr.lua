local counter = 1;

if counter == nil then
	counter = 1;
end
counter = counter + 1;
print ('Hello from Testfr.lua !  ' .. tostring(counter));

print ('Hello from Testfr.lua !  ' .. this.type);
print ('Hello from Testfr.lua !  ' .. this:getName());

function Test()
	--log:msg("Event "..args.name)
	--local test = eventArgs.mbutton
	--log:msg("Mouse "..test)
	local ma = to_mouseargs(eventArgs)
	log:msg("Mouse state "..tostring(ma.mstate))
	if ma.mstate == EventArgs.Up then
		local static = this:find("TestSubStaticWindow")
		local edit = to_editbox(this:find("TestSubEditWindow"))
		edit.password = not edit.password
		to_framewindow(this.parent).movable = not to_framewindow(this.parent).movable
		log:msg("Password mode is "..tostring(edit.password))
		to_statictext(static).text = "After click"
		--to_statictext(static):setText ("After click")
	end
end

function Hide()
	this.parent.parent.visible = false
end

function On_Load(wnd)
	log:msg('Callback test ' .. tostring(counter) .. "  " .. wnd:getName());
	counter = counter + 1;
	-- this.parent.parent.visible = false
end

local TestFrame = {};

function TestFrame:OnLoad(frame)
 self.view = frame;
	-- if self.counter == nil then
	-- 	self:counter = 1;
	-- else
	-- 	self:counter = self:counter + 1;
	-- end

	-- log:msg("TEST MSG "..tostring(self:counter))
end

function TestFrame.Test(frame)
end