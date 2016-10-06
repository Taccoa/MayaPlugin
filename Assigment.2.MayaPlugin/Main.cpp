#include "maya_includes.h"
#include <iostream>

using namespace std;
MCallbackId sId;

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

EXPORT MStatus initializePlugin(MObject obj)
{
	MStatus res = MS::kSuccess;

	MFnPlugin myPlugin(obj, "Maya plugin", "1.0", "Any", &res);
	if (MFAIL(res)) {
		CHECK_MSTATUS(res);
	}

	MGlobal::displayInfo("Maya plugin loaded!");

	sId = MDGMessage::addNodeAddedCallback(onNodeCreated, "dependNode", NULL, &res);
	
	return res;
}

EXPORT MStatus uninitializePlugin(MObject obj)
{
	MFnPlugin plugin(obj);

	MGlobal::displayInfo("Maya plugin unloaded!");

	MDGMessage::removeCallback(sId);

	return MS::kSuccess;
}