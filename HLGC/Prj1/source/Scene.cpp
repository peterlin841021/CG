#include "../Include/Scene.h"
#include<vector>
#include<string>
#include<time.h>

#define HAIR 0 
#define FACE 1
#define BODY 2
#define SKIRT 3
#define LEFT_FORE_ARM 4
#define LEFT_HIND_ARM 5
#define LEFT_HAND 6
#define RIGHT_FORE_ARM 7
#define RIGHT_HIND_ARM 8
#define RIGHT_HAND 9
#define LEFT_THIGH 10
#define LEFT_CALF 11
#define RIGHT_THIGH 12
#define RIGHT_CALF 13
#define SCALLION 14
#define INVISIBLE_SCALLION 15
#define GROUND 16
#define MILKYWAY 17

using namespace glm;
using namespace std;
ViewManager* Scene::camera = new ViewManager();//Camera

std::vector<mat4> origin, action;

const vec3 hair(0, 0.f, 0);
const vec3 face(0, 2.5f/2, 0);
const vec3 body(0, 0, 0);
const vec3 skirt(0, -0.1f/2, 0);
const vec3 leftForeArm(0.5f/2, 1.8f/2, 0);
const vec3 leftHindArm(1.4f/2, -0.7f/2, 0);
const vec3 leftHand(2.8f/2, -1.3f/2, 0);
const vec3 rightForeArm(-0.5f/2, 1.8f/2, 0);
const vec3 rightHindArm(-1.4f/2, -0.7f/2, 0);
const vec3 rightHand(-2.8f/2, -1.3f/2, 0);
const vec3 scallion(0, 0, 0);
const vec3 leftThigh(0.5f/2, -0.3f/2, 0);
const vec3 leftCalf(-0.05f/2, -2.2f/2, 0);
const vec3 rightThigh(-0.5f/2, -0.3f/2, 0);
const vec3 rightCalf(0.05f/2, -2.2f/2, 0);
const vec3 milkyway(-10.f,2.f, 0);
const vec3 scale_ratio(0.025f, 0.025f, 0.025f);

int index = 0;//Motion present
int miku_effect = 0;
int miku_hair_effect = 0;
int ground_effect = 0;
int sky_effect = 0;
int scallion_effect = 0;
int flydir = 1;


bool actionfixed;
bool scallion_use = false;
bool invisible_scallion_use = false;
bool isMiku = true;
bool action_list[9] = { false };

