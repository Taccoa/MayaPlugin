#include "maya_includes.h"
#include <iostream>

using namespace std;
MCallbackIdArray callbackIds;

float time = 0;

void onNameChanged(MObject &node, const MString &str, void *clientData)
{
	MString string;
	string = "# A Node has Changed Name from " + str + " to " + MFnDagNode(node).name() + " #";
	MGlobal::displayInfo(string);
}

void onTransformationNodeChanged(MObject& transformNode, MDagMessage::MatrixModifiedFlags& modified, void* clientData)
{
		MString string = "# " + MFnTransform(transformNode).name() + " Transformation has Changed #";
		MGlobal::displayInfo(string);
}

void onTimeChanged(float elapsedTime, float lastTime, void *clientData)
{
	MString string = "# Time : ";
	MString string2 = " #";
	time += elapsedTime;
	MGlobal::displayInfo(string + time + string2);
}

void onVertexMoved(MNodeMessage::AttributeMessage msg, MPlug &plug1, MPlug &plug2, void*clientData)
{
	if (msg & MNodeMessage::kAttributeSet && !plug1.isArray() && plug1.isElement())
	{
		MString string;
		float x = plug1.child(0).asFloat();
		float y = plug1.child(1).asFloat();
		float z = plug1.child(2).asFloat();
		string = "# " + plug1.name() + " has changed attributes to " + x + ", " + y + ", " + z + ", " + " #";
		MGlobal::displayInfo(string);
	}
}

void onNodeCreated(MObject& node, void* clientData)
{
	MString string;
	if (node.hasFn(MFn::kTransform)) {
		string = "# A Node has been Created named " + MFnTransform(node).name() + " #";
		MGlobal::displayInfo(string);
		MStatus r = MS::kSuccess;
		MFnTransform transform = node;
		MDagPath meshDag = MDagPath::getAPathTo(transform.child(0));
		MCallbackId newId = MDagMessage::addWorldMatrixModifiedCallback(meshDag, onTransformationNodeChanged, NULL, &r);
		if (r == MS::kSuccess)
		{
			if (callbackIds.append(newId) == MS::kSuccess)
			{
				MGlobal::displayInfo("# Transform Changed Succeeded #");
			}
			else
				MGlobal::displayInfo("# Name Changed Callback Failed #");
		}
		newId = MNodeMessage::addNameChangedCallback(transform.child(0), onNameChanged, NULL, &r);
		if (r == MS::kSuccess)
		{
			if (callbackIds.append(newId) == MS::kSuccess)
			{
				MGlobal::displayInfo("# Name Changed Succeeded #");
			}
			else
				MGlobal::displayInfo("# Name Changed Callback Failed #");
		}
		if (transform.child(0).hasFn(MFn::kMesh))
		{
			newId = MNodeMessage::addAttributeChangedCallback(transform.child(0), onVertexMoved, NULL, &r);
			if (r == MS::kSuccess)
			{
				if (callbackIds.append(newId) == MS::kSuccess)
					MGlobal::displayInfo("# Vertex Moved Succeeded #");
			}
			else
				MGlobal::displayInfo("# Vertex Moved Callback Failed #");
		}
	}
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

	//Node Created
	MCallbackId newId = MDGMessage::addNodeAddedCallback(onNodeCreated, kDefaultNodeType, NULL, &r);
	
	if (r == MS::kSuccess)
	{
		if(callbackIds.append(newId) == MS::kSuccess)
			MGlobal::displayInfo("# Node Added Succeeded #");
	}
	else
		MGlobal::displayInfo("# Node Added Callback Failed #");

	//Timer
	newId = MTimerMessage::addTimerCallback(5, onTimeChanged, NULL, &r);

	if (r == MS::kSuccess)
	{
		if (callbackIds.append(newId) == MS::kSuccess)
			MGlobal::displayInfo("# Timer Succeeded #");
	}
	else
		MGlobal::displayInfo("# Timer Callback Failed #");

	//Node Name Changed, Transformation and Vertex moved
	MItDag meshIt(MItDag::kBreadthFirst, MFn::kTransform, &res);
	for (; !meshIt.isDone(); meshIt.next())
	{
		MFnTransform transform = meshIt.currentItem();
		if (transform.child(0).hasFn(MFn::kCamera) || transform.child(0).hasFn(MFn::kMesh))
		{
			MDagPath meshDag = MDagPath::getAPathTo(transform.child(0));
			MCallbackId newId = MNodeMessage::addNameChangedCallback(transform.child(0), onNameChanged, NULL, &r);

			if (r == MS::kSuccess)
			{
				if (callbackIds.append(newId) == MS::kSuccess)
					MGlobal::displayInfo("# Name Changed Succeeded #");
			}
			else
				MGlobal::displayInfo("# Name Changed Callback Failed #");

			newId = MDagMessage::addWorldMatrixModifiedCallback(meshDag, onTransformationNodeChanged, NULL, &r);

			if (r == MS::kSuccess)
			{
				if (callbackIds.append(newId) == MS::kSuccess)
					MGlobal::displayInfo("# Transform Changed Succeeded #");
			}
			else
				MGlobal::displayInfo("# Transform Changed Callback Failed #");
		}
		if (transform.child(0).hasFn(MFn::kMesh))
		{
			MCallbackId newId = MNodeMessage::addAttributeChangedCallback(transform.child(0), onVertexMoved, NULL, &r);

			if (r == MS::kSuccess)
			{
				if (callbackIds.append(newId) == MS::kSuccess)
					MGlobal::displayInfo("# Vertex Moved Succeeded #");
			}
			else
				MGlobal::displayInfo("# Vertex Moved Callback Failed #");
		}
	}
	
	return res;
}

EXPORT MStatus uninitializePlugin(MObject obj)
{
	MFnPlugin plugin(obj);

	MGlobal::displayInfo("Maya plugin unloaded!");

	MMessage::removeCallbacks(callbackIds);

	return MS::kSuccess;
}