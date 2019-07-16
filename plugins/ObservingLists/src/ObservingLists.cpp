/*
 * Copyright (C) 2019 Alexander Wolf
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
 */

#include "StelUtils.hpp"
#include "StelProjector.hpp"
#include "StelPainter.hpp"
#include "StelApp.hpp"
#include "StelCore.hpp"
#include "StelFileMgr.hpp"
#include "StelLocaleMgr.hpp"
#include "StelModuleMgr.hpp"
#include "StelGui.hpp"
#include "StelGuiItems.hpp"
#include "StelVertexArray.hpp"
#include "ObservingLists.hpp"
#include "ObservingListsDialog.hpp"

#include <QDebug>
#include <QTimer>
#include <QPixmap>
#include <QSettings>
#include <QKeyEvent>
#include <QMouseEvent>
#include <cmath>

//! This method is the one called automatically by the StelModuleMgr just
//! after loading the dynamic library
StelModule* ObservingListsStelPluginInterface::getStelModule() const
{
	return new ObservingLists();
}

StelPluginInfo ObservingListsStelPluginInterface::getPluginInfo() const
{
	// Allow to load the resources when used as a static plugin
	Q_INIT_RESOURCE(ObservingLists);

	StelPluginInfo info;
	info.id = "ObservingLists";
	info.displayedName = N_("Observing Lists");
	info.authors = "Alexander Wolf";
	info.contact = "https://github.com/Stellarium/stellarium";
	info.description = N_("The tool for management of observing lists");
	info.version = OBSERVINGLISTS_PLUGIN_VERSION;
	info.license = OBSERVINGLISTS_PLUGIN_LICENSE;
	return info;
}

ObservingLists::ObservingLists()
{
	setObjectName("ObservingLists");

	configDialog = new ObservingListsDialog();
	conf = StelApp::getInstance().getSettings();

}

ObservingLists::~ObservingLists()
{
	delete configDialog;
}

bool ObservingLists::configureGui(bool show)
{
	if (show)
		configDialog->setVisible(true);
	return true;
}

//! Determine which "layer" the plugin's drawing will happen on.
double ObservingLists::getCallOrder(StelModuleActionName actionName) const
{
	if (actionName==StelModule::ActionDraw)
		return StelApp::getInstance().getModuleMgr().getModule("LandscapeMgr")->getCallOrder(actionName)+10.;	
	return 0;
}

void ObservingLists::init()
{
	StelApp& app = StelApp::getInstance();

	// Create action for enable/disable & hook up signals	
	//addAction("actionShow_Observing_Lists", N_("Observing Lists"), N_("Observing Lists"), "enabled", "Ctrl+A");

	// Initialize the message strings and make sure they are translated when
	// the language changes.

	//connect(&app, SIGNAL(languageChanged()), this, SLOT(updateMessageText()));
}

void ObservingLists::update(double deltaTime)
{

}

//! Draw any parts on the screen which are for our module
void ObservingLists::draw(StelCore* core)
{

}

