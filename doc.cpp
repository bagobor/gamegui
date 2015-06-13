XML files structure:	

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

	<Label> - для отображения текста
		Parameters:
			- Font 			- имя шрифта
			- Spacing		- float (надо проверить работу возможно работает с ошибками)
			- Centred		- true/false/0/1 (ref: Bool)
			- Text 			- отображаемый текст
			- Formatting 	- параметры форматирования текста (ref: TextFormatting)
				- RightAligned
				- Centred
				- Justified
				- WordWrapLeftAligned
				- WordWrapRightAligned
				- WordWrapCentred
				- WordWrapJustified
				- LeftAligned;

	ImageBox - для отображения статичной картинки
		Parameters:
			- Imageset			- имя имаджсета
			- Image 			- имя спрайта в имаджсете
			- VertFormatting	- вертикальный режим отображения [T|t]ile или [S|s]tretch (ref: ImageOps)
			- HorzFormatting	- горизонтальный режим отображения [T|t]ile или [S|s]tretch (ref: ImageOps)

	Button - кнопка с текстом, умеет корретно растягиваться, является расширением Label. состоит из 3х спрайтов (лево, серединка, право) и текста.
		Parameters:
			- FrameImagery 
				- атрибут Imageset - имя имаджсета из которого загружать спрайты кнопки.
				<Hovered|Pushed|Normal> - состояние кнопки 
					<Background|Left|Right Image="имя имаджа в имадж сете"/>

	<Panel> - панель с фоном и рамкой, умеет корретно изменять размер. имеет фон, рамку.
		<Parameters>:
			- <FrameImagery Imageset="имя имаджсета из которого загружать спрайты панели">
				<Background|Top|TopLeft|TopRight|Left|Right|Bottom|BottomLeft|BottomRight Image="имя имаджа в имадж сете"/>


	<FrameWindow> - окно, производное от Panel. уммет изменять размер. имеет фон, рамку и заголовок.
		<Parameters>:
			- Title 		- текст заголовка окна
			- Formatting 	- параметры форматирования текста заголовка (ref: TextFormatting ref: Label )
			- CaptionColor 	- цвет текста заголовка rgba "%f %f %f %f" (ref: Color)
			- Movable		- можно ли перетаскивать окно мышкой true/false/0/1 (ref: Bool)
			- ClampToScreen - ограничивает перемещение окна в пределах экрана true/false/0/1 (ref: Bool)
			- <Caption Imageset="имя имаджсета из которого загружать спрайты заголовка"
				- <Background|Left|Right Image="имя спрайта заголовка окна"

	<ImageButton - кнопка фиксированного размера с цельнами картнками состояний, является производной от просто Button 
		<Parameters>:
			- <StateImagery Imageset="имя имаджсета из которого загружать спрайты"
				<Hovered|Pushed|Normal Image="имя спрайта состояния кнопки"
	
	
	
	m_factory->RegisterCreator<Progress>();
	m_factory->RegisterCreator<ProgressBar>();
	m_factory->RegisterCreator<Combobox>();
	m_factory->RegisterCreator<CategorizedList>();
	m_factory->RegisterCreator<ScrollPane>();
	m_factory->RegisterCreator<BaseList>();
	m_factory->RegisterCreator<ListBox>();	
	m_factory->RegisterCreator<ImageBar>();
	m_factory->RegisterCreator<Thumb>();
	m_factory->RegisterCreator<ScrollThumb>();
	m_factory->RegisterCreator<Slider>();
	m_factory->RegisterCreator<ScrollBar>();
	m_factory->RegisterCreator<Checkbox>();
	m_factory->RegisterCreator<Editbox>();
	m_factory->RegisterCreator<KeyBinder>();
	m_factory->RegisterCreator<MarkupBase>();
	m_factory->RegisterCreator<MarkupText>();