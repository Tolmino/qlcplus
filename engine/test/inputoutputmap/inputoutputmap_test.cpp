/*
  Q Light Controller Plus - Unit test
  inputoutputmap_test.cpp

  Copyright (c) Massimo Callegari

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/
#include <QSignalSpy>
#include <QtTest>

#define private public
#include "iopluginstub.h"
#include "inputoutputmap_test.h"
#include "inputoutputmap.h"
#include "qlcinputsource.h"
#include "grandmaster.h"
#include "outputpatch.h"
#include "inputpatch.h"
#include "qlcconfig.h"
#include "universe.h"
#include "qlcfile.h"
#include "doc.h"
#undef private

#define TESTPLUGINDIR "../iopluginstub"
#define ENGINEDIR "../../src"
#include "../common/resource_paths.h"

static QDir testPluginDir()
{
    QDir dir(TESTPLUGINDIR);
    dir.setFilter(QDir::Files);
    dir.setNameFilters(QStringList() << QString("*%1").arg(KExtPlugin));
    return dir;
}

void InputOutputMap_Test::initTestCase()
{
    m_doc = new Doc(this);
    m_doc->ioPluginCache()->load(testPluginDir());
    QVERIFY(m_doc->ioPluginCache()->plugins().size() != 0);
}

void InputOutputMap_Test::cleanupTestCase()
{
    delete m_doc;
    m_doc = NULL;
}

void InputOutputMap_Test::initial()
{
    InputOutputMap im(m_doc, 4);
    QVERIFY(im.universesCount() == 4);
    QVERIFY(im.m_universeArray.count() == 4);
    QVERIFY(im.universeNames().count() == 4);
    QVERIFY(im.m_profiles.size() == 0);
    QVERIFY(im.profileNames().size() == 0);
}

void InputOutputMap_Test::pluginNames()
{
    InputOutputMap im(m_doc, 4);
    QCOMPARE(im.outputPluginNames().size(), 1);
    QCOMPARE(im.outputPluginNames().at(0), QString("I/O Plugin Stub"));
}

void InputOutputMap_Test::pluginInputs()
{
    InputOutputMap im(m_doc, 4);

    QVERIFY(im.pluginInputs("Foo").size() == 0);

    IOPluginStub* stub = static_cast<IOPluginStub*>
                                (m_doc->ioPluginCache()->plugins().at(0));
    QVERIFY(stub != NULL);

    QVERIFY(im.pluginInputs(stub->name()).size() == 4);
    QVERIFY(im.pluginInputs(stub->name()) == stub->inputs());
    QVERIFY(im.inputPluginNames().count() == 1);
    QVERIFY(im.inputPluginNames().at(0) == stub->name());
    QVERIFY(im.pluginSupportsFeedback(stub->name()) == false);
}

void InputOutputMap_Test::pluginOutputs()
{
    InputOutputMap om(m_doc, 4);

    IOPluginStub* stub = static_cast<IOPluginStub*>
                                (m_doc->ioPluginCache()->plugins().at(0));
    QVERIFY(stub != NULL);

    QStringList ls(om.pluginOutputs(stub->name()));
    QVERIFY(ls == stub->outputs());

    QVERIFY(om.pluginOutputs("Foobar").isEmpty() == true);
}

void InputOutputMap_Test::configurePlugin()
{
    InputOutputMap im(m_doc, 4);

    QCOMPARE(im.canConfigurePlugin("Foo"), false);

    IOPluginStub* stub = static_cast<IOPluginStub*>
                                (m_doc->ioPluginCache()->plugins().at(0));
    QVERIFY(stub != NULL);

    QCOMPARE(im.canConfigurePlugin("Foo"), false);
    QCOMPARE(im.canConfigurePlugin(stub->name()), false);
    stub->m_canConfigure = true;
    QCOMPARE(im.canConfigurePlugin(stub->name()), true);

    /* Must be able to call multiple times */
    im.configurePlugin(stub->name());
    QVERIFY(stub->m_configureCalled == 1);
    im.configurePlugin(stub->name());
    QVERIFY(stub->m_configureCalled == 2);
    im.configurePlugin(stub->name());
    QVERIFY(stub->m_configureCalled == 3);
}