float cheerangles[20] = {0.0};
float walkangles[40] = { 0.0 };
float flyangles[10] = {0};
float flyheight = 0.1f;
float clapangles[40] = {0.0};
float bowangles[16] = {0};
float takeoffangles[15] = { 0 };
float drawangles[16] = { 0 };
float excaliburangle[70] = {0};
float loitumaangle[300] = {0};
float all_rotate = 0.f;
float power = 0.f;
void Drawsword(float angle)
{
	mat4 identity(1.0);
	float armangle = 0.8f;

	action[RIGHT_FORE_ARM] = identity;
	action[RIGHT_HIND_ARM] = identity;
	action[RIGHT_HAND] = identity;
	action[SCALLION] = identity;
	
	action[RIGHT_FORE_ARM] *= translate(identity, vec3(rightForeArm.x, rightForeArm.y, rightForeArm.z));
	action[RIGHT_FORE_ARM] *= rotate(identity, armangle, vec3(0, 0, 1));
	action[RIGHT_FORE_ARM] *= rotate(identity, angle, vec3(1,-1, -1));
	action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
	action[RIGHT_HIND_ARM] *= rotate(identity, angle,vec3(0, 1, 0));
	action[RIGHT_HAND] *= translate(action[RIGHT_HIND_ARM], rightHand - rightHindArm);
	if (isMiku || scallion_use)
	{
		action[RIGHT_HAND] *= rotate(identity, -angle*3, vec3(1, 1, 0));		
		//action[SCALLION] *= rotate(identity, angle, vec3(0, 1, 0));
		action[SCALLION] *= translate(action[RIGHT_HAND], scallion);
	}
	
	for (size_t i = RIGHT_FORE_ARM; i <= RIGHT_HAND; i++)
	{
		action[i] *= scale(identity, scale_ratio);
	}
	if (isMiku || scallion_use)
		action[SCALLION] *= scale(identity, scale_ratio);
}
void Excalibur(float angle,bool prepare,bool slash)
{
	if (scallion_use && isMiku)
	{
		float armin = 0.8f;
		mat4 identity(1.0);
		action[HAIR] = identity;
		action[FACE] = identity;
		action[LEFT_FORE_ARM] = identity;
		action[LEFT_HIND_ARM] = identity;
		action[LEFT_HAND] = identity;

		action[LEFT_FORE_ARM] = identity;
		action[LEFT_HIND_ARM] = identity;
		action[LEFT_HAND] = identity;

		action[RIGHT_FORE_ARM] = identity;
		action[RIGHT_HIND_ARM] = identity;
		action[RIGHT_HAND] = identity;

		action[LEFT_THIGH] = identity;
		action[LEFT_CALF] = identity;
		action[RIGHT_THIGH] = identity;
		action[RIGHT_CALF] = identity;

		action[SCALLION] = identity;
		
		action[BODY] = identity;
		action[SKIRT] = identity;

		if (slash)
		{
			action[BODY] *= rotate(identity, angle*0.7f, vec3(1, 0, 0));
			invisible_scallion_use = true;
			action[INVISIBLE_SCALLION] = identity;			
			power += 0.2f;
		}
					
		action[FACE] *= translate(action[BODY], face);
		action[HAIR] *= translate(action[FACE], hair);
		action[SKIRT] *= translate(action[BODY], skirt);

		//Cross
		action[LEFT_THIGH]*= translate(action[BODY], leftThigh);
		action[RIGHT_THIGH] *= translate(action[BODY], rightThigh);
		

		action[LEFT_FORE_ARM] *= translate(action[BODY], leftForeArm);
		action[LEFT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, -1));
		//
		if(slash)
			action[LEFT_FORE_ARM] *= rotate(identity,angle*5.f, vec3(1, 1, 1));
		float a = 0.38f;
		if (prepare)
		{			
			action[LEFT_FORE_ARM] *= rotate(identity, -a, vec3(1, 0, 0));
			action[LEFT_FORE_ARM] *= rotate(identity, a * 3.f, vec3(0, -1, 0));
		}
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
		
		if (prepare && !slash)
		{			
			action[LEFT_HIND_ARM] *= rotate(identity, -angle * 2.f, vec3(0, 0, 1));			
		}			
		else if (slash)
		{
			action[LEFT_HIND_ARM] *= rotate(identity, -a * 2.f, vec3(0, 0, 1));
			if (!actionfixed)
				action[LEFT_THIGH] *= rotate(identity, angle/2.f, vec3(1, 1, 1));
			else
				action[LEFT_THIGH] *= rotate(identity, -0.28f / 2.f, vec3(1, 1, 1));
		}	
		action[LEFT_CALF] *= translate(action[LEFT_THIGH], leftCalf);
		action[LEFT_HAND] *= translate(action[LEFT_HIND_ARM], leftHand - leftHindArm);

		action[RIGHT_FORE_ARM] *= translate(action[BODY], rightForeArm);
		action[RIGHT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, 1));
		//
		if (slash)
		{
			action[RIGHT_FORE_ARM] *= rotate(identity, angle*5.f, vec3(1, -1, 1));
			if (angle < -0.28f)
				actionfixed = true;
			if(!actionfixed)
				action[RIGHT_THIGH] *= rotate(identity, -angle / 2.f, vec3(1, 1, 1));
			else
				action[RIGHT_THIGH] *= rotate(identity, 0.28f / 2.f, vec3(1, 1, 1));			
		}
		action[RIGHT_CALF] *= translate(action[RIGHT_THIGH], rightCalf);
		if (!prepare)
		{			
			action[RIGHT_FORE_ARM] *= rotate(identity, -angle, vec3(1, 0, 0));
			action[RIGHT_FORE_ARM] *= rotate(identity, -angle * 3.f, vec3(0, -1, 0));
			action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);			
			action[RIGHT_HAND] *= translate(action[RIGHT_HIND_ARM], rightHand - rightHindArm);
			action[RIGHT_HAND] *= rotate(identity, -angle * 0.5f, vec3(0, 1, 0));			
		}
		else
		{
			float a = 0.4f;
			
			action[RIGHT_FORE_ARM] *= rotate(identity, -a, vec3(1, 0, 0));
			action[RIGHT_FORE_ARM] *= rotate(identity, -a * 3.f, vec3(0, -1, 0));
			action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
						
			if(!slash)
				action[RIGHT_HIND_ARM] *= rotate(identity, angle*1.5f, vec3(0, 0, 1));
			else
				action[RIGHT_HIND_ARM] *= rotate(identity, a *1.5f, vec3(0, 0, 1));
			action[RIGHT_HAND] *= translate(action[RIGHT_HIND_ARM], rightHand - rightHindArm);

			action[RIGHT_HAND] *= rotate(identity, a * 0.5f, vec3(0, 1, 0));
		}		
		
		action[SCALLION] *= translate(action[RIGHT_HAND], scallion);						
		action[INVISIBLE_SCALLION] *= translate(action[SCALLION], vec3(0,0,0));
		action[INVISIBLE_SCALLION] *= rotate(identity,-5.f, vec3(1, 1, 1));
		action[INVISIBLE_SCALLION] *= scale(identity, vec3(scale_ratio.x*power * 2, scale_ratio.y*power * 2, scale_ratio.z*power*4.f));
		for (size_t i = HAIR; i <= RIGHT_CALF; i++)
		{
			action[i] *= scale(identity, scale_ratio);
		}		
		action[SCALLION] *= scale(identity, scale_ratio);
		
	}
}
void Stand() 
{
	mat4 identity(1.0);
	float armangle = 0.8f;

	action[BODY] *= translate(identity, body);
	action[FACE] *= translate(action[BODY], face);
	action[HAIR] *= translate(action[FACE], hair);	
	action[SKIRT] *= translate(action[BODY], skirt);	
	action[LEFT_FORE_ARM] *= translate(action[BODY], vec3(leftForeArm.x, leftForeArm.y,leftForeArm.z));
	action[LEFT_FORE_ARM] *= rotate(identity, armangle, vec3(0, 0, -1));
	action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);		
	action[LEFT_HAND] *= translate(action[LEFT_FORE_ARM], leftHand);					
	action[RIGHT_FORE_ARM] *= translate(action[BODY], vec3(rightForeArm.x, rightForeArm.y, rightForeArm.z));
	action[RIGHT_FORE_ARM] *= rotate(identity, armangle, vec3(0, 0, 1));	
	action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);		
	action[RIGHT_HAND] *= translate(action[RIGHT_FORE_ARM], rightHand);					
	action[LEFT_THIGH] *= translate(action[BODY], leftThigh);
	action[LEFT_CALF] *= translate(action[LEFT_THIGH], leftCalf);	
	action[RIGHT_THIGH] *= translate(action[BODY], rightThigh);
	action[RIGHT_CALF] *= translate(action[RIGHT_THIGH], rightCalf);	
	action[SCALLION] *= translate(action[RIGHT_HAND], scallion);
		
	for (size_t i = 0; i < GROUND; i++)
	{		
		action[i] *= scale(identity, scale_ratio);
	}	
	action[GROUND] *= translate(identity, vec3(0.f, -8.2f, -8.f));
	action[GROUND] *= scale(identity, vec3(0.2f, 0.2f, 0.2f));

	action[MILKYWAY] = translate(identity, milkyway);
	action[MILKYWAY] *= rotate(identity,180.f, vec3(0, 0, 1));
	action[MILKYWAY] *= scale(identity, vec3(2.5f, 2.5f, 2.5f));
	for (size_t i = 0; i < action.size(); i++)
	{
		origin.push_back(action[i]);
	}
}
void Cheer(float angle)
{
	mat4 identity(1.0);	
	action[LEFT_FORE_ARM] = identity;
	action[LEFT_HIND_ARM] = identity;
	action[LEFT_HAND] = identity;
	action[RIGHT_FORE_ARM] = identity;
	action[RIGHT_HIND_ARM] = identity;
	action[RIGHT_HAND] = identity;	
	if(isMiku)
		action[SCALLION] = identity;

	action[LEFT_FORE_ARM] *= translate(identity, leftForeArm);
	action[LEFT_FORE_ARM] *= rotate(identity, angle, vec3(-1, 0, 0));
	action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);	
	action[LEFT_HAND] *= translate(action[LEFT_FORE_ARM], leftHand);			

	action[RIGHT_FORE_ARM] *= translate(identity, rightForeArm);
	action[RIGHT_FORE_ARM] *= rotate(identity, angle, vec3(-1, 0, 0));
	action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);	
	action[RIGHT_HAND] *= translate(action[RIGHT_FORE_ARM], rightHand);	
	if (isMiku)
	{
		action[SCALLION] *= translate(action[RIGHT_HAND], scallion);
		action[SCALLION] *= scale(identity, scale_ratio);
	}	
	for (size_t i = LEFT_FORE_ARM; i <= RIGHT_HAND; i++)
	{
		action[i] *= scale(identity, scale_ratio);
	}	
}
void Walk(float angle)
{	
	mat4 identity(1.0);
	float armin = 0.8f;
	action[LEFT_FORE_ARM] = identity;
	action[LEFT_HIND_ARM] = identity;
	action[LEFT_HAND] = identity;
	action[RIGHT_FORE_ARM] = identity;
	action[RIGHT_HIND_ARM] = identity;
	action[RIGHT_HAND] = identity;
	action[LEFT_THIGH] = identity;
	action[LEFT_CALF] = identity;
	action[RIGHT_THIGH] = identity;
	action[RIGHT_CALF] = identity;
	action[SKIRT] = identity;
	if (scallion_use)
		action[SCALLION] = identity;

	//ARM
	action[LEFT_FORE_ARM] *= translate(identity, leftForeArm);
	action[LEFT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, -1));
	action[LEFT_FORE_ARM] *= rotate(identity, angle/2, vec3(-1, 0, 0));
	action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);	
	action[LEFT_HAND] *= translate(action[LEFT_FORE_ARM], leftHand);
			
	action[RIGHT_FORE_ARM] *= translate(identity, rightForeArm);
	action[RIGHT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, 1));
	action[RIGHT_FORE_ARM] *= rotate(identity, angle/2, vec3(1, 0, 0));
	action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);	
	action[RIGHT_HAND] *= translate(action[RIGHT_FORE_ARM], rightHand);		
	if (scallion_use) 
	{
		action[SCALLION] *= translate(action[RIGHT_HAND], scallion);
		action[SCALLION] *= scale(identity, scale_ratio);
	}
		
	//LEG
	float kneeAngle = 0;
	float legAngle = angle / 5.f;
	action[LEFT_THIGH] *= translate(identity, leftThigh);
	action[LEFT_THIGH] *= rotate(identity, legAngle, vec3(1, 0, 0));
	action[LEFT_CALF] *= translate(action[LEFT_THIGH], leftCalf);
	//if (angle <= 2.0  && angle >= 1.8f || angle <= -1.8 && angle >= -2.f)
	if ((angle <= 2.f && angle >=1.9f) || (angle > -2.f && angle <= -1.9f ))
		kneeAngle = 0;
	else
		kneeAngle = legAngle < 0 ? legAngle + 0.4f : legAngle + 0.4f;

	action[LEFT_CALF] *= rotate(identity, kneeAngle, vec3(1, 0, 0));

	action[RIGHT_THIGH] *= translate(identity, rightThigh);	
	action[RIGHT_THIGH] *= rotate(identity, -legAngle, vec3(1, 0, 0));
	action[RIGHT_CALF] *= translate(action[RIGHT_THIGH], rightCalf);	
	if ((angle <= 2.f && angle >= 1.9f) || (angle > -2.f && angle <= -1.9f))
		kneeAngle = 0;
	else
		kneeAngle = -legAngle < -2.f ? -legAngle - 0.4f : -legAngle + 0.4f;
	action[RIGHT_CALF] *= rotate(identity, kneeAngle, vec3(1, 0, 0));

	action[SKIRT] *= translate(identity, skirt);
	kneeAngle = ((angle / 6) < -angle / 6 ? angle / 6 : -(angle / 6));
	action[SKIRT] *= rotate(identity, kneeAngle,vec3(1, 0, 0));

	for (size_t i = SKIRT; i <= RIGHT_CALF; i++)
	{
		action[i] *= scale(identity, scale_ratio);
	}
}
void Fly(float angle) 
{
	mat4 identity(1.0);
	for (size_t i = 0; i <= SCALLION; i++)
	{
		action[i] = identity;
	}
	
	action[BODY] *= translate(identity, body);
	action[FACE] *= translate(action[BODY], face);
	action[HAIR] *= translate(action[FACE], hair);			
	

	action[LEFT_FORE_ARM] *= translate(identity, leftForeArm);
	action[LEFT_FORE_ARM] *= rotate(identity, angle*0.4f, vec3(0, 0, -1));
	action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
	action[LEFT_HAND] *= translate(action[LEFT_FORE_ARM], leftHand);

	action[RIGHT_FORE_ARM] *= translate(identity, rightForeArm);
	action[RIGHT_FORE_ARM] *= rotate(identity, angle*0.4f, vec3(0, 0, 1));
	action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
	action[RIGHT_HAND] *= translate(action[RIGHT_FORE_ARM], rightHand);
	if(isMiku)
		action[SCALLION] *= translate(action[RIGHT_HAND], scallion);

	//LEG
	float kneeAngle = 0;
	float leg = angle / 10.f;
	action[LEFT_THIGH] *= translate(identity, leftThigh);
	action[LEFT_THIGH] *= rotate(identity, leg, vec3(1, 0, 0));
	action[LEFT_CALF] *= translate(action[LEFT_THIGH], leftCalf);
	//action[LEFT_CALF] *= rotate(identity, kneeAngle, vec3(1, 0, 0));
	action[RIGHT_THIGH] *= translate(identity, rightThigh);
	action[RIGHT_THIGH] *= rotate(identity, -leg, vec3(1, 0, 0));
	action[RIGHT_CALF] *= translate(action[RIGHT_THIGH], rightCalf);
	//action[RIGHT_CALF] *= rotate(identity, kneeAngle, vec3(1, 0, 0));
	action[SKIRT] *= translate(identity, skirt);
	action[SKIRT] *= rotate(identity, leg*2, vec3(1, 0, 0));

	for (size_t i = 0; i <= SCALLION; i++)
	{		
		action[i] *= translate(identity, vec3(0, flyheight, 0));
	}
	flyheight += flydir * 0.01f;
	if (flyheight >= 0.5f)
		flydir = -1;
	else if(flyheight < 0)
		flydir = 1;
	for (size_t i = 0; i <= SCALLION; i++)
	{
		action[i] *= scale(identity, scale_ratio);
	}
}
void Clap(float angle,bool prepare)
{
	mat4 identity(1.0);	
	action[LEFT_FORE_ARM] = identity;
	action[LEFT_HIND_ARM] = identity;
	action[LEFT_HAND] = identity;
	action[RIGHT_FORE_ARM] = identity;
	action[RIGHT_HIND_ARM] = identity;
	action[RIGHT_HAND] = identity;
	action[LEFT_FORE_ARM] *= translate(identity, leftForeArm);
	
	if (prepare)
	{	
		action[LEFT_FORE_ARM] *= rotate(identity, 0.1f, vec3(1, 0, 0));
		action[LEFT_FORE_ARM] *= rotate(identity, angle, vec3(0, 1, 0));
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);				
		if (angle < -1.f)
		{
			action[LEFT_HAND] *= translate(action[LEFT_HIND_ARM], leftHand - leftHindArm);
			action[LEFT_HAND] *= rotate(identity, 5.f, vec3(1, 0, 0));
		}
		else
		{
			action[LEFT_HAND] *= translate(action[LEFT_FORE_ARM], leftHand);
		}
	}
	else
	{
		action[LEFT_FORE_ARM] *= rotate(identity, -angle, vec3(1, 0, 0));
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
		action[LEFT_HAND] *= translate(action[LEFT_FORE_ARM], leftHand);
	}
	
	action[RIGHT_FORE_ARM] *= translate(identity, rightForeArm);
	
	if (prepare)
	{	
		action[RIGHT_FORE_ARM] *= rotate(identity, 0.1f, vec3(1, 0, 0));
		action[RIGHT_FORE_ARM] *= rotate(identity, -angle, vec3(0, 1, 0));
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
		
		if (angle < -1.f)
		{	
			action[RIGHT_HAND] *= translate(action[RIGHT_HIND_ARM], rightHand - rightHindArm);
			action[RIGHT_HAND] *= rotate(identity, 5.f, vec3(1, 0, 0));
		}	
		else
		{
			action[RIGHT_HAND] *= translate(action[RIGHT_FORE_ARM], rightHand);
		}
	}
	else
	{
		action[RIGHT_FORE_ARM] *= rotate(identity, -angle, vec3(1, 0, 0));
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
		action[RIGHT_HAND] *= translate(action[RIGHT_FORE_ARM], rightHand);
	}
	action[SCALLION] = identity;
	action[SCALLION] *= translate(identity, vec3(-0.5f,-0.5f,-0.1f));
	action[SCALLION] *= rotate(identity,30.f,vec3(1,0,0));
	action[SCALLION] *= rotate(identity, 30.f, vec3(0, 0, 1));
	for (size_t i = LEFT_FORE_ARM; i <= RIGHT_HAND; i++)
	{
		action[i] *= scale(identity, scale_ratio);
	}
	action[SCALLION] *= scale(identity, scale_ratio);
}
void Bow(float angle, bool prepare)
{
	mat4 identity(1.0);
	float armin = 0.8f;
	action[HAIR] = identity;
	action[FACE] = identity;
	action[BODY] = identity;
	action[LEFT_FORE_ARM] = identity;
	action[LEFT_HIND_ARM] = identity;
	action[LEFT_HAND] = identity;
	action[RIGHT_FORE_ARM] = identity;
	action[RIGHT_HIND_ARM] = identity;
	action[RIGHT_HAND] = identity;
	action[SCALLION] = identity;

	action[BODY] *= translate(identity, body);	
	if (angle > 0.f)
	{
		action[BODY] *= rotate(identity, angle * 0.6f, vec3(1, 0, 0));		
	}
	action[FACE] *= translate(action[BODY], face);
	action[HAIR] *= translate(action[FACE], hair);

	action[LEFT_FORE_ARM] *= translate(action[BODY], leftForeArm);
	action[RIGHT_FORE_ARM] *= translate(action[BODY], rightForeArm);
	action[RIGHT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, 1));
	action[LEFT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, -1));
	if (prepare)
	{
		if (angle > 0.7f)
		{
			action[LEFT_FORE_ARM] *= rotate(identity, -0.2f, vec3(1, 0, 0));
			action[RIGHT_FORE_ARM] *= rotate(identity, -0.2f, vec3(1, 0, 0));
		}
		action[LEFT_FORE_ARM] *= rotate(identity, -0.5f, vec3(0, 1, 1));
		action[RIGHT_FORE_ARM] *= rotate(identity, 0.5f, vec3(0, 1, 1));		
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
		action[LEFT_HAND] *= translate(action[LEFT_FORE_ARM], leftHand);
		action[RIGHT_HAND] *= translate(action[RIGHT_FORE_ARM], rightHand);
		if (angle > 0.7f)
		{						
			action[LEFT_HAND] *= rotate(identity, angle, vec3(1, 0, 0));
			action[RIGHT_HAND] *= rotate(identity, angle, vec3(1, 0, 0));
			action[LEFT_HAND] *= rotate(identity, -0.5f, vec3(0, 0, 1));
			action[RIGHT_HAND] *= rotate(identity, 0.5f, vec3(0, 0, 1));
		}				
	}
	else
	{
		action[LEFT_FORE_ARM] *= rotate(identity, angle, vec3(0, 1, 1));
		action[RIGHT_FORE_ARM] *= rotate(identity, -angle, vec3(0, 1, 1));		
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
		action[LEFT_HAND] *= translate(action[LEFT_FORE_ARM], leftHand);
		action[RIGHT_HAND] *= translate(action[RIGHT_FORE_ARM], rightHand);
	}
	action[SCALLION] *= translate(action[RIGHT_HAND], scallion);
	for (size_t i = HAIR; i <= RIGHT_HAND; i++)
	{
		if (i == SKIRT)continue;
		action[i] *= scale(identity, scale_ratio);
	}
	action[SCALLION] *= scale(identity, scale_ratio);
}
void Lift(float angle, bool prepare)
{
	mat4 identity(1.0);
	float armin = 0.8f;
	action[HAIR] = identity;
	action[FACE] = identity;	
	action[BODY] = identity;
	action[SKIRT] = identity;
	action[LEFT_FORE_ARM] = identity;
	action[LEFT_HIND_ARM] = identity;
	action[LEFT_HAND] = identity;
	action[RIGHT_FORE_ARM] = identity;
	action[RIGHT_HIND_ARM] = identity;
	action[RIGHT_HAND] = identity;

	action[BODY] *= translate(identity, body);
	action[FACE] *= translate(action[BODY], face);
	action[HAIR] *= translate(action[FACE], hair);
	action[SKIRT] *= translate(action[BODY], skirt);
	action[LEFT_FORE_ARM] *= translate(action[BODY], leftForeArm);
	action[RIGHT_FORE_ARM] *= translate(action[BODY], rightForeArm);
	action[RIGHT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, 1));
	action[LEFT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, -1));	
	if (prepare)
	{
		float a = -0.2f;
		action[LEFT_FORE_ARM] *= rotate(identity, a + angle*1.5f, vec3(0, 0, 1));
		action[RIGHT_FORE_ARM] *= rotate(identity,-a - angle*1.5f, vec3(0, 0, 1));
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);

		action[LEFT_HIND_ARM] *= rotate(identity, -angle * 2.5f, vec3(1, 0,0));
		action[LEFT_HIND_ARM] *= rotate(identity, -angle * 3.f, vec3(0, 0, 1));

		action[RIGHT_HIND_ARM] *= rotate(identity, -angle* 2.5f, vec3(1, 0, 0));
		action[RIGHT_HIND_ARM] *= rotate(identity, angle * 3.f, vec3(0, 0, 1));

		action[LEFT_HAND] *= translate(action[LEFT_HIND_ARM], leftHand - leftHindArm);
		action[RIGHT_HAND] *= translate(action[RIGHT_HIND_ARM], rightHand - rightHindArm);
		action[LEFT_HAND] *= rotate(identity, 1.f, vec3(1, 0, 0));
		action[RIGHT_HAND] *= rotate(identity, 1.f, vec3(1, 0, 0));

		action[LEFT_HAND] *= rotate(identity, -1.05f, vec3(0, 0, 1));
		action[RIGHT_HAND] *= rotate(identity, 1.05f, vec3(0, 0, 1));

		action[LEFT_HAND] *= rotate(identity, 0.2f, vec3(0, 1, 0));
		action[RIGHT_HAND] *= rotate(identity,-0.2f, vec3(0, 1, 0));
		action[SKIRT] *= rotate(identity, -angle*1.7f, vec3(1, 0, 0));
	}
	else
	{
		action[LEFT_FORE_ARM] *= rotate(identity, angle*1.5f, vec3(0, 1, 1));
		action[RIGHT_FORE_ARM] *= rotate(identity, -angle * 1.5f, vec3(0, 1, 1));
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);

		action[LEFT_HIND_ARM] *= rotate(identity, angle, vec3(0, 1, 1));
		action[RIGHT_HIND_ARM] *= rotate(identity, -angle, vec3(0, 1, 1));
		action[LEFT_HAND] *= translate(action[LEFT_HIND_ARM], leftHand - leftHindArm);
		action[RIGHT_HAND] *= translate(action[RIGHT_HIND_ARM], rightHand - rightHindArm);
		//printf("%f\n", angle);
	}
	action[SCALLION] = identity;
	action[SCALLION] *= translate(identity, vec3(-0.5f, -0.5f, -0.1f));
	action[SCALLION] *= rotate(identity, 30.f, vec3(1, 0, 0));
	action[SCALLION] *= rotate(identity, 30.f, vec3(0, 0, 1));
	for (size_t i = HAIR; i <= RIGHT_HAND; i++)
	{
		/*if (i == SKIRT )continue;*/
		action[i] *= scale(identity, scale_ratio);
	}
	action[SCALLION] *= scale(identity, scale_ratio);
}
//預備動作
void act1(float angle, vector<mat4> act,bool fixed)
{
	mat4 identity(1.0);
	float armin = 0.8f;
	for (size_t i = 0; i < SCALLION; i++)
	{
		action[i] = identity;
	}
	float start_fix = 0.9f;
	action[BODY] *= translate(identity, body);
	if (fixed)
	{
		action[BODY] *= rotate(identity, -start_fix / 3.f, vec3(0, 1, 0));
		//action[BODY] *= rotate(identity, angle, vec3(0, 0, 1));
	}		
	else
		action[BODY] *= rotate(identity, -angle / 3.f, vec3(0, 1, 0));
	action[FACE] *= translate(action[BODY], face);
	if (fixed)
		action[FACE] *= rotate(identity, start_fix / 3.f + angle /4.f, vec3(0, 0, 1));
	else
		action[FACE] *= rotate(identity, angle / 3.f, vec3(0, 0, 1));
	//printf("angle:%f\n", angle);
	action[HAIR] *= translate(action[FACE], hair);
	action[SKIRT] *= translate(action[BODY], skirt);
	action[LEFT_FORE_ARM] *= translate(action[BODY], vec3(leftForeArm.x, leftForeArm.y, leftForeArm.z));
	action[LEFT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, -1));
	action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
	action[LEFT_HAND] *= translate(action[LEFT_FORE_ARM], leftHand);

	action[RIGHT_FORE_ARM] *= translate(action[BODY], vec3(rightForeArm.x, rightForeArm.y, rightForeArm.z));
	action[RIGHT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, 1));
	float arm = angle * 2.5f;
	if (fixed)
	{
		arm = start_fix * 2.5f;
		action[RIGHT_FORE_ARM] *= rotate(identity, -arm, vec3(1, 0, 0));
		action[RIGHT_FORE_ARM] *= rotate(identity, angle, vec3(0, 0, 1));
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
		action[RIGHT_HIND_ARM] *= rotate(identity, arm *1.2f, vec3(0, 1, 0));
		action[RIGHT_HAND] *= translate(action[RIGHT_HIND_ARM], rightHand - rightHindArm);		
	}
	else
	{
		action[RIGHT_FORE_ARM] *= rotate(identity, -arm, vec3(1, 0, 0));
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
		action[RIGHT_HIND_ARM] *= rotate(identity, arm *1.2f, vec3(0, 1, 0));
		action[RIGHT_HAND] *= translate(action[RIGHT_HIND_ARM], rightHand - rightHindArm);
		//action[SCALLION] *= translate(action[RIGHT_HAND], scallion);
	}
	if (fixed)
	{
		action[LEFT_THIGH] *= translate(action[BODY], leftThigh);
		action[LEFT_THIGH] *= rotate(identity, start_fix / 4.f + angle/4.f, vec3(0, 0, 1));
		action[LEFT_THIGH] *= rotate(identity, -start_fix, vec3(0, 1, 0));
		action[LEFT_CALF] *= translate(action[LEFT_THIGH], leftCalf);
		action[LEFT_CALF] *= rotate(identity, start_fix / 2.f, vec3(1, 0, 0));

		action[RIGHT_THIGH] *= translate(action[BODY], rightThigh);
		action[RIGHT_THIGH] *= rotate(identity, -start_fix, vec3(0, 1, 0));
		action[RIGHT_CALF] *= translate(action[RIGHT_THIGH], rightCalf);
	}
	else
	{
		action[LEFT_THIGH] *= translate(action[BODY], leftThigh);
		action[LEFT_THIGH] *= rotate(identity, angle / 4.f, vec3(0, 0, 1));
		action[LEFT_THIGH] *= rotate(identity, -angle, vec3(0, 1, 0));
		action[LEFT_CALF] *= translate(action[LEFT_THIGH], leftCalf);
		action[LEFT_CALF] *= rotate(identity, angle / 2.f, vec3(1, 0, 0));

		action[RIGHT_THIGH] *= translate(action[BODY], rightThigh);
		action[RIGHT_THIGH] *= rotate(identity, -angle, vec3(0, 1, 0));
		action[RIGHT_CALF] *= translate(action[RIGHT_THIGH], rightCalf);
	}
	action[SCALLION] = identity;
	action[SCALLION] *= translate(action[RIGHT_HAND], scallion);
	for (size_t i = 0; i <= SCALLION; i++)
	{
		action[i] *= scale(identity, scale_ratio);
	}
}
//雙交叉
void act2(float angle, vector<mat4> act, bool fixed)
{
	mat4 identity(1.0);
	float armin = 0.8f;
	for (size_t i = 0; i < SCALLION; i++)
	{
		action[i] = identity;
	}
	action[BODY] *= translate(identity, body);		
	action[FACE] *= translate(action[BODY], face);	
	action[HAIR] *= translate(action[FACE], hair);
	action[SKIRT] *= translate(action[BODY], skirt);
	//Cross X
	float arm = angle*3.f;
	action[LEFT_FORE_ARM] *= translate(action[BODY], leftForeArm);
	action[LEFT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, -1));

	action[LEFT_FORE_ARM] *= rotate(identity, arm, vec3(1,1, 0));	
	action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
	action[LEFT_HIND_ARM] *= rotate(identity, arm/ 2.f, vec3(0, 0, 1));
	action[LEFT_HAND] *= translate(action[LEFT_HIND_ARM], leftHand - leftHindArm);

	action[RIGHT_FORE_ARM] *= translate(action[BODY], rightForeArm);
	action[RIGHT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, 1));

	action[RIGHT_FORE_ARM] *= rotate(identity, arm, vec3(1,-1, 0));	
	action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
	action[RIGHT_HIND_ARM] *= rotate(identity, -arm/2.f, vec3(0, 0, 1));
	action[RIGHT_HAND] *= translate(action[RIGHT_HIND_ARM], rightHand - rightHindArm);

	action[LEFT_THIGH] *= translate(action[BODY], leftThigh);
	action[LEFT_THIGH] *= rotate(identity, -angle/5.f, vec3(0, 0, 1));
	action[LEFT_CALF] *= translate(action[LEFT_THIGH], leftCalf);
	action[RIGHT_THIGH] *= translate(action[BODY], rightThigh);
	action[RIGHT_THIGH] *= rotate(identity, angle / 5.f, vec3(0, 0, 1));
	action[RIGHT_CALF] *= translate(action[RIGHT_THIGH], rightCalf);
	action[SCALLION] = identity;
	action[SCALLION] *= translate(action[RIGHT_HAND], scallion);
	for (size_t i = 0; i <= SCALLION; i++)
	{
		action[i] *= scale(identity, scale_ratio);
	}
}
//右交叉
void act3(float angle, vector<mat4> act, bool fixed)
{
	mat4 identity(1.0);
	float armin = 0.8f;
	for (size_t i = 0; i < SCALLION; i++)
	{
		action[i] = identity;
	}
	action[BODY] *= translate(identity, body);
	action[BODY] *= rotate(identity, angle, vec3(0, 1,0));
	action[FACE] *= translate(action[BODY], face);
	action[HAIR] *= translate(action[FACE], hair);
	action[SKIRT] *= translate(action[BODY], skirt);
	action[LEFT_FORE_ARM] *= translate(action[BODY], leftForeArm);	
	action[LEFT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, -1));
	//
	
	//Cross
	if (fixed)
	{
		float a = 1.0f;
		action[LEFT_FORE_ARM] *= rotate(identity,a + angle/2.f, vec3(0, 0, 1));
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
		action[LEFT_HIND_ARM] *= rotate(identity, -a * 2.f, vec3(1, 1, 0));
		action[LEFT_HIND_ARM] *= rotate(identity, -a * 1.5f, vec3(0, 0, 1));
		action[LEFT_HAND] *= translate(action[LEFT_HIND_ARM], leftHand - leftHindArm);
	}
	else
	{
		action[LEFT_FORE_ARM] *= rotate(identity, -angle, vec3(1, 1, 0));
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
		action[LEFT_HIND_ARM] *= rotate(identity, -angle * 2.f, vec3(1, 1, 0));
		action[LEFT_HIND_ARM] *= rotate(identity, -angle * 1.5f, vec3(0, 0, 1));
		action[LEFT_HAND] *= translate(action[LEFT_HIND_ARM], leftHand - leftHindArm);
	}
	
	//printf("angle:%f\n", angle);
	action[RIGHT_FORE_ARM] *= translate(action[BODY], rightForeArm);
	action[RIGHT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, 1));
	//		
	//Cross
	if (fixed)
	{
		float a = 1.0;
		action[RIGHT_FORE_ARM] *= rotate(identity, a - angle/2.f, vec3(0, 0, -1));
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
		action[RIGHT_HIND_ARM] *= rotate(identity, -a * 2.f, vec3(1, -1, 0));
		action[RIGHT_HIND_ARM] *= rotate(identity, -a * 1.5f, vec3(0, 0, -1));
		action[RIGHT_HAND] *= translate(action[RIGHT_HIND_ARM], rightHand - rightHindArm);
	}
	else
	{
		action[RIGHT_FORE_ARM] *= rotate(identity, -angle, vec3(1, -1, 0));
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
		action[RIGHT_HIND_ARM] *= rotate(identity, -angle * 2.f, vec3(1, -1, 0));
		action[RIGHT_HIND_ARM] *= rotate(identity, -angle * 1.5f, vec3(0, 0, -1));
		action[RIGHT_HAND] *= translate(action[RIGHT_HIND_ARM], rightHand - rightHindArm);
	}
	action[SCALLION] = identity;
	action[SCALLION] *= translate(action[RIGHT_HAND], scallion);
	//Kick
	action[LEFT_THIGH] *= translate(action[BODY], leftThigh);
	action[LEFT_CALF] *= translate(action[LEFT_THIGH], leftCalf);
	action[LEFT_CALF] *= rotate(identity, angle/2.f, vec3(1, 0, 0));
	action[RIGHT_THIGH] *= translate(action[BODY], rightThigh);
	action[RIGHT_THIGH] *= rotate(identity, -angle / 2.f, vec3(1, 0, 0));
	action[RIGHT_CALF] *= translate(action[RIGHT_THIGH], rightCalf);	
	for (size_t i = 0; i <= SCALLION; i++)
	{
		action[i] *= scale(identity, scale_ratio);
	}
}
//左交叉
void act4(float angle, vector<mat4> act, bool fixed)
{
	mat4 identity(1.0);
	float armin = 0.8f;
	for (size_t i = 0; i < SCALLION; i++)
	{
		action[i] = identity;
	}
	action[BODY] *= translate(identity, body);
	action[BODY] *= rotate(identity, -angle, vec3(0, 1, 0));
	action[FACE] *= translate(action[BODY], face);
	action[HAIR] *= translate(action[FACE], hair);
	action[SKIRT] *= translate(action[BODY], skirt);
	action[LEFT_FORE_ARM] *= translate(action[BODY], leftForeArm);
	action[LEFT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, -1));
	
	//Cross
	if (fixed)
	{
		float a = 1.0f;
		action[LEFT_FORE_ARM] *= rotate(identity, a + angle / 2.f, vec3(0, 0, 1));
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
		action[LEFT_HIND_ARM] *= rotate(identity, -a * 2.f, vec3(1, 1, 0));
		action[LEFT_HIND_ARM] *= rotate(identity, -a * 1.5f, vec3(0, 0, 1));
		action[LEFT_HAND] *= translate(action[LEFT_HIND_ARM], leftHand - leftHindArm);
	}
	else
	{
		action[LEFT_FORE_ARM] *= rotate(identity, -angle, vec3(1, 1, 0));
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
		action[LEFT_HIND_ARM] *= rotate(identity, -angle * 2.f, vec3(1, 1, 0));
		action[LEFT_HIND_ARM] *= rotate(identity, -angle * 1.5f, vec3(0, 0, 1));
		action[LEFT_HAND] *= translate(action[LEFT_HIND_ARM], leftHand - leftHindArm);
	}
	//printf("angle:%f\n", angle);
	action[RIGHT_FORE_ARM] *= translate(action[BODY], rightForeArm);
	action[RIGHT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, 1));
	
	//Cross
	if (fixed)
	{
		float a = 1.0;
		action[RIGHT_FORE_ARM] *= rotate(identity, a - angle / 2.f, vec3(0, 0, -1));
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
		action[RIGHT_HIND_ARM] *= rotate(identity, -a * 2.f, vec3(1, -1, 0));
		action[RIGHT_HIND_ARM] *= rotate(identity, -a * 1.5f, vec3(0, 0, -1));
		action[RIGHT_HAND] *= translate(action[RIGHT_HIND_ARM], rightHand - rightHindArm);
	}
	else
	{
		action[RIGHT_FORE_ARM] *= rotate(identity, -angle, vec3(1, -1, 0));
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
		action[RIGHT_HIND_ARM] *= rotate(identity, -angle * 2.f, vec3(1, -1, 0));
		action[RIGHT_HIND_ARM] *= rotate(identity, -angle * 1.5f, vec3(0, 0, -1));
		action[RIGHT_HAND] *= translate(action[RIGHT_HIND_ARM], rightHand - rightHindArm);
	}
	action[SCALLION] = identity;
	action[SCALLION] *= translate(action[RIGHT_HAND], scallion);
	//Kick
	action[LEFT_THIGH] *= translate(action[BODY], leftThigh);
	action[LEFT_THIGH] *= rotate(identity, -angle / 2.f, vec3(1, 0, 0));
	action[LEFT_CALF] *= translate(action[LEFT_THIGH], leftCalf);	
	action[RIGHT_THIGH] *= translate(action[BODY], rightThigh);	
	action[RIGHT_CALF] *= translate(action[RIGHT_THIGH], rightCalf);
	action[RIGHT_CALF] *= rotate(identity, angle / 2.f, vec3(1, 0, 0));	
	for (size_t i = 0; i <= SCALLION; i++)
	{
		action[i] *= scale(identity, scale_ratio);
	}
}
//手上下轉
void act5(float angle, vector<mat4> act, bool fixed)
{
	mat4 identity(1.0);
	float armin = 0.8f;
	for (size_t i = 0; i < SCALLION; i++)
	{
		action[i] = identity;
	}
	action[BODY] *= translate(identity, body);	
	action[FACE] *= translate(action[BODY], face);
	action[HAIR] *= translate(action[FACE], hair);
	action[SKIRT] *= translate(action[BODY], skirt);
	action[LEFT_FORE_ARM] *= translate(action[BODY], leftForeArm);
	
	if (fixed)
	{
		float a = 0.9f;
		action[LEFT_FORE_ARM] *= rotate(identity, a - angle / 8.f, vec3(0, 0, -1));
		action[LEFT_FORE_ARM] *= rotate(identity, -a, vec3(1, 1, 0));
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
		action[LEFT_HIND_ARM] *= rotate(identity, -a * 2.f, vec3(1, 1, 0));
		//
		action[LEFT_HIND_ARM] *= rotate(identity, -angle*1.3f, vec3(0, 0, 1));
	}
	else
	{
		action[LEFT_FORE_ARM] *= rotate(identity, armin - angle/8.f, vec3(0, 0, -1));
		action[LEFT_FORE_ARM] *= rotate(identity, -angle, vec3(1, 1, 0));
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
		action[LEFT_HIND_ARM] *= rotate(identity, -angle * 2.f, vec3(1, 1, 0));
	}		
	action[LEFT_HAND] *= translate(action[LEFT_HIND_ARM], leftHand - leftHindArm);	
	action[RIGHT_FORE_ARM] *= translate(action[BODY], rightForeArm);	
	if (fixed)
	{
		float a = 0.9f;
		action[RIGHT_FORE_ARM] *= rotate(identity, a + angle / 8.f, vec3(0, 0, 1));
		action[RIGHT_FORE_ARM] *= rotate(identity, -a, vec3(1, -1, 0));
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
		action[RIGHT_HIND_ARM] *= rotate(identity, -a * 2.f, vec3(1, -1, 0));
		//
		action[RIGHT_HIND_ARM] *= rotate(identity, -angle, vec3(0, 0, 1));
	}
	else
	{
		action[RIGHT_FORE_ARM] *= rotate(identity, armin + angle / 8.f, vec3(0, 0, 1));
		action[RIGHT_FORE_ARM] *= rotate(identity, -angle, vec3(1, -1, 0));
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
		action[RIGHT_HIND_ARM] *= rotate(identity, -angle * 2.f, vec3(1, -1, 0));
	}		
	action[RIGHT_HAND] *= translate(action[RIGHT_HIND_ARM], rightHand - rightHindArm);
	
	action[LEFT_THIGH] *= translate(action[BODY], leftThigh);	
	action[LEFT_CALF] *= translate(action[LEFT_THIGH], leftCalf);
	action[RIGHT_THIGH] *= translate(action[BODY], rightThigh);
	action[RIGHT_CALF] *= translate(action[RIGHT_THIGH], rightCalf);	
	action[SCALLION] = identity;
	action[SCALLION] *= translate(action[RIGHT_HAND], scallion);
	for (size_t i = 0; i <= SCALLION; i++)
	{
		action[i] *= scale(identity, scale_ratio);
	}
}
//手左右繞
void act6(float angle, vector<mat4> act, bool fixed)
{
	mat4 identity(1.0);
	float armin = 0.8f;
	for (size_t i = 0; i < SCALLION; i++)
	{
		action[i] = identity;
	}

	action[BODY] *= translate(identity, body);
	action[FACE] *= translate(action[BODY], face);
	action[HAIR] *= translate(action[FACE], hair);
	action[SKIRT] *= translate(action[BODY], skirt);
	action[LEFT_FORE_ARM] *= translate(action[BODY], leftForeArm);
	float armraise = -0.8f;
	action[LEFT_FORE_ARM] *= rotate(identity, armraise, vec3(1, 0, 0));
	
	if (fixed)
	{
		action[LEFT_FORE_ARM] *= rotate(identity, armin + angle / 1.5f, vec3(0, 0, -1));
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
		action[LEFT_HIND_ARM] *= rotate(identity, -angle, vec3(0, 0, 1));
	}
	else
	{
		action[LEFT_FORE_ARM] *= rotate(identity, armin - angle, vec3(0, 0, -1));		
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
		action[LEFT_HIND_ARM] *= rotate(identity, angle, vec3(0, 0, 1));
	}
	action[LEFT_HAND] *= translate(action[LEFT_HIND_ARM], leftHand - leftHindArm);
	action[RIGHT_FORE_ARM] *= translate(action[BODY], rightForeArm);

	action[RIGHT_FORE_ARM] *= rotate(identity, armraise, vec3(1, -1, 0));
	if (fixed)
	{
		action[RIGHT_FORE_ARM] *= rotate(identity, armin - angle/1.5f, vec3(0, 0, 1));		
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
		action[RIGHT_HIND_ARM] *= rotate(identity, -angle, vec3(0, 0, 1));
	}
	else
	{
		action[RIGHT_FORE_ARM] *= rotate(identity, armin + angle, vec3(0, 0, 1));		
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
		action[RIGHT_HIND_ARM] *= rotate(identity, angle, vec3(0, 0, 1));
	}
	action[RIGHT_HAND] *= translate(action[RIGHT_HIND_ARM], rightHand - rightHindArm);

	action[LEFT_THIGH] *= translate(action[BODY], leftThigh);	
	if (fixed)
		action[LEFT_THIGH] *= rotate(identity, angle / 2.5f, vec3(0, 0, 1));
	action[LEFT_CALF] *= translate(action[LEFT_THIGH], leftCalf);
	action[RIGHT_THIGH] *= translate(action[BODY], rightThigh);
	if(!fixed)
		action[RIGHT_THIGH] *= rotate(identity, -angle / 2.5f, vec3(0, 0, 1));
	action[RIGHT_CALF] *= translate(action[RIGHT_THIGH], rightCalf);

	action[SCALLION] = identity;
	action[SCALLION] *= translate(action[RIGHT_HAND], scallion);
	for (size_t i = 0; i <= SCALLION; i++)
	{
		action[i] *= scale(identity, scale_ratio);
	}
}
//手上下擺
void act7(float angle, vector<mat4> act, bool fixed,float body_rotate)
{
	mat4 identity(1.0);
	float arm = -0.7f;
	for (size_t i = 0; i < SCALLION; i++)
	{
		action[i] = identity;
	}

	action[BODY] *= translate(identity, body);
	action[BODY] *= translate(identity, vec3(0,-angle/8.f,0));
	action[BODY] *= rotate(identity, body_rotate, vec3(0, 1, 0));
	action[FACE] *= translate(action[BODY], face);
	action[HAIR] *= translate(action[FACE], hair);
	action[SKIRT] *= translate(action[BODY], skirt);
	action[LEFT_FORE_ARM] *= translate(action[BODY], leftForeArm);	
	
	if (fixed)
	{
		action[LEFT_FORE_ARM] *= rotate(identity, angle * 2.f, vec3(1, 0, 0));
		action[LEFT_FORE_ARM] *= rotate(identity, arm, vec3(0, 0, -1));
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
		action[LEFT_HIND_ARM] *= rotate(identity, arm*1.5f, vec3(0, 0, -1));
	}
	else
	{
		action[LEFT_FORE_ARM] *= rotate(identity, arm, vec3(0, 0, -1));		
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);
		action[LEFT_HIND_ARM] *= rotate(identity, arm*1.5f, vec3(0, 0, -1));
	}
	action[LEFT_HAND] *= translate(action[LEFT_HIND_ARM], leftHand - leftHindArm);
	action[RIGHT_FORE_ARM] *= translate(action[BODY], rightForeArm);
	
	if (fixed)
	{
		action[RIGHT_FORE_ARM] *= rotate(identity, arm, vec3(0, 0, 1));
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
		action[RIGHT_HIND_ARM] *= rotate(identity, arm * 1.5f, vec3(0, 0, 1));
	}
	else
	{
		action[RIGHT_FORE_ARM] *= rotate(identity, angle * 2.f, vec3(1, 0, 0));
		action[RIGHT_FORE_ARM] *= rotate(identity, arm, vec3(0, 0, 1));		
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
		action[RIGHT_HIND_ARM] *= rotate(identity, arm * 1.5f, vec3(0, 0, 1));
	}
	action[RIGHT_HAND] *= translate(action[RIGHT_HIND_ARM], rightHand - rightHindArm);

	action[SKIRT] *= rotate(identity, -angle / 4.f, vec3(1, 0, 0));
	action[LEFT_THIGH] *= translate(action[BODY], leftThigh);
	action[LEFT_THIGH] *= rotate(identity, -angle /4.f, vec3(1, 0, 0));
	action[LEFT_CALF] *= translate(action[LEFT_THIGH], leftCalf);
	action[LEFT_CALF] *= rotate(identity, angle / 2.f, vec3(1, 0, 0));

	action[RIGHT_THIGH] *= translate(action[BODY], rightThigh);
	action[RIGHT_THIGH] *= rotate(identity, -angle / 4.f, vec3(1, 0, 0));
	action[RIGHT_CALF] *= translate(action[RIGHT_THIGH], rightCalf);
	action[RIGHT_CALF] *= rotate(identity, angle / 2.f, vec3(1, 0, 0));
	
	action[SCALLION] = identity;
	action[SCALLION] *= translate(action[RIGHT_HAND], scallion);
	for (size_t i = 0; i <= SCALLION; i++)
	{
		action[i] *= scale(identity, scale_ratio);
	}
}
//轉圈圈
void act8(float angle, vector<mat4> act, bool fixed)
{
	mat4 identity(1.0);
	if (index < 290 && index > 230)
		all_rotate -= 0.107f;	
	if (index < 270 && index > 230)
		act7(loitumaangle[(int)loitumaangle[index]], action, false,all_rotate);
	else if (index < 290 && index >= 270)
		act7(loitumaangle[(int)loitumaangle[index]], action, true, all_rotate);	
	else if (index < 299 && index >= 290)
	{
		float a = 1.0f;
		float armin = 0.8f;
		for (size_t i = 0; i < SCALLION; i++)
		{
			action[i] = identity;
		}
		action[BODY] *= translate(identity, body);		
		action[FACE] *= translate(action[BODY], face);
		action[HAIR] *= translate(action[FACE], hair);
		action[SKIRT] *= translate(action[BODY], skirt);

		action[LEFT_FORE_ARM] *= translate(action[BODY], leftForeArm);
		action[LEFT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, -1));
		action[LEFT_FORE_ARM] *= rotate(identity, angle/1.5f, vec3(0, 0, 1));
		action[LEFT_HIND_ARM] *= translate(action[LEFT_FORE_ARM], leftHindArm);		
		action[LEFT_HIND_ARM] *= rotate(identity, -angle * 1.5f, vec3(0, 0, 1));
		action[LEFT_HAND] *= translate(action[LEFT_HIND_ARM], leftHand - leftHindArm);

		action[RIGHT_FORE_ARM] *= translate(action[BODY], rightForeArm);
		action[RIGHT_FORE_ARM] *= rotate(identity, armin, vec3(0, 0, 1));
		action[RIGHT_FORE_ARM] *= rotate(identity, -angle / 1.5f, vec3(0, 0, 1));
		action[RIGHT_HIND_ARM] *= translate(action[RIGHT_FORE_ARM], rightHindArm);
		action[RIGHT_HIND_ARM] *= rotate(identity, angle *1.5f, vec3(0, 0, 1));
		action[RIGHT_HAND] *= translate(action[RIGHT_HIND_ARM], rightHand - rightHindArm);
		
		action[SCALLION] = identity;
		action[SCALLION] *= translate(action[RIGHT_HAND], scallion);

		action[LEFT_THIGH] *= translate(action[BODY], leftThigh);
		action[LEFT_CALF] *= translate(action[LEFT_THIGH], leftCalf);
		action[RIGHT_THIGH] *= translate(action[BODY], rightThigh);		
		action[RIGHT_CALF] *= translate(action[RIGHT_THIGH], rightCalf);

		for (size_t i = 0; i <= SCALLION; i++)
		{
			action[i] *= scale(identity, scale_ratio);
		}
	}	
}

