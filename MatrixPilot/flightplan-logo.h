// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2012 MatrixPilot Team
// See the AUTHORS.TXT file for a list of authors of MatrixPilot.
//
// MatrixPilot is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MatrixPilot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MatrixPilot.  If not, see <http://www.gnu.org/licenses/>.


////////////////////////////////////////////////////////////////////////////////
// UDB LOGO Waypoint handling

// Move on to the next waypoint when getting within this distance of the current goal (in meters)
#define WAYPOINT_RADIUS         25

// Origin Location
// When using relative waypoints, the default is to interpret those waypoints as relative to the
// plane's power-up location.  Here you can choose to use any specific, fixed 3D location as the
// origin point for your relative waypoints.
//
// USE_FIXED_ORIGIN should be 0 to use the power-up location as the origin for relative waypoints.
// Set it to 1 to use a fixed location as the origin, no matter where you power up.
// FIXED_ORIGIN_LOCATION is the location to use as the origin for relative waypoints.  It uses the
// format { X, Y, Z } where:
// X is Logitude in degrees * 10^7
// Y is Latitude in degrees * 10^7
// Z is altitude above sea level, in meters, as a floating point value.
// 
// If you are using waypoints for an autonomous landing, it is a good idea to set the altitude value
// to be the altitude of the landing point, and then express the heights of all of the waypoints with
// respect to the landing point.
// If you are using OpenLog, an easy way to determine the altitude of your landing point is to
// examine the telemetry after a flight, take a look in the .csv file, it will be easy to spot the
// altitude, expressed in meters.

//#define USE_FIXED_ORIGIN        0
//#define FIXED_ORIGIN_LOCATION   { -1219950467, 374124664, 2.00 }    // A point in Baylands Park in Sunnyvale, CA


////////////////////////////////////////////////////////////////////////////////
// UDB LOGO Flight Planning definitions
// 
// The UDB Logo flight plan language lets you use a language similar to Logo, aka Turtle graphics, to
// control your plane.  You are commanding an imaginary "turtle" to move to specific locations, and the
// plane will head towards the turtle.
// 
// You can also control the camera targeting code by switching from the plane turtle, to the camera turtle
// by using the SET_TURTLE(CAMERA) command.  Then logo commands will move the location that the camera
// is targeting, instead of the location to aim the plane.
// 
// Each time you enter waypoint mode, the state is reset and your logo program starts from the top.  If
// you enter RTL mode, the state is reset and your RTL logo program is run instead.
// The following state is cleared when entering waypoint mode or RTL mode: (but not when your program
// ends and starts over)
//   - The plane turtle and camera turtle begin at the plane's current position and altitude.
//   - Both turtles begin pointing in the plane's current heading.
//   - The flags are all turned off.
//   - The pen is down, and the PLANE turtle is active.
// 
// To use UDB Logo, set FLIGHT_PLAN_TYPE to FP_LOGO in options.h.



////////////////////////////////////////////////////////////////////////////////
// Commands
// 
// Use the following commands to create your logo paths:
// 
// HOME                 - Return the turtle to the origin, aiming North.
// 
// FD(x)                - Move the turtle forward x meters, in the turtle's current direction.
// BK(x)                - Move the turtle backwards x meters, in the turtle's current direction.
// USE_CURRENT_POS      - Move the turtle to the plane's current {X,Y} position.  Mostly useful
//                        while being sneaky using PEN_UP.

// RT(x)                - Rotate the turtle to the right by x degrees.
// LT(x)                - Rotate the turtle to the left by x degrees.
// RT_BANK              - Perform roll to a fixed bank x deg for 30 deg heading change to the right and fly on for ~2 sec, position/navigation will be ignored (THERMALLING_MISSION)
// LEVEL_1S             - Perform level flight for 1 second, position/navigation will be ignored. This is used for centering in thermals (THERMALLING_MISSION)
// SET_ANGLE(x)         - Set the turtle to point x degrees clockwise from N.
// USE_CURRENT_ANGLE    - Aim the turtle in the direction that the plane is currently headed.
// USE_ANGLE_TO_GOAL    - Aim the turtle in the direction of the goal from the location of the plane.

// EAST(x)              - Move the turtle x meters East.
// WEST(x)              - Move the turtle x meters West.
// SET_X_POS(x)         - Set the X value of the turtle (meters East of the origin) to x.
// 
// NORTH(y)             - Move the turtle y meters North.
// SOUTH(y)             - Move the turtle y meters South.
// SET_Y_POS(y)         - Set the Y value of the turtle (meters North of the origin) to y.
// 
// SET_POS(x, y)        - Set both x and y at the same time.
// SET_ABS_POS(x, y)    - Set absolute X,Y location (long,lat) in degrees * 10^7

// ALT_UP(z)            - Gain z meters of altitude.
// ALT_DOWN(z)          - Drop z meters of altitude.
// SET_ALT(z)           - Set altitude to z.

// SPEED_INCREASE(x)    - Increases the target speed by x m/s
// SPEED_DECREASE(x)    - Decreases the target speed by x m/s
// SET_SPEED(x)         - Sets the target speed to x m/s

// REPEAT(n)            - Repeat all of the instructions until the matching END, n times
// REPEAT_FOREVER       - Repeat all of the instructions until the matching END, forever
// END                  - End the current REPEAT loop or Subroutine definition

// IF_EQ(val, x)        - Looks up a system value (listed below) and checks if it's equal to x.
//                        If so, runs commands until reaching ELSE or END.  If not, skips to ELSE 
//                        and runs until END, or just skips to END if there's no ELSE.
//                        Available IF commands: IF_EQ(equal), IF_NE(not equal), 
//                        IF_GT(val>x), IF_LT(val<x),IF_GE(val>=x), IF_LE(val<=x).
// ELSE                 - Starts a list of commands that get run if the preceding IF failed.

// PEN_UP               - While the pen is up, logo code execution does not stop to wait for the
//                        plane to move to each new position of the turtle before continuing.
//                        This allows you to use multiple logo instructions to get the turtle to
//                        the next goal location before commanding the plane to fly there by 
//                        putting the pen back down.
// PEN_DOWN             - When the pen is down, the plane moves to each new position of the turtle
//                        before more logo instructions are interpereted.
// PEN_TOGGLE           - Toggle the pen between up and down.

// SET_TURTLE(T)        - Choose to control either the plane's turtle, or the camera turtle.
//                        Use either SET_TURTLE(PLANE) or SET_TURTLE(CAMERA).


// Commands for Modifying Flags
// 
// FLAG_ON(F)           - Turn on flag F.  (See below for a list of flags.)
// FLAG_OFF(F)          - Turn off flag F.
// FLAG_TOGGLE(F)       - Toggle flag F.
// 
// The supported flags are the following:
// 
// F_TAKEOFF            - More quickly gain altitude at takeoff.
// F_INVERTED           - Fly with the plane upside down. (only if STABILIZE_INVERTED_FLIGHT is set to 1 in options.h)
// F_HOVER              - Hover the plane with the nose up. (only if STABILIZE_HOVER is set to 1 in options.h)
//                        NOTE: while hovering, no navigation is performed, and throttle is under manual control.
// F_TRIGGER            - Trigger an action to happen at this point in the flight.  (See the Trigger Action section of the options.h file.) 
// F_ALTITUDE_GOAL      - Climb or descend to the given altitude.
// F_CROSS_TRACK        - Navigate using cross-tracking.  Best used for longer flight legs.
// F_LAND               - Fly with the throttle off.


// Commands for Creating and Calling Subroutines
//
// TO(FUNC)             - Begin defining subroutine FUNC (requires #define FUNC N where N is an
//                        integer, unique among your defined subroutines.  End each subroutine
//                        definition with an END.
// DO(FUNC)             - Run subroutine FUNC.  When it finishes, control returns to the line after
//                        the DO() instruction.
// EXEC(FUNC)           - Call FUNC as though it were the beginning of the logo program.  This will never return.
//                        When/if FUNC finishes, logo will start back at the beginning of the program.
// DO(LOGO_MAIN) or
// EXEC(LOGO_MAIN)      - Restart at the top of the LOGO program
// DO_ARG(FUNC, PARAM)  - Run subroutine FUNC, using an integer value as a parameter.
// EXEC_ARG(FUNC, PARAM)- Exec subroutine FUNC, using an integer value as a parameter.
// 
// FD_PARAM             - From within a subroutine, call the FD command using the parameter
//                        passed to this subroutine as the distance.
// RT_PARAM             - From within a subroutine, call the RT command using the parameter
//                        passed to this subroutine as the angle.
// REPEAT_PARAM         - Start a REPEAT block, using the current subroutine's parameter as the
//                        number of times to repeat.
// DO_PARAM(FUNC)       - Call subroutine FUNC with a parameter equal to the current subroutine's
//                        parameter value.
// 
// PARAM_ADD(x)         - Adds x to the current subroutine's current parameter value.  Fun
//                        inside repeats inside subroutines!
// PARAM_SUB(x)         - Subtracts x from the current subroutine's current parameter value.
// PARAM_MUL(x)         - Multiplies the current subroutine's current parameter value by x.
// PARAM_DIV(x)         - Divides the current subroutine's current parameter value by x.
// PARAM_SET(x)         - Sets the current subroutine's current parameter value to x.
// 
// LOAD_TO_PARAM(val)   - Loads a system value (listed below) into the current subroutine's parameter value.
// 
// All parameter-related commands: 
//        FD_PARAM, BK_PARAM, RT_PARAM, LT_PARAM, SET_ANGLE_PARAM, 
//        EAST_PARAM, WEST_PARAM, NORTH_PARAM, SOUTH_PARAM, ALT_UP_PARAM, ALT_DOWN_PARAM, 
//        SET_X_POS_PARAM, SET_Y_POS_PARAM, SET_ALT_PARAM, 
//        SPEED_INCREASE_PARAM, SPEED_DECREASE_PARAM, SET_SPEED_PARAM
//        REPEAT_PARAM, DO_PARAM(FUNC), EXEC_PARAM(FUNC)
//        PARAM_SET(x), PARAM_ADD(x), PARAM_SUB(x), PARAM_MUL(x), PARAM_DIV(x)
//        IF_EQ_PARAM(x), IF_NE_PARAM(x), IF_GT_PARAM(x), IF_LT_PARAM(x), IF_GE_PARAM(x), IF_LE_PARAM(x)


// SET_INTERRUPT(f)     - Sets a user-defined logo function to be called at 40Hz.  Be careful not to modify
//                        the turtle location from within your interrupt function unless you really want to!
//                        Usually you'll just want your interrupt function to check some condition, and do
//                        something only if it's true.  (Like fly home only if you get too far away.)
// CLEAR_INTERRUPT      - Clears/disables the interrupt function.  Not usually needed.


// System Values for use with LOAD_TO_PARAM(val) and IF_XX() commands
// 
// DIST_TO_HOME         - in m
// DIST_TO_GOAL         - in m
// ALT                  - in m
// CURRENT_ANGLE        - in degrees. 0-359 (clockwise, 0=North)
// ANGLE_TO_HOME        - in degrees. 0-359 (clockwise, 0=North)
// ANGLE_TO_GOAL        - in degrees. 0-359 (clockwise, 0=North)
// REL_ANGLE_TO_HOME    - in degrees. -180-179 (0=heading directly towards Home. Home to the right of the nose of the plane is positive)
// REL_ANGLE_TO_GOAL    - in degrees. -180-179 (0=heading directly towards Goal. Goal to the right of the nose of the plane is positive)
// REL_ANGLE_TO_UPWIND_POINT - in degrees. -180-179 (0=heading directly towards Upwind point. Upwind point to the right of the nose of the plane is positive) (THERMALLING_MISSION)
// GROUND_SPEED         - in cm/s
// AIR_SPEED            - in cm/s
// AIR_SPEED_Z          - in cm/s
// WIND_SPEED           - in cm/s
// WIND_SPEED_X         - in cm/s
// WIND_SPEED_Y         - in cm/s
// WIND_SPEED_Z         - in cm/s
// WIND_FROM_ANGLE      - in degrees Wind from 0-359 (clockwise, 0=North).  (THERMALLING_MISSION)
// PARAM                - current param value
// XX_INPUT_CHANNEL     - channel value from 2000-4000 (any channel defined in options.h, e.g. THROTTLE_INPUT_CHANNEL)



////////////////////////////////////////////////////////////////////////////////
// Notes:
//  - Altitudes are relative to the starting point, and the initial altitude goal is 100 meters up.
//  - All angles are in degrees.
//  - Repeat commands and subroutines can be nested up to 12-deep.
//  - If the end of the list of instructions is reached, we start over at the top from the current location and angle.
//    This does not take up one of the 12 nested repeat levels.
//  - If you use many small FD() commands to make curves, I suggest enabling cross tracking: FLAG_ON(F_CROSS_TRACK)
//  - All Subroutines have to appear after the end of your main logo program.


