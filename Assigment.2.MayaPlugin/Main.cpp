#include "maya_includes.h"
#include <iostream>

using namespace std;
MCallbackId NACId, NCCId, ACCId, TCId;

void onNodeCreated(MObject& node, void* clientData)
{
	MString nodeName;
	MString string;
	if (node.hasFn(MFn::kTransform)) {
		MFnTransform trans(node);
		nodeName = trans.name();
		string = "# A Node has been Created named " + nodeName + " #";
	}
	else {
		MFnMesh mesh(node);
		nodeName = mesh.name();
		string = "# A Node has been Created named " + nodeName + " #";
	}
	MGlobal::displayInfo(string);
}

void onNameChanged(MObject &node, const MString &str, void *clientData)
{
	MString newName;
	MString string;
	if (node.hasFn(MFn::kTransform)) {
		MFnTransform trans(node);
		newName = trans.name();
		string = "# A Node has Changed Name from " + str + " to " + newName + " #";
	}
	if (node.hasFn(MFn::kMesh)) {
		MFnMesh mesh(node);
		newName = mesh.name();
		string = "# A Node has Changed Name from " + str + " to " + newName + " #";
	}
	MGlobal::displayInfo(string);
}

void onTransformationNodeChanged(MNodeMessage::AttributeMessage msg, MPlug &plug1, MPlug &plug2, void *clientData)
{
	MString string = "# " + plug1.name() + " Transformation has Changed #";
	MGlobal::displayInfo(string);
}

void onTimeChanged(float elapsedTime, float lastTime, void *clientData)
{
	MString string = "# Time : ";
	MString string2 = " #";
	MGlobal::displayInfo(string + elapsedTime + string2);
}

EXPORT MStatus initializePlugin(MObject obj)
{
	MStatus res = MS::kSuccess;

	MFnPlugin myPlugin(obj, "Maya plugin", "1.0", "Any", &res);
	if (MFAIL(res)) {
		CHECK_MSTATUS(res);
	}

	MGlobal::displayInfo("Maya plugin loaded!");

	MStatus r = MS::kSuccess;

	NACId = MDGMessage::addNodeAddedCallback(onNodeCreated, "dependNode", NULL, &r);

	if(r==MS::kFailure)
		MGlobal::displayInfo("# Node Added Callback Failed #");

	NCCId = MNodeMessage::addNameChangedCallback(MObject::kNullObj, onNameChanged, NULL, &r);

	if (r == MS::kFailure)
		MGlobal::displayInfo("# Name Changed Callback Failed #");

	ACCId = MNodeMessage::addAttributeChangedCallback(obj, onTransformationNodeChanged, NULL, &r);

	if (r == MS::kFailure)
		MGlobal::displayInfo("# Transformation Changed Failed #");

	float time = 5.0;
	TCId = MTimerMessage::addTimerCallback(time, onTimeChanged, NULL, &r);

	if (r == MS::kFailure)
		MGlobal::displayInfo("# Time Callback Failed #");
	
	return res;
}

EXPORT MStatus uninitializePlugin(MObject obj)
{
	MFnPlugin plugin(obj);

	MGlobal::displayInfo("Maya plugin unloaded!");

	MDGMessage::removeCallback(NACId);
	MNodeMessage::removeCallback(NCCId);
	MNodeMessage::removeCallback(ACCId);
	MTimerMessage::removeCallback(TCId);

	return MS::kSuccess;
}