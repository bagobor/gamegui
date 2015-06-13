gamegui
=======


Game GUI library.

With Lua sripting support (heavy inspired by World Of Warcraft UI) 


XML files structure:
=======

	<root> - system 'root' window
		Create automaticly on system state
	 	This winwdow is 'parent' for all other windows.

	<base_window> - base window (widget) base class for all other widgets/windows. Have no any visual.
		Parameters:
			Visible - true/false/0/1 - determite window visibility (and all child windows) (type: Bool)
			Align - flags lis with delemeters ", |"
				[l|L]eft 	- 
				[t|T]op  	-
				[r|R]ight 	-
				[b|B]ottom 	-
				[h|H]center -
				[v|V]center -
			Stick - flags lis with delemeters ", |"
				[l|L]eft 	- 
				[t|T]op  	-
				[r|R]ight 	-
				[b|B]ottom 	-
				[h|H]center -
				[v|V]center -
			StickRect - region x y w h
			Area - windows size и position (x y w h) "%f %f %f %f" (type: Rect)
				x,y - top-left angle position
				w,h - widht and height
			Backcolor - window backgourd color rgba "%f %f %f %f" (type: Color)
			Forecolor - window fore (base or content) rgba "%f %f %f %f" (type: Color)
			TabStop - true/false/0/1  (type: Bool)
			Draggable - true/false/0/1  (type: Bool)
			AcceptDrop - 
			AlwaysOnTop
			Tooltip -  true/false/0/1 is tooltip enabled (type: Bool)
			Size - "%f %f" w h window size 
			Pos - "%f %f" x y windows position
			IgnoreInputEvents - true/false/0/1  (type: Bool)

		Events - Script Event Handlers, 
			- event params are passed as 'eventArgs' variable with fields:
				- string name - event name 
				- bool handled - 'true' if event already was handled by other window/control

			Base windows script events:
			- On_Draw - on window render. 
			- On_Load - called just after window fully created
			- On_MouseEnter
			- On_MouseLeave
			- On_MouseMove
			- On_MouseWheel
			- On_MouseButton
			- On_Char
			- On_KeyboardButton
			- On_CaptureGained
			- On_CaptureLost
			- On_SuspendLayout
			- On_ResumeLayout
			- On_FocusGained
			- On_FocusLost
			- On_Sized
			- On_Moved
			- On_Tick - called on each gui update. disabled by default. controlled by 'startTick' and 'stopTick' functions.
			- On_TooltipShow
			- On_TooltipHide

		Children - child widgets