////////////////////////////////////////////////////////////////////////////////
// Define the main flight plan as:
// 
// #define FOO 1
// 
// const struct logoInstructionDef instructions[] = {
//		instruction1
//		instruction2
//		etc.
//		END
//
//		TO (FOO)
//			etc.
//		END
//	};
// 
// and the Failsafe RTL course as:
// 
// #define BAR 2
// 
// const struct logoInstructionDef rtlInstructions[] = {
//		instruction1
//		instruction2
//		etc.
//		END
//
//		TO (BAR)
//			etc.
//		END
//	};


/*
////////////////////////////////////////////////////////////////////////////////
// Main Flight Plan
//
// Fly a 100m square at an altitude of 100m, beginning above the origin, pointing North

#define SQUARE 1

const struct logoInstructionDef instructions[] = {

	SET_ALT(100)

	// Go Home and point North
	HOME

	REPEAT_FOREVER
		DO_ARG(SQUARE, 100)
	END

	TO (SQUARE)
		REPEAT(4)
			FD_PARAM
			RT(90)
		END
	END
};

////////////////////////////////////////////////////////////////////////////////
// RTL Flight Plan
// 
// On entering RTL mode, turn off the engine, fly home, and circle indefinitely until touching down

const struct logoInstructionDef rtlInstructions[] = {

	// Use cross-tracking for navigation
	FLAG_ON(F_CROSS_TRACK)

	// Turn off engine for RTL
	// Move this line down below the HOME to return home with power before circling unpowered.
	FLAG_ON(F_LAND)

	// Fly home
	HOME

	// Once we arrive home, aim the turtle in the
	// direction that the plane is already moving.
	USE_CURRENT_ANGLE

	REPEAT_FOREVER
		// Fly a circle (36-point regular polygon)
		REPEAT(36)
			RT(10)
			FD(8)
		END
	END
	
};
*/


////////////////////////////////////////////////////////////////////////////////
// More Examples

/*
// Fly a 200m square starting at the current location and altitude, in the current direction
	REPEAT(4)
		FD(200)
		RT(90)
	END
*/

/*
// Fly a round-cornered square
	FLAG_ON(F_CROSS_TRACK)

	REPEAT(4)
		FD(170)
		REPEAT(6)
			LT(15)
			FD(10)
		END
	END
*/

/*
// Set the camera target to a point 100m North of the origin, then circle that point
	SET_TURTLE(CAMERA)
	HOME
	FD(100)
	SET_TURTLE(PLANE)

	FLAG_ON(F_CROSS_TRACK)

	HOME
	LT(90)

	REPEAT_FOREVER
		// Fly a circle (36-point regular polygon)
		REPEAT(36)
			RT(10)
			FD(20)
		END
	END
*/

/*
// Fly a giant, 2.5km diameter, 10-pointed star with external loops at each point
	FLAG_ON(F_CROSS_TRACK)

	REPEAT(10)
		FD(2000)

		REPEAT(18)
			RT(14) // == RT((180+72)/18)
			FD(50)
		END
	END
*/

/*
// Come in for an automatic landing at the HOME position
// from the current direction of the plane.
// 1. Aim for 32m altitude at 250m from HOME
// 2. Fly to 200m from HOME and turn off power
// 3. Aim for -32m altitude, 200m past home, which should
//    touch down very close to HOME.

	FLAG_ON(F_CROSS_TRACK)

	SET_ALT(32)

	PEN_UP
	HOME
	USE_ANGLE_TO_GOAL
	BK(250)
	PEN_DOWN
	FLAG_ON(F_LAND)
	PEN_UP
	HOME
	USE_ANGLE_TO_GOAL
	BK(200)
	PEN_DOWN
	SET_ALT(-32)
	PEN_UP
	HOME
	USE_ANGLE_TO_GOAL
	FD(200)
	PEN_DOWN
*/

/*
// Example of using some math on PARAM values to make cool spirals
#define SPIRAL_IN                   1
#define SPIRAL_OUT                  2
#define FWD_100_MINUS_PARAM_OVER_2  3

const struct logoInstructionDef instructions[] = {
	
DO_ARG(SPIRAL_IN, 10)
RT(100)
DO_ARG(SPIRAL_OUT,  70)

END



TO (SPIRAL_IN)
	REPEAT(30)
		DO_PARAM(FWD_100_MINUS_PARAM_OVER_2)
		RT_PARAM
		PARAM_ADD(2)
	END
END


TO (SPIRAL_OUT)
	REPEAT(30)
		PARAM_SUB(2)
		RT_PARAM
		DO_PARAM(FWD_100_MINUS_PARAM_OVER_2)
	END
END


TO (FWD_100_MINUS_PARAM_OVER_2)
	PARAM_MUL(-1)
	PARAM_ADD(100)
	PARAM_DIV(2)
	FD_PARAM
END
*/

/*
// Example of using an interrupt handler to stop the plane from getting too far away
// Notice mid-pattern if we get >200m away from home, and if so, fly home.
#define INT_HANDLER                 1

const struct logoInstructionDef instructions[] = {

SET_INTERRUPT(INT_HANDLER)

REPEAT_FOREVER
	FD(20)
	RT(10)
END

END


TO (INT_HANDLER)
	IF_GT(DIST_TO_HOME, 200)
		HOME
	END
END
*/

/*
// Example of using an interrupt handler to toggle between 2 flight plans.
// When starting the flightplan, decide whether to circle left or right, based on which direction
// initially turns towards home.  From then on, the circling direction can be changed by moving the
// rudder input channel to one side or the other.

#define CIRCLE_RIGHT                1
#define CIRCLE_LEFT                 2
#define INT_HANDLER_RIGHT           3
#define INT_HANDLER_LEFT            4

const struct logoInstructionDef instructions[] = {

IF_GT(REL_ANGLE_TO_HOME, 0)
	EXEC(CIRCLE_RIGHT)
ELSE
	EXEC(CIRCLE_LEFT)
END


TO (CIRCLE_RIGHT)
	USE_CURRENT_POS
	SET_INTERRUPT(INT_HANDLER_RIGHT)
	REPEAT_FOREVER
		FD(10)
		RT(10)
	END
END

TO (CIRCLE_LEFT)
	USE_CURRENT_POS
	SET_INTERRUPT(INT_HANDLER_LEFT)
	REPEAT_FOREVER
		FD(10)
		LT(10)
	END
END


TO (INT_HANDLER_RIGHT)
	IF_LT(RUDDER_INPUT_CHANNEL, 2800)
		EXEC(CIRCLE_LEFT)
	END
END

TO (INT_HANDLER_LEFT)
	IF_GT(RUDDER_INPUT_CHANNEL, 3200)
		EXEC(CIRCLE_RIGHT)
	END
END
};
*/

/*
////////////////////////////////////////////////////////////////////////////////
// Landing circuit Flight Plan
//

#define FINAL_ALT                      22  // in meters. Landing circuit: start of Final, used for 3 points in the landing circuit
//
// Fly a 100m square at an altitude of 100m, beginning above the origin, pointing North

#define SQUARE 1

#define SET_ALT_ALT                          62
#define LOITER_LAND                          57
#define DESCENT_PATTERN                      59
#define DOWNWIND                             41
#define BASE                                 43
#define FINAL                                45


const struct logoInstructionDef instructions[] = {


	//Main  -  main program when motor is off
    SET_ALT(100)

	//takeoff
	REPEAT(15)
		USE_CURRENT_ANGLE
		FD(12)
	END

    DO_ARG(SQUARE, 300)
	DO (LOITER_LAND)

	TO (SQUARE)
		REPEAT(4)
			FD_PARAM
			RT(90)
		END
	END



	TO (SET_ALT_ALT)
	//TO (FS_SET_ALT_ALT)
		//target the current altitude, preventing motor or butterfly

		//this is used for setting up the starting point of a slope
		//note: SET_ALT(ALT) is wrong!!!
		LOAD_TO_PARAM(ALT)
		SET_ALT_PARAM
	END
	END



	TO (DESCENT_PATTERN)
	//TO (FS_DESCENT_PATTERN)

		REPEAT(2)
			//turn
			REPEAT(18)
				IF_GT(ALT,FINAL_ALT*3)
					ALT_DOWN(1)  //keep going down
					FLAG_ON(F_LAND) //brake if you have to
				ELSE
					FLAG_OFF(F_LAND)  // no brakes
				END
				RT(20)
				FD(DESIRED_SPEED_NORMAL_F0/5)
			END
		END
	END
	END



	TO (LOITER_LAND)
	//TO (FS_LOITER_LAND)
		//Fly to circuit entrypoint

		//Motor may be activated (slider up) if < 80m and if the charge level of the battery is sufficient
		//Glide if > 80m
		//When close to the entry point;
 		//If high, circle down to 80m
		//if low (<50m) go straight for an "emergency" final, this may be with tailwind (dont know if the charge level of the battery is sufficient)
		CLEAR_INTERRUPT
		FLAG_ON(F_CROSS_TRACK)
		SET_SPEED(DESIRED_SPEED_NORMAL_F0) //dm/s

		FD(DESIRED_SPEED_NORMAL_F0/10)
		SET_ALT((FINAL_ALT*3)+10)
		//Motor may be activated (slider up) if low and if the charge level of the battery is sufficient
		FLAG_OFF(F_LAND)  //Motor on if needed, no brakes
		//to entry point
		PEN_UP

			// goto circuit entry point / hoog aanknopings punt
			HOME

			// Get circuit hand and heading  - configure these headings for your flying field
			// -270 == Lefthand circuit, final heading West 270, 270 == Righthand circuit, final heading West 270
			//	  NE	E	|	SE		S   ||   SW	     W   |   NW		 N|
			//  23-67,68-112,113-157,158-202, 203-247,248-292,293-337,338-22
			//                                 wind FROM
			IF_LT(WIND_FROM_ANGLE,203)                    //  NE	  E		 SE		 S   (N)
				IF_LT(WIND_FROM_ANGLE,113)                //  NE	  E   (N)
					IF_LT(WIND_FROM_ANGLE,64)             //  N(E)
						IF_LT(WIND_FROM_ANGLE,22)         //  0..22
						   PARAM_SET(-326)  	//  N
						ELSE
						   PARAM_SET(64)    	//  NE
						END
					ELSE
						PARAM_SET(64)        	//  E
					END
				ELSE                            //  SE		 S
					IF_LT(WIND_FROM_ANGLE,158)
						PARAM_SET(148)       	//  SE
					ELSE
						PARAM_SET(148)       	//  S
					END
				END
			ELSE
				//>= 203                        //  SW	  W      NW		 N
				IF_LT(WIND_FROM_ANGLE,293)               	//  SW	  W
					IF_LT(WIND_FROM_ANGLE,248)
						PARAM_SET(-243)      	//  SW
					ELSE
						PARAM_SET(-243)      	//  W
					END
				ELSE                            //  NW		 N
					IF_LT(WIND_FROM_ANGLE,338)
						PARAM_SET(-326)      	//  NW
					ELSE
						PARAM_SET(-326)      	//  N
					END
				END
			END

			IF_GT(PARAM,0)  //param > 0
				//right hand circuit
				SET_ANGLE_PARAM   //virtually upwind of runway
				FD(150)
				RT(90)
			ELSE
				//left hand circuit
				PARAM_MUL(-1)
				SET_ANGLE_PARAM   //virtually upwind of runway
				PARAM_MUL(-1)
				FD(150)
				LT(90)
			END
			FD(150)
		PEN_DOWN    //to entry point

		//point to downwind
		IF_GE(PARAM,0)  //param => 0
			//right hand circuit
			SET_ANGLE_PARAM   //virtually upwind of runway
		ELSE
			//left hand circuit
			PARAM_MUL(-1)
			SET_ANGLE_PARAM   //virtually upwind of runway
			PARAM_MUL(-1)
		END
		//reverse heading, same for left and right
		RT(180)

		FD(DESIRED_SPEED_NORMAL_F0/3) //~3 sec  bring pattern closer to Home

		//set up an up-slope to entry point  in case we are low, else just glide there
		REPEAT_FOREVER
			//We have arrived above the circuit entry point
			//If high, fly circles down to 80m
			//if low (<50m) go straight for an "emergency" final  (dont know if the charge level of the battery is sufficient)

			IF_GT(ALT,(FINAL_ALT*3)+20)
				DO (SET_ALT_ALT)
				//DO (FS_SET_ALT_ALT)
				DO (DESCENT_PATTERN)   //use a subroutine to preserve PARAM with circuit data
				//DO (FS_DESCENT_PATTERN)   //use a subroutine to preserve PARAM with circuit data
			ELSE
				IF_GT(ALT,(FINAL_ALT)+10)  //else go straight for an "emergency" final
					DO (SET_ALT_ALT)
					//DO (FS_SET_ALT_ALT)

					// goto circuit entry point again / hoog aanknopings punt
					PEN_UP

						HOME

						IF_GE(PARAM,0)  //param => 0
							//right hand circuit
							SET_ANGLE_PARAM   //virtually upwind of runway
							FD(150)
							RT(90)
						ELSE
							//left hand circuit
							PARAM_MUL(-1)
							SET_ANGLE_PARAM   //virtually upwind of runway
							PARAM_MUL(-1)
							FD(150)
							LT(90)
						END
						FD(150)
					PEN_DOWN

					//Downwind
					DO_PARAM (DOWNWIND)
					//DO_PARAM (FS_DOWNWIND)

					//Base
					DO_PARAM (BASE)
					//DO_PARAM (FS_BASE)
				END // >50 m

				//Final
				DO_PARAM (FINAL)
				//DO_PARAM (FS_FINAL)

			END // 70m
 		END // repeat
	END
	END



	TO (DOWNWIND)
	//TO (FS_DOWNWIND)
		// assume 50m < alt < 80m

		SET_ALT((FINAL_ALT*2))
		FLAG_ON(F_LAND) //brake if you have to
		//turn to and fly the downwind leg
		PEN_UP
			HOME
			IF_GE(PARAM,0)  //param => 0
				//right hand circuit
				SET_ANGLE_PARAM   //virtually upwind of runway
				RT(90)
				FD(150)
				RT(90)
			ELSE
				//left hand circuit
				PARAM_MUL(-1)
				SET_ANGLE_PARAM   //virtually upwind of runway
				PARAM_MUL(-1)
				LT(90)
				FD(150)
				LT(90)
			END
			FD(150)
		PEN_DOWN

		FLAG_OFF(F_LAND) // no braking in turns
		IF_GE(PARAM,0)  //param => 0
			//right hand circuit
			REPEAT(5)
				RT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT*2)
					ALT_DOWN(1)
				END
			END
		ELSE
			//left hand circuit
			REPEAT(5)
				LT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT*2)
					ALT_DOWN(1)
				END
			END
		END
	END
	END



	TO (BASE)
	//TO (FS_BASE)
		//assume alt less than 50m

		FLAG_OFF(F_LAND) // no braking in turns

		//turn to and fly the base leg
		SET_SPEED(DESIRED_SPEED_NORMAL_F0) //dm/s
		//set up the startingpoint of the glideslope
		IF_GE(PARAM,0)  //param => 0
			//right hand circuit
			REPEAT(5)               //5 not 4: compensate offset flighttrack vs waypoint for precise landing
				RT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT*2-5)
				    ALT_DOWN(1)
				END
			END
		ELSE
			//left hand circuit
			REPEAT(5)               //5 not 4: compensate offset flighttrack vs waypoint for precise landing
				LT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT*2-5)
					ALT_DOWN(1)
				END
			END
		END
		SET_ALT(FINAL_ALT)
		PEN_UP     //base leg
			//set up the endpoint of the glideslope
			FD(60)
			FLAG_ON(F_LAND)    //use brakes / butterfly
		PEN_DOWN

		DO (SET_ALT_ALT)
		//DO (FS_SET_ALT_ALT)
		FLAG_OFF(F_LAND)    // no brakes in turn
		IF_GE(PARAM,0)  //param => 0
			//right hand circuit
			REPEAT(5)            
				RT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT-5)
				    ALT_DOWN(2)
				END
			END
		ELSE
			//left hand circuit
			REPEAT(5)
				LT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT-5)
				    ALT_DOWN(2)
				END    
			END
		END
	END
	END



	TO (FINAL)
	//TO (FS_FINAL)
		//turn to and fly the final

		//assume alt less than 50m  (target 35m)
		FLAG_OFF(F_LAND)    // no brakes in turn
		//set up the startingpoint of the glideslope
		IF_GE(PARAM,0)  //param => 0
			//right hand circuit
			REPEAT(4)
				RT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT)
				    ALT_DOWN(2)
				END    
			END
		ELSE
			//left hand circuit
			REPEAT(4)
				LT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT)
				    ALT_DOWN(2)
				END    
			END
		END


		//set up the endpoint of the glideslope   1:7
		FLAG_ON(F_LAND)     //use brakes / butterfly
		SET_ALT(FINAL_ALT*-1) //target altitude for the next waypoint below ground
		PEN_UP
			HOME
			USE_ANGLE_TO_GOAL
			//compensate offset in flighttrack vs waypoint for precise landing
			IF_GE(PARAM,0)  //param => 0
				//right hand circuit
				RT(10)    
			ELSE
				//left hand circuit
				LT(10)
			END

			FD(80)
		PEN_DOWN

		// go around if landing did not happen
		EXEC (LOGO_MAIN)

	END
	END

};
*/


