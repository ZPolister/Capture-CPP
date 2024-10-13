#ifndef STRCUT_H
#define STRCUT_H

#define BUTTON_HEIGHT 50
#define BUTTON_WIDTH  50

#define LABEL_HEIGHT 20
#define LABEL_WIDTH 60

#define ITEM_LENGTH 15

#define MARGIN 10

//当前绘制状态
enum DrawStatus
{
	WAITDRAW = 0,
	DRAWINIG,
	DRAWEND,
};

//鼠标当前位置
enum CursorLocation
{
	TOPLEFT = 0,
	TOP,
	TOPRIGHT,
	LEFT,
	MIDDLE,
	RIGHT,
	BUTTOMLEFT,
	BUTTOM,
	BUTTOMRIGHT,
	INVALID = -1,
};

//添加标记状态
enum DrawEditFlag
{
	DRAWLINE = 0,
	DRAWRECT,
	DRAWCIRCLE,
	DRAWTEXT,
	DRAWDRAG,
};


#endif
