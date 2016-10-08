local counter = 1;

if counter == nil then
	counter = 1;
end
counter = counter + 1;
-- print ('Hello from Testfr.lua !  ' .. tostring(counter));

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