// ****************************************************************
//    LET e-glider mission - 2015
// ****************************************************************

// This script performs a LET (Local Endurance Thermalling) mission
// It does geofencing, motor climbs and thermalling
// The program navigates the area randomly to search for rising air.
// The program detects and then moves the glider to the center of thermals, even if the thermals drift downwind.
// The script has two similar sets of scripts, for two main program flows "gliding" and "motor climb"
// This is needed to remember a motorclimb was started in case of an interruption (there is no method to read a flag in LOGO)
// Two types of geofences are used: normal geofence and wind geofence
// The wind geofence allows for optimisation of the position of the glider, to have enough space to thermal if rising air is found.
// the wind geofence is only enforced if no thermals are found
// the soft geofences are smaller in size and are intended to position the aircraft in the geofence, crossing the bigger (real) geofence should be a rare event
// The motor is controlled to maintain altitude when no thermals are found.
// The fixed throttle climbrate is optimized for longer flights. The motor is stopped when too much sink or much lift is encountered, to preserve battery power.
// multiple smaller subroutines are used to allow recording as waypoints in telemetry for debugging
// 'Check' routines have an even number, odd numbered subroutines (actions) will be logged as waypoint
// 'Forward' commands are usualy timed to last about 1 sec
// Gps data is used to measure altitude and climbrate
// Glide speed control is selected by LOGO (slow, normal and fast). Flaps (camber) are controlled simultaneously (F4,F0 or F-4)

// to use this script:
//
//1. add this line to Options.h:
//  #define THERMALLING_MISSION        1    // enable code to calculate angle and distance to UPWIND_POINT
//
//2. #define WAYPOINT_RADIUS           40
//
// select AIRFRAME_TYPE  AIRFRAME_GLIDER   (required)
// 		for pure gliders setup THROTTLE_INPUT_CHANNEL  UNUSED; the code for motor control will be disabled
//		configure mixer settings in servomix.c  (after changing mixer settings only this module will need recompilation.)
// For this mission GLIDE_AIRSPEED_CONTROL is recommended
// To enable GLIDE_AIRSPEED_CONTROL setup the following options:
//  - in options.h             SPEED_CONTROL = 1          and configure DESIRED_SPEED
//	- in airspeed_options.h    GLIDE_AIRSPEED_CONTROL = 1  and configure settings for your aircraft
//	- in gain_variables.h      ALTITUDE_GAINS_VARIABLE=1
//
// see https://groups.google.com/forum/#!topic/uavdevboard/yn5PnR6pk7Q
//

#define GEOFENCE_SIZE                 400  // radius of circle to keep aircaft within line of sight
#define UPWIND_POINT_FACTOR           0.5  // multiply "windspeed in cm/s" by factor for distance from home i.e. windspeed = 540 cm/s (3 bft) * 0.5 = 270 M from home
#define UPWIND_POINT_DISTANCE_LIMIT   270  // in meters

#define MOTOR_ON_TRIGGER_ALT          200  // in meters
#define MOTOR_ON_IN_SINK_ALT          180  // in meters, set low. Altitude where ground objects must be avoided using motor despite sink
#define MOTOR_OFF_TRIGGER_ALT         230  // in meters
#define MAX_THERMALLING_ALT           300  // in meters

#define CLIMBR_THERMAL_TRIGGER         40  // cm/sec >= 0.2 m/s climb is the trigger to start thermalling
#define CLIMBR_THERMAL_CLIMB_MIN     -140  // cm/sec > -1.0 maximum sink allowed, else abort thermalling
#define MOTOR_CLIMB_MIN               -50  // cm/sec minimal climbrate that is expected   else abort the Motor climb
#define MOTOR_CLIMB_MAX               320  // cm/sec maximal climbrate that is expected   else start thermalling

#define FINAL_ALT                      22  // in meters. Landing circuit: start of Final, used for 3 points in the landing circuit


//Geofences
#define CRUISE                                3
#define RETURN_GEOFENCE                       5
#define CHECK_WIND_GEOFENCE                   7
#define CHECK_SOFT_GEOFENCE                   9
#define CHECK_SOFT_WIND_GEOFENCE             11

//Thermals
#define CHECK_THERMALS                       12
#define WAIT_DECREASE_CLIMBRATE              13
#define THERMALLING_TURN                     15
#define THERMALLING_SHIFT_CIRCLE             17
#define SINK                                 19

#define INT_FORCE_TARGET_AHAID                2

//Motor
#define MOTOR_CLIMB                          21    // main program when motor is on
#define RETURN_MC_GEOFENCE                   23
#define CHECK_MC_SOFT_WIND_GEOFENCE          25
#define MOTOR_CLIMB_FORWARD                  27
#define CHECKS_MC                            30    // is motor still needed, landing requested, is pilot in control?
#define CHECK_MC_WIND_GEOFENCE               31
#define SOFT_CHECKS_MC                       32
#define PILOT_INPUT_IN_MC                    33
#define CHECK_MC_SOFT_GEOFENCE               35

//Misc
#define SOFT_CHECKS                          46
#define PILOT_INPUT                          47
#define CHECKS                               48    // is motor needed, landing requested, is pilot in control?
#define CHECKS_RETURN_GF                     50
#define TOO_HIGH                             53
#define SET_ALT_ALT                          54
#define BETTER_LIFT                          55
#define RESET_NAVIGATION                     56

#define LOITER_LAND                          57
#define DESCENT_PATTERN                      59
#define DOWNWIND                             41
#define BASE                                 43
#define FINAL                                45

//#define FAILSAFE                             61
#define FS_SET_ALT_ALT                       62
#define FS_DOWNWIND                          63
#define FS_BASE                              65
#define FS_FINAL                             67
#define FS_DESCENT_PATTERN                   69
#define FS_LOITER_LAND                       71