void InputOutputMap_Test::inputPluginStatus()
{
    InputOutputMap im(m_doc, 4);

    QVERIFY(im.inputPluginStatus("Foo", QLCIOPlugin::invalidLine()).contains("Nothing selected"));
    QVERIFY(im.inputPluginStatus("Bar", 0).contains("Nothing selected"));
    QVERIFY(im.inputPluginStatus("Baz", 1).contains("Nothing selected"));
    QVERIFY(im.inputPluginStatus("Xyzzy", 2).contains("Nothing selected"));
    QVERIFY(im.inputPluginStatus("AYBABTU", 3).contains("Nothing selected"));

    IOPluginStub* stub = static_cast<IOPluginStub*>
                                (m_doc->ioPluginCache()->plugins().at(0));
    QVERIFY(stub != NULL);

    QVERIFY(im.inputPluginStatus(stub->name(), QLCIOPlugin::invalidLine()) == stub->inputInfo(QLCIOPlugin::invalidLine()));
    QVERIFY(im.inputPluginStatus(stub->name(), 0) == stub->inputInfo(0));
    QVERIFY(im.inputPluginStatus(stub->name(), 1) == stub->inputInfo(1));
    QVERIFY(im.inputPluginStatus(stub->name(), 2) == stub->inputInfo(2));

    QVERIFY(im.pluginDescription("Foo") == "");
    QVERIFY(im.pluginDescription(stub->name()) == stub->pluginInfo());
}

void InputOutputMap_Test::outputPluginStatus()
{
    InputOutputMap om(m_doc, 4);

    QVERIFY(om.outputPluginStatus("Foo", QLCIOPlugin::invalidLine()).contains("Nothing selected"));
    QVERIFY(om.outputPluginStatus("Bar", 0).contains("Nothing selected"));
    QVERIFY(om.outputPluginStatus("Baz", 1).contains("Nothing selected"));
    QVERIFY(om.outputPluginStatus("Xyzzy", 2).contains("Nothing selected"));
    QVERIFY(om.outputPluginStatus("AYBABTU", 3).contains("Nothing selected"));

    IOPluginStub* stub = static_cast<IOPluginStub*>
                                (m_doc->ioPluginCache()->plugins().at(0));
    QVERIFY(stub != NULL);

    QVERIFY(om.outputPluginStatus(stub->name(), 4) == stub->outputInfo(QLCIOPlugin::invalidLine()));
    QVERIFY(om.outputPluginStatus(stub->name(), 0) == stub->outputInfo(0));
    QVERIFY(om.outputPluginStatus(stub->name(), 1) == stub->outputInfo(1));
    QVERIFY(om.outputPluginStatus(stub->name(), 2) == stub->outputInfo(2));
}

void InputOutputMap_Test::universeNames()
{
    InputOutputMap iom(m_doc, 4);

    QCOMPARE(quint32(iom.universeNames().size()), iom.universesCount());
    QVERIFY(iom.universeNames().at(0).contains("Universe"));
    QVERIFY(iom.universeNames().at(1).contains("Universe"));
    QVERIFY(iom.universeNames().at(2).contains("Universe"));
    QVERIFY(iom.universeNames().at(3).contains("Universe"));

    IOPluginStub* stub = static_cast<IOPluginStub*>
                                (m_doc->ioPluginCache()->plugins().at(0));
    QVERIFY(stub != NULL);

    iom.setOutputPatch(0, stub->name(), "", 3);
    QCOMPARE(quint32(iom.universeNames().size()), iom.universesCount());
    QCOMPARE(iom.universeNames().at(0), QString("Universe 1"));
    QCOMPARE(iom.universeNames().at(1), QString("Universe 2"));
    QCOMPARE(iom.universeNames().at(2), QString("Universe 3"));
    QCOMPARE(iom.universeNames().at(3), QString("Universe 4"));

    iom.setOutputPatch(3, stub->name(), "", 2);
    QCOMPARE(quint32(iom.universeNames().size()), iom.universesCount());
    QCOMPARE(iom.universeNames().at(0), QString("Universe 1"));
    QCOMPARE(iom.universeNames().at(1), QString("Universe 2"));
    QCOMPARE(iom.universeNames().at(2), QString("Universe 3"));
    QCOMPARE(iom.universeNames().at(3), QString("Universe 4"));

    iom.setUniverseName(1, "Name Changed");
    iom.setUniverseName(42, "This is not the Universe you're looking for");

    QCOMPARE(iom.getUniverseNameByIndex(1), QString("Name Changed"));
    QCOMPARE(iom.getUniverseNameByIndex(2), QString("Universe 3"));
    QCOMPARE(iom.getUniverseNameByIndex(42), QString());
    QCOMPARE(iom.getUniverseNameByID(3), QString("Universe 4"));
}

void InputOutputMap_Test::addUniverse()
{
    InputOutputMap im(m_doc, 4);
    QVERIFY(im.universesCount() == 4);
    QVERIFY(im.addUniverse() == true);
    QVERIFY(im.universesCount() == 5);
    QVERIFY(im.getUniverseID(4) == 4);
    QVERIFY(im.getUniverseID(42) == Universe::invalid());

    /* try to add an existing universe */
    QVERIFY(im.addUniverse(3) == false);
    QVERIFY(im.universesCount() == 5);

    /* add a universe with high id and check that
     * there's no gaps */
    QVERIFY(im.addUniverse(8) == true);
    QVERIFY(im.universesCount() == 9);
}