void Loituma(float angle,int index)
{	
	if (isMiku)
	{
		if (index < 10 && index > -1)
			act1(angle, action, false);
		else if (index < 40 && index > 10)
			act1(angle, action, true);
		else if (index < 50 && index > 40)
			act2(angle, action, false);
		else if (index < 60 && index > 50)
			act3(angle, action, false);
		else if (index < 70 && index > 60)
			act3(angle, action, true);
		else if (index < 80 && index > 70)
			act4(angle, action, false);
		else if (index < 90 && index > 80)
			act4(angle, action, true);
		else if (index < 100 && index > 90)
			act5(angle, action, false);
		else if (index < 120 && index > 100)
			act5(angle, action,true);
		else if (index < 150 && index > 120)
			act5(angle, action, true);
		else if (index < 170 && index > 150)
			act6(angle, action, false);
		else if (index < 190 && index > 170)
			act6(angle, action, true);
		else if (index < 210 && index > 190)
			act7(angle, action, false,0);
		else if (index < 230 && index > 210)
			act7(angle, action, true,0);
		else if (index < 300 && index > 230)
			act8(angle, action, false);
	}	
}
char *stringToChar(string str)
{
	char *c = new char[str.length() + 1];
	return strcpy(c, str.c_str());
}
void Scene::initMiku()
{
	string parts_objs[] =
	{
		"mikuhair","mikuface","mikubody","mikuskirt",
		"mikuleftforearm","mikulefthindarm","mikulefthand","mikurightforearm","mikurighthindarm","mikurighthand",
		"mikuleftthigh","mikuleftcalf","mikurightthigh","mikurightcalf"
	};
	for (size_t i = 0; i < sizeof(parts_objs) / sizeof(string); i++)
	{
		parts_objs[i].append(".obj");
	}
	string parts_textures[] =
	{
		"mikuhair","mikuface","mikubody","scallion"
	};
	for (size_t i = 0; i < sizeof(parts_textures) / sizeof(string); i++)
	{
		parts_textures[i].append(".png");		
	}
	for (size_t i = 0; i < sizeof(parts_objs) / sizeof(string); i++)
	{
		if (i == 0 || i == 1)
		{
			models.push_back(new BaseModel(stringToChar(parts_objs[i]), stringToChar(parts_textures[i])));
		}
		else
		{
			models.push_back(new BaseModel(stringToChar(parts_objs[i]), stringToChar(parts_textures[2])));
		}		
	}
	models.push_back(new BaseModel(stringToChar("scallion.obj"), stringToChar(parts_textures[3])));	
	//Invisible
	models.push_back(new BaseModel(stringToChar("Invisiblescallion.obj"), stringToChar(parts_textures[3])));
	mat4 identity(1.0);
	for (size_t i = 0; i < models.size(); i++)
	{
		action.push_back(identity);
	}
	action.push_back(identity);
}
void Scene::initOthers()
{
	string parts_objs[] =
	{
		"none","head","body","none",
		"leftarm","none","none","rightarm","none","none",
		"leftleg","none","rightleg","none"
	};

	for (size_t i = 0; i < sizeof(parts_objs) / sizeof(string); i++)
	{
		parts_objs[i].append(".obj");
	}
	string  parts_textures[] =
	{
		"robot"
	};

	for (size_t i = 0; i < sizeof(parts_textures) / sizeof(string); i++)
	{
		parts_textures[i].append(".png");
	}

	for (size_t i = 0; i < sizeof(parts_objs) / sizeof(string); i++)
	{
		models.push_back(new BaseModel(stringToChar(parts_objs[i]), stringToChar(parts_textures[0])));
	}
	//Transformation matrix
	mat4 identity(1.0);
	for (size_t i = 0; i < models.size(); i++)
	{
		action.push_back(identity);
	}
}
void Scene::initScenery()
{	
	models.push_back(new BaseModel("SFMC_main.obj","SFMC_main.png"));
	models.push_back(new BaseModel("milkyway.obj", "milkyway.png"));
}
Scene::Scene()
{
	//Initialize	
	camera = new ViewManager();
	if (isMiku)
	{
		initMiku();
	}
	else
	{
		initOthers();
	}
	initScenery();
	Stand();	
}
void Scene::MouseEvent(int button, int state, int x, int y)
{
	camera->mouseEvents(button, state, x, y);
}

