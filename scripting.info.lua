Lua interface:

type: Point
---------------------
constructor:
    () - 0,0
    (x,y) - x,y
properties:
    - x: number
    - y: number


type: Size
---------------------
###constructor:
    - ()
    - (width, height)
###properties:
    - width: number
    - height: number


type: Rect
---------------------
###properties:
    - position:Point
    - size : Size
    - wight : number
    - height : float

###functions:
    -getPosition : Point
    -getWidth : number
    -getHeight : number
    -getSize : Size
    -setWidth(number)
    -setHeight(number)
    -setSize(Size)
    -getIntersection
    -offset(Point) - сдвигает текущий прямоугольник
    -isPointInRect(Point) : bool - проверяет попадает ли текущая точка во внутрь области
    -scale
    -scale_u


type: Color
-----------------
###constructors:
    - ()
    - (r,g,b,a)
###properties:
    -r,g,b,a: number


type: System
----------------------
###properties:
    - root : BaseWindow
    - cursor
    - tooltip
    - menu
    - tooltipdelay : number
    - tooltiplifetime : number

###functions:
    - getRootWindow() - return root window. (parent control for all other controls/panels)
    - getRenderer() - return renderer object
    - getWindowManager() - return window manager object
    - getCursor() - return cursor object. cursor is special control object to display mouse pointer(s)
    - getwindowWithFocus()
    - createWindow
    - log
    - find(name) - find root child control with 'name' specified.
    - startDrag
    - freezeDrag
    - unfreezeDrag (todo: rename to 'resume')
    - isSysKeyPressed(key)
    - loadXml (filename)
    - loadWindow (parent, filename)
    - queryCaptureInput


type: Cursor
-------------
###properties:
    - type - string
    - position - Point {x,y}


type: LogHelper - доступен через глобальную переменную 'log'
----------------
###functions:
    - warn(string)
    - err(string)
    - msg(string)
    - crit(string)


type: RenderHelper
----------------------
###functions:
        - drawImage
        - drawText


type: WindowManager
----------------------
###functions:
    - getImageset
    - getFont


type: Image
----------------------
###constants:
    ImageOps:
      - Stretch
      - Tile

###properties:
    - AdditiveBlend : bool - режим блендинга modulate/additive

###functions:
    - GetName()
    - GetSize
    - setAdditiveBlend
    - getAdditiveBlend


type: Imageset
--------------------
###functions:
    - getImage
    - getName
    - getImageCount
    - getImageByIdx



type: BaseWindow derived from ScriptObjectBase
------------------------------------------------
###properties:
    - parent : BaseWindow
    - name : string
    - area : Rect
    - type : string - тип контрола
    - visible : bool
    - enabled : bool
    - backcolor : color
    - forecolor : color
    - tag : number
    - alwaysOnTop : bool
    - draggable : bool
    - acceptDrop : bool


###functions:
    -getName : string
    -getType : string
    -setArea(Rect)
    -getArea : Rect
    -isCursorInside : bool
    -hitTest
    -setVisible(bool)
    -getVisible : bool
    -setEnabled(bool)
    -getEnabled : bool
    -setBackColor(color)
    -getBackColor : color
    -setForeColor(color)
    -getForeColor : color
    -setUserData(number)
    -getUserData : number
    -moveToFront()
    -bringToBack()
    -setAlwaysOnTop(bool)
    -getAlwaysOnTop : bool
    -enableTooltip(bool)
    -hasTooltip : bool
    -find(string) : BaseWindow
    -suspendLayout
    -resumeLayout
    -addChild(BaseWindow)
    -setInputFocus
    -resetInputFocus
    -hasInputFocus : bool
    -setIgnoreInputEvents
    -subscribe
    -unsubscribe
    -send
    -addScriptEventHandler
    -startTick
    -stopTick
    -transformToWndCoord
    -transformToRootCoord


global cast functions: приведение BaseWindow к нужному типу контролов. если приведение не возможно - вернет nil
-------------------
    - to_statictext
    - to_staticimage
    - to_checkbox
    - to_editbox
    - to_button
    - to_panel
    - to_framewindow
    - to_progress
    - to_thumb
    - to_slider
    - to_scrollbar
    - to_scrollpane
    - to_list
    - to_listbox
    - to_combobox
    - to_markup
    - to_tooltip
    - to_dragcontainer
    - to_menu
    - to_catlist

