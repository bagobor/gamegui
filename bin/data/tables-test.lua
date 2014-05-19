print('Hello world!');


AdvancedDoor = 
{
	counter = 0
};

function AdvancedDoor:OnReset()
	-- ':' automaticly passes 'self' to scope
	self.counter = self.counter + 1;
	print('AdvancedDoor:OnReset!  ' .. tostring(self.counter));
end;

-- local door = AdvancedDoor();
AdvancedDoor:OnReset();