void Scene::KeyBoardEvent(int key)
{

}

void Scene::KeyBoardEvent(unsigned char key)
{
	camera->keyEvents(key);
	float cheer_v = 0.3f;
	float walk_v = 0.2f;
	float fly_v = 0.2f;
	float clap_v = 0.1f;
	float bow_v = 0.1f;
	float take_v = 0.1f;
	float start = 0.f;
	mat4 identity(1.0);//Flying
	switch (key)
	{
	case 'z':
	case 'Z':		
		break;
	case 'x':
	case 'X':
		//models[1]->Rotate(glm::vec3(0, 1, 0), 0.1f);
		break;
	case 'c':
	case 'C':	
		//models[1]->Translate(glm::vec3(-0.1, 0, 0));
		break;
	case 'v':
	case 'V':
		//models[1]->Translate(glm::vec3(0.1, 0, 0));
		break;
	case 'r':
	case 'R':		
		//mv = matrix_stack.top();
		break;	
	case 'w':
	case 'W':		
		break;
	case 'f':
	case 'F':		
		break;
	case 'b':
	case 'B':		
		break;
	case 't':
	case 'T':	
		break;
	case 32:
		//action = origin;
		for (size_t i = 0; i < 9; i++)
		{
			action_list[i] = false;
		}
		scallion_use = false;
		break;
	default:
		break;
	}
}
void Scene::MenuEvent(int item)
{	
	float start = 0.f;
	float v = 0.1f;
	int act7_index = 190;
	mat4 identity(1.0);
	float signal = 1;
	action = origin;
	int effect = -1;	
	//int menu_offset[8] = { 0,10,29,48,67,86,105,111 };
	int menu_offset[8] = { 0,10,30,50,70,90,110,117 };
	//Action manager
	for (size_t i = 0; i < 9; i++)
	{
		action_list[i] = false;
	}	
	switch (item)
	{	
		case 1:
			index = 0;
			for (size_t i = 0; i < 20; i++)
			{
				cheerangles[i] = start;
				if (i == 10)
					signal = -1;
				start += v * 3 * signal;
			}		
			action_list[0] = !action_list[0];
			break;
		case 2:
			index = 0;
			for (size_t i = 0; i < 40; i++)
			{
				walkangles[i] = start;
				if (i == 10)
					signal = -1;
				if(i == 30)
					signal = 1;
				start += v * 2 * signal;
			}		
			action_list[1] = !action_list[1];
			break;
		case 3:
			flyheight = 0.f;
			action = origin;
			index = 0;
			start = 0.5f;		
			for (size_t i = 0; i < 10; i++)
			{
				flyangles[i] = start;
				if (i == 5)
					signal = -1;
				start -= v * 2 * signal;
			}		
			action_list[2] = !action_list[2];
			break;
		case 4:
			index = 0;
			for (size_t i = 0; i < 40; i++)
			{
				clapangles[i] = start;
				if (i == 8 || i == 28)
				{
					if (i == 8)
						start = 0;
					signal = -1;
				}				
				if(i == 24 || i == 32)
					signal = 1;
				if (i == 36)
					signal = -1.3f;
				start += v * signal;
			}
			action_list[3] = !action_list[3];
			break;
		case 5:
			index = 0;
			for (size_t i = 0; i < 16; i++)
			{
				bowangles[i] = start;
				if (i == 6)
				{
					start = 0;
					signal = -1;
				}			
				start -= v * signal;
			}			
			action_list[4] = !action_list[4];
			break;
		case 6:
			index = 0;
			signal = 0.5f;
			for (size_t i = 0; i < 15; i++)
			{
				takeoffangles[i] = start;
				if (i == 4)
				{
					start = 0;
					signal = -0.4f;
				}
				start -= v * signal;			
			}				
			action_list[5] = !action_list[5];
			break;
		case 7:
			index = 0;
			for (size_t i = 0; i < 16; i++)
			{
				drawangles[i] = start;
				if (i == 10)
					signal = -2.f;
				start -= v * signal;
			}
			action_list[6] = !action_list[6];
			break;	
		case 8:
			if (scallion_use)
			{
				invisible_scallion_use = false;			
				index = 0;
				power = 0;
				for (size_t i = 0; i < 70; i++)
				{
					excaliburangle[i] = start;
					if (i == 5)
					{
						signal = -1;
					}
					if (i == 10 || i == 15)
					{
						if (i == 15)//Hold
							start = 0;
						signal = 1;
					}
					if (i == 20)
					{
						start = 0;
						signal = -0.25f;
					}					
					if (i == 30)
						signal = 0.f;
					if (i == 50)
						signal = 0.21f;
					start += v * signal;
				}					
				action_list[7] = !action_list[7];
			}		
			break;
		case 9:
			index = 0;
			for (size_t i = 0; i < 300; i++)
			{
				loitumaangle[i] = start;
				if (i == 10 || i == 18 || i == 26 || i == 34)
				{
					if(i == 10)
						start = 0;
					signal = 0.5f;
				}
				if (i == 14 || i == 22 || i == 30)
				{				
					signal = -0.5f;
				}
				if (i == 50 || i == 70 || i == 90 || i == 100 || i == 110 || i == 130 || i == 140 || i == 150 || i == 170 || i == 290)
				{
					if (i == 50 || i == 70 || i == 90 || i == 100 || i == 150 || i == 170 || i == 290)
						start = 0;
					signal = 1;
				}
				if (i == 190 || i == 210)
				{
					start = 0;
					signal = 2;
				}
				if (i == 200 || i == 220)
				{
					//start = 0;
					signal = -2;
				}
				if (i == 40 || i == 60 || i == 80 || i == 105 || i == 115 || i == 135 || i == 145 || i == 160 || i == 180)
				{
					if(i == 40)
						start = 0;
					signal = -1;
				}
				if (i == 120)
				{				
					signal = 3;
				}
				if (i > 229 && i < 290)
				{
					start = act7_index;
					act7_index++;
					if (act7_index > 229)
						act7_index = 0;
				}
				else
				{
					start += signal * v;
				}			
			}
			action_list[8] = !action_list[8];
			break;
	//Reset
	case 10:
		action = origin;
		for (size_t i = 0; i < 9; i++)
		{
			action_list[i] = false;
		}	
		break;
	}
	if (item >= menu_offset[1] && item < menu_offset[2])//Global
	{
		miku_effect = item - menu_offset[1];
		scallion_effect = miku_effect;
		ground_effect= miku_effect;
		sky_effect = miku_effect;
	}
	else if(item >= menu_offset[2] && item < menu_offset[3])//Miku effect
	{
		miku_effect = item - menu_offset[2];
	}
	else if (item >= menu_offset[3] && item < menu_offset[4])//Ground effect
	{
		ground_effect = item - menu_offset[3];
	}
	else if (item >= menu_offset[4] && item < menu_offset[5])//Ground effect
	{
		sky_effect = item - menu_offset[4];
	}
	else if (item >= menu_offset[5] && item < menu_offset[6])//Scallion effect
	{
		scallion_effect = item - menu_offset[5];
	}
	else if (item >= menu_offset[6] && item < menu_offset[7])//Miku haircut effect
	{
		miku_hair_effect = item - menu_offset[6] + 21;
	}	
}

