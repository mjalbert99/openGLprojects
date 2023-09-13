#include <string>
#include "core.h"
#include <stdio.h>
#include <iostream>
#include "Tokenizer.h"

class Keyframe {
public:
	float time;
	float value;
	float tanIn;
	float tanOut;
	float A, B, C, D;
	float diffTime;
	std::string ruleIn;
	std::string ruleOut;

public:
	Keyframe() {
		time = value = tanIn = tanOut = 0;
		A = B = C = D = diffTime = 0;
		ruleIn = ruleOut = "";
	}

	void computeTangent(Keyframe* prev, Keyframe* next) {
		// Single Frame
		if (prev == nullptr && next == nullptr){
				tanIn = 0;
				tanOut = 0;
		}

		//First Frame
		else if (prev == nullptr){
			if (ruleOut == "linear"|| ruleOut == "smooth")
				tanOut = (next->value - value)/ (next->time - time);
		}

		// Last Frame
		else if (next == nullptr){
			if (ruleIn == "linear"|| ruleIn == "smooth")
				tanIn = (value - prev->value)/ (time - prev->time);
		}

		// Mid Sections
		else{
			if (ruleIn == "linear")
{
				tanIn = (value - prev->value)/ (time - prev->time);
			}
			else if (ruleIn  == "smooth")
			{
				tanIn = (next->value - prev->value)/ (next->time - prev->time);
				tanOut = tanIn;
			}
			if (ruleOut == "linear"){
				tanOut = (next->value - value)/ (next->time - time);
			}
		}
	}

	void computeCubic(Keyframe* next) {
		if (next != nullptr){
			diffTime = next->time - time;

			glm::mat4 mat = glm::mat4(2.0f, -3.0f, 0.f, 1.0f, -2.0f, 3.0f, 0.f, 0.f, 1.0f, -2.0f, 1.0f, 0.f, 1.0f, -1.0f, 0.f, 0.f); 
			glm::vec4 coefs = mat * glm::vec4(value, next->value, (next->time - time) * tanOut, (next->time - time) * next->tanIn);

			A = coefs.x;
			B = coefs.y;
			C = coefs.z;
			D = coefs.w;
		}
		else{
			A = 0.0f;
			B = 0.0f;
			C = 0.0f;
			D = value;
		}
	}

	float Keyframe::evaluate(float t){
		float u = (t - time) / diffTime;
		return D + u * (C + u * (B + u * A));
	}
};
