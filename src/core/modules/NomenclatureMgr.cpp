/*
 * Copyright (C) 2017 Alexander Wolf
 * Copyright (C) 2017 Teresa Huertas Roldán
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

#include "StelPainter.hpp"
#include "StelApp.hpp"
#include "StelCore.hpp"
#include "StelModuleMgr.hpp"
#include "StelObjectMgr.hpp"
#include "StelTextureMgr.hpp"
#include "StelFileMgr.hpp"
#include "StelUtils.hpp"
#include "StelTranslator.hpp"
#include "StelLocaleMgr.hpp"
#include "NomenclatureMgr.hpp"
#include "NomenclatureItem.hpp"

#include <QSettings>
#include <QFile>
#include <QDir>

NomenclatureMgr::NomenclatureMgr()
{
	setObjectName("NomenclatureMgr");
	conf = StelApp::getInstance().getSettings();
	font.setPixelSize(StelApp::getInstance().getBaseFontSize());
}

NomenclatureMgr::~NomenclatureMgr()
{
	StelApp::getInstance().getStelObjectMgr().unSelect();
}

double NomenclatureMgr::getCallOrder(StelModuleActionName actionName) const
{
	if (actionName==StelModule::ActionDraw)
		return StelApp::getInstance().getModuleMgr().getModule("SolarSystem")->getCallOrder(actionName)+10.;
	return 0;
}

void NomenclatureMgr::init()
{
	NomenclatureItem::init();

	texPointer = StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/pointeur2.png");

	// Load the nomenclature
	loadNomenclature();

	setColor(StelUtils::strToVec3f(conf->value("color/planet_nomenclature_color", "0.1,1.0,0.1").toString()));
	setFlagLabels(conf->value("astro/flag_planets_nomenclature", false).toBool());

	GETSTELMODULE(StelObjectMgr)->registerStelObjectMgr(this);

	StelApp *app = &StelApp::getInstance();
	connect(app, SIGNAL(languageChanged()), this, SLOT(updateI18n()));

	QString displayGroup = N_("Display Options");
	addAction("actionShow_Planets_Nomenclature", displayGroup, N_("Nomenclature labels"), "nomenclatureDisplayed");
}

void NomenclatureMgr::loadNomenclature()
{
	qDebug() << "Loading nomenclature for Solar system bodies ...";

	SolarSystem* ssystem = GETSTELMODULE(SolarSystem);
	nomenclatureItems.clear();

	// Get list of all planet names
	QStringList sso = ssystem->getAllPlanetEnglishNames();

	// regular expression to find the comments and empty lines
	QRegExp commentRx("^(\\s*#.*|\\s*)$");

	// regular expression to find the nomenclature data
	// Rules:
	// One rule per line. Each rule contains six elements with white space (or "tab char") as delimiter.
	// Format:
	//	ID of surface feature			: unique string
	//	translatable name of surface feature	: string
	//	type of surface feature			: string
	//	latitude of surface feature		: float (decimal degrees)
	//	longitude of surface feature		: float (decimal degrees)
	//	size of surface feature			: float (kilometers)
	QRegExp recRx("^\\s*([\\w\\d\\-]+)\\s+_[(]\"(.*)\"[)]\\s+(\\w+)\\s+([\\-\\+\\.\\d]+)\\s+([\\-\\+\\.\\d]+)\\s+([\\-\\+\\.\\d]+)(.*)");
	QString record;

	// Let's check existence of nomenclature data for each planet
	foreach (QString planet, sso)
	{
		QString surfNamesFile = StelFileMgr::findFile("data/nomenclature/" + planet.toLower() + ".fab");
		if (!surfNamesFile.isEmpty()) // OK, the file is exist!
		{
			// Open file
			QFile planetSurfNamesFile(surfNamesFile);
			if (!planetSurfNamesFile.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				qDebug() << "Cannot open file" << QDir::toNativeSeparators(surfNamesFile);
				continue;
			}

			// keep track of how many records we processed.
			int totalRecords=0;
			int readOk=0;
			int lineNumber=0;

			QString id, name, type;
			float latitude, longitude, size;

			PlanetP p = ssystem->searchByEnglishName(planet);
			if (!p.isNull())
			{
				while (!planetSurfNamesFile.atEnd())
				{
					record = QString::fromUtf8(planetSurfNamesFile.readLine());
					lineNumber++;

					// Skip comments
					if (commentRx.exactMatch(record))
						continue;

					totalRecords++;
					if (!recRx.exactMatch(record))
						qWarning() << "ERROR - cannot parse record at line" << lineNumber << "in surface nomenclature file" << QDir::toNativeSeparators(surfNamesFile);
					else
					{
						// Read the ID of feature
						id		= recRx.capturedTexts().at(1).trimmed();
						// Read the name of feature
						name	= recRx.capturedTexts().at(2).trimmed();
						// Read the type of feature
						type	= recRx.capturedTexts().at(3).trimmed();
						// Read the latitude of feature
						latitude	= recRx.capturedTexts().at(4).toFloat();
						// Read the longitude of feature
						longitude	= recRx.capturedTexts().at(5).toFloat();
						// Read the size of feature
						size	= recRx.capturedTexts().at(6).toFloat();

						NomenclatureItemP nom = NomenclatureItemP(new NomenclatureItem(p, id, name, type.toLower(), latitude, longitude, size));
						if (!nom.isNull())
							nomenclatureItems.append(nom);

						readOk++;
					}
				}
			}

			planetSurfNamesFile.close();
			qDebug() << "Loaded" << readOk << "/" << totalRecords << "items of surface nomenclature for" << planet;

		}
	}
}

void NomenclatureMgr::deinit()
{
	nomenclatureItems.clear();
	texPointer.clear();
}

void NomenclatureMgr::draw(StelCore* core)
{
	StelProjectorP prj = core->getProjection(StelCore::FrameJ2000);
	StelPainter painter(prj);
	painter.setFont(font);

	foreach (const NomenclatureItemP& nItem, nomenclatureItems)
	{
		if (nItem && nItem->initialized)
			nItem->draw(core, &painter);
	}

	if (GETSTELMODULE(StelObjectMgr)->getFlagSelectedObjectPointer())
		drawPointer(core, painter);

}

void NomenclatureMgr::drawPointer(StelCore* core, StelPainter& painter)
{
	const StelProjectorP prj = core->getProjection(StelCore::FrameJ2000);

	const QList<StelObjectP> newSelected = GETSTELMODULE(StelObjectMgr)->getSelectedObject("NomenclatureItem");
	if (!newSelected.empty())
	{
		const StelObjectP obj = newSelected[0];
		Vec3d pos=obj->getJ2000EquatorialPos(core);

		Vec3d screenpos;
		// Compute 2D pos and return if outside screen
		if (!painter.getProjector()->project(pos, screenpos))
			return;

		const Vec3f& c(obj->getInfoColor());
		painter.setColor(c[0],c[1],c[2]);
		texPointer->bind();
		painter.setBlending(true);
		painter.drawSprite2dMode(screenpos[0], screenpos[1], 13.f, StelApp::getInstance().getTotalRunTime()*40.);
	}
}

QList<StelObjectP> NomenclatureMgr::searchAround(const Vec3d& av, double limitFov, const StelCore*) const
{
	QList<StelObjectP> result;

	Vec3d v(av);
	v.normalize();
	double cosLimFov = cos(limitFov * M_PI/180.);
	Vec3d equPos;

	if (getFlagLabels())
	{
		foreach(const NomenclatureItemP& nItem, nomenclatureItems)
		{
			if (nItem->initialized)
			{
				equPos = nItem->XYZ;
				equPos.normalize();
				if (equPos[0]*v[0] + equPos[1]*v[1] + equPos[2]*v[2]>=cosLimFov)
				{
					result.append(qSharedPointerCast<StelObject>(nItem));
				}
			}
		}
	}

	return result;
}

StelObjectP NomenclatureMgr::searchByName(const QString& englishName) const
{
	if (getFlagLabels())
	{
		foreach(const NomenclatureItemP& nItem, nomenclatureItems)
		{
			if (nItem->getEnglishName().toUpper() == englishName.toUpper())
				return qSharedPointerCast<StelObject>(nItem);
		}
	}

	return Q_NULLPTR;
}

StelObjectP NomenclatureMgr::searchByNameI18n(const QString& nameI18n) const
{
	if (getFlagLabels())
	{
		foreach(const NomenclatureItemP& nItem, nomenclatureItems)
		{
			if (nItem->getNameI18n().toUpper() == nameI18n.toUpper())
				return qSharedPointerCast<StelObject>(nItem);
		}
	}

	return Q_NULLPTR;
}

QStringList NomenclatureMgr::listMatchingObjects(const QString& objPrefix, int maxNbItem, bool useStartOfWords, bool inEnglish) const
{
	return StelObjectModule::listMatchingObjects(objPrefix, maxNbItem, useStartOfWords, inEnglish);
}

QStringList NomenclatureMgr::listAllObjects(bool inEnglish) const
{
	QStringList result;

	if (getFlagLabels())
	{
		if (inEnglish)
		{
			foreach(const NomenclatureItemP& nItem, nomenclatureItems)
			{
				result << nItem->getEnglishName();
			}
		}
		else
		{
			foreach(const NomenclatureItemP& nItem, nomenclatureItems)
			{
				result << nItem->getNameI18n();
			}
		}
	}
	return result;
}

NomenclatureItemP NomenclatureMgr::searchByEnglishName(QString nomenclatureItemEnglishName) const
{
	if (getFlagLabels())
	{
		foreach (const NomenclatureItemP& p, nomenclatureItems)
		{
			if (p->getEnglishName() == nomenclatureItemEnglishName)
				return p;
		}
	}
	return NomenclatureItemP();
}

void NomenclatureMgr::setColor(const Vec3f& c)
{
	NomenclatureItem::color = c;
	emit nomenclatureColorChanged(c);
}

const Vec3f& NomenclatureMgr::getColor(void) const
{
	return NomenclatureItem::color;
}

void NomenclatureMgr::setFlagLabels(bool b)
{
	if (getFlagLabels() != b)
	{
		foreach (NomenclatureItemP i, nomenclatureItems)
			i->setFlagLabels(b);
		emit nomenclatureDisplayedChanged(b);
	}
}

bool NomenclatureMgr::getFlagLabels() const
{
	foreach (NomenclatureItemP i, nomenclatureItems)
	{
		if (i->getFlagLabels())
			return true;
	}
	return false;
}

// Update i18 names from english names according to current sky culture translator
void NomenclatureMgr::updateI18n()
{
	const StelTranslator& trans = StelApp::getInstance().getLocaleMgr().getSkyTranslator();
	foreach (NomenclatureItemP i, nomenclatureItems)
		i->translateName(trans);
}
