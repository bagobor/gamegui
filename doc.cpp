	<root> - системного корневое окно.
		создается автоматически на старте системы
	 	в него добавляются остальные окна и контролы. 

	<base_window> - базовое окно (виджет) от которого произошли все остальные контролы, не имеет собственного отображения.
		Parameters:
			Visible - true/false/0/1 - определяет видимо ли это окно и все его дочерние окна. (ref: Bool)
			Align - список флагов разделенных символами ", |"
				[l|L]eft 	- 
				[t|T]op  	-
				[r|R]ight 	-
				[b|B]ottom 	-
				[h|H]center -
				[v|V]center -
			Stick - список флагов разделенных символами ", |"
				[l|L]eft 	- 
				[t|T]op  	-
				[r|R]ight 	-
				[b|B]ottom 	-
				[h|H]center -
				[v|V]center -
			StickRect - регион x y w h
			Area - область и позиция контрола (x y w h) "%f %f %f %f" (ref: Color)
				x,y - позиция левого верхнего угла 
				w,h - ширина и высота контрола
			Backcolor - фоновый цвет окна rgba "%f %f %f %f" (ref: Color)
			Forecolor - основной цвет окна rgba "%f %f %f %f" (ref: Color)
			TabStop - true/false/0/1 будет ли контрол получать фокус по клавише TAB (ref: Bool)
			Dragable - true/false/0/1 можно ли контрол перетаскивать (ref: Bool)
			AcceptDrop - 
			AlwaysOnTop
			Tooltip -  true/false/0/1 показывать ли тултип (ref: Bool)
			Size - "%f %f" w h размер контрола, 
			Pos - "%f %f" x y позиция левого верхнего угла контрола
			IgnoreInputEvents - true/false/0/1 игнорировать или нет контром события ввода. (ref: Bool)

		Events - Script Event Handlers, 
			- параметры вызова эвента передаются в переменной 'eventArgs':		  
				- string name - имя эвента 
				- bool handled - был ли он уже обработан

			Эвенты базового контрола:		
			- On_Draw - вызывается при рендере контрола. наличие этого обработчика включает кастомную отрисовку контрола через скрипт.
			- On_Load - вызывается после окончания загрузки контрола.
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
			- On_Tick - вызывается каждый апдейт. по умолчанию выключен. включается\выключается ф-иями startTick/stopTick
			- On_TooltipShow
			- On_TooltipHide

		Children - дочерние контролы

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