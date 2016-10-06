#include "maya_includes.h"
#include <iostream>

using namespace std;
MCallbackId NACId, NCCId;

void onNodeCreated(MObject& node, void* clientData)
{
	MString nodeName;
	if (node.hasFn(MFn::kDagNode)) {
		MFnDagNode dagObj(node);
		nodeName = dagObj.fullPathName();
	}
	else {
		MFnDependencyNode dn(node);
		nodeName = dn.name();
	}
	MString string = "# Node Created " + nodeName + " #";
	MGlobal::displayInfo(string);
}

void onNameChanged(MObject &node, const MString &str, void *clientData)
{
	MString newName;

	if (node.hasFn(MFn::kDagNode)) {
		MFnDagNode dagNode(node);
		newName = dagNode.fullPathName();
	}
	else {
		MFnDependencyNode node(node);
		newName = node.name();
	}

	MString string = "# Name Changed on Node from " + str + " to " + newName + " #";
	MGlobal::displayInfo(string);
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
		MGlobal::displayInfo("# Node Add Failed #");

	NCCId = MNodeMessage::addNameChangedCallback(MObject::kNullObj, onNameChanged, NULL, &r);

	if (r == MS::kFailure)
		MGlobal::displayInfo("# Name Changed Failed #");
	
	return res;
}

EXPORT MStatus uninitializePlugin(MObject obj)
{
	MFnPlugin plugin(obj);

	MGlobal::displayInfo("Maya plugin unloaded!");

	MDGMessage::removeCallback(NACId);
	MDGMessage::removeCallback(NCCId);

	return MS::kSuccess;
}