void Scene::Render()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);	
	for (size_t i = 0; i < models.size(); i++)
	{		
		if (i == SCALLION && scallion_use)
		{
			models[i]->Render(action[SCALLION], scallion_effect, clock());
			if (invisible_scallion_use)
			{
				models[INVISIBLE_SCALLION]->Render(action[INVISIBLE_SCALLION], 13, clock());				
			}				
		}
		else if (i != GROUND && i != SCALLION && i != MILKYWAY && i != INVISIBLE_SCALLION)
		{
			if(invisible_scallion_use)
				models[i]->Render(action[i], 13, clock());
			else
				models[i]->Render(action[i], miku_effect, clock());
		}
		else if (i == GROUND)
		{
			if (invisible_scallion_use)
				models[i]->Render(action[i], 18, clock());
			else
				models[i]->Render(action[i], ground_effect, clock());
		}
		else if (i == MILKYWAY)
		{
			models[i]->Render(action[i], sky_effect, clock());
		}
		if (i == HAIR && miku_effect == 0) 
		{
			models[i]->Render(action[i], miku_hair_effect, clock());
		}
	}
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
}

void Scene::Update(float dt)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);	
	int actioncode = -1;
	for (size_t i = 0; i < 9; i++)
	{
		if (action_list[i])
			actioncode = i;
	}
	if (actioncode != -1)
	{
		switch (actioncode)
		{
		case 0:
			Cheer(cheerangles[index]);
			index++;
			if (index == sizeof(cheerangles) / sizeof(float))
				index = 0;
			break;
		case 1:
			Walk(walkangles[index]);
			index++;
			if (index == sizeof(walkangles) / sizeof(float))
				index = 0;
			break;
		case 2:
			Fly(flyangles[index]);
			index++;
			if (index == sizeof(flyangles) / sizeof(float))
				index = 0;
			break;
		case 3:			
			if (index < 10)
				Clap(clapangles[index], false);
			else if (index < 28)
				Clap(clapangles[index], true);
			else if (index > 28)
				Clap(clapangles[index], true);
			index++;
			if (index == sizeof(clapangles) / sizeof(float))
			{
				action_list[3] = false;
				mat4 identity(1.0);
				action[LEFT_HAND] *= rotate(identity, -0.3f, vec3(1, 0, 0));
				action[LEFT_HAND] *= rotate(identity, 0.65f, vec3(0, 1, 0));
				action[RIGHT_HAND] *= rotate(identity, -0.3f, vec3(1, 0, 0));
				action[RIGHT_HAND] *= rotate(identity, -0.65f, vec3(0, 1, 0));
				index = 0;
			}
			break;
		case 4:
			if (index < 6)
				Bow(bowangles[index], false);
			else if (index >= 6)
				Bow(bowangles[index], true);
			index++;
			if (index == sizeof(bowangles) / sizeof(float))
			{
				index = 0;
				action_list[4] = false;
			}
			break;
		case 5:
			if (index < 5)
				Lift(takeoffangles[index], false);
			else if (index >= 5)
				Lift(takeoffangles[index], true);
			index++;
			if (index == sizeof(takeoffangles) / sizeof(float))
			{
				index = 0;
				mat4 identity(1.0);
				action_list[5] = false;
			}
			break;
		case 6:
			Drawsword(drawangles[index]);
			index++;
			if (index == 11)
				scallion_use = !scallion_use;
			if (index == sizeof(drawangles) / sizeof(float))
			{
				index = 0;
				action_list[6] = false;
			}
			break;
		case 7:
			if (index < 15)
				Excalibur(excaliburangle[index], false, false);
			else if (index < 20 && index > 15)
				Excalibur(excaliburangle[index], true, false);
			else if (index < 70 && index > 20)
				Excalibur(excaliburangle[index], true, true);

			index++;			
			if (index == sizeof(excaliburangle) / sizeof(float))
			{
				index = 0;
				action_list[7] = false;
				invisible_scallion_use = false;				
			}
			break;
		case 8:
			Loituma(loitumaangle[index], index);
			index++;
			if (index == sizeof(loitumaangle) / sizeof(float))
			{
				index = 0;
				action_list[8] = false;
			}
			break;
		}
	}	
	action[MILKYWAY] *= rotate(mat4(1.0),0.001f,vec3(0,1,0));
	glDisable(GL_DEPTH_TEST);
}

std::vector<BaseModel*> Scene::GetModels()
{
	return models;
}