void InputOutputMap_Test::removeUniverse()
{
    InputOutputMap im(m_doc, 4);
    QVERIFY(im.universesCount() == 4);

    // Creating a gap in the universe list is forbidden
    QVERIFY(im.removeUniverse(1) == false);
    QVERIFY(im.universesCount() == 4);

    // Removing the last universe is OK
    QVERIFY(im.removeUniverse(3) == true);
    QVERIFY(im.universesCount() == 3);

    QVERIFY(im.removeUniverse(7) == false);
    im.removeAllUniverses();
    QVERIFY(im.universesCount() == 0);
}

void InputOutputMap_Test::universe()
{
    InputOutputMap im(m_doc, 4);
    QVERIFY(im.universes().count() == 4);

    im.setUniversePassthrough(1, true);
    QVERIFY(im.getUniversePassthrough(1) == true);
    im.setUniversePassthrough(42, true);
    QVERIFY(im.getUniversePassthrough(42) == false);

    im.setUniverseMonitor(2, true);
    QVERIFY(im.getUniverseMonitor(2) == true);
    im.setUniverseMonitor(42, true);
    QVERIFY(im.getUniverseMonitor(42) == false);
}

void InputOutputMap_Test::profiles()
{
    InputOutputMap im(m_doc, 4);
    QVERIFY(im.m_profiles.size() == 0);

    QLCInputProfile* prof = new QLCInputProfile();
    prof->setManufacturer("Foo");
    prof->setModel("Bar");

    QVERIFY(im.addProfile(prof) == true);
    QVERIFY(im.m_profiles.size() == 1);
    QVERIFY(im.addProfile(prof) == false);
    QVERIFY(im.m_profiles.size() == 1);

    QVERIFY(im.profileNames().size() == 1);
    QVERIFY(im.profileNames().at(0) == prof->name());
    QVERIFY(im.profile(prof->name()) == prof);
    QVERIFY(im.profile("Foobar") == NULL);

    QVERIFY(im.removeProfile("Foobar") == false);
    QVERIFY(im.m_profiles.size() == 1);
    QVERIFY(im.removeProfile(prof->name()) == true);
    QVERIFY(im.m_profiles.size() == 0);
}

void InputOutputMap_Test::setInputPatch()
{
    InputOutputMap im(m_doc, 4);

    IOPluginStub* stub = static_cast<IOPluginStub*>
                                (m_doc->ioPluginCache()->plugins().at(0));
    QVERIFY(stub != NULL);

    QLCInputProfile* prof = new QLCInputProfile();
    prof->setManufacturer("Foo");
    prof->setModel("Bar");
    im.addProfile(prof);

    QVERIFY(im.inputPatch(0) == NULL);
    QVERIFY(im.inputPatch(1) == NULL);
    QVERIFY(im.inputPatch(2) == NULL);
    QVERIFY(im.inputPatch(3) == NULL);
    QVERIFY(im.inputMapping(stub->name(), 0) == InputOutputMap::invalidUniverse());
    QVERIFY(im.inputMapping(stub->name(), 1) == InputOutputMap::invalidUniverse());
    QVERIFY(im.inputMapping(stub->name(), 2) == InputOutputMap::invalidUniverse());
    QVERIFY(im.inputMapping(stub->name(), 3) == InputOutputMap::invalidUniverse());
    QVERIFY(im.isUniversePatched(0) == false);
    QVERIFY(im.isUniversePatched(42) == false);

    QVERIFY(im.setInputPatch(0, "Foobar", "", 0, prof->name()) == true);
    QVERIFY(im.inputPatch(0) == NULL);
    QVERIFY(im.inputMapping(stub->name(), 0) == InputOutputMap::invalidUniverse());

    QVERIFY(im.inputPatch(1) == NULL);
    QVERIFY(im.inputMapping(stub->name(), 1) == InputOutputMap::invalidUniverse());

    QVERIFY(im.inputPatch(2) == NULL);
    QVERIFY(im.inputMapping(stub->name(), 2) == InputOutputMap::invalidUniverse());

    QVERIFY(im.inputPatch(3) == NULL);
    QVERIFY(im.inputMapping(stub->name(), 3) == InputOutputMap::invalidUniverse());

    QVERIFY(im.setInputPatch(0, stub->name(), stub->inputs().at(0), 0) == true);
    QVERIFY(im.inputPatch(0)->plugin() == stub);
    QVERIFY(im.inputPatch(0)->input() == 0);
    QVERIFY(im.inputPatch(0)->profile() == NULL);
    QVERIFY(im.inputMapping(stub->name(), 0) == 0);
    QVERIFY(im.isUniversePatched(0) == true);

    QVERIFY(im.inputPatch(1) == NULL);
    QVERIFY(im.inputMapping(stub->name(), 1) == InputOutputMap::invalidUniverse());

    QVERIFY(im.inputPatch(2) == NULL);
    QVERIFY(im.inputMapping(stub->name(), 2) == InputOutputMap::invalidUniverse());

    QVERIFY(im.inputPatch(3) == NULL);
    QVERIFY(im.inputMapping(stub->name(), 3) == InputOutputMap::invalidUniverse());

    QVERIFY(im.setInputPatch(2, stub->name(), stub->inputs().at(3), 3, prof->name()) == true);
    QVERIFY(im.inputPatch(0)->plugin() == stub);
    QVERIFY(im.inputPatch(0)->input() == 0);
    QVERIFY(im.inputPatch(0)->profile() == NULL);
    QVERIFY(im.inputMapping(stub->name(), 0) == 0);

    QVERIFY(im.inputPatch(1) == NULL);
    QVERIFY(im.inputMapping(stub->name(), 1) == InputOutputMap::invalidUniverse());

    QVERIFY(im.inputPatch(2)->plugin() == stub);
    QVERIFY(im.inputPatch(2)->input() == 3);
    QVERIFY(im.inputPatch(2)->profile() == prof);
    QVERIFY(im.inputMapping(stub->name(), 2) == InputOutputMap::invalidUniverse());

    QVERIFY(im.inputPatch(3) == NULL);
    QVERIFY(im.inputMapping(stub->name(), 3) == 2);

    // Universe out of bounds
    QVERIFY(im.setInputPatch(im.universesCount(), stub->name(), stub->inputs().at(0), 0) == false);
}


