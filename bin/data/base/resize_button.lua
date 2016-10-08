ResizeButton = {
	State = {
	}
}


local startResizePos;
local counter = 1;

function ResizeButton.On_Clicked()
	print("ResizeButton.On_Clicked");
	counter = counter + 1;
	print("ResizeButton.On_Clicked "..counter);
end	

function ResizeButton.On_Load()
	-- counter = 1;
	-- print("ResizeButton.On_Load");

	-- local area = this.parent.area;
	-- local newSize = Size(area.size.width*2, area.size.height*2);
	-- local newRect = Rect(area.position, newSize);

	-- this.parent.area = newRect;

end	

function  ResizeButton.On_MouseMove()
	if startResizePos == nil then
		print("startResizePos is empty");
		return;
	end
	-- print("ResizeButton.On_MouseMove");

	local wnd_pos = this.parent.area.pos;
    local cursor = gui.cursor;
    local pos = cursor.position;
    local deltaPos = Point(pos.x-startResizePos.x, pos.y-startResizePos.y);
	-- -- local deltaPos =  - startResizePos;
	-- startResizePos = pos;

	-- local area = this.parent.area;
	-- local newSize = Size(area.size.width + deltaPos.x, area.size.height + deltaPos.y);
	-- local newRect = Rect(area.position, newSize);
	-- cursor.visible = true;
	-- this.parent.area = newRect;

	-- gui.cursor.type = "CursorResizerUpDown";
	--/.setSize(size);

end

function ResizeButton.On_MouseButton()
	print("ResizeButton.On_MouseButton");

    local cursor = gui.cursor;
    local pos = cursor.position;

	startResizePos = pos;

	-- local pos = Point(x, y)
 --    wnd.area = Rect(pos, area.size)
end