#if ( THERMALLING_MISSION == 1 )
const struct logoInstructionDef instructions[] = {


	//Main  -  main program when motor is off

		//cleanup   
		SET_ALT(MAX_THERMALLING_ALT-20)    // the last 20m will be used to gradually apply brakes  - depends on brake gain
		FLAG_ON(F_LAND)	 //Motor off
		SET_SPEED(DESIRED_SPEED_NORMAL_F0) //dm/s
		PEN_DOWN
		SET_INTERRUPT(INT_FORCE_TARGET_AHAID)

		DO (CHECKS)
		DO (SOFT_CHECKS)
		DO (CHECK_THERMALS)	  //geofence will be monitored, end and restart if needed

		DO (CHECK_WIND_GEOFENCE)
		DO (CHECK_SOFT_GEOFENCE)
		DO (CHECK_SOFT_WIND_GEOFENCE)

		DO (CRUISE)   // prevent overshoots

	END
	END



	TO (CRUISE)

		FD(DESIRED_SPEED_NORMAL_F0/10)
	END
	END



	// Normal geofence routines

			
	TO (RETURN_GEOFENCE)
		//IF_GT(DIST_TO_HOME, ( GEOFENCE_SIZE * 2 / 3) )             // sgf  at least outside soft geofence
		//IF_GT(DIST_TO_UPWIND_POINT, GEOFENCE_SIZE )              // wgf  !!!non-standard LOGO command!!!
		//IF_GT(DIST_TO_UPWIND_POINT, ( GEOFENCE_SIZE * 4 / 5 ) )  // swgf !!!non-standard LOGO command!!!

			//perform a precalculated turn and a level stretch  to aim 30 deg right of home (home @ -30)
			LOAD_TO_PARAM(REL_ANGLE_TO_HOME)   // gf -179..180)
			//perform a precalculated turn and a level stretch to aim 30 deg right of virtual upwind point (@ -30)
			//LOAD_TO_PARAM(REL_ANGLE_TO_UPWIND_POINT)	// wgf !!!non-standard LOGO command!!! -	-179..180)

			IF_LT(REL_ANGLE_TO_HOME, -30) 			 //  gf	angle < -30 	0-140 L		 (-179-41)		  -179=150L  -30=10L
			//IF_LT(REL_ANGLE_TO_UPWIND_POINT, -30) // wgf	angle < -30 	0-140 L
				//make the turn to Home a smooth one
				PARAM_MUL(-1)
				PARAM_SUB(30)
				PARAM_DIV(10)
				REPEAT_PARAM
					DO (CHECKS)  //maintain min and max altitudes
					//DO (SOFT_CHECKS)
					//DO (CHECK_THERMALS)
					//DO (CHECKS_MC)
					//DO (SOFT_CHECKS_MC)
					//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
					LT(10)
					FD(DESIRED_SPEED_NORMAL_F0/10)
				END
			 ELSE
				IF_GT(REL_ANGLE_TO_HOME, 150) 			 // gf  angle > 151	  130-150 L 		(151-180 > 131-150 > 13..15 * 10)
				//IF_GT(REL_ANGLE_TO_UPWIND_POINT, 150) // wgf angle > 151	  130-150 L 		(151-180 > 131-150 > 13..15 * 10)
					PARAM_SUB(360)
					PARAM_MUL(-1)
					PARAM_DIV(10)
					REPEAT_PARAM
						DO (CHECKS)  //maintain min and max altitudes
						//DO (SOFT_CHECKS)
						//DO (CHECK_THERMALS)
						//DO (CHECKS_MC)
						//DO (SOFT_CHECKS_MC)
						//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
						LT(10)
						FD(DESIRED_SPEED_NORMAL_F0/10)
					END
				ELSE
					IF_GE(REL_ANGLE_TO_HOME, -30) 			 //  gf	angle > -19  (-19..150) = 0..179 R
					//IF_GE(REL_ANGLE_TO_UPWIND_POINT, -30) // wgf	angle > -19  (-19..150) = 0..179 R
						PARAM_ADD(30)
						PARAM_DIV(10)
						REPEAT_PARAM
							DO (CHECKS)  //maintain min and max altitudes
							//DO (SOFT_CHECKS)
							//DO (CHECK_THERMALS)
							//DO (CHECKS_MC)
							//DO (SOFT_CHECKS_MC)
							//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
							RT(10)
							FD(DESIRED_SPEED_NORMAL_F0/10)
						END
					END //-30
				END //>150
			END //-30

			// end with straight
			FD(DESIRED_SPEED_NORMAL_F0/2) //no need to wait for navigation to search for thermals
			//FD(DESIRED_SPEED_NORMAL_F0/10) //wait for navigation to search for thermals
			
			//fix a hangup (prevent nesting?)
			EXEC (LOGO_MAIN)
			
		//END //if
	END //geof
	END


	 TO (CHECK_SOFT_GEOFENCE)
		IF_GT(DIST_TO_HOME, ( GEOFENCE_SIZE * 2 / 3) )             // sgf  at least outside soft geofence
		//IF_GT(DIST_TO_UPWIND_POINT, GEOFENCE_SIZE )              // wgf  !!!non-standard LOGO command!!!
		//IF_GT(DIST_TO_UPWIND_POINT, ( GEOFENCE_SIZE * 4 / 5 ) )  // swgf !!!non-standard LOGO command!!!

			//perform a precalculated turn and a level stretch  to aim 30 deg right of home (home @ -30)
			LOAD_TO_PARAM(REL_ANGLE_TO_HOME)   // gf -179..180)
			//perform a precalculated turn and a level stretch to aim 30 deg right of virtual upwind point (@ -30)
			//LOAD_TO_PARAM(REL_ANGLE_TO_UPWIND_POINT)	// wgf !!!non-standard LOGO command!!! -	-179..180)

			IF_LT(REL_ANGLE_TO_HOME, -30) 			 //  gf	angle < -30 	0-140 L		 (-179-41)		  -179=150L  -30=10L
			//IF_LT(REL_ANGLE_TO_UPWIND_POINT, -30) // wgf	angle < -30 	0-140 L
				//make the turn to Home a smooth one
				PARAM_MUL(-1)
				PARAM_SUB(30)
				PARAM_DIV(10)
				REPEAT_PARAM
					DO (CHECKS)  //maintain min and max altitudes
					DO (SOFT_CHECKS)
					DO (CHECK_THERMALS)
					//DO (CHECKS_MC)
					//DO (SOFT_CHECKS_MC)
					//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
					LT(10)
					FD(DESIRED_SPEED_NORMAL_F0/10)
				END
			 ELSE
				IF_GT(REL_ANGLE_TO_HOME, 150) 			 // gf  angle > 151	  130-150 L 		(151-180 > 131-150 > 13..15 * 10)
				//IF_GT(REL_ANGLE_TO_UPWIND_POINT, 150) // wgf angle > 151	  130-150 L 		(151-180 > 131-150 > 13..15 * 10)
					PARAM_SUB(360)
					PARAM_MUL(-1)
					PARAM_DIV(10)
					REPEAT_PARAM
						DO (CHECKS)  //maintain min and max altitudes
						DO (SOFT_CHECKS)
						DO (CHECK_THERMALS)
						//DO (CHECKS_MC)
						//DO (SOFT_CHECKS_MC)
						//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
						LT(10)
						FD(DESIRED_SPEED_NORMAL_F0/10)
					END
				ELSE
					IF_GE(REL_ANGLE_TO_HOME, -30) 			 //  gf	angle > -19  (-19..150) = 0..179 R
					//IF_GE(REL_ANGLE_TO_UPWIND_POINT, -30) // wgf	angle > -19  (-19..150) = 0..179 R
						PARAM_ADD(30)
						PARAM_DIV(10)
						REPEAT_PARAM
							DO (CHECKS)  //maintain min and max altitudes
							DO (SOFT_CHECKS)
							DO (CHECK_THERMALS)
							//DO (CHECKS_MC)
							//DO (SOFT_CHECKS_MC)
							//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
							RT(10)
							FD(DESIRED_SPEED_NORMAL_F0/10)
						END
					END //-30
				END //>150
			END //-30

			// end with straight
			//FD(DESIRED_SPEED_NORMAL_F0/2) //no need to wait for navigation to search for thermals
			FD(DESIRED_SPEED_NORMAL_F0/10) //wait for navigation to search for thermals
		END //if
	END
	END



  //Wind geofence routines


	TO (CHECK_WIND_GEOFENCE)
		//IF_GT(DIST_TO_HOME, ( GEOFENCE_SIZE * 2 / 3) )             // sgf  at least outside soft geofence
		IF_GT(DIST_TO_UPWIND_POINT, GEOFENCE_SIZE )              // wgf  !!!non-standard LOGO command!!!
		//IF_GT(DIST_TO_UPWIND_POINT, ( GEOFENCE_SIZE * 4 / 5 ) )  // swgf !!!non-standard LOGO command!!!

			//perform a precalculated turn and a level stretch  to aim 30 deg right of home (home @ -30)
			LOAD_TO_PARAM(REL_ANGLE_TO_HOME)   // gf -179..180)
			//perform a precalculated turn and a level stretch to aim 30 deg right of virtual upwind point (@ -30)
			//LOAD_TO_PARAM(REL_ANGLE_TO_UPWIND_POINT)	// wgf !!!non-standard LOGO command!!! -	-179..180)

			IF_LT(REL_ANGLE_TO_HOME, -30) 			 //  gf	angle < -30 	0-140 L		 (-179-41)		  -179=150L  -30=10L
			//IF_LT(REL_ANGLE_TO_UPWIND_POINT, -30) // wgf	angle < -30 	0-140 L
				//make the turn to Home a smooth one
				PARAM_MUL(-1)
				PARAM_SUB(30)
				PARAM_DIV(10)
				REPEAT_PARAM
					DO (CHECKS)  //maintain min and max altitudes
					DO (SOFT_CHECKS)
					DO (CHECK_THERMALS)
					//DO (CHECKS_MC)
					//DO (SOFT_CHECKS_MC)
					//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
					LT(10)
					FD(DESIRED_SPEED_NORMAL_F0/10)
				END
			 ELSE
				IF_GT(REL_ANGLE_TO_HOME, 150) 			 // gf  angle > 151	  130-150 L 		(151-180 > 131-150 > 13..15 * 10)
				//IF_GT(REL_ANGLE_TO_UPWIND_POINT, 150) // wgf angle > 151	  130-150 L 		(151-180 > 131-150 > 13..15 * 10)
					PARAM_SUB(360)
					PARAM_MUL(-1)
					PARAM_DIV(10)
					REPEAT_PARAM
						DO (CHECKS)  //maintain min and max altitudes
						DO (SOFT_CHECKS)
						DO (CHECK_THERMALS)
						//DO (CHECKS_MC)
						//DO (SOFT_CHECKS_MC)
						//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
						LT(10)
						FD(DESIRED_SPEED_NORMAL_F0/10)
					END
				ELSE
					IF_GE(REL_ANGLE_TO_HOME, -30) 			 //  gf	angle > -19  (-19..150) = 0..179 R
					//IF_GE(REL_ANGLE_TO_UPWIND_POINT, -30) // wgf	angle > -19  (-19..150) = 0..179 R
						PARAM_ADD(30)
						PARAM_DIV(10)
						REPEAT_PARAM
							DO (CHECKS)  //maintain min and max altitudes
							DO (SOFT_CHECKS)
							DO (CHECK_THERMALS)
							//DO (CHECKS_MC)
							//DO (SOFT_CHECKS_MC)
							//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
							RT(10)
							FD(DESIRED_SPEED_NORMAL_F0/10)
						END
					END //-30
				END //>150
			END //-30

			// end with straight
			//FD(DESIRED_SPEED_NORMAL_F0/2) //no need to wait for navigation to search for thermals
			FD(DESIRED_SPEED_NORMAL_F0/10) //wait for navigation to search for thermals
		END //if
	END
	END



	TO (CHECK_SOFT_WIND_GEOFENCE)
		//IF_GT(DIST_TO_HOME, ( GEOFENCE_SIZE * 2 / 3) )             // sgf  at least outside soft geofence
		//IF_GT(DIST_TO_UPWIND_POINT, GEOFENCE_SIZE )              // wgf  !!!non-standard LOGO command!!!
		IF_GT(DIST_TO_UPWIND_POINT, ( GEOFENCE_SIZE * 4 / 5 ) )  // swgf !!!non-standard LOGO command!!!

			//perform a precalculated turn and a level stretch  to aim 30 deg right of home (home @ -30)
			LOAD_TO_PARAM(REL_ANGLE_TO_HOME)   // gf -179..180)
			//perform a precalculated turn and a level stretch to aim 30 deg right of virtual upwind point (@ -30)
			//LOAD_TO_PARAM(REL_ANGLE_TO_UPWIND_POINT)	// wgf !!!non-standard LOGO command!!! -	-179..180)

			IF_LT(REL_ANGLE_TO_HOME, -30) 			 //  gf	angle < -30 	0-140 L		 (-179-41)		  -179=150L  -30=10L
			//IF_LT(REL_ANGLE_TO_UPWIND_POINT, -30) // wgf	angle < -30 	0-140 L
				//make the turn to Home a smooth one
				PARAM_MUL(-1)
				PARAM_SUB(30)
				PARAM_DIV(10)
				REPEAT_PARAM
					DO (CHECKS)  //maintain min and max altitudes
					DO (SOFT_CHECKS)
					DO (CHECK_THERMALS)
					//DO (CHECKS_MC)
					//DO (SOFT_CHECKS_MC)
					//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
					LT(10)
					FD(DESIRED_SPEED_NORMAL_F0/10)
				END
			 ELSE
				IF_GT(REL_ANGLE_TO_HOME, 150) 			 // gf  angle > 151	  130-150 L 		(151-180 > 131-150 > 13..15 * 10)
				//IF_GT(REL_ANGLE_TO_UPWIND_POINT, 150) // wgf angle > 151	  130-150 L 		(151-180 > 131-150 > 13..15 * 10)
					PARAM_SUB(360)
					PARAM_MUL(-1)
					PARAM_DIV(10)
					REPEAT_PARAM
						DO (CHECKS)  //maintain min and max altitudes
						DO (SOFT_CHECKS)
						DO (CHECK_THERMALS)
						//DO (CHECKS_MC)
						//DO (SOFT_CHECKS_MC)
						//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
						LT(10)
						FD(DESIRED_SPEED_NORMAL_F0/10)
					END
				ELSE
					IF_GE(REL_ANGLE_TO_HOME, -30) 			 //  gf	angle > -19  (-19..150) = 0..179 R
					//IF_GE(REL_ANGLE_TO_UPWIND_POINT, -30) // wgf	angle > -19  (-19..150) = 0..179 R
						PARAM_ADD(30)
						PARAM_DIV(10)
						REPEAT_PARAM
							DO (CHECKS)  //maintain min and max altitudes
							DO (SOFT_CHECKS)
							DO (CHECK_THERMALS)
							//DO (CHECKS_MC)
							//DO (SOFT_CHECKS_MC)
							//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
							RT(10)
							FD(DESIRED_SPEED_NORMAL_F0/10)
						END
					END //-30
				END //>150
			END //-30

			// end with straight
			//FD(DESIRED_SPEED_NORMAL_F0/2) //no need to wait for navigation to search for thermals
			FD(DESIRED_SPEED_NORMAL_F0/10) //wait for navigation to search for thermals
		END //if
	END
	END





//Thermalling routines

	TO (CHECK_THERMALS)
		//check for thermals

		IF_GE(AIR_SPEED_Z,CLIMBR_THERMAL_TRIGGER)  //>= 0.4 m/s climb is the trigger		also check CHECK_INSIDE_WIND_GEOFENCE
			// lift found
			// checks
			//DO (CHECK_VERTICAL_PARAMETERS)
			// keep flying straight until decreasing lift
			//wait for decrease of lift
			DO (WAIT_DECREASE_CLIMBRATE)	  //wait up to 6 sec for the climbrate decrease, keep the best climbrate
			//current is less
			//now beyond the best climbrate..
			//turn up to 270 deg + 3sec straight if not better
			//abort the turn if better climbrate is found
			EXEC (THERMALLING_TURN)
			//every check: if positive, take action, then restart program
			EXEC (LOGO_MAIN)
		END // 0.4 m/s trigger
	END
	END



	TO (WAIT_DECREASE_CLIMBRATE)
		//wait up to 6 sec for the climbrate to decrease, keep the best climbrate

		LOAD_TO_PARAM(AIR_SPEED_Z)
		SET_SPEED(DESIRED_SPEED_SLOW_F4)
		REPEAT(30)    //6 sec * 5
		//IF_GE(AIR_SPEED_Z,PARAM) wrong! "val vs. val" not allowed // current >= last (while not decreasing)	 param may be equal to AIR_SPEED_Z first loop

			//roughly compare current climbrate and stored value, wait until current climbrate drops below PARAM
			IF_GE(AIR_SPEED_Z,200)
				IF_GE(PARAM,200)      //both better than 2.0 m/s: wait as long as possible
					LOAD_TO_PARAM(AIR_SPEED_Z)  //now this value will be used to decrease from
					FD(DESIRED_SPEED_SLOW_F4/50)  //1 sec / 5; small steps to detect changes sooner without waiting a long time for arrival
				END
			ELSE
				IF_GE(AIR_SPEED_Z,150)
					IF_GE(PARAM,150)
						LOAD_TO_PARAM(AIR_SPEED_Z)  //now this value will be used to decrease from
						FD(DESIRED_SPEED_SLOW_F4/50)  //1 sec / 5; small steps to detect changes sooner without waiting a long time for arrival
					END
				ELSE
					IF_GE(AIR_SPEED_Z,100)
						IF_GE(PARAM,100)
							LOAD_TO_PARAM(AIR_SPEED_Z)  //now this value will be used to decrease from
							FD(DESIRED_SPEED_SLOW_F4/50)  //1 sec / 5; small steps to detect changes sooner without waiting a long time for arrival
						END
					ELSE
						IF_GE(AIR_SPEED_Z,50)
							IF_GE(PARAM,50)
								LOAD_TO_PARAM(AIR_SPEED_Z)  //now this value will be used to decrease from
								FD(DESIRED_SPEED_SLOW_F4/50)  //1 sec / 5; small steps to detect changes sooner without waiting a long time for arrival
							END
						END
					END
 				END
			END
			DO (CHECKS)
			DO (SOFT_CHECKS)
		END  //repeat
		//return.. check not cruise..
	END
	END


	//270 deg method
	TO (THERMALLING_TURN)

		SET_SPEED(DESIRED_SPEED_SLOW_F4)
		LOAD_TO_PARAM(AIR_SPEED_Z)    //to detect better lift
		PARAM_ADD(50)				  //add a margin
		//do while turn upto 270 deg if climb does not improve irt startvalue	if improves: race to exit
		REPEAT(9) //14 sec =~ 270 deg = 9 * "30 deg per loop"

			//IF_LE_PARAM(AIR_SPEED_Z)  wrong! "val vs. val" not allowed // current <= start  == if not better	 //"current climbrate" <= "old climbrate" 	0.5 m/s margin , wait decr has made best lower

			//roughly compare current climbrate and stored value. if current better than stored+margin: exit better lift
			IF_GE(AIR_SPEED_Z,200)
				IF_LE(PARAM,150)      //both better than 2.0 m/s: wait as long as possible
					EXEC (BETTER_LIFT)  // report just once, start new thermalling cycle
				END
			ELSE
				IF_GE(AIR_SPEED_Z,150)
					IF_LE(PARAM,100)      //both better than 2.0 m/s: wait as long as possible
						EXEC (BETTER_LIFT)  // report just once, start new thermalling cycle
					END
				ELSE
					IF_GE(AIR_SPEED_Z,100)
						IF_LE(PARAM,50)      //both better than 2.0 m/s: wait as long as possible
							EXEC (BETTER_LIFT)  // report just once, start new thermalling cycle
						END
					END
 				END
			END

			//Custom solution using new command RT_BANK()
			RT_BANK(30)   // perform roll to a fixed bank x deg for 30 deg heading change to the right and fly on for ~2 sec, position/navigation will be ignored
			DO (RESET_NAVIGATION)

			DO (CHECKS)
			DO (SOFT_CHECKS)
		END  //repeat
		DO (RESET_NAVIGATION)

		//true in normal exit (no better climb found)

		//IF_LE_PARAM(AIR_SPEED_Z) wrong! "val vs. val" not allowed  // current <= start  == if not better	 //"current climbrate" <= "old climbrate" 	1.0 m/s margin , wait decr has made best lower

			//if param 0 (bug?) shift only done if current clibrate <= 0
			//Shift the circle for 3 sec
			EXEC (THERMALLING_SHIFT_CIRCLE)
		//ELSE
		//	EXEC (BETTER_LIFT)  // report just once, start new thermalling cycle
		END
	END
	END
	END


	TO (THERMALLING_SHIFT_CIRCLE)
		//Level off/Shift the circle for 3 sec, log the action as a "waypoint"
		SET_SPEED(DESIRED_SPEED_SLOW_F4)
		LEVEL_1S  //custom command
		LEVEL_1S  //custom command
		LEVEL_1S  //custom command
		DO (RESET_NAVIGATION)

		EXEC (LOGO_MAIN)
	END
	END


	TO (SINK)
		//show in telemetry, then MAIN
		//USE_CURRENT_ANGLE
		//USE_CURRENT_POS	//clear accumulated turtle errors
		//FD(WAYPOINT_RADIUS)	//waypoint margin: puts turtle on finishline
		SET_SPEED(DESIRED_SPEED_FAST_FMIN4) //dm/s
		FD(DESIRED_SPEED_FAST_FMIN4/10)	//"SINK"
		EXEC(LOGO_MAIN)
	END
	END



// Motor Climb  routines

	// main program when motor is on

	TO (MOTOR_CLIMB)    //use motor if too low, switch off if too high , check geofence

		//start motor
		//wait a few seconds
		//check climb >0.3 and < 1.0 m/s
		//else glide
		//new mc will start at MOTOR_ON_TRIGGER_ALT

		//direct to goal, add to target to prevent throttle down until altitude reached
		
		// modified use of F_LAND
		// set target higher and F_LAND off: motor starts        (used in motorclimb)
		// set target higher and F_LAND on : gliding             (used in looking for thermals, and thermalling below max)
		// set target lower and F_LAND off : gliding             (not used - maintain minimal alt)
		// set target lower and F_LAND on : brakes will be used  (used close to max alt and while landing)
		
		//SET_ALT(MAX_THERMALLING_ALT)
		FLAG_OFF(F_LAND)	 //Motor on
		
		PEN_DOWN
		//remember we are in motorclimb
		SET_SPEED(DESIRED_SPEED_NORMAL_F0)


		//settle into climb before testing climbrate
		REPEAT(5)
			//allow for level takeoff in current direection when in autonmous mode
		   	IF_LT(ALT, 10)  //below: auto takeoff / hand launch with motor on in Autonomous mode
				//if relative angle is much different from turtle,correct
   		        REPEAT(20)
 		            LEVEL_1S  //allow heading to stabilize on takeoff
   		        END
				DO (RESET_NAVIGATION)
			END
			DO (CHECKS_MC)
			DO (MOTOR_CLIMB_FORWARD)
			DO (SOFT_CHECKS_MC)
			DO (MOTOR_CLIMB_FORWARD)
		END
		//no return to main until done (0-250, consecutive 40 sec max), no thermalling
		REPEAT(300)
			IF_GT(ALT,MOTOR_OFF_TRIGGER_ALT)
				//settle into gliding
				FLAG_ON(F_LAND)	//Motor off
				//settle into gliding
				REPEAT(6)
					DO (CHECKS)
					DO (SOFT_CHECKS)
					DO (CRUISE)   // prevent overshoots
				END
				EXEC (LOGO_MAIN)
			END
			DO (CHECKS_MC)
			DO (SOFT_CHECKS_MC)
			DO (CHECK_MC_SOFT_GEOFENCE)
			DO (MOTOR_CLIMB_FORWARD)    // prevent overshoots
			DO (CHECK_MC_WIND_GEOFENCE)
			DO (CHECK_MC_SOFT_WIND_GEOFENCE)
			FLAG_OFF(F_LAND)	//Motor on
			DO (MOTOR_CLIMB_FORWARD)	// prevent overshoots
		END
		//timeout...
		//settle into gliding
		FLAG_ON(F_LAND)	//Motor off
		//settle into gliding
		REPEAT(6)
			DO (CHECKS)
			DO (SOFT_CHECKS)
			DO (CRUISE)
		END
		EXEC (LOGO_MAIN)
	END
	END



	TO (RETURN_MC_GEOFENCE)

		//IF_GT(DIST_TO_HOME, ( GEOFENCE_SIZE * 2 / 3) )             // sgf  at least outside soft geofence
		//IF_GT(DIST_TO_UPWIND_POINT, GEOFENCE_SIZE )              // wgf  !!!non-standard LOGO command!!!
		//IF_GT(DIST_TO_UPWIND_POINT, ( GEOFENCE_SIZE * 4 / 5 ) )  // swgf !!!non-standard LOGO command!!!

			//perform a precalculated turn and a level stretch  to aim 30 deg right of home (home @ -30)
			LOAD_TO_PARAM(REL_ANGLE_TO_HOME)   // gf -179..180)
			//perform a precalculated turn and a level stretch to aim 30 deg right of virtual upwind point (@ -30)
			//LOAD_TO_PARAM(REL_ANGLE_TO_UPWIND_POINT)	// wgf !!!non-standard LOGO command!!! -	-179..180)

			IF_LT(REL_ANGLE_TO_HOME, -30) 			 //  gf	angle < -30 	0-140 L		 (-179-41)		  -179=150L  -30=10L
			//IF_LT(REL_ANGLE_TO_UPWIND_POINT, -30) // wgf	angle < -30 	0-140 L
				//make the turn to Home a smooth one
				PARAM_MUL(-1)
				PARAM_SUB(30)
				PARAM_DIV(10)
				REPEAT_PARAM
					//DO (CHECKS)  //maintain min and max altitudes
					//DO (SOFT_CHECKS)
					//DO (CHECK_THERMALS)
					DO (CHECKS_MC)
					//DO (SOFT_CHECKS_MC)
					//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
					LT(10)
					FD(DESIRED_SPEED_NORMAL_F0/10)
				END
			 ELSE
				IF_GT(REL_ANGLE_TO_HOME, 150) 			 // gf  angle > 151	  130-150 L 		(151-180 > 131-150 > 13..15 * 10)
				//IF_GT(REL_ANGLE_TO_UPWIND_POINT, 150) // wgf angle > 151	  130-150 L 		(151-180 > 131-150 > 13..15 * 10)
					PARAM_SUB(360)
					PARAM_MUL(-1)
					PARAM_DIV(10)
					REPEAT_PARAM
						//DO (CHECKS)  //maintain min and max altitudes
						//DO (SOFT_CHECKS)
						//DO (CHECK_THERMALS)
						DO (CHECKS_MC)
						//DO (SOFT_CHECKS_MC)
						//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
						LT(10)
						FD(DESIRED_SPEED_NORMAL_F0/10)
					END
				ELSE
					IF_GE(REL_ANGLE_TO_HOME, -30) 			 //  gf	angle > -19  (-19..150) = 0..179 R
					//IF_GE(REL_ANGLE_TO_UPWIND_POINT, -30) // wgf	angle > -19  (-19..150) = 0..179 R
						PARAM_ADD(30)
						PARAM_DIV(10)
						REPEAT_PARAM
							//DO (CHECKS)  //maintain min and max altitudes
							//DO (SOFT_CHECKS)
							//DO (CHECK_THERMALS)
							DO (CHECKS_MC)
							//DO (SOFT_CHECKS_MC)
							//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
							RT(10)
							FD(DESIRED_SPEED_NORMAL_F0/10)
						END
					END //-30
				END //>150
			END //-30

			// end with straight
			FD(DESIRED_SPEED_NORMAL_F0/2) //no need to wait for navigation to search for thermals
			//FD(DESIRED_SPEED_NORMAL_F0/10) //wait for navigation to search for thermals
		//END //if
	END //geof
	END


	TO (CHECK_MC_SOFT_GEOFENCE)

		IF_GT(DIST_TO_HOME, ( GEOFENCE_SIZE * 2 / 3) )             // sgf  at least outside soft geofence
		//IF_GT(DIST_TO_UPWIND_POINT, GEOFENCE_SIZE )              // wgf  !!!non-standard LOGO command!!!
		//IF_GT(DIST_TO_UPWIND_POINT, ( GEOFENCE_SIZE * 4 / 5 ) )  // swgf !!!non-standard LOGO command!!!

			//perform a precalculated turn and a level stretch  to aim 30 deg right of home (home @ -30)
			LOAD_TO_PARAM(REL_ANGLE_TO_HOME)   // gf -179..180)
			//perform a precalculated turn and a level stretch to aim 30 deg right of virtual upwind point (@ -30)
			//LOAD_TO_PARAM(REL_ANGLE_TO_UPWIND_POINT)	// wgf !!!non-standard LOGO command!!! -	-179..180)

			IF_LT(REL_ANGLE_TO_HOME, -30) 			 //  gf	angle < -30 	0-140 L		 (-179-41)		  -179=150L  -30=10L
			//IF_LT(REL_ANGLE_TO_UPWIND_POINT, -30) // wgf	angle < -30 	0-140 L
				//make the turn to Home a smooth one
				PARAM_MUL(-1)
				PARAM_SUB(30)
				PARAM_DIV(10)
				REPEAT_PARAM
					//DO (CHECKS)  //maintain min and max altitudes
					//DO (SOFT_CHECKS)
					//DO (CHECK_THERMALS)
					DO (CHECKS_MC)
					DO (SOFT_CHECKS_MC)
					//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
					LT(10)
					FD(DESIRED_SPEED_NORMAL_F0/10)
				END
			 ELSE
				IF_GT(REL_ANGLE_TO_HOME, 150) 			 // gf  angle > 151	  130-150 L 		(151-180 > 131-150 > 13..15 * 10)
				//IF_GT(REL_ANGLE_TO_UPWIND_POINT, 150) // wgf angle > 151	  130-150 L 		(151-180 > 131-150 > 13..15 * 10)
					PARAM_SUB(360)
					PARAM_MUL(-1)
					PARAM_DIV(10)
					REPEAT_PARAM
						//DO (CHECKS)  //maintain min and max altitudes
						//DO (SOFT_CHECKS)
						//DO (CHECK_THERMALS)
						DO (CHECKS_MC)
						DO (SOFT_CHECKS_MC)
						//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
						LT(10)
						FD(DESIRED_SPEED_NORMAL_F0/10)
					END
				ELSE
					IF_GE(REL_ANGLE_TO_HOME, -30) 			 //  gf	angle > -19  (-19..150) = 0..179 R
					//IF_GE(REL_ANGLE_TO_UPWIND_POINT, -30) // wgf	angle > -19  (-19..150) = 0..179 R
						PARAM_ADD(30)
						PARAM_DIV(10)
						REPEAT_PARAM
							//DO (CHECKS)  //maintain min and max altitudes
							//DO (SOFT_CHECKS)
							//DO (CHECK_THERMALS)
							DO (CHECKS_MC)
							DO (SOFT_CHECKS_MC)
							//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
							RT(10)
							FD(DESIRED_SPEED_NORMAL_F0/10)
						END
					END //-30
				END //>150
			END //-30

			// end with straight
			//FD(DESIRED_SPEED_NORMAL_F0/2) //no need to wait for navigation to search for thermals
			FD(DESIRED_SPEED_NORMAL_F0/10) //wait for navigation to search for thermals
		END //if
	END //geof
	END


  //Mc Wind geofence routines



	TO (CHECK_MC_WIND_GEOFENCE)

		//IF_GT(DIST_TO_HOME, ( GEOFENCE_SIZE * 2 / 3) )             // sgf  at least outside soft geofence
		IF_GT(DIST_TO_UPWIND_POINT, GEOFENCE_SIZE )              // wgf  !!!non-standard LOGO command!!!
		//IF_GT(DIST_TO_UPWIND_POINT, ( GEOFENCE_SIZE * 4 / 5 ) )  // swgf !!!non-standard LOGO command!!!

			//perform a precalculated turn and a level stretch  to aim 30 deg right of home (home @ -30)
			LOAD_TO_PARAM(REL_ANGLE_TO_HOME)   // gf -179..180)
			//perform a precalculated turn and a level stretch to aim 30 deg right of virtual upwind point (@ -30)
			//LOAD_TO_PARAM(REL_ANGLE_TO_UPWIND_POINT)	// wgf !!!non-standard LOGO command!!! -	-179..180)

			IF_LT(REL_ANGLE_TO_HOME, -30) 			 //  gf	angle < -30 	0-140 L		 (-179-41)		  -179=150L  -30=10L
			//IF_LT(REL_ANGLE_TO_UPWIND_POINT, -30) // wgf	angle < -30 	0-140 L
				//make the turn to Home a smooth one
				PARAM_MUL(-1)
				PARAM_SUB(30)
				PARAM_DIV(10)
				REPEAT_PARAM
					//DO (CHECKS)  //maintain min and max altitudes
					//DO (SOFT_CHECKS)
					//DO (CHECK_THERMALS)
					DO (CHECKS_MC)
					DO (SOFT_CHECKS_MC)
					//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
					LT(10)
					FD(DESIRED_SPEED_NORMAL_F0/10)
				END
			 ELSE
				IF_GT(REL_ANGLE_TO_HOME, 150) 			 // gf  angle > 151	  130-150 L 		(151-180 > 131-150 > 13..15 * 10)
				//IF_GT(REL_ANGLE_TO_UPWIND_POINT, 150) // wgf angle > 151	  130-150 L 		(151-180 > 131-150 > 13..15 * 10)
					PARAM_SUB(360)
					PARAM_MUL(-1)
					PARAM_DIV(10)
					REPEAT_PARAM
						//DO (CHECKS)  //maintain min and max altitudes
						//DO (SOFT_CHECKS)
						//DO (CHECK_THERMALS)
						DO (CHECKS_MC)
						DO (SOFT_CHECKS_MC)
						//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
						LT(10)
						FD(DESIRED_SPEED_NORMAL_F0/10)
					END
				ELSE
					IF_GE(REL_ANGLE_TO_HOME, -30) 			 //  gf	angle > -19  (-19..150) = 0..179 R
					//IF_GE(REL_ANGLE_TO_UPWIND_POINT, -30) // wgf	angle > -19  (-19..150) = 0..179 R
						PARAM_ADD(30)
						PARAM_DIV(10)
						REPEAT_PARAM
							//DO (CHECKS)  //maintain min and max altitudes
							//DO (SOFT_CHECKS)
							//DO (CHECK_THERMALS)
							DO (CHECKS_MC)
							DO (SOFT_CHECKS_MC)
							//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
							RT(10)
							FD(DESIRED_SPEED_NORMAL_F0/10)
						END
					END //-30
				END //>150
			END //-30

			// end with straight
			//FD(DESIRED_SPEED_NORMAL_F0/2) //no need to wait for navigation to search for thermals
			FD(DESIRED_SPEED_NORMAL_F0/10) //wait for navigation to search for thermals
		END //if
	END //geof
	END


	TO (CHECK_MC_SOFT_WIND_GEOFENCE)

		//IF_GT(DIST_TO_HOME, ( GEOFENCE_SIZE * 2 / 3) )             // sgf  at least outside soft geofence
		//IF_GT(DIST_TO_UPWIND_POINT, GEOFENCE_SIZE )              // wgf  !!!non-standard LOGO command!!!
		IF_GT(DIST_TO_UPWIND_POINT, ( GEOFENCE_SIZE * 4 / 5 ) )  // swgf !!!non-standard LOGO command!!!

			//perform a precalculated turn and a level stretch  to aim 30 deg right of home (home @ -30)
			LOAD_TO_PARAM(REL_ANGLE_TO_HOME)   // gf -179..180)
			//perform a precalculated turn and a level stretch to aim 30 deg right of virtual upwind point (@ -30)
			//LOAD_TO_PARAM(REL_ANGLE_TO_UPWIND_POINT)	// wgf !!!non-standard LOGO command!!! -	-179..180)

			IF_LT(REL_ANGLE_TO_HOME, -30) 			 //  gf	angle < -30 	0-140 L		 (-179-41)		  -179=150L  -30=10L
			//IF_LT(REL_ANGLE_TO_UPWIND_POINT, -30) // wgf	angle < -30 	0-140 L
				//make the turn to Home a smooth one
				PARAM_MUL(-1)
				PARAM_SUB(30)
				PARAM_DIV(10)
				REPEAT_PARAM
					//DO (CHECKS)  //maintain min and max altitudes
					//DO (SOFT_CHECKS)
					//DO (CHECK_THERMALS)
					DO (CHECKS_MC)
					DO (SOFT_CHECKS_MC)
					//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
					LT(10)
					FD(DESIRED_SPEED_NORMAL_F0/10)
				END
			 ELSE
				IF_GT(REL_ANGLE_TO_HOME, 150) 			 // gf  angle > 151	  130-150 L 		(151-180 > 131-150 > 13..15 * 10)
				//IF_GT(REL_ANGLE_TO_UPWIND_POINT, 150) // wgf angle > 151	  130-150 L 		(151-180 > 131-150 > 13..15 * 10)
					PARAM_SUB(360)
					PARAM_MUL(-1)
					PARAM_DIV(10)
					REPEAT_PARAM
						//DO (CHECKS)  //maintain min and max altitudes
						//DO (SOFT_CHECKS)
						//DO (CHECK_THERMALS)
						DO (CHECKS_MC)
						DO (SOFT_CHECKS_MC)
						//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
						LT(10)
						FD(DESIRED_SPEED_NORMAL_F0/10)
					END
				ELSE
					IF_GE(REL_ANGLE_TO_HOME, -30) 			 //  gf	angle > -19  (-19..150) = 0..179 R
					//IF_GE(REL_ANGLE_TO_UPWIND_POINT, -30) // wgf	angle > -19  (-19..150) = 0..179 R
						PARAM_ADD(30)
						PARAM_DIV(10)
						REPEAT_PARAM
							//DO (CHECKS)  //maintain min and max altitudes
							//DO (SOFT_CHECKS)
							//DO (CHECK_THERMALS)
							DO (CHECKS_MC)
							DO (SOFT_CHECKS_MC)
							//DO (CHECK_THERMALS_MC)	  //geofence will be monitored, end and restart if needed
							RT(10)
							FD(DESIRED_SPEED_NORMAL_F0/10)
						END
					END //-30
				END //>150
			END //-30

			// end with straight
			//FD(DESIRED_SPEED_NORMAL_F0/2) //no need to wait for navigation to search for thermals
			FD(DESIRED_SPEED_NORMAL_F0/10) //wait for navigation to search for thermals
		END //if
	END //geof
	END



	TO (MOTOR_CLIMB_FORWARD)
		//start/continue a slow climb with motor

		FD(DESIRED_SPEED_NORMAL_F0/10)
	END
	END



	TO (TOO_HIGH)
		//indicates lift while too high

		//USE_CURRENT_ANGLE                  		//USE_CURRENT_POS	//clear accumulated turtle errors
		//FD(WAYPOINT_RADIUS)	//waypoint margin: puts turtle on finishline
		FD(DESIRED_SPEED_NORMAL_F0/10)
		EXEC (LOGO_MAIN)
	END
	END



	TO (BETTER_LIFT)
		//indicates lift is better then it was at the beginnning of the thermalling turn

		//USE_CURRENT_ANGLE
		//USE_CURRENT_POS	//clear accumulated turtle errors
		//FD(WAYPOINT_RADIUS)	//waypoint margin: puts turtle on finishline
		FD(DESIRED_SPEED_NORMAL_F0/10)
		EXEC(LOGO_MAIN)
	END
	END


	TO (CHECKS)        // is motor needed, landing requested, is pilot in control?   
	   	//see if calling subroutine needs to end

	   	IF_LT(ALT, MOTOR_ON_TRIGGER_ALT)    // not too low  check every cycle
#if ( THROTTLE_INPUT_CHANNEL != CHANNEL_UNUSED )     //no motor support in case of gliders
		   	IF_LT(ALT, MOTOR_ON_IN_SINK_ALT)    // not too low  check every cycle
				//very low, must use motor
		        IF_GT(THROTTLE_INPUT_CHANNEL, 3400)     // matches level at wich ESC would start motor, which is close to full throttle
  					EXEC (MOTOR_CLIMB) 					// if true, restart to main to avoid an extra nesting level
				END
			END
			IF_GT(AIR_SPEED_Z,MOTOR_CLIMB_MIN - 90)	// > ?? m/s climb is expected
		        IF_GT(THROTTLE_INPUT_CHANNEL, 3400)     // matches level at wich ESC would start motor, which is close to full throttle
					EXEC (MOTOR_CLIMB) 					// if true, restart to main to avoid an extra nesting level
				END
			END   
#endif
		END
		IF_LT(BRAKE_THR_SEL_INPUT_CHANNEL ,2700)     // automode only
			IF_GT(BRAKE_THR_SEL_INPUT_CHANNEL ,1700) // abstract flightplan workaround: only real low, ignore 0
				EXEC (LOITER_LAND)
			END
		END
		IF_LT(BATTERY_VOLTAGE, VOLTAGE_SENSOR_ALARM)     // land automatically when battery is low, usefull when no telemetry is available
			EXEC (LOITER_LAND)
		END
		IF_LT(AILERON_INPUT_CHANNEL ,2850)
			DO (PILOT_INPUT)
		END
		IF_GT(AILERON_INPUT_CHANNEL ,3150)
			DO (PILOT_INPUT)
		END
	END
	END


	TO (SOFT_CHECKS)           
	   	//see if calling subroutine needs to end

		//used by main, xgf an thermal
	    //not allowed to be called by RETURN_GEOFENCE  
		IF_GT(DIST_TO_HOME, GEOFENCE_SIZE )
			SET_SPEED(DESIRED_SPEED_NORMAL_F0) //dm/s
			EXEC (RETURN_GEOFENCE)
		END
	   	IF_GE(AIR_SPEED_Z,CLIMBR_THERMAL_TRIGGER)    // not too low  check every cycle
			//avoid extra nesting level?
			IF_GT(ALT,MAX_THERMALLING_ALT )	    // not too high
				EXEC (TOO_HIGH)					// if true, restart to main to avoid an extra nesting level
			END
		END
		IF_LT(AIR_SPEED_Z,CLIMBR_THERMAL_CLIMB_MIN) //limit sink to -1 m/s,	if so, exit the sink
			EXEC (SINK)
		END
	END	
	END



	TO (CHECKS_MC)      // is motor still needed, landing requested, is pilot in control?   
		//remember we are MotorClimb mode , use DO 's

		//MotorClimb -- Interrupt routine
		//reset on ail input
		IF_LT(THROTTLE_INPUT_CHANNEL ,2400)
			//stop motor, restart
			EXEC (LOGO_MAIN)
		END
		IF_LT(BRAKE_THR_SEL_INPUT_CHANNEL ,2700)     // automode only
			IF_GT(BRAKE_THR_SEL_INPUT_CHANNEL ,1700) // abstract flightplan workaround: only real low, ignore 0
				EXEC (LOITER_LAND)
			END
		END
		IF_LT(BATTERY_VOLTAGE, VOLTAGE_SENSOR_ALARM)     // land automatically when battery is low, usefull when no telemetry is available
			EXEC (LOITER_LAND)
		END
		IF_LT(AILERON_INPUT_CHANNEL ,2850)
			DO (PILOT_INPUT_IN_MC)
		END
		IF_GT(AILERON_INPUT_CHANNEL ,3150)
			DO (PILOT_INPUT_IN_MC)
		END
	END
	END



	TO (SOFT_CHECKS_MC)           //see if calling subroutine needs to end
	    //not allowed to be called by RETURN_GEOFENCE

		IF_GT(DIST_TO_HOME, GEOFENCE_SIZE )
			DO (RETURN_MC_GEOFENCE)
		END
		IF_GT(AIR_SPEED_Z, MOTOR_CLIMB_MAX) //Skip when in a thermal
			// lift found
			EXEC (LOGO_MAIN)
 		END
		IF_LT(AIR_SPEED_Z, MOTOR_CLIMB_MIN) //limit sink to -1 m/s,	if so, stop motor, exit the sink
			EXEC (LOGO_MAIN)
		END
	END	



	TO (RESET_NAVIGATION)
		//the pilot has changed the angle of the aircraft, now use that as the new heading

		PEN_UP
			USE_CURRENT_ANGLE
			USE_CURRENT_POS		//centre on waypoint 'here', removing the drift error
			FD(WAYPOINT_RADIUS)	//back to edge of radius,	target is now directly in front again (on arrival point)
		PEN_DOWN
	END
	END



	TO (PILOT_INPUT)

		REPEAT(10)			//keep pilot control as long stick is off-centre,max 10 loops
			IF_LT(AILERON_INPUT_CHANNEL ,2850)
				LT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
			END
			IF_GT(AILERON_INPUT_CHANNEL,3150)
				RT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
			END
		END
		EXEC (LOGO_MAIN)
	END
	END



	TO (PILOT_INPUT_IN_MC)

		REPEAT(10)			//keep pilot control as long stick is off-centre,max 10 loops
			IF_LT(AILERON_INPUT_CHANNEL ,2850)
				LT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
			END
			IF_GT(AILERON_INPUT_CHANNEL,3150)
				RT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
			END
		END
		EXEC (MOTOR_CLIMB)
	END
	END



	TO (SET_ALT_ALT)
	//TO (FS_SET_ALT_ALT)
		//target the current altitude, preventing motor or butterfly

		//this is used for setting up the starting point of a slope
		//note: SET_ALT(ALT) is wrong!!!
		LOAD_TO_PARAM(ALT)
		SET_ALT_PARAM
	END
	END



	TO (DESCENT_PATTERN)
	//TO (FS_DESCENT_PATTERN)

		REPEAT(2)
			//turn
			REPEAT(18)
				IF_GT(ALT,FINAL_ALT*3)
					ALT_DOWN(1)  //keep going down
					FLAG_ON(F_LAND) //brake if you have to
				ELSE
					FLAG_OFF(F_LAND)  // no brakes
				END
				RT(20)
				FD(DESIRED_SPEED_NORMAL_F0/5)
			END
		END
	END
	END



	TO (LOITER_LAND)
	//TO (FS_LOITER_LAND)
		//Fly to circuit entrypoint

		//Motor may be activated (slider up) if < 80m and if the charge level of the battery is sufficient
		//Glide if > 80m
		//When close to the entry point;
 		//If high, circle down to 80m
		//if low (<50m) go straight for an "emergency" final, this may be with tailwind (dont know if the charge level of the battery is sufficient)
		CLEAR_INTERRUPT
		FLAG_ON(F_CROSS_TRACK)
		SET_SPEED(DESIRED_SPEED_NORMAL_F0) //dm/s

		FD(DESIRED_SPEED_NORMAL_F0/10)
		SET_ALT((FINAL_ALT*3)+10)
		//Motor may be activated (slider up) if low and if the charge level of the battery is sufficient
		FLAG_OFF(F_LAND)  //Motor on if needed, no brakes
		//to entry point
		PEN_UP

			// goto circuit entry point / hoog aanknopings punt
			HOME

			// Get circuit hand and heading  - configure these headings for your flying field
			// -270 == Lefthand circuit, final heading West 270, 270 == Righthand circuit, final heading West 270
			//	  NE	E	|	SE		S   ||   SW	     W   |   NW		 N|
			//  23-67,68-112,113-157,158-202, 203-247,248-292,293-337,338-22
			//                                 wind FROM
			IF_LT(WIND_FROM_ANGLE,203)                    //  NE	  E		 SE		 S   (N)
				IF_LT(WIND_FROM_ANGLE,113)                //  NE	  E   (N)
					IF_LT(WIND_FROM_ANGLE,64)             //  N(E)
						IF_LT(WIND_FROM_ANGLE,22)         //  0..22
						   PARAM_SET(-326)  	//  N
						ELSE
						   PARAM_SET(64)    	//  NE
						END
					ELSE
						PARAM_SET(64)        	//  E
					END
				ELSE                            //  SE		 S
					IF_LT(WIND_FROM_ANGLE,158)
						PARAM_SET(148)       	//  SE
					ELSE
						PARAM_SET(148)       	//  S
					END
				END
			ELSE
				//>= 203                        //  SW	  W      NW		 N
				IF_LT(WIND_FROM_ANGLE,293)               	//  SW	  W
					IF_LT(WIND_FROM_ANGLE,248)
						PARAM_SET(-243)      	//  SW
					ELSE
						PARAM_SET(-243)      	//  W
					END
				ELSE                            //  NW		 N
					IF_LT(WIND_FROM_ANGLE,338)
						PARAM_SET(-326)      	//  NW
					ELSE
						PARAM_SET(-326)      	//  N
					END
				END
			END

			IF_GT(PARAM,0)  //param > 0
				//right hand circuit
				SET_ANGLE_PARAM   //virtually upwind of runway
				FD(150)
				RT(90)
			ELSE
				//left hand circuit
				PARAM_MUL(-1)
				SET_ANGLE_PARAM   //virtually upwind of runway
				PARAM_MUL(-1)
				FD(150)
				LT(90)
			END
			FD(150)
		PEN_DOWN    //to entry point

		//point to downwind
		IF_GE(PARAM,0)  //param => 0
			//right hand circuit
			SET_ANGLE_PARAM   //virtually upwind of runway
		ELSE
			//left hand circuit
			PARAM_MUL(-1)
			SET_ANGLE_PARAM   //virtually upwind of runway
			PARAM_MUL(-1)
		END
		//reverse heading, same for left and right
		RT(180)

		FD(DESIRED_SPEED_NORMAL_F0/3) //~3 sec  bring pattern closer to Home

		//set up an up-slope to entry point  in case we are low, else just glide there
		REPEAT_FOREVER
			//We have arrived above the circuit entry point
			//If high, fly circles down to 80m
			//if low (<50m) go straight for an "emergency" final  (dont know if the charge level of the battery is sufficient)

			IF_GT(ALT,(FINAL_ALT*3)+20)
				DO (SET_ALT_ALT)
				//DO (FS_SET_ALT_ALT)
				DO (DESCENT_PATTERN)   //use a subroutine to preserve PARAM with circuit data
				//DO (FS_DESCENT_PATTERN)   //use a subroutine to preserve PARAM with circuit data
			ELSE
				IF_GT(ALT,(FINAL_ALT)+10)  //else go straight for an "emergency" final
					DO (SET_ALT_ALT)
					//DO (FS_SET_ALT_ALT)

					// goto circuit entry point again / hoog aanknopings punt
					PEN_UP

						HOME

						IF_GE(PARAM,0)  //param => 0
							//right hand circuit
							SET_ANGLE_PARAM   //virtually upwind of runway
							FD(150)
							RT(90)
						ELSE
							//left hand circuit
							PARAM_MUL(-1)
							SET_ANGLE_PARAM   //virtually upwind of runway
							PARAM_MUL(-1)
							FD(150)
							LT(90)
						END
						FD(150)
					PEN_DOWN

					//Downwind
					DO_PARAM (DOWNWIND)
					//DO_PARAM (FS_DOWNWIND)

					//Base
					DO_PARAM (BASE)
					//DO_PARAM (FS_BASE)
				END // >50 m

				//Final
				DO_PARAM (FINAL)
				//DO_PARAM (FS_FINAL)

			END // 70m
 		END // repeat
	END
	END



	TO (DOWNWIND)
	//TO (FS_DOWNWIND)
		// assume 50m < alt < 80m

		SET_ALT((FINAL_ALT*2))
		FLAG_ON(F_LAND) //brake if you have to
		//turn to and fly the downwind leg
		PEN_UP
			HOME
			IF_GE(PARAM,0)  //param => 0
				//right hand circuit
				SET_ANGLE_PARAM   //virtually upwind of runway
				RT(90)
				FD(150)
				RT(90)
			ELSE
				//left hand circuit
				PARAM_MUL(-1)
				SET_ANGLE_PARAM   //virtually upwind of runway
				PARAM_MUL(-1)
				LT(90)
				FD(150)
				LT(90)
			END
			FD(150)
		PEN_DOWN

		FLAG_OFF(F_LAND) // no braking in turns
		IF_GE(PARAM,0)  //param => 0
			//right hand circuit
			REPEAT(5)
				RT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT*2)
					ALT_DOWN(1)
				END
			END
		ELSE
			//left hand circuit
			REPEAT(5)
				LT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT*2)
					ALT_DOWN(1)
				END
			END
		END
	END
	END



	TO (BASE)
	//TO (FS_BASE)
		//assume alt less than 50m

		FLAG_OFF(F_LAND) // no braking in turns

		//turn to and fly the base leg
		SET_SPEED(DESIRED_SPEED_NORMAL_F0) //dm/s
		//set up the startingpoint of the glideslope
		IF_GE(PARAM,0)  //param => 0
			//right hand circuit
			REPEAT(5)               //5 not 4: compensate offset flighttrack vs waypoint for precise landing
				RT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT*2-5)
				    ALT_DOWN(1)
				END
			END
		ELSE
			//left hand circuit
			REPEAT(5)               //5 not 4: compensate offset flighttrack vs waypoint for precise landing
				LT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT*2-5)
					ALT_DOWN(1)
				END
			END
		END
		SET_ALT(FINAL_ALT)
		PEN_UP     //base leg
			//set up the endpoint of the glideslope
			FD(60)
			FLAG_ON(F_LAND)    //use brakes / butterfly
		PEN_DOWN

		DO (SET_ALT_ALT)
		//DO (FS_SET_ALT_ALT)
		FLAG_OFF(F_LAND)    // no brakes in turn
		IF_GE(PARAM,0)  //param => 0
			//right hand circuit
			REPEAT(5)            
				RT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT-5)
				    ALT_DOWN(2)
				END
			END
		ELSE
			//left hand circuit
			REPEAT(5)
				LT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT-5)
				    ALT_DOWN(2)
				END    
			END
		END
	END
	END



	TO (FINAL)
	//TO (FS_FINAL)
		//turn to and fly the final

		//assume alt less than 50m  (target 35m)
		FLAG_OFF(F_LAND)    // no brakes in turn
		//set up the startingpoint of the glideslope
		IF_GE(PARAM,0)  //param => 0
			//right hand circuit
			REPEAT(4)
				RT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT)
				    ALT_DOWN(2)
				END    
			END
		ELSE
			//left hand circuit
			REPEAT(4)
				LT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT)
				    ALT_DOWN(2)
				END    
			END
		END


		//set up the endpoint of the glideslope   1:7
		FLAG_ON(F_LAND)     //use brakes / butterfly
		SET_ALT(FINAL_ALT*-1) //target altitude for the next waypoint below ground
		PEN_UP
			HOME
			USE_ANGLE_TO_GOAL
			//compensate offset in flighttrack vs waypoint for precise landing
			IF_GE(PARAM,0)  //param => 0
				//right hand circuit
				RT(10)    
			ELSE
				//left hand circuit
				LT(10)
			END

			FD(80)
		PEN_DOWN

		// go around if landing did not happen
		EXEC (LOGO_MAIN)

	END
	END


   	TO (INT_FORCE_TARGET_AHAID)  //interrupt routine
		//IF_GT(DIST_TO_GOAL, WAYPOINT_RADIUS )  // not arrived
			//check if relative angle is much different from planes angle, if so, correct
			IF_LT(REL_ANGLE_TO_GOAL,-60)
				PEN_UP
					USE_CURRENT_ANGLE
					USE_CURRENT_POS		//centre on waypoint 'here', removing the drift error
					FD(WAYPOINT_RADIUS)	//back to edge of radius,	target is now directly in front again (on arrival point)
				PEN_DOWN
			ELSE
				IF_GT(REL_ANGLE_TO_GOAL,60)
					//DO (RESET_NAVIGATION)
					PEN_UP
						USE_CURRENT_ANGLE
						USE_CURRENT_POS		//centre on waypoint 'here', removing the drift error
						FD(WAYPOINT_RADIUS)	//back to edge of radius,	target is now directly in front again (on arrival point)
					PEN_DOWN
				END
			END
	END
	END
};