void InputOutputMap_Test::setOutputPatch()
{
    InputOutputMap iom(m_doc, 4);

    IOPluginStub* stub = static_cast<IOPluginStub*>
                                (m_doc->ioPluginCache()->plugins().at(0));
    QVERIFY(stub != NULL);

    QVERIFY(iom.setOutputPatch(0, "Foobar", "", 0) == false);
    QVERIFY(iom.outputPatch(0) == NULL);
    QVERIFY(iom.outputPatch(1) == NULL);
    QVERIFY(iom.outputPatch(2) == NULL);
    QVERIFY(iom.outputPatch(3) == NULL);

    QVERIFY(iom.setOutputPatch(4, stub->name(), "", 0) == false);
    QVERIFY(iom.outputPatch(0) == NULL);
    QVERIFY(iom.outputPatch(1) == NULL);
    QVERIFY(iom.outputPatch(2) == NULL);
    QVERIFY(iom.outputPatch(3) == NULL);

    QVERIFY(iom.setOutputPatch(4, stub->name(), "", 4) == false);
    QVERIFY(iom.outputPatch(0) == NULL);
    QVERIFY(iom.outputPatch(1) == NULL);
    QVERIFY(iom.outputPatch(2) == NULL);
    QVERIFY(iom.outputPatch(3) == NULL);

    QVERIFY(iom.setOutputPatch(3, stub->name(), stub->outputs().at(0), 0) == true);
    QVERIFY(iom.outputPatch(3)->plugin() == stub);
    QVERIFY(iom.outputPatch(3)->output() == 0);

    QVERIFY(iom.setOutputPatch(2, stub->name(), stub->outputs().at(1), 1) == true);
    QVERIFY(iom.outputPatch(2)->plugin() == stub);
    QVERIFY(iom.outputPatch(2)->output() == 1);

    QVERIFY(iom.setOutputPatch(1, stub->name(), stub->outputs().at(2), 2) == true);
    QVERIFY(iom.outputPatch(1)->plugin() == stub);
    QVERIFY(iom.outputPatch(1)->output() == 2);

    QVERIFY(iom.setOutputPatch(0, stub->name(), stub->outputs().at(3), 3) == true);
    QVERIFY(iom.outputPatch(0)->plugin() == stub);
    QVERIFY(iom.outputPatch(0)->output() == 3);

    QVERIFY(iom.outputMapping("Foo", 42) == QLCIOPlugin::invalidLine());
    QVERIFY(iom.outputMapping(stub->name(), 0) == 3);

    QVERIFY(iom.feedbackPatch(42) == NULL);
    QVERIFY(iom.feedbackPatch(0) == NULL);
}

