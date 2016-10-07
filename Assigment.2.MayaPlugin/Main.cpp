#include "maya_includes.h"
#include <iostream>

using namespace std;
MCallbackId NACId, NCCId, WMMCId;

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
	MString string = "# A Node has been Created named " + nodeName + " #";
	MGlobal::displayInfo(string);
}

void onNameChanged(MObject &node, const MString &str, void *clientData)
{
	MString newName;

	if (node.hasFn(MFn::kDagNode)) {
		MFnDagNode dagNode(node);
		newName = dagNode.fullPathName();
	}
	else if (node.hasFn(MFn::kDependencyNode)) {
		MFnDependencyNode node(node);
		newName = node.name();
	}

	MString string = "# A Node has Changed Name from " + str + " to " + newName + " #";
	MGlobal::displayInfo(string);
}

void onTransformationNodeChanged(MObject &transformNode, MDagMessage::MatrixModifiedFlags &modified, void *clientData)
{
	MString string = "# She Moves Her Own Way #";
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

	MDagPath dagPath;
	MStatus status;
	MStatus re;

	if (!obj.isNull())
	{
		status = MDagPath::getAPathTo(obj, dagPath);
		if (status.error())
		{
			MGlobal::displayInfo("# DagPath SNAFU #");
		}
		else
		{
			WMMCId = MDagMessage::addWorldMatrixModifiedCallback(dagPath, onTransformationNodeChanged, NULL, &r);

			if (r == MS::kFailure)
				MGlobal::displayInfo("# Transformation Changed Failed #");
		}
	}
	
	return res;
}

EXPORT MStatus uninitializePlugin(MObject obj)
{
	MFnPlugin plugin(obj);

	MGlobal::displayInfo("Maya plugin unloaded!");

	MDGMessage::removeCallback(NACId);
	MNodeMessage::removeCallback(NCCId);
	MDagMessage::removeCallback(WMMCId);

	return MS::kSuccess;
}