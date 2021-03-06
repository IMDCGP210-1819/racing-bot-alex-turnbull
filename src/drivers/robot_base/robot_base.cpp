/***************************************************************************

    file                 : robot_base.cpp
    created              : Mon 13 Feb 11:40:23 GMT 2017
    copyright            : (C) 2002 Author

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <math.h>

#include <tgf.h> 
#include <track.h> 
#include <car.h> 
#include <raceman.h> 
#include <robottools.h>
#include <robot.h>

#include <iostream>

#include "behaviourTree.h"
#include "blackboard.h"

static tTrack	*curTrack;

static void initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation *s); 
static void newrace(int index, tCarElt* car, tSituation *s); 
static void drive(int index, tCarElt* car, tSituation *s); 
static void endrace(int index, tCarElt *car, tSituation *s);
static void shutdown(int index);
static int  InitFuncPt(int index, void *pt); 
Sequence *root;

//function called to build the behaviour tree when the race/car is initialized
static auto createTree()
{
	//declaring the nodes that will be needed/included in the behaviour tree
	Sequence *root = new Sequence, *driveSequence = new Sequence;
	Selector *selector1 = new Selector;
	Sequence *stuckSequence = new Sequence();

	carStatus *status = new carStatus{false};

	CheckIfStuck *checkStuck = new CheckIfStuck(status);
	AttemptToUnstick *unstick1 = new AttemptToUnstick(status);
	Accelerate *accelerate1 = new Accelerate(status);
	Brake *brake1 = new Brake();
	Turn *turn1 = new Turn();
	ChangeGear *changegear1 = new ChangeGear();
	

	//building the tree by applying/connection the child nodes into the correct parents 
	root->addChild(selector1);

	stuckSequence->addChild(checkStuck);
	stuckSequence->addChild(unstick1);

	selector1->addChild(stuckSequence);
	selector1->addChild(driveSequence);	

	driveSequence->addChild(accelerate1);
	driveSequence->addChild(turn1);
	driveSequence->addChild(brake1);
	driveSequence->addChild(changegear1);

	return root;

}


/* 
 * Module entry point  
 */ 
extern "C" int 
robot_base(tModInfo *modInfo) 
{
    memset(modInfo, 0, 10*sizeof(tModInfo));

    modInfo->name    = strdup("robot_base");		/* name of the module (short) */
    modInfo->desc    = strdup("");	/* description of the module (can be long) */
    modInfo->fctInit = InitFuncPt;		/* init function */
    modInfo->gfId    = ROB_IDENT;		/* supported framework version */
    modInfo->index   = 1;

    return 0; 
} 

/* Module interface initialization. */
static int 
InitFuncPt(int index, void *pt) 
{ 
    tRobotItf *itf  = (tRobotItf *)pt; 

    itf->rbNewTrack = initTrack; /* Give the robot the track view called */ 
				 /* for every track change or new race */ 
    itf->rbNewRace  = newrace; 	 /* Start a new race */
    itf->rbDrive    = drive;	 /* Drive during race */
    itf->rbPitCmd   = NULL;
    itf->rbEndRace  = endrace;	 /* End of the current race */
    itf->rbShutdown = shutdown;	 /* Called before the module is unloaded */
    itf->index      = index; 	 /* Index used if multiple interfaces */
	root = createTree(); //create the behaviour tree
    return 0; 
} 

/* Called for every track change or new race. */
static void  
initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation *s) 
{ 
    curTrack = track;
    *carParmHandle = NULL; 
} 

/* Start a new race. */
static void  
newrace(int index, tCarElt* car, tSituation *s) 
{ 
	//assign the car into the blackboard instance
	blackboard::Instance()->car = car;

}

/* Drive during race. */
static void  
drive(int index, tCarElt* car, tSituation *s) 
{ 
	memset((void *)&car->ctrl, 0, sizeof(tCarCtrl));

	//execute the tree
	if (root->run()) {}


	//////////////////////////////
	//Debugging Values


	std::cout << "-----------------------" << std::endl;
	std::cout << car->pub.trkPos.type << std::endl;
	std::cout << car->pub.trkPos.toMiddle << std::endl;
	std::cout << car->pub.speed << std::endl;
	std::cout << "-----------------------" << std::endl;

	float angle;
    const float SC = 1.0;

    angle = RtTrackSideTgAngleL(&(car->_trkPos)) - car->_yaw;
    NORM_PI_PI(angle); // put the angle back in the range from -PI to PI
    angle -= SC*car->_trkPos.toMiddle/car->_trkPos.seg->width;

    // set up the values to return
	std::cout << "-----------------------" << std::endl;
	std::cout << car->_steerLock << std::endl;
	std::cout << angle << std::endl;
	std::cout << car->pub.speed << std::endl;
	std::cout << car->_trkPos.seg->next->type << std::endl;
	std::cout << car->race.curLapTime << std::endl;
	std::cout << car->race.distRaced << std::endl;
	std::cout << "-----------------------" << std::endl;
	/////////////////////////////

}

/* End of the current race */
static void
endrace(int index, tCarElt *car, tSituation *s)
{
}

/* Called before the module is unloaded */
static void
shutdown(int index)
{
}
