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