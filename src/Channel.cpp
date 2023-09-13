#include "core.h"
#include <vector>
#include <string>
#include <stdio.h>
#include "Keyframe.cpp"

class Channel {

public:
	int index;
	float startTime;
	float endTime;
	float startTan;
	float endTan;
	float period;
	float offset;
	std::string exIn;
	std::string exOut;
	std::vector<Keyframe*> keyframes;

public: 
	Channel() {
		index = 0;
		startTime = endTime = startTan = endTan = period = offset = 0;
	}

	void setUp() {
		startTime = keyframes.front()->time;
		endTime = keyframes.back()->time;
		startTan = keyframes.front()->tanIn;
		endTan = keyframes.back()->tanOut;
		period = endTime - startTime;
		offset = keyframes.back()->value - keyframes.front()->value;
	}

	void precompute() {
		int keyLength= keyframes.size();
		if (keyLength== 1)
		{
			keyframes[0]->computeTangent(nullptr, nullptr);
			keyframes[0]->computeCubic(nullptr);
		}
		else if (keyLength== 2)
		{
			keyframes[0]->computeTangent(nullptr, keyframes[1]);
			keyframes[1]->computeTangent(keyframes[0], nullptr);
			keyframes[0]->computeCubic(keyframes[1]);
		}
		else
		{
			keyframes.front()->computeTangent(nullptr, keyframes[1]);
			for (int i = 1; i < keyLength- 1; i++)
			{
				keyframes[i]->computeTangent(keyframes[i - 1], keyframes[i + 1]);
			}
			keyframes.back()->computeTangent(keyframes[keyLength- 2], nullptr);

			for (int i = 0; i < keyLength- 1; i++)
			{
				keyframes[i]->computeCubic(keyframes[i + 1]);
			}
		}
		setUp();
	}

	float evaluate(float t) {
		float value_offset = 0.0f;

		// Time less than startTime
		if (t <= startTime) {
			if (exIn == "constant") {
				return keyframes.front()->value;
			}
			else if (exIn == "linear") {
				return startTan * (t - startTime) + keyframes.front()->value;
			}
			else if (exIn == "cycle") {
				int count = (endTime - t) / period;
				t += count * period;
			}
			else if (exIn == "cycle_offset") {
				int count = (endTime - t) / period;
				t += count * period;
				value_offset = -offset * count;
			}
		}
		
		// Time greater than endTime
		else if (t >= endTime) {
		  if (exOut == "constant") {
		    return keyframes.back()->value;
		  } 
		  else if (exOut == "linear") {
		    return endTan * (t - endTime) + keyframes.back()->value;
		  } 
		  else if (exOut == "cycle"){
		    int count = (t - startTime) / period;
		    t -= count * period;
		  }
		  else if (exOut == "cycle_offset") {
		    int count = (t - startTime) / period;
		    t -= count * period;
		    value_offset = offset * count;
		  }
		}

		// Does the evaluation
		int index = 0;
		for (int i = 0; i < keyframes.size(); i++){
			if (keyframes[i]->time >= t){
				index = i;
				break;
			}
		}
		if (index == 0)
			return keyframes[0]->evaluate(0.0f) + value_offset;
		return keyframes[index-1]->evaluate(t) + value_offset;
	}
};