void InputOutputMap_Test::setMultipleOutputPatches()
{
    InputOutputMap iom(m_doc, 4);

    IOPluginStub* stub = static_cast<IOPluginStub*>
                                (m_doc->ioPluginCache()->plugins().at(0));
    QVERIFY(stub != NULL);

    // add an output patch
    QVERIFY(iom.setOutputPatch(1, stub->name(), "", 0, false, 0) == true);
    QVERIFY(iom.outputPatchesCount(1) == 1);
    QVERIFY(iom.outputPatch(1, 0)->plugin() == stub);
    QVERIFY(iom.outputPatch(1, 0)->output() == 0);

    // add another output patch
    QVERIFY(iom.setOutputPatch(1, stub->name(), "", 0, false, 1) == true);
    QVERIFY(iom.outputPatchesCount(1) == 2);
    QVERIFY(iom.outputPatch(1, 1)->plugin() == stub);
    QVERIFY(iom.outputPatch(1, 1)->output() == 0);

    // remove the first output patch
    QVERIFY(iom.setOutputPatch(1, stub->name(), "", QLCIOPlugin::invalidLine(), false, 0) == true);
    QVERIFY(iom.outputPatchesCount(1) == 1);
    QVERIFY(iom.outputPatch(1, 0)->plugin() == stub);
    QVERIFY(iom.outputPatch(1, 0)->output() == 0);
    QVERIFY(iom.outputPatch(1, 1) == NULL);

    // remove the first output patch again
    QVERIFY(iom.setOutputPatch(1, stub->name(), "", QLCIOPlugin::invalidLine(), false, 0) == true);
    QVERIFY(iom.outputPatchesCount(1) == 0);
}

void InputOutputMap_Test::slotValueChanged()
{
    InputOutputMap im(m_doc, 4);

    IOPluginStub* stub = static_cast<IOPluginStub*>
                                (m_doc->ioPluginCache()->plugins().at(0));
    QVERIFY(stub != NULL);

    QVERIFY(im.setInputPatch(0, stub->name(), stub->inputs().at(0), 0) == true);
    QVERIFY(im.inputPatch(0)->plugin() == stub);
    QVERIFY(im.inputPatch(0)->input() == 0);

    QSignalSpy spy(&im, SIGNAL(inputValueChanged(quint32, quint32, uchar, const QString&)));
    stub->emitValueChanged(UINT_MAX, 0, 15, UCHAR_MAX);
    QVERIFY(spy.size() == 0);
    im.flushInputs();
    QVERIFY(spy.size() == 1);
    QVERIFY(spy.at(0).at(0) == 0);
    QVERIFY(spy.at(0).at(1) == 15);
    QVERIFY(spy.at(0).at(2) == UCHAR_MAX);

    /* Invalid mapping for this plugin -> no signal */
    stub->emitValueChanged(UINT_MAX, 3, 15, UCHAR_MAX);
    QVERIFY(spy.size() == 1);
    im.flushInputs();
    QVERIFY(spy.size() == 1);
    QVERIFY(spy.at(0).at(0) == 0);
    QVERIFY(spy.at(0).at(1) == 15);
    QVERIFY(spy.at(0).at(2) == UCHAR_MAX);

    /* Invalid mapping for this plugin -> no signal */
    stub->emitValueChanged(UINT_MAX, 1, 15, UCHAR_MAX);
    QVERIFY(spy.size() == 1);
    im.flushInputs();
    QVERIFY(spy.size() == 1);
    QVERIFY(spy.at(0).at(0) == 0);
    QVERIFY(spy.at(0).at(1) == 15);
    QVERIFY(spy.at(0).at(2) == UCHAR_MAX);

    stub->emitValueChanged(UINT_MAX, 0, 5, 127);
    QVERIFY(spy.size() == 1);
    im.flushInputs();
    QVERIFY(spy.size() == 2);
    QVERIFY(spy.at(0).at(0) == 0);
    QVERIFY(spy.at(0).at(1) == 15);
    QVERIFY(spy.at(0).at(2) == UCHAR_MAX);
    QVERIFY(spy.at(1).at(0) == 0);
    QVERIFY(spy.at(1).at(1) == 5);
    QVERIFY(spy.at(1).at(2) == 127);

    stub->emitValueChanged(UINT_MAX, 0, 2, 0);
    QVERIFY(spy.size() == 2);
    stub->emitValueChanged(UINT_MAX, 0, 2, UCHAR_MAX);
    QVERIFY(spy.size() == 3);
    QVERIFY(spy.at(0).at(0) == 0);
    QVERIFY(spy.at(0).at(1) == 15);
    QVERIFY(spy.at(0).at(2) == UCHAR_MAX);
    QVERIFY(spy.at(1).at(0) == 0);
    QVERIFY(spy.at(1).at(1) == 5);
    QVERIFY(spy.at(1).at(2) == 127);
    QVERIFY(spy.at(2).at(0) == 0);
    QVERIFY(spy.at(2).at(1) == 2);
    QVERIFY(spy.at(2).at(2) == 0);
    im.flushInputs();
    QVERIFY(spy.size() == 4);
    QVERIFY(spy.at(0).at(0) == 0);
    QVERIFY(spy.at(0).at(1) == 15);
    QVERIFY(spy.at(0).at(2) == UCHAR_MAX);
    QVERIFY(spy.at(1).at(0) == 0);
    QVERIFY(spy.at(1).at(1) == 5);
    QVERIFY(spy.at(1).at(2) == 127);
    QVERIFY(spy.at(2).at(0) == 0);
    QVERIFY(spy.at(2).at(1) == 2);
    QVERIFY(spy.at(2).at(2) == 0);
    QVERIFY(spy.at(3).at(0) == 0);
    QVERIFY(spy.at(3).at(1) == 2);
    QVERIFY(spy.at(3).at(2) == UCHAR_MAX);
}