Конвертация цвета в число и на оборот:
    - color2hex
    - hex2color


<On_MouseKey>
    local mouseArgs = to_mouseargs(eventArgs);
    --mouseArgs.mbutton
    if mouseArgs.mstate == EventArgs.ButtonState.Down then
        -- show some window
    end
</On_MouseKey>

-- .def_readwrite("mbutton", &MouseEventArgs::mbutton)
-- .def_readwrite("mstate", &MouseEventArgs::mstate)
-- .def_readwrite("mouseX", &MouseEventArgs::mouseX)
-- .def_readwrite("mouseY", &MouseEventArgs::mouseY)
-- .def_readwrite("wheelDelta", &MouseEventArgs::wheelDelta)


type: StaticText
-------------------
###properties:
    - text : string
    - spacing : number
    - formatting

###functions:
    - setFont
    - setText
    - appendText
    - getText
    - setSpacing
    - getSpacing
    - setTextFormatting
    - getTextFormatting


type: ImageBox
---------------------
###properties:
    - imageset
    - image
    - vformat
    - hformat

###funtions:
    - setImageset
    - getImageset
    - setImage
    - getImage
    - setVertFormat
    - getVertFormat
    - setHorzFormat
    - getHorzFormat


type: FrameWindow
---------------------
###properties:
    - caption : string
    - movable : bool
    - clamp : bool
    - formatting
    - captioncolor : color

###functions:
    - setFont
    - setCaption
    - getCaption
    - setMovable
    - getMovable
    - setClamp
    - getClamp
    - setCaptionFormatting
    - getCaptionFormatting
    - setCaptionColor
    - getCaptionColor


type: Editbox
---------------------
###properties:
    - text
    - password
    - numeric

###functions:
    - setText
    - getText
    - setPassword
    - getPassword
    - setNumeric
    - isNumeric
    - clearSelection
    - setSelection
    - setCaretPos
    - getMaxLength
    - setMaxLength


type: Combobox
---------------------
###functions:
 - AddItem


type: Checkbox
---------------------
###properties:
    - checked : bool
#functions:
    - isChecked
    - setChecked


type: Progress
---------------------
###properties:
    - progress : number


type: Thumb
---------------------
###properties:
    - progress
    - trackarea

###functions:
    - getProgress
    - setProgress
    - getTrackarea
    - setTrackarea


type: ScrollThumb ref:Thumb
---------------------


type: Button ref Label
---------------------
###properties:
    - name : string

type: Slider
---------------------
###properties:
    - scroll
    - document
    - step

###functions:
    - getScroll
    - setScroll
    - getDocumentSize
    - setDocumentSize
    - getStepSize
    - setStepSize

type: ScrollBar : Slider
-------------------------

type: ScrollPane
-------------------------
###functions:
    - setTarget

type: BaseList
-------------------------
###properties:
    - columnwidth
    - columns

###functions:
 - Clear
 - getColumns
 - setColumns
 - getColumnWidth
 - setColumnWidth

type: ListBox : BaseList
-------------------------
###functions:
- AddItem
- GetSelectedItem

type: CategorizedList : BaseList
-------------------------
###functions:
- AddCategory
- GetCategoryByIndex
- GetCategoryByName
- GetCategoryCount
- RemoveCategory


type: CategorizedList::Category
----------------------------------
###functions:
    - Rename
    - Add
    - Remove
    - GetWndByIndex
    - GetChildrenCount
    - Collapse
    - Expand
    - IsCollapsed

type: MarkupBase : Label
-------------------------

type: MarkupText : MarkupBase
-------------------------

type: MenuItem
-------------------------
###functions:
    - constructor()

###properties
    - text
    - icon
    - col
    - selcol


type: Menu : Panel
-------------------------
###properties:
     - fadein
     - fadeout

###functions:
    - getItemById
    - getItem
    - addItem
    - clear
    - show
    - hide