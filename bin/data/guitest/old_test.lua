
local imageset = gui:getWindowManager():getImageset("skin");
log:msg('imageset name is ' .. imageset:getName());

local img = imageset:getImage("Background");
log:msg('image name is ' .. img:getName() );


local state = ImageButton.Normal;

--gui:loadXml('guitest/debug_menu.xml')