void InputOutputMap_Test::slotConfigurationChanged()
{
    InputOutputMap im(m_doc, 4);

    IOPluginStub* stub = static_cast<IOPluginStub*>
                                (m_doc->ioPluginCache()->plugins().at(0));
    QVERIFY(stub != NULL);

    QSignalSpy spy(&im, SIGNAL(pluginConfigurationChanged(QString, bool)));
    stub->configure();
    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy.at(0).size(), 2);
    QCOMPARE(spy.at(0).at(0).toString(), QString(stub->name()));
}

void InputOutputMap_Test::loadInputProfiles()
{
    InputOutputMap im(m_doc, 4);

    // No profiles in a nonexistent directory
    QDir dir("/path/to/a/nonexistent/place/beyond/this/universe");
    dir.setFilter(QDir::Files);
    dir.setNameFilters(QStringList() << QString("*%1").arg(KExtInputProfile));
    im.loadProfiles(dir);
    QVERIFY(im.profileNames().isEmpty() == true);

    // No profiles in an existing directory
    dir = testPluginDir();
    dir.setFilter(QDir::Files);
    dir.setNameFilters(QStringList() << QString("*%1").arg(KExtInputProfile));
    im.loadProfiles(dir);
    QVERIFY(im.profileNames().isEmpty() == true);

    // Should be able to load profiles
    dir.setPath(INTERNAL_PROFILEDIR);
    im.loadProfiles(dir);
    QStringList names(im.profileNames());
    QVERIFY(names.size() > 0);

    // Shouldn't load duplicates
    im.loadProfiles(dir);
    QCOMPARE(names, im.profileNames());
}

void InputOutputMap_Test::inputSourceNames()
{
    InputOutputMap im(m_doc, 4);

    IOPluginStub* stub = static_cast<IOPluginStub*> (m_doc->ioPluginCache()->plugins().at(0));
    QVERIFY(stub != NULL);

    QDir dir(INTERNAL_PROFILEDIR);
    dir.setFilter(QDir::Files);
    dir.setNameFilters(QStringList() << QString("*%1").arg(KExtInputProfile));
    im.loadProfiles(dir);

    // Allow unpatched universe
    QString uni, ch;
    QVERIFY(im.inputSourceNames(new QLCInputSource(0, 0), uni, ch) == true);
    QCOMPARE(uni, QString("%1 -UNPATCHED-").arg(1));
    QCOMPARE(ch, QString("%1: ?").arg(1));

    // Don't allow unexisting universe
    QVERIFY(im.inputSourceNames(new QLCInputSource(100, 0), uni, ch) == false);

    QVERIFY(im.setInputPatch(0, stub->name(), stub->inputs().at(0), 0, QString("Generic MIDI")) == true);
    QVERIFY(im.inputSourceNames(new QLCInputSource(0, 0), uni, ch) == true);
    QCOMPARE(uni, tr("%1: Generic MIDI").arg(1));
    QCOMPARE(ch, tr("%1: Bank select MSB").arg(1));

    uni.clear();
    ch.clear();
    QVERIFY(im.inputSourceNames(new QLCInputSource(0, 50000), uni, ch) == true);
    QCOMPARE(uni, tr("%1: Generic MIDI").arg(1));
    QCOMPARE(ch, tr("%1: ?").arg(50001));

    QVERIFY(im.setInputPatch(0, stub->name(), stub->inputs().at(0), 0, QString()) == true);

    uni.clear();
    ch.clear();
    QVERIFY(im.inputSourceNames(new QLCInputSource(0, 0), uni, ch) == true);
    QCOMPARE(uni, tr("%1: %2").arg(1).arg(stub->name()));
    QCOMPARE(ch, tr("%1: ?").arg(1));

    QVERIFY(im.inputSourceNames(new QLCInputSource(0, QLCInputSource::invalidChannel), uni, ch) == false);
    QVERIFY(im.inputSourceNames(new QLCInputSource(InputOutputMap::invalidUniverse(), 0), uni, ch) == false);
    QVERIFY(im.inputSourceNames(new QLCInputSource(), uni, ch) == false);
}

