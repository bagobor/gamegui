#include "StdAfx.h"
#include "renderer.h"
#include "renderhelper.h"
#include "window.h"
#include "panel.h"
#include "dragcontainer.h"
#include "tooltip.h"
#include "menu.h"
#include "windowmanager.h"
#include "system.h"

#include "label.h"
#include "imagebox.h"
#include "editbox.h"
#include "button.h"
#include "checkbox.h"
#include "framewindow.h"
#include "progress.h"
#include "slider.h"
#include "scrollpane.h"
#include "list.h"
#include "combobox.h"
#include "markuptext.h"

#include "utils.h"

namespace gui
{

void System::makeLuaBinding(void)
{
	logEvent(log::system, "Making LUA bindings...");
	using namespace luabind;
	module(m_scriptSys.getLuaState())
		[
			class_ <System>("System")
			.property("root", &System::getRootWindow)
			.property("cursor", &System::getCursorConst)
			.property("tooltip", &System::getTooltip)
			.property("dragcontainer", &System::getDragContainer)
			.property("menu", &System::getMenu)
			.property("tooltipdelay", &System::getTooltipDelay, &System::setTooltipDelay)
			.property("tooltiplifetime", &System::getTooltipLifetime, &System::setTooltipLifetime)
			.def("getRootWindow", &System::getRootWindow)
			.def("getRenderer", &System::getRenderer)
			.def("getWindowManager", &System::getWindowManager)
			.def("getCursor", &System::getCursor)
			.def("getWindowWithFocus", &System::getWindowWithFocus)
			.def("createWindow", &System::createWindow)
			.def("log", &System::logEvent)
			.def("find", &System::find)
			.def("startDrag", &System::startDrag)
			.def("stopDrag", &System::stopDrag)
			.def("freezeDrag", &System::freezeDrag)
			.def("unfreezeDrag", &System::unfreezeDrag)
			.def("isSysKeyPressed", &System::isSysKeyPressed)
			.def("loadXml", (base_window*(System::*)(const std::string&))&System::loadXml)
			.def("loadWindow", (base_window*(System::*)(base_window&, const std::string&))&System::loadXml)
			.def("queryCaptureInput", &System::queryCaptureInput)
			,
			class_ <Cursor>("Cursor")
			.property("type", &Cursor::getType, &Cursor::setType)
			.property("position", &Cursor::getPosition, &Cursor::setPosition)
			,			
			class_ <Rect>("Rect")
			.def(constructor<>())
			.def(constructor<float, float, float, float>())
			.def(constructor<point, Size>())
			.property("position", &Rect::getPosition, &Rect::setPosition)
			.property("size", &Rect::getSize, &Rect::setSize)
			.property("width", &Rect::getWidth, &Rect::setWidth)
			.property("height", &Rect::getHeight, &Rect::setHeight)
			.def("getPosition", &Rect::getPosition)
			.def("getWidth", &Rect::getWidth)
			.def("getHeight", &Rect::getHeight)
			.def("getSize", &Rect::getSize)
			.def("setWidth", &Rect::setWidth)
			.def("setHeight", &Rect::setHeight)
			.def("setSize", &Rect::setSize)
			.def("getIntersection", &Rect::getIntersection)
			.def("offset", &Rect::offset)
			.def("isPointInRect", &Rect::isPointInRect)
			.def("scale", &Rect::scale)
            .def("scale_u", &Rect::scale_u)
			,
			class_ <point>("point")
			.def(constructor<>())
			.def(constructor<float, float>())
			.def_readwrite("x", &point::x)
			.def_readwrite("y", &point::y)
			,
			class_ <Size>("Size")
			.def(constructor<>())
			.def(constructor<float, float>())
			.def_readwrite("width", &Size::width)
			.def_readwrite("height", &Size::height)
			,
			class_ <Color>("Color")
			.def(constructor<>())
			.def(constructor<float, float, float, float>())
			.def(constructor<const Color&>())
			.property("r", &Color::getRed, &Color::setRed)
			.property("g", &Color::getGreen, &Color::setGreen)
			.property("b", &Color::getBlue, &Color::setBlue)
			.property("a", &Color::getAlpha, &Color::setAlpha)
			,
			class_ <log>("LogHelper")
			.def("warn", &log::wrn)
			.def("err", &log::err)
			.def("msg", &log::msg)
			.def("crit", &log::crit)
			,
			class_ <RenderHelper>("RenderHelper")
			.def("drawImage", &RenderHelper::drawImage)
			.def("drawText", &RenderHelper::drawText)
			,
			class_ <WindowManager>("WindowManager")
			.def("getImageset", &WindowManager::getImageset)
			.def("getFont", &WindowManager::getFont)
			,
			class_ <Image>("Image")
			.enum_("ImageOps")
			[
				value("Stretch", Stretch),
				value("Tile", Tile)
			]
			.def("GetName", &Image::GetName)
			.def("GetSize", &Image::GetSize)
			.property("AdditiveBlend", &Image::getAdditiveBlend, &Image::setAdditiveBlend)
			.def("setAdditiveBlend", &Image::setAdditiveBlend)
			.def("getAdditiveBlend", &Image::getAdditiveBlend)
			,
			class_ <Imageset>("Imageset")
			.def("getImage", &Imageset::GetImagePtr)
			.def("getName", &Imageset::GetName)
			.def("getImageCount", &Imageset::ImagesCount)
			.def("getImageByIdx", &Imageset::GetImageByIdx)
			,
			class_ <base_window, bases<ScriptObjectBase> >("BaseWindow")
			.property("parent", (base_window* (base_window::*)() const)&base_window::getParentConst)
			.property("name", &base_window::getName, &base_window::setName)
			.property("area", &base_window::getArea, &base_window::setArea)
			.property("type", &base_window::getType)
			.def("getName", &base_window::getName)
			.def("getType", &base_window::getType)
			.def("setArea", &base_window::setArea)
			.def("getArea", &base_window::getArea)
			.def("isCursorInside", &base_window::isCursorInside)
			.def("hitTest", &base_window::hitTest)
			.property("visible", &base_window::getVisible, &base_window::setVisible)
			.def("setVisible", &base_window::setVisible)
			.def("getVisible", &base_window::getVisible)
			.property("enabled", &base_window::getEnabled, &base_window::setEnabled)
			.def("setEnabled", &base_window::setEnabled)
			.def("getEnabled", &base_window::getEnabled)
			.property("backcolor", &base_window::getBackColor, &base_window::setBackColor)
			.def("setBackColor", &base_window::setBackColor)
			.def("getBackColor", &base_window::getBackColor)
			.property("forecolor", &base_window::getForeColor, &base_window::setForeColor)
			.def("setForeColor", &base_window::setForeColor)
			.def("getForeColor", &base_window::getForeColor)
			.property("tag", &base_window::getUserData, &base_window::setUserData)
			.def("setUserData", &base_window::setUserData)
			.def("getUserData", &base_window::getUserData)
			.def("moveToFront", (void(base_window::*)(void))&base_window::moveToFront)
			.def("bringToBack", (void(base_window::*)(void))&base_window::bringToBack)
			.property("alwaysOnTop", &base_window::getAlwaysOnTop, &base_window::setAlwaysOnTop)
			.def("setAlwaysOnTop", &base_window::setAlwaysOnTop)
			.def("getAlwaysOnTop", &base_window::getAlwaysOnTop)
			.def("enableTooltip", &base_window::enableTooltip)
			.def("hasTooltip", &base_window::hasTooltip)
			.def("find", &base_window::findChildWindow)
			.def("suspendLayout", &base_window::suspendLayout)
			.def("resumeLayout", &base_window::resumeLayout)
			.def("addChild", &base_window::addChildWindow)
			.def("setInputFocus", &base_window::setInputFocus)
			.def("resetInputFocus", &base_window::resetInputFocus)
			.def("hasInputFocus", &base_window::hasInputFocus)
			.def("setIgnoreInputEvents", &base_window::setIgnoreInputEvents)
			.def("subscribe", &base_window::subscribeNamedEvent)
			.def("unsubscribe", &base_window::unsubscribeNamedEvent)
			.def("send", &base_window::sendNamedEvent)
			.def("addScriptEventHandler", &base_window::addScriptEventHandler)
			.def("startTick", &base_window::startTick)
			.def("stopTick", &base_window::stopTick)
			.def("transformToWndCoord", &base_window::transformToWndCoord)
			.def("transformToRootCoord", &base_window::transformToRootCoord)
			.property("draggable", &base_window::isDraggable, &base_window::setDraggable)
			.property("acceptDrop", &base_window::isAcceptDrop, &base_window::setAcceptDrop)
			,
			def("to_statictext", &window_caster<Label>::apply),
			def("to_staticimage", &window_caster<ImageBox>::apply),
			def("to_checkbox", &window_caster<Checkbox>::apply),
			def("to_editbox", &window_caster<Editbox>::apply),
			def("to_button", &window_caster<Button>::apply),
			def("to_panel", &window_caster<Panel>::apply),
			def("to_framewindow", &window_caster<FrameWindow>::apply),
			def("to_progress", &window_caster<Progress>::apply),
			def("to_thumb", &window_caster<Thumb>::apply),
			def("to_slider", &window_caster<Slider>::apply),
			def("to_scrollbar", &window_caster<ScrollBar>::apply),
			def("to_scrollpane", &window_caster<ScrollPane>::apply),
			def("to_list", &window_caster<BaseList>::apply),
			def("to_listbox", &window_caster<ListBox>::apply),
			def("to_combobox", &window_caster<Combobox>::apply),
			def("to_markup", &window_caster<MarkupBase>::apply),
			def("to_tooltip", &window_caster<Tooltip>::apply),
			def("to_dragcontainer", &window_caster<DragContainer>::apply),
			def("to_menu", &window_caster<Menu>::apply),
			def("to_catlist", &window_caster<CategorizedList>::apply),
			def("color2hex", &ColorToHexString),
			def("hex2color", &HexStringToColor)
			,
			class_ <Tooltip, bases<base_window> >("Tooltip")
			.property("fadein", &Tooltip::getFadeIn, &Tooltip::setFadeIn)
			.property("fadeout", &Tooltip::getFadeOut, &Tooltip::setFadeOut)
			,
			class_ <DragContainer, bases<base_window> >("DragContainer")
			,
			class_ <Label, bases<base_window> >("StaticText")
			.property("text", &Label::getText, &Label::setText)
			.property("spacing", &Label::getSpacing, &Label::setSpacing)
			.def("setFont", (void(Label::*)(const std::string& font))&Label::setFont)
			.def("setText", &Label::setText)
			.def("appendText", &Label::appendText)
			.def("getText", &Label::getText)
			.def("setSpacing", &Label::setSpacing)
			.def("getSpacing", &Label::getSpacing)
			.property("formatting", &Label::getTextFormatting, &Label::setTextFormatting)
			.def("setTextFormatting", &Label::setTextFormatting)
			.def("getTextFormatting", &Label::getTextFormatting)
			,
			class_ <ImageBox, bases<base_window> >("StaticImage")
			.property("imageset", &ImageBox::getImageset, &ImageBox::setImageset)
			.def("setImageset", &ImageBox::setImageset)
			.def("getImageset", &ImageBox::getImageset)
			.property("image", &ImageBox::getImage, &ImageBox::setImage)
			.def("setImage", &ImageBox::setImage)
			.def("getImage", &ImageBox::getImage)
			.property("vformat", &ImageBox::getVertFormat, &ImageBox::setVertFormat)
			.def("setVertFormat", &ImageBox::setVertFormat)
			.def("getVertFormat", &ImageBox::getVertFormat)
			.property("hformat", &ImageBox::getHorzFormat, &ImageBox::setHorzFormat)
			.def("setHorzFormat", &ImageBox::setHorzFormat)
			.def("getHorzFormat", &ImageBox::getHorzFormat)
			,
			class_ <Panel, bases<base_window> >("Panel")
			,
			class_ <FrameWindow, bases<Panel> >("FrameWindow")
			//.def("setFont", &FrameWindow::setFont)
			.property("caption", &FrameWindow::getCaption, &FrameWindow::setCaption)
			.def("setCaption", &FrameWindow::setCaption)
			.def("getCaption", &FrameWindow::getCaption)
			.property("movable", &FrameWindow::getMovable, &FrameWindow::setMovable)
			.def("setMovable", &FrameWindow::setMovable)
			.def("getMovable", &FrameWindow::getMovable)
			.property("clamp", &FrameWindow::getClamp, &FrameWindow::setClamp)
			.def("setClamp", &FrameWindow::setClamp)
			.def("getClamp", &FrameWindow::getClamp)
			.property("formatting", &FrameWindow::getCaptionFormatting, &FrameWindow::setCaptionFormatting)
			.def("setCaptionFormatting", &FrameWindow::setCaptionFormatting)
			.def("getCaptionFormatting", &FrameWindow::getCaptionFormatting)
			.property("captioncolor", &FrameWindow::getCaptionColor, &FrameWindow::setCaptionColor)
			.def("setCaptionColor", &FrameWindow::setCaptionColor)
			.def("getCaptionColor", &FrameWindow::getCaptionColor)
			,
			class_ <Editbox, bases<Label> >("Editbox")
			.property("text", &Editbox::getText, &Editbox::setText)
			.def("setText", &Editbox::setText)
			.def("getText", &Editbox::getText)
			.property("password", &Editbox::getPassword, &Editbox::setPassword)
			.def("setPassword", &Editbox::setPassword)
			.def("getPassword", &Editbox::getPassword)
			.property("numeric", &Editbox::isNumeric, &Editbox::setNumeric)
			.def("setNumeric", &Editbox::setNumeric)
			.def("isNumeric", &Editbox::isNumeric)
			.def("clearSelection", &Editbox::clearSelection)
			.def("setSelection", &Editbox::setSelection)
			.def("setCaretPos", &Editbox::setCaretPos)
			.def("getMaxLength", &Editbox::getMaxLength)
			.def("setMaxLength", &Editbox::setMaxLength)
			,
			class_ <KeyBinder, bases<Label> >("KeyBinder")
			,
			class_ <Combobox, bases<Editbox> >("Combobox")
			.def("AddItem", &Combobox::AddItem)
			,
			class_ <Checkbox, bases<Label> >("Checkbox")
			.property("checked", &Checkbox::isChecked, &Checkbox::setChecked)
			.def("isChecked", &Checkbox::isChecked)
			.def("setChecked", &Checkbox::setChecked)
			,
			class_ <Progress, bases<Label> >("Progress")
			.property("progress", &Progress::getProgress, &Progress::setProgress)
			.def("getProgress", &Progress::getProgress)
			.def("setProgress", &Progress::setProgress)
			,
			class_ <Thumb, bases<Label> >("Thumb")
			.property("progress", &Thumb::getProgress, &Thumb::setProgress)
			.def("getProgress", &Thumb::getProgress)
			.def("setProgress", &Thumb::setProgress)
			.property("trackarea", &Thumb::getTrackarea, &Thumb::setTrackarea)
			.def("getTrackarea", &Thumb::getTrackarea)
			.def("setTrackarea", &Thumb::setTrackarea)
			,
			class_ <ScrollThumb, bases<Thumb> >("ScrollThumb")			
			,
			class_ <Button, bases<Label> >("Button")
			.property("name", &Button::getName)
			,
			class_ <Slider, bases<base_window> >("Slider")
			.property("scroll", &Slider::getScrollPosition, &Slider::setScrollPosition)
			.def("getScroll", &Slider::getScrollPosition)
			.def("setScroll", &Slider::setScrollPosition)
			.property("document", &Slider::getDocumentSize, &Slider::setDocumentSize)
			.def("getDocumentSize", &Slider::getDocumentSize)
			.def("setDocumentSize", &Slider::setDocumentSize)
			.property("step", &Slider::getStepSize, &Slider::setStepSize)
			.def("getStepSize", &Slider::getStepSize)
			.def("setStepSize", &Slider::setStepSize)
			,
			class_ <ScrollBar, bases<Slider> >("ScrollBar")			
			,
			class_ <ScrollPane, bases<base_window> >("ScrollPane")
			.def("setTarget", &ScrollPane::setTarget)
			,
			class_ <BaseList, bases<base_window> >("BaseList")
			.def("Clear", &BaseList::Clear)
			.property("columns", &BaseList::getColumns, &BaseList::setColumns)
			.def("getColumns", &BaseList::getColumns)
			.def("setColumns", &BaseList::setColumns)
			.property("columnwidth", &BaseList::getColumnWidth, &BaseList::setColumnWidth)
			.def("getColumnWidth", &BaseList::getColumnWidth)
			.def("setColumnWidth", &BaseList::setColumnWidth)
			,
			class_ <ListBox, bases<BaseList> >("ListBox")
			.def("AddItem", &ListBox::AddItem)
			.def("GetSelectedItem", &ListBox::GetSelectedItem)
			,
			class_ <CategorizedList, bases<BaseList> >("CategorizedList")
			.def("AddCategory", &CategorizedList::AddCategory)
			.def("GetCategoryByIndex", &CategorizedList::GetCategoryByIndex)
			//.def("GetCategoryByName", &CategorizedList::GetCategoryByName)
			.def("GetCategoryCount", &CategorizedList::GetCategoryCount)
			.def("RemoveCategory", &CategorizedList::RemoveCategory)
			,
			class_ <CategorizedList::Category>("Category")
			.def("Rename", &CategorizedList::Category::Rename)
			.def("Add", &CategorizedList::Category::Add)
			.def("Remove", &CategorizedList::Category::Remove)
			.def("GetWndByIndex", &CategorizedList::Category::GetWndByIndex)
			.def("GetChildrenCount", &CategorizedList::Category::GetChildrenCount)
			.def("Collapse", &CategorizedList::Category::Collapse)
			.def("Expand", &CategorizedList::Category::Expand)
			.def("IsCollapsed", &CategorizedList::Category::IsCollapsed)
			,
			class_ <MarkupBase, bases<Label> >("MarkupBase")
			,
			class_ <MarkupText, bases<MarkupBase> >("MarkupText")
			,
			class_ <MenuItem>("MenuItem")
			.def(constructor<>())
			.def_readwrite("text", &MenuItem::text)
			.def_readwrite("icon", &MenuItem::icon)
			.def_readwrite("col", &MenuItem::col)
			.def_readwrite("selcol", &MenuItem::selcol)
			,
			class_ <Menu, bases<Panel> >("Menu")
			.def("getItemById", (MenuItem*(Menu::*)(std::string))&Menu::getItem)
			.def("getItem", (MenuItem*(Menu::*)(size_t))&Menu::getItem)
			.def("addItem", &Menu::addItem)
			.def("clear", &Menu::clear)
			.def("show", &Menu::show)
			.def("hide", &Menu::reset)
			.property("fadein", &Menu::getFadeIn, &Menu::setFadeIn)
			.property("fadeout", &Menu::getFadeOut, &Menu::setFadeOut)
			,
			class_ <Font>("Font")
			.enum_("TextFormatting")
			[
				value("LeftAligned", LeftAligned),
				value("RightAligned", RightAligned),
				value("Centred", Centred),
				value("Justified", Justified),
				value("WordWrapLeftAligned", WordWrapLeftAligned),
				value("WordWrapRightAligned", WordWrapRightAligned),
				value("WordWrapCentred", WordWrapCentred),
				value("WordWrapJustified", WordWrapJustified)
			]
			,
			class_ <EventArgs>("EventArgs")
			.enum_("ButtonState")
			[
				value("Up", EventArgs::Up),
				value("Down", EventArgs::Down)
			]
			.enum_("MouseButtons")
			[
				value("Left", EventArgs::Left),
				value("Middle", EventArgs::Middle),
				value("Right", EventArgs::Right)
			]
			.enum_("Keys")
			[
				value("K_LBUTTON", EventArgs::K_LBUTTON),
				value("K_RBUTTON", EventArgs::K_RBUTTON),
				value("K_CANCEL", EventArgs::K_CANCEL),
				value("K_MBUTTON", EventArgs::K_MBUTTON),
				value("K_XBUTTON1", EventArgs::K_XBUTTON1),
				value("K_XBUTTON2", EventArgs::K_XBUTTON2),
				value("K_BACK", EventArgs::K_BACK),
				value("K_TAB", EventArgs::K_TAB),
				value("K_CLEAR", EventArgs::K_CLEAR),
				value("K_RETURN", EventArgs::K_RETURN),
				value("K_SHIFT", EventArgs::K_SHIFT),
				value("K_CONTROL", EventArgs::K_CONTROL),
				value("K_MENU", EventArgs::K_MENU),
				value("K_PAUSE", EventArgs::K_PAUSE),
				value("K_CAPITAL", EventArgs::K_CAPITAL),
				value("K_KANA", EventArgs::K_KANA),
				value("K_HANGUEL", EventArgs::K_HANGUEL),
				value("K_HANGUL", EventArgs::K_HANGUL),
				value("K_JUNJA", EventArgs::K_JUNJA),
				value("K_FINAL", EventArgs::K_FINAL),
				value("K_HANJA", EventArgs::K_HANJA),
				value("K_KANJI", EventArgs::K_KANJI),
				value("K_ESCAPE", EventArgs::K_ESCAPE),
				value("K_CONVERT", EventArgs::K_CONVERT),
				value("K_NONCONVERT", EventArgs::K_NONCONVERT),
				value("K_ACCEPT", EventArgs::K_ACCEPT),
				value("K_MODECHANGE", EventArgs::K_MODECHANGE),
				value("K_SPACE", EventArgs::K_SPACE),
				value("K_PRIOR", EventArgs::K_PRIOR),
				value("K_NEXT", EventArgs::K_NEXT),
				value("K_END", EventArgs::K_END),
				value("K_HOME", EventArgs::K_HOME),
				value("K_LEFT", EventArgs::K_LEFT),
				value("K_UP", EventArgs::K_UP),
				value("K_RIGHT", EventArgs::K_RIGHT),
				value("K_DOWN", EventArgs::K_DOWN),
				value("K_SELECT", EventArgs::K_SELECT),
				value("K_PRINT", EventArgs::K_PRINT),
				value("K_EXECUTE", EventArgs::K_EXECUTE),
				value("K_SNAPSHOT", EventArgs::K_SNAPSHOT),
				value("K_INSERT", EventArgs::K_INSERT),
				value("K_DELETE", EventArgs::K_DELETE),
				value("K_HELP", EventArgs::K_HELP),
				value("K_0", EventArgs::K_0),
				value("K_1", EventArgs::K_1),
				value("K_2", EventArgs::K_2),
				value("K_3", EventArgs::K_3),
				value("K_4", EventArgs::K_4),
				value("K_5", EventArgs::K_5),
				value("K_6", EventArgs::K_6),
				value("K_7", EventArgs::K_7),
				value("K_8", EventArgs::K_8),
				value("K_9", EventArgs::K_9),
				value("K_A", EventArgs::K_A),
				value("K_B", EventArgs::K_B),
				value("K_C", EventArgs::K_C),
				value("K_D", EventArgs::K_D),
				value("K_E", EventArgs::K_E),
				value("K_F", EventArgs::K_F),
				value("K_G", EventArgs::K_G),
				value("K_H", EventArgs::K_H),
				value("K_I", EventArgs::K_I),
				value("K_J", EventArgs::K_J),
				value("K_K", EventArgs::K_K),
				value("K_L", EventArgs::K_L),
				value("K_M", EventArgs::K_M),
				value("K_N", EventArgs::K_N),
				value("K_O", EventArgs::K_O),
				value("K_P", EventArgs::K_P),
				value("K_Q", EventArgs::K_Q),
				value("K_R", EventArgs::K_R),
				value("K_S", EventArgs::K_S),
				value("K_T", EventArgs::K_T),
				value("K_U", EventArgs::K_U),
				value("K_V", EventArgs::K_V),
				value("K_W", EventArgs::K_W),
				value("K_X", EventArgs::K_X),
				value("K_Y", EventArgs::K_Y),
				value("K_Z", EventArgs::K_Z),
				value("K_LWIN", EventArgs::K_LWIN),
				value("K_RWIN", EventArgs::K_RWIN),
				value("K_APPS", EventArgs::K_APPS),
				value("K_SLEEP", EventArgs::K_SLEEP),
				value("K_NUMPAD0", EventArgs::K_NUMPAD0),
				value("K_NUMPAD1", EventArgs::K_NUMPAD1),
				value("K_NUMPAD2", EventArgs::K_NUMPAD2),
				value("K_NUMPAD3", EventArgs::K_NUMPAD3),
				value("K_NUMPAD4", EventArgs::K_NUMPAD4),
				value("K_NUMPAD5", EventArgs::K_NUMPAD5),
				value("K_NUMPAD6", EventArgs::K_NUMPAD6),
				value("K_NUMPAD7", EventArgs::K_NUMPAD7),
				value("K_NUMPAD8", EventArgs::K_NUMPAD8),
				value("K_NUMPAD9", EventArgs::K_NUMPAD9),
				value("K_MULTIPLY", EventArgs::K_MULTIPLY),
				value("K_ADD", EventArgs::K_ADD),
				value("K_SEPARATOR", EventArgs::K_SEPARATOR),
				value("K_SUBTRACT", EventArgs::K_SUBTRACT),
				value("K_DECIMAL", EventArgs::K_DECIMAL),
				value("K_DIVIDE", EventArgs::K_DIVIDE),
				value("K_F1", EventArgs::K_F1),
				value("K_F2", EventArgs::K_F2),
				value("K_F3", EventArgs::K_F3),
				value("K_F4", EventArgs::K_F4),
				value("K_F5", EventArgs::K_F5),
				value("K_F6", EventArgs::K_F6),
				value("K_F7", EventArgs::K_F7),
				value("K_F8", EventArgs::K_F8),
				value("K_F9", EventArgs::K_F9),
				value("K_F10", EventArgs::K_F10),
				value("K_F11", EventArgs::K_F11),
				value("K_F12", EventArgs::K_F12),
				value("K_F13", EventArgs::K_F13),
				value("K_F14", EventArgs::K_F14),
				value("K_F15", EventArgs::K_F15),
				value("K_F16", EventArgs::K_F16),
				value("K_F17", EventArgs::K_F17),
				value("K_F18", EventArgs::K_F18),
				value("K_F19", EventArgs::K_F19),
				value("K_F20", EventArgs::K_F20),
				value("K_F21", EventArgs::K_F21),
				value("K_F22", EventArgs::K_F22),
				value("K_F23", EventArgs::K_F23),
				value("K_F24", EventArgs::K_F24),
				value("K_NUMLOCK", EventArgs::K_NUMLOCK),
				value("K_SCROLL", EventArgs::K_SCROLL),
				value("K_LSHIFT", EventArgs::K_LSHIFT),
				value("K_RSHIFT", EventArgs::K_RSHIFT),
				value("K_LCONTROL", EventArgs::K_LCONTROL),
				value("K_RCONTROL", EventArgs::K_RCONTROL),
				value("K_LMENU", EventArgs::K_LMENU),
				value("K_RMENU", EventArgs::K_RMENU),
				value("K_BROWSER_BACK", EventArgs::K_BROWSER_BACK),
				value("K_BROWSER_FORWARD", EventArgs::K_BROWSER_FORWARD),
				value("K_BROWSER_REFRESH", EventArgs::K_BROWSER_REFRESH),
				value("K_BROWSER_STOP", EventArgs::K_BROWSER_STOP),
				value("K_BROWSER_SEARCH", EventArgs::K_BROWSER_SEARCH),
				value("K_BROWSER_FAVORITES", EventArgs::K_BROWSER_FAVORITES),
				value("K_BROWSER_HOME", EventArgs::K_BROWSER_HOME),
				value("K_VOLUME_MUTE", EventArgs::K_VOLUME_MUTE),
				value("K_VOLUME_DOWN", EventArgs::K_VOLUME_DOWN),
				value("K_VOLUME_UP", EventArgs::K_VOLUME_UP),
				value("K_MEDIA_NEXT_TRACK", EventArgs::K_MEDIA_NEXT_TRACK),
				value("K_MEDIA_PREV_TRACK", EventArgs::K_MEDIA_PREV_TRACK),
				value("K_MEDIA_STOP", EventArgs::K_MEDIA_STOP),
				value("K_MEDIA_PLAY_PAUSE", EventArgs::K_MEDIA_PLAY_PAUSE),
				value("K_LAUNCH_MAIL", EventArgs::K_LAUNCH_MAIL),
				value("K_LAUNCH_MEDIA_SELECT", EventArgs::K_LAUNCH_MEDIA_SELECT),
				value("K_LAUNCH_APP1", EventArgs::K_LAUNCH_APP1),
				value("K_LAUNCH_APP2", EventArgs::K_LAUNCH_APP2),
				value("K_OEM_1", EventArgs::K_OEM_1),
				value("K_OEM_PLUS", EventArgs::K_OEM_PLUS),
				value("K_OEM_COMMA", EventArgs::K_OEM_COMMA),
				value("K_OEM_MINUS", EventArgs::K_OEM_MINUS),
				value("K_OEM_PERIOD", EventArgs::K_OEM_PERIOD),
				value("K_OEM_2", EventArgs::K_OEM_2),
				value("K_OEM_3", EventArgs::K_OEM_3),
				value("K_OEM_4", EventArgs::K_OEM_4),
				value("K_OEM_5", EventArgs::K_OEM_5),
				value("K_OEM_6", EventArgs::K_OEM_6),
				value("K_OEM_7", EventArgs::K_OEM_7),
				value("K_OEM_8", EventArgs::K_OEM_8),
				value("K_OEM_102", EventArgs::K_OEM_102),
				value("K_PROCESSKEY", EventArgs::K_PROCESSKEY),
				value("K_PACKET", EventArgs::K_PACKET),
				value("K_ATTN", EventArgs::K_ATTN),
				value("K_CRSEL", EventArgs::K_CRSEL),
				value("K_EXSEL", EventArgs::K_EXSEL),
				value("K_EREOF", EventArgs::K_EREOF),
				value("K_PLAY", EventArgs::K_PLAY),
				value("K_ZOOM", EventArgs::K_ZOOM),
				value("K_PA1", EventArgs::K_PA1),
				value("K_OEM_CLEAR", EventArgs::K_OEM_CLEAR)
			]
			.def_readwrite("name", &EventArgs::name)
			.def_readwrite("handled", &EventArgs::handled)
			,
			class_ <MouseEventArgs, bases<EventArgs> >("MouseEventArgs")
			.def_readwrite("mbutton", &MouseEventArgs::mbutton)
			.def_readwrite("mstate", &MouseEventArgs::mstate)
			.def_readwrite("mouseX", &MouseEventArgs::mouseX)
			.def_readwrite("mouseY", &MouseEventArgs::mouseY)
			.def_readwrite("wheelDelta", &MouseEventArgs::wheelDelta)
			,
			class_ <MarkupEventArgs, bases<MouseEventArgs> >("MarkupEventArgs")
			.def_readwrite("id", &MarkupEventArgs::id)
			.def_readwrite("type", &MarkupEventArgs::type)
			,
			class_ <MenuEventArgs, bases<MouseEventArgs> >("MenuEventArgs")
			.def_readwrite("id", &MenuEventArgs::id)
			,
			class_ <KeyEventArgs, bases<EventArgs> >("KeyEventArgs")
			.def_readwrite("button", &KeyEventArgs::button)
			.def_readwrite("state", &KeyEventArgs::state)
			,
			class_ <DragEventArgs, bases<EventArgs> >("DragEventArgs")
			.def_readwrite("offset", &DragEventArgs::offset)
			.def_readwrite("subj", &DragEventArgs::subj)
			,
			class_ <TickEventArgs, bases<EventArgs> >("TickEventArgs")
			.def_readwrite("delta", &TickEventArgs::delta)
			,
			class_ <BinderEventArgs, bases<EventArgs> >("BinderEventArgs")
			.def_readwrite("isMouse", &BinderEventArgs::isMouse)
			.def_readwrite("isKbd", &BinderEventArgs::isKbd)
			.def_readwrite("kbutton", &BinderEventArgs::kbutton)
			.def_readwrite("mbutton", &BinderEventArgs::mbutton)
			.def_readwrite("systemkey", &BinderEventArgs::systemkey)
			.def_readwrite("isWheel", &BinderEventArgs::isWheel)
			.def_readwrite("mwheel", &BinderEventArgs::mwheel)
			.def_readwrite("doubleClick", &BinderEventArgs::doubleClick)
			,
			def("to_mouseargs", &event_caster<MouseEventArgs>::apply),
			def("to_keyargs", &event_caster<KeyEventArgs>::apply),
			def("to_dragargs", &event_caster<DragEventArgs>::apply),
			def("to_markupargs", &event_caster<MarkupEventArgs>::apply),
			def("to_menuargs", &event_caster<MenuEventArgs>::apply),
			def("to_tickargs", &event_caster<TickEventArgs>::apply),
			def("to_bindargs", &event_caster<BinderEventArgs>::apply)
		];

		globals(m_scriptSys.getLuaState())["gui"] = this;
		globals(m_scriptSys.getLuaState())["render"] = m_renderHelper.get();
		globals(m_scriptSys.getLuaState())["log"] = &m_logger;
}

}
