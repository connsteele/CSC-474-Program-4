#pragma once
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

class keyframe
	{
	public:
		quat quaternion;
		vec3 translation;
		long long timestamp_ms;
	};
class animation_per_bone
	{
	public:
		string name;
		long long duration;
		int frames;
		string bone;
		vector<keyframe> keyframes;
	};
class all_animations
	{
	public:
		vector<animation_per_bone> animations;

	};


//**************************************************

class bone
{
public:
	vector<animation_per_bone*> animation;	//all the animations of the bone
	string name;
	vec3 pos;
	quat q;
	bone *parent = NULL;
	vector<bone*> kids;			
	unsigned int index;			//a unique number for each bone, at the same time index of the animatiom matrix array
	mat4 *mat = NULL;			//address of one lement from the animation matrix array
	// searches for the animation and sets the animation matrix element to the recent matrix gained from the keyframe
	void play_animation(int keyframenumber, string animationname, float t) //This method plays one anim, make it play 2, this is the calc_matrix method
		{

		for (int i = 0; i < animation.size(); i++)
			if (animation[i]->name == animationname)
				{
				if (animation[i]->keyframes.size() > keyframenumber)
					{
					quat q = animation[i]->keyframes[keyframenumber].quaternion;
					vec3 tr = animation[i]->keyframes[keyframenumber].translation;
					mat4 M = mat4(q);
					mat4 T = translate(mat4(1), tr);
					M = T * M;
					if (mat)
						{
						mat4 parentmat = mat4(1);
						if (parent)
							parentmat = *parent->mat;
						*mat = parentmat * M;
						}
					}
				else
					*mat = mat4(1);
				}
		for (int i = 0; i < kids.size(); i++)
			kids[i]->play_animation(keyframenumber,animationname, t);
		}

	void myplayanim(float keyframe, string animationname1, string animationname2, float t)
	{
		animation_per_bone *anim1;
		animation_per_bone *anim2;

		for (int i = 0; i < animation.size(); i++) //save animation 1 and 2 so you can interpolate between them
		{
			if (animation[i]->name == animationname1)
			{
				anim1 = animation[i];
			}
			if (animation[i]->name == animationname2)
			{
				anim2 = animation[i];
			}
		}
		
		float ratio = 1. * anim2->keyframes.size() / anim1->keyframes.size(); //ratio between keyframes

		//solve for transformation and quaternions
		if (anim1->keyframes.size()-1 > keyframe ) //only play the animations if the keyframe var is shorter than it 
		{
			//// Keyframes ////
			float kfLow = floor(keyframe);
			float kfUp = kfLow + 1;
			float dec = keyframe - kfLow; //what percent of each animation to use

			/*if (  ) //reset
			{

			}*/

			//// QUATS /////
			//anim 1 quats
			quat q1Low = anim1->keyframes[kfLow].quaternion;
			quat q1Up = anim1->keyframes[kfUp].quaternion; //out of bounds issues, added size()-1 to parameters to make it work
			quat q1mix = slerp(q1Low, q1Up, dec );

			float newKflow = ratio * kfLow;
			float newKfUp = newKflow + 1;

			float newDec = (ratio * keyframe) - newKflow;

			//anim 2 quats
			quat q2Low = anim2->keyframes[newKflow].quaternion;
			quat q2Up = anim2->keyframes[newKfUp].quaternion;
			quat q2mix = slerp(q2Low, q2Up, newDec);

			//// Translation ////
			glm::vec3 t1Low = anim1->keyframes[kfLow].translation;
			glm::vec3 t1Up = anim1->keyframes[kfUp].translation;
			glm::vec3 t1mix = mix(t1Low, t1Up, dec);  //mix is a linear interpolation function
			//glm::vec3 t1mix = ((1-dec) * t1Low) + (t1Up * dec);  // test manual linear interpolation

			glm::vec3 t2Low = anim2->keyframes[newKflow].translation;
			glm::vec3 t2Up = anim2->keyframes[newKfUp].translation;
			glm::vec3 t2mix = mix(t2Low, t2Up, newDec);
			//glm::vec3 t2mix = ((1 - dec) * t2Low) + (t2Up * dec);
			
			//Get the interpolation between mixed quats and translations
			quat qmix = normalize(slerp(q1mix, q2mix, t));
			glm::vec3 tmix = mix(t1mix, t2mix, t);
			if (name == "Humanoid:Hips")
				tmix = vec3(0, 0, 0);

			//Get the actual quaternion and translation matrix
			mat4 M = mat4(qmix); //Orientation
			mat4 T = translate(mat4(1), tmix); //Translation
			M = T * M;
			if (mat)
			{
				mat4 parentmat = mat4(1);
				if (parent)
					parentmat = *parent->mat;
				*mat = parentmat * M;
			}

		}
		else
			*mat = mat4(1);
		for (int i = 0; i < kids.size(); i++)
			kids[i]->myplayanim(keyframe, animationname1, animationname2, t);

		////Do Q1 and Q2 stuff for both animations
		////after u do the interpolation stuff do the stuff from the OG function then recurse on the children with this func
		//for (int i = 0; i < animation.size(); i++)
		//	if (animation[i]->name == animationname1)
		//	{
		//		if (animation[i]->keyframes.size() > keyframe)
		//		{
		//			quat q = animation[i]->keyframes[keyframe].quaternion;
		//			vec3 tr = animation[i]->keyframes[keyframe].translation;
		//			mat4 M = mat4(q);
		//			mat4 T = translate(mat4(1), tr);
		//			M = T * M;
		//			if (mat)
		//			{
		//				mat4 parentmat = mat4(1);
		//				if (parent)
		//					parentmat = *parent->mat;
		//				*mat = parentmat * M;
		//			}
		//		}
		//		else
		//			*mat = mat4(1);
		//	}
		//for (int i = 0; i < kids.size(); i++)
		//	kids[i]->myplayanim(keyframe, animationname1, animationname2, t);
	}

	//writes into the segment positions and into the animation index VBO
	void write_to_VBOs(vec3 origin, vector<vec3> &vpos, vector<unsigned int> &imat)
		{
		vpos.push_back(origin);
		vec3 endp = origin + pos;
		vpos.push_back(endp);

		if(parent)
			imat.push_back(parent->index);
		else
			imat.push_back(index);
		imat.push_back(index);

		for (int i = 0; i < kids.size(); i++)
			kids[i]->write_to_VBOs(endp, vpos, imat);
		}
	//searches for the correct animations as well as sets the correct element from the animation matrix array
	void set_animations(all_animations *all_anim,mat4 *matrices,int &animsize)
		{
		for (int ii = 0; ii < all_anim->animations.size(); ii++)
			if (all_anim->animations[ii].bone == name)
				animation.push_back(&all_anim->animations[ii]);

		mat = &matrices[index];
		animsize++;

		for (int i = 0; i < kids.size(); i++)
			kids[i]->set_animations(all_anim, matrices, animsize);
		}

	int getFrameCount(std::string animationName) //get the duration of the animation in Frames
	{
		for (auto anim : animation) 
		{
			if (anim->name == animationName) 
			{
				return anim->frames;
			}
		}

		return 1;
	}

	long long getMS(std::string animationName) //get the animation duration in MS
	{
		for (auto anim : animation) 
		{
			if (anim->name == animationName) 
			{
				return anim->duration;
			}
		}

		return 0;
	}

};
int readtobone(string file, string file2, all_animations *all_animation , bone **proot);