void InputOutputMap_Test::profileDirectories()
{
    QDir dir = InputOutputMap::systemProfileDirectory();
    QDir ipDir;
    ipDir.setPath(INPUTPROFILEDIR);
    QVERIFY(dir.filter() & QDir::Files);
    QVERIFY(dir.nameFilters().contains(QString("*%1").arg(KExtInputProfile)));
    QCOMPARE(dir.absolutePath(), ipDir.absolutePath());

    dir = InputOutputMap::userProfileDirectory();
#ifndef SKIP_TEST
    QVERIFY(dir.exists() == true);
#endif
    QVERIFY(dir.filter() & QDir::Files);
    QVERIFY(dir.nameFilters().contains(QString("*%1").arg(KExtInputProfile)));
    QVERIFY(dir.absolutePath().contains(USERINPUTPROFILEDIR));
}

void InputOutputMap_Test::claimReleaseDumpReset()
{
    InputOutputMap iom(m_doc, 4);

    IOPluginStub* stub = static_cast<IOPluginStub*>
                                (m_doc->ioPluginCache()->plugins().at(0));
    QVERIFY(stub != NULL);

    iom.setOutputPatch(0, stub->name(), stub->outputs().at(0), 0);
    iom.setOutputPatch(1, stub->name(), stub->outputs().at(1), 1);
    iom.setOutputPatch(2, stub->name(), stub->outputs().at(2), 2);
    iom.setOutputPatch(3, stub->name(), stub->outputs().at(3), 3);

    QList<Universe*> unis = iom.claimUniverses();
    for (int i = 0; i < 512; i++)
        unis[0]->write(i, 'a');
    for (int i = 0; i < 512; i++)
        unis[1]->write(i, 'b');
    for (int i = 0; i < 512; i++)
        unis[2]->write(i, 'c');
    for (int i = 0; i < 512; i++)
        unis[3]->write(i, 'd');
    iom.releaseUniverses();

    foreach (Universe *universe, unis)
    {
        const QByteArray postGM = universe->postGMValues()->mid(0, universe->usedChannels());
        universe->dumpOutput(postGM);
    }

    for (int i = 0; i < 512; i++)
        QCOMPARE(stub->m_universe.data()[i], 'a');

    for (int i = 512; i < 1024; i++)
        QCOMPARE(stub->m_universe.data()[i], 'b');

    for (int i = 1024; i < 1536; i++)
        QCOMPARE(stub->m_universe.data()[i], 'c');

    for (int i = 1536; i < 2048; i++)
        QCOMPARE(stub->m_universe.data()[i], 'd');

    iom.resetUniverses();
    for (int u = 0; u < iom.m_universeArray.size(); u++)
    {
        for (quint32 i = 0; i < 512; i++)
            QVERIFY(iom.m_universeArray.at(u)->preGMValues().data()[i] == 0);
    }
}

