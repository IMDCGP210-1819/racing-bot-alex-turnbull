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

static tTrack	*curTrack;

static void initTrack(int index, tTrack* track, void *carHandle, void **carParmHandle, tSituation *s); 
static void newrace(int index, tCarElt* car, tSituation *s); 
static void drive(int index, tCarElt* car, tSituation *s); 
static void endrace(int index, tCarElt *car, tSituation *s);
static void shutdown(int index);
static int  InitFuncPt(int index, void *pt); 
Sequence *root;

static auto createTree()
{
	Sequence *root = new Sequence, *sequence1 = new Sequence;

	Accelerate *accelerate1 = new Accelerate();
	Brake *brake1 = new Brake();
	Turn *turn1 = new Turn();
	ChangeGear *changegear1 = new ChangeGear();

	root->addChild(sequence1);
	sequence1->addChild(accelerate1);
	sequence1->addChild(brake1);
	sequence1->addChild(turn1);
	sequence1->addChild(changegear1);
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
	root = createTree();
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

	//while (!root->run())
	//	std::cout << "--------------------" << std::endl;
	//std::cout << std::endl << "Operation complete.  Behaviour tree exited." << std::endl;

}

/* Drive during race. */
static void  
drive(int index, tCarElt* car, tSituation *s) 
{ 
	memset((void *)&car->ctrl, 0, sizeof(tCarCtrl));

	if (root->run()) {}

	////car->ctrl.gear = 1;



 ////   car->ctrl.accelCmd = 1.0;
	////if(car->pub.trkPos.toMiddle > 0)
	////{
	////	car->ctrl.steer = -0.1;
	////}
	////else
	////{
	////	car->ctrl.steer = 0.1;
	////}
	////std::cout << "-----------------------" << std::endl;
	////std::cout << car->pub.trkPos.type << std::endl;
	////std::cout << car->pub.trkPos.toMiddle << std::endl;
	////std::cout << car->pub.speed << std::endl;
	////std::cout << "-----------------------" << std::endl;

	//float angle;
 //   const float SC = 1.0;

 //   angle = RtTrackSideTgAngleL(&(car->_trkPos)) - car->_yaw;
 //   NORM_PI_PI(angle); // put the angle back in the range from -PI to PI
 //   angle -= SC*car->_trkPos.toMiddle/car->_trkPos.seg->width;

 //   // set up the values to return
	//std::cout << "-----------------------" << std::endl;
	//std::cout << car->_steerLock << std::endl;
	//std::cout << angle << std::endl;
	//std::cout << car->pub.speed << std::endl;
	//std::cout << car->_trkPos.seg->next->type << std::endl;
	//std::cout << car->_trkPos.seg->arc << std::endl;
	//std::cout << "-----------------------" << std::endl;

	//float speed = car->pub.speed;

 //   car->ctrl.steer = angle / car->_steerLock;
 //   car->ctrl.gear = 1; // first gear
	//if (speed > 15)
	//{
	//	car->ctrl.gear = 2;
	//}if (speed > 30)
	//{
	//	car->ctrl.gear = 3;
	//}if (speed > 40)
	//{
	//	car->ctrl.gear = 4;
	//}if(speed > 50)
	//{
	//	car->ctrl.gear = 5;
	//}if(speed > 60)
	//{
	//	car->ctrl.gear = 6;
	//}
 //   car->ctrl.accelCmd = 0.4; // 30% accelerator pedal
	//if(speed > 30 && car->_trkPos.seg->next->type != 3)
	//	car->ctrl.accelCmd = 0.0;
	//	car->ctrl.brakeCmd = calculateBraking(speed,car->_trkPos.seg->next->arc);
 //   car->ctrl.brakeCmd = 0.0; // no brakes

	

    /* 
     * add the driving code here to modify the 
     * car->_steerCmd 
     * car->_accelCmd 
     * car->_brakeCmd 
     * car->_gearCmd 
     * car->_clutchCmd 
	 if (car->priv.enginerpm >= car->priv.enginerpmRedLine)
     */ 
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