////////////////////////////////////////////////////////////////////////////////
// RTL Flight Plan
// 
// On entering RTL mode, turn off the engine, fly home, and circle indefinitely until touching down

const struct logoInstructionDef rtlInstructions[] = {


    EXEC (FS_LOITER_LAND)
    END
    END



	//TO (SET_ALT_ALT)
	TO (FS_SET_ALT_ALT)
		//target the current altitude, preventing motor or butterfly

		//this is used for setting up the starting point of a slope
		//note: SET_ALT(ALT) is wrong!!!
		LOAD_TO_PARAM(ALT)
		SET_ALT_PARAM
	END
	END



	//TO (DESCENT_PATTERN)
	TO (FS_DESCENT_PATTERN)

		REPEAT(2)
			//turn
			REPEAT(18)
				IF_GT(ALT,FINAL_ALT*3)
					ALT_DOWN(1)  //keep going down
					FLAG_ON(F_LAND) //brake if you have to
				ELSE
					FLAG_OFF(F_LAND)  // no brakes
				END
				RT(20)
				FD(DESIRED_SPEED_NORMAL_F0/5)
			END
		END
	END
	END



	//TO (LOITER_LAND)
	TO (FS_LOITER_LAND)
		//Fly to circuit entrypoint

		//Motor may be activated (slider up) if < 80m and if the charge level of the battery is sufficient
		//Glide if > 80m
		//When close to the entry point;
 		//If high, circle down to 80m
		//if low (<50m) go straight for an "emergency" final, this may be with tailwind (dont know if the charge level of the battery is sufficient)
		CLEAR_INTERRUPT
		FLAG_ON(F_CROSS_TRACK)
		SET_SPEED(DESIRED_SPEED_NORMAL_F0) //dm/s

		FD(DESIRED_SPEED_NORMAL_F0/10)
		SET_ALT((FINAL_ALT*3)+10)
		//Motor may be activated (slider up) if low and if the charge level of the battery is sufficient
		FLAG_OFF(F_LAND)  //Motor on if needed, no brakes
		//to entry point
		PEN_UP

			// goto circuit entry point / hoog aanknopings punt
			HOME

			// Get circuit hand and heading  - configure these headings for your flying field
			// -270 == Lefthand circuit, final heading West 270, 270 == Righthand circuit, final heading West 270
			//	  NE	E	|	SE		S   ||   SW	     W   |   NW		 N|
			//  23-67,68-112,113-157,158-202, 203-247,248-292,293-337,338-22
			//                                 wind FROM
			IF_LT(WIND_FROM_ANGLE,203)                    //  NE	  E		 SE		 S   (N)
				IF_LT(WIND_FROM_ANGLE,113)                //  NE	  E   (N)
					IF_LT(WIND_FROM_ANGLE,64)             //  N(E)
						IF_LT(WIND_FROM_ANGLE,22)         //  0..22
						   PARAM_SET(-326)  	//  N
						ELSE
						   PARAM_SET(64)    	//  NE
						END
					ELSE
						PARAM_SET(64)        	//  E
					END
				ELSE                            //  SE		 S
					IF_LT(WIND_FROM_ANGLE,158)
						PARAM_SET(148)       	//  SE
					ELSE
						PARAM_SET(148)       	//  S
					END
				END
			ELSE
				//>= 203                        //  SW	  W      NW		 N
				IF_LT(WIND_FROM_ANGLE,293)               	//  SW	  W
					IF_LT(WIND_FROM_ANGLE,248)
						PARAM_SET(-243)      	//  SW
					ELSE
						PARAM_SET(-243)      	//  W
					END
				ELSE                            //  NW		 N
					IF_LT(WIND_FROM_ANGLE,338)
						PARAM_SET(-326)      	//  NW
					ELSE
						PARAM_SET(-326)      	//  N
					END
				END
			END

			IF_GT(PARAM,0)  //param > 0
				//right hand circuit
				SET_ANGLE_PARAM   //virtually upwind of runway
				FD(150)
				RT(90)
			ELSE
				//left hand circuit
				PARAM_MUL(-1)
				SET_ANGLE_PARAM   //virtually upwind of runway
				PARAM_MUL(-1)
				FD(150)
				LT(90)
			END
			FD(150)
		PEN_DOWN    //to entry point

		//point to downwind
		IF_GE(PARAM,0)  //param => 0
			//right hand circuit
			SET_ANGLE_PARAM   //virtually upwind of runway
		ELSE
			//left hand circuit
			PARAM_MUL(-1)
			SET_ANGLE_PARAM   //virtually upwind of runway
			PARAM_MUL(-1)
		END
		//reverse heading, same for left and right
		RT(180)

		FD(DESIRED_SPEED_NORMAL_F0/3) //~3 sec  bring pattern closer to Home

		//set up an up-slope to entry point  in case we are low, else just glide there
		REPEAT_FOREVER
			//We have arrived above the circuit entry point
			//If high, fly circles down to 80m
			//if low (<50m) go straight for an "emergency" final  (dont know if the charge level of the battery is sufficient)

			IF_GT(ALT,(FINAL_ALT*3)+20)
				//DO (SET_ALT_ALT)
				DO (FS_SET_ALT_ALT)
				//DO (DESCENT_PATTERN)   //use a subroutine to preserve PARAM with circuit data
				DO (FS_DESCENT_PATTERN)   //use a subroutine to preserve PARAM with circuit data
			ELSE
				IF_GT(ALT,(FINAL_ALT)+10)  //else go straight for an "emergency" final
					//DO (SET_ALT_ALT)
					DO (FS_SET_ALT_ALT)

					// goto circuit entry point again / hoog aanknopings punt
					PEN_UP

						HOME

						IF_GE(PARAM,0)  //param => 0
							//right hand circuit
							SET_ANGLE_PARAM   //virtually upwind of runway
							FD(150)
							RT(90)
						ELSE
							//left hand circuit
							PARAM_MUL(-1)
							SET_ANGLE_PARAM   //virtually upwind of runway
							PARAM_MUL(-1)
							FD(150)
							LT(90)
						END
						FD(150)
					PEN_DOWN

					//Downwind
					//DO_PARAM (DOWNWIND)
					DO_PARAM (FS_DOWNWIND)

					//Base
					//DO_PARAM (BASE)
					DO_PARAM (FS_BASE)
				END // >50 m

				//Final
				//DO_PARAM (FINAL)
				DO_PARAM (FS_FINAL)

			END // 70m
 		END // repeat
	END
	END



	//TO (DOWNWIND)
	TO (FS_DOWNWIND)
		// assume 50m < alt < 80m

		SET_ALT((FINAL_ALT*2))
		FLAG_ON(F_LAND) //brake if you have to
		//turn to and fly the downwind leg
		PEN_UP
			HOME
			IF_GE(PARAM,0)  //param => 0
				//right hand circuit
				SET_ANGLE_PARAM   //virtually upwind of runway
				RT(90)
				FD(150)
				RT(90)
			ELSE
				//left hand circuit
				PARAM_MUL(-1)
				SET_ANGLE_PARAM   //virtually upwind of runway
				PARAM_MUL(-1)
				LT(90)
				FD(150)
				LT(90)
			END
			FD(150)
		PEN_DOWN

		FLAG_OFF(F_LAND) // no braking in turns
		IF_GE(PARAM,0)  //param => 0
			//right hand circuit
			REPEAT(5)
				RT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT*2)
					ALT_DOWN(1)
				END
			END
		ELSE
			//left hand circuit
			REPEAT(5)
				LT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT*2)
					ALT_DOWN(1)
				END
			END
		END
	END
	END



	//TO (BASE)
	TO (FS_BASE)
		//assume alt less than 50m

		FLAG_OFF(F_LAND) // no braking in turns

		//turn to and fly the base leg
		SET_SPEED(DESIRED_SPEED_NORMAL_F0) //dm/s
		//set up the startingpoint of the glideslope
		IF_GE(PARAM,0)  //param => 0
			//right hand circuit
			REPEAT(5)               //5 not 4: compensate offset flighttrack vs waypoint for precise landing
				RT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT*2-5)
				    ALT_DOWN(1)
				END
			END
		ELSE
			//left hand circuit
			REPEAT(5)               //5 not 4: compensate offset flighttrack vs waypoint for precise landing
				LT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT*2-5)
					ALT_DOWN(1)
				END
			END
		END
		SET_ALT(FINAL_ALT)
		PEN_UP     //base leg
			//set up the endpoint of the glideslope
			FD(60)
			FLAG_ON(F_LAND)    //use brakes / butterfly
		PEN_DOWN

		DO (SET_ALT_ALT)
		//DO (FS_SET_ALT_ALT)
		FLAG_OFF(F_LAND)    // no brakes in turn
		IF_GE(PARAM,0)  //param => 0
			//right hand circuit
			REPEAT(5)            
				RT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT-5)
				    ALT_DOWN(2)
				END
			END
		ELSE
			//left hand circuit
			REPEAT(5)
				LT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT-5)
				    ALT_DOWN(2)
				END    
			END
		END
	END
	END



	//TO (FINAL)
	TO (FS_FINAL)
		//turn to and fly the final

		//assume alt less than 50m  (target 35m)
		FLAG_OFF(F_LAND)    // no brakes in turn
		//set up the startingpoint of the glideslope
		IF_GE(PARAM,0)  //param => 0
			//right hand circuit
			REPEAT(4)
				RT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT)
				    ALT_DOWN(2)
				END    
			END
		ELSE
			//left hand circuit
			REPEAT(4)
				LT(10)
				FD(DESIRED_SPEED_NORMAL_F0/10)
				IF_GT(ALT,FINAL_ALT)
				    ALT_DOWN(2)
				END    
			END
		END


		//set up the endpoint of the glideslope   1:7
		FLAG_ON(F_LAND)     //use brakes / butterfly
		SET_ALT(FINAL_ALT*-1) //target altitude for the next waypoint below ground
		PEN_UP
			HOME
			USE_ANGLE_TO_GOAL
			//compensate offset in flighttrack vs waypoint for precise landing
			IF_GE(PARAM,0)  //param => 0
				//right hand circuit
				RT(10)    
			ELSE
				//left hand circuit
				LT(10)
			END

			FD(80)
		PEN_DOWN

		// go around if landing did not happen
		EXEC (LOGO_MAIN)

	END
	END

};
#else
#error select the correct flightplan and rtl flightplan
#endif // THERMALLING_MISSION == 1 