void InputOutputMap_Test::blackout()
{
    InputOutputMap iom(m_doc, 4);

    IOPluginStub* stub = static_cast<IOPluginStub*>
                                (m_doc->ioPluginCache()->plugins().at(0));
    QVERIFY(stub != NULL);

    iom.setOutputPatch(0, stub->name(), stub->outputs().at(0), 0);
    iom.setOutputPatch(1, stub->name(), stub->outputs().at(1), 1);
    iom.setOutputPatch(2, stub->name(), stub->outputs().at(2), 2);
    iom.setOutputPatch(3, stub->name(), stub->outputs().at(3), 3);

    QList<Universe*> unis = iom.claimUniverses();
    unis[0]->setChannelCapability(42, QLCChannel::Intensity);
    unis[1]->setChannelCapability(42, QLCChannel::Intensity);
    unis[2]->setChannelCapability(42, QLCChannel::Intensity);
    unis[3]->setChannelCapability(42, QLCChannel::Intensity);

    for (int i = 0; i < 512; i++)
        unis[0]->write(i, 'a');
    for (int i = 0; i < 512; i++)
        unis[1]->write(i, 'b');
    for (int i = 0; i < 512; i++)
        unis[2]->write(i, 'c');
    for (int i = 0; i < 512; i++)
        unis[3]->write(i, 'd');
    iom.releaseUniverses();

    foreach (Universe *universe, unis)
    {
        const QByteArray postGM = universe->postGMValues()->mid(0, universe->usedChannels());
        universe->dumpOutput(postGM);
    }

    iom.setBlackout(true);
    QVERIFY(iom.blackout() == true);

    foreach (Universe *universe, unis)
    {
        const QByteArray postGM = universe->postGMValues()->mid(0, universe->usedChannels());
        universe->dumpOutput(postGM);
    }

    int offset = 0;

    for (int u = 0; u < 4; u++)
    {
        for (int i = 0; i < 512; i++)
        {
            if (i == 42)
                QVERIFY(stub->m_universe[offset + i] == (char) 0);
            else if (u == 0)
                QVERIFY(stub->m_universe[offset + i] == (char) 'a');
            else if (u == 1)
                QVERIFY(stub->m_universe[offset + i] == (char) 'b');
            else if (u == 2)
                QVERIFY(stub->m_universe[offset + i] == (char) 'c');
            else if (u == 3)
                QVERIFY(stub->m_universe[offset + i] == (char) 'd');
        }

        offset += 512;
    }

    iom.setBlackout(true);
    QVERIFY(iom.blackout() == true);

    foreach (Universe *universe, unis)
    {
        const QByteArray postGM = universe->postGMValues()->mid(0, universe->usedChannels());
        universe->dumpOutput(postGM);
    }

    offset = 0;

    for (int u = 0; u < 4; u++)
    {
        for (int i = 0; i < 512; i++)
        {
            if (i == 42)
                QVERIFY(stub->m_universe[offset + i] == (char) 0);
            else if (u == 0)
                QVERIFY(stub->m_universe[offset + i] == (char) 'a');
            else if (u == 1)
                QVERIFY(stub->m_universe[offset + i] == (char) 'b');
            else if (u == 2)
                QVERIFY(stub->m_universe[offset + i] == (char) 'c');
            else if (u == 3)
                QVERIFY(stub->m_universe[offset + i] == (char) 'd');
        }

        offset += 512;
    }

    iom.toggleBlackout();
    QVERIFY(iom.blackout() == false);

    foreach (Universe *universe, unis)
    {
        const QByteArray postGM = universe->postGMValues()->mid(0, universe->usedChannels());
        universe->dumpOutput(postGM);
    }

    for (int i = 0; i < 512; i++)
        QVERIFY(stub->m_universe[i] == 'a');
    for (int i = 512; i < 1024; i++)
        QVERIFY(stub->m_universe[i] == 'b');
    for (int i = 1024; i < 1536; i++)
        QVERIFY(stub->m_universe[i] == 'c');
    for (int i = 1536; i < 2048; i++)
        QVERIFY(stub->m_universe[i] == 'd');

    iom.setBlackout(false);
    QVERIFY(iom.blackout() == false);

    foreach (Universe *universe, unis)
    {
        const QByteArray postGM = universe->postGMValues()->mid(0, universe->usedChannels());
        universe->dumpOutput(postGM);
    }

    for (int i = 0; i < 512; i++)
        QVERIFY(stub->m_universe[i] == 'a');
    for (int i = 512; i < 1024; i++)
        QVERIFY(stub->m_universe[i] == 'b');
    for (int i = 1024; i < 1536; i++)
        QVERIFY(stub->m_universe[i] == 'c');
    for (int i = 1536; i < 2048; i++)
        QVERIFY(stub->m_universe[i] == 'd');

    iom.toggleBlackout();
    QVERIFY(iom.blackout() == true);

    foreach (Universe *universe, unis)
    {
        const QByteArray postGM = universe->postGMValues()->mid(0, universe->usedChannels());
        universe->dumpOutput(postGM);
    }

    offset = 0;

    for (int u = 0; u < 4; u++)
    {
        for (int i = 0; i < 512; i++)
        {
            if (i == 42)
                QVERIFY(stub->m_universe[offset + i] == (char) 0);
            else if (u == 0)
                QVERIFY(stub->m_universe[offset + i] == (char) 'a');
            else if (u == 1)
                QVERIFY(stub->m_universe[offset + i] == (char) 'b');
            else if (u == 2)
                QVERIFY(stub->m_universe[offset + i] == (char) 'c');
            else if (u == 3)
                QVERIFY(stub->m_universe[offset + i] == (char) 'd');
        }

        offset += 512;
    }
}

void InputOutputMap_Test::grandMaster()
{
    InputOutputMap iom(m_doc, 4);

    QVERIFY(iom.grandMasterChannelMode() == GrandMaster::Intensity);
    QVERIFY(iom.grandMasterValueMode() == GrandMaster::Reduce);
    QVERIFY(iom.grandMasterValue() == 255);

    iom.setGrandMasterValue(100);
    QVERIFY(iom.grandMasterValue() == 100);

    iom.setGrandMasterChannelMode(GrandMaster::AllChannels);
    QVERIFY(iom.grandMasterChannelMode() == GrandMaster::AllChannels);

    iom.setGrandMasterValueMode(GrandMaster::Limit);
    QVERIFY(iom.grandMasterValueMode() == GrandMaster::Limit);
}

QTEST_APPLESS_MAIN(InputOutputMap_Test)
