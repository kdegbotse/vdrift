/************************************************************************/
/*                                                                      */
/* This file is part of VDrift.                                         */
/*                                                                      */
/* VDrift is free software: you can redistribute it and/or modify       */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or    */
/* (at your option) any later version.                                  */
/*                                                                      */
/* VDrift is distributed in the hope that it will be useful,            */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/*                                                                      */
/* You should have received a copy of the GNU General Public License    */
/* along with VDrift.  If not, see <http://www.gnu.org/licenses/>.      */
/*                                                                      */
/************************************************************************/

#ifndef _GUIPAGE_H
#define _GUIPAGE_H

#include "graphics/scenenode.h"
#include "signalslot.h"

#include <map>
#include <list>
#include <vector>
#include <string>
#include <ostream>

class GUILANGUAGE;
class GUIWIDGET;
class GUICONTROL;
class GUILABEL;
class FONT;
class PATHMANAGER;
class Config;
class ContentManager;

typedef std::map<std::string, Signal1<const std::string &>*> VSIGNALMAP;
typedef std::map<std::string, Slot2<int, std::vector<std::string> &>*> VNACTIONMAP;
typedef std::map<std::string, Slot1<const std::string &>*> VACTIONMAP;
typedef std::map<std::string, Slot0*> ACTIONMAP;

class GUIPAGE
{
public:
	GUIPAGE();

	~GUIPAGE();

	bool Load(
		const std::string & path,
		const std::string & texpath,
		const float screenhwratio,
		const GUILANGUAGE & lang,
		const FONT & font,
		VSIGNALMAP vsignalmap,
		VNACTIONMAP vnactionmap,
		VACTIONMAP vactionmap,
		ACTIONMAP actionmap,
		SCENENODE & parentnode,
		ContentManager & content,
		std::ostream & error_output);

	void SetVisible(SCENENODE & parent, bool value);

	void SetAlpha(SCENENODE & parent, float value);

	/// execute game actions and update gui options
	void ProcessInput(
		float cursorx, float cursory,
		bool cursormoved, bool cursordown, bool cursorjustup,
		bool moveleft, bool moveright,
		bool moveup, bool movedown,
		bool select, bool cancel);

	/// tell all child widgets to do as update tick
	void Update(SCENENODE & parent, float dt);

	void SetLabelText(const std::map<std::string, std::string> & label_text);

	GUILABEL * GetLabel(const std::string & name);

	SCENENODE & GetNode(SCENENODE & parentnode);

private:
	std::map <std::string, GUILABEL *> labels;
	std::vector <GUICONTROL *> controls;			// active widgets (process input)
	std::vector <GUIWIDGET *> widgets;				// passive widgets
	GUICONTROL * default_control;					// default active control
	GUICONTROL * active_control;					// current active control
	keyed_container <SCENENODE>::handle s;
	std::string name;

	// each control registers a ControlCB
	// which other controls can signal to focus(activate) it
	// it uses PAGE::SetActiveControl
	struct ControlCb
	{
		GUIPAGE * page;
		GUICONTROL * control;
		Slot0 action;

		ControlCb();
		ControlCb(const ControlCb & other);
		ControlCb & operator=(const ControlCb & other);
		void call();
	};
	// for a control to be able to modify a float property of a widget
	// a WidgetCB holding the value is registered
	struct WidgetCb
	{
		float value;
		Delegate1<void, float> set;
		Slot0 action;

		WidgetCb();
		WidgetCb(const WidgetCb & other);
		WidgetCb & operator=(const WidgetCb & other);
		void call();
	};
	// AcionCb allows a conrol to signal slots with a parameter
	// it can be seen as an adapter from Signal0 + value to Slot1
	struct ActionCb
	{
		std::string value;
		Signal1<const std::string &> signal;
		Slot0 action;

		ActionCb();
		ActionCb(const ActionCb & other);
		ActionCb & operator=(const ActionCb & other);
		void call();
	};
	std::vector<ControlCb> control_set;		// control focus callbacks
	std::vector<WidgetCb> widget_set;		// widget property callbacks
	std::vector<ActionCb> action_set;		// action value callbacks
	Signal1<const std::string &> tooltip;	// tooltip text signal
	Signal0 onfocus, oncancel;				// page action signals

	void Clear(SCENENODE & parentnode);

	void SetActiveControl(GUICONTROL & control);
};

#endif
