#include "core.h"
#include <vector>
#include <string>
#include <stdio.h>
#include "Tokenizer.h"
#include "Channel.cpp"
#include "Skin.cpp"

class Animation {
public:
	int numChannels;
	float startTime;
	float endTime;
	std::vector<Channel*> channels;

public: 
	Animation() {
		startTime = endTime = 0;
		numChannels = 0;
	}

	bool load(const char* file) {

		Tokenizer t = Tokenizer();
		t.Open(file);

		char temp[256];
		t.GetToken(temp);
		if (strcmp(temp, "animation") == 0)
		{
			t.SkipLine();
			t.GetToken(temp);
			if (strcmp(temp, "range") == 0)
			{
				startTime = t.GetFloat();
				endTime = t.GetFloat();

				int num = 0;
				t.GetToken(temp);
				if (strcmp(temp, "numchannels") == 0)
				{
					num = t.GetInt();
				}

				for (int i = 0; i < num; i++)
				{
					t.GetToken(temp);
					if (strcmp(temp, "channel") != 0)
						continue;

					Channel* currChannel = new Channel();
					currChannel->index = t.GetInt();
					t.SkipLine();
					t.GetToken(temp);

					if (strcmp(temp, "extrapolate") == 0) {
						t.GetToken(temp);
						currChannel->exIn = temp;
						t.GetToken(temp);
						currChannel->exOut = temp;

						t.GetToken(temp);
						if (strcmp(temp, "keys") == 0) {

							int numOfKey = t.GetInt();
							t.SkipLine();

							for (int i = 0; i < numOfKey; i++) {
								Keyframe* newKey = new Keyframe();
								newKey->time = t.GetFloat();
								newKey->value = t.GetFloat();

								t.GetToken(temp);
								newKey->ruleIn = temp;
								if (strcmp(temp, "flat") == 0) {
									newKey->tanIn = 0;
								}
								t.GetToken(temp);
								newKey->ruleOut = temp;
								if (strcmp(temp, "flat") == 0) {
									newKey->tanOut = 0;
								}

								currChannel->keyframes.push_back(newKey);
							}
							t.GetToken(temp);
						}
					}
					t.GetToken(temp);
					currChannel->precompute();
					channels.push_back(currChannel);
				}
			}
		}
		t.Close();
		return true;
	}

	void update(float& t, std::vector<Joint*> joints, Skeleton* skel) {
		float timeMod = fmod(t, (endTime - startTime));

		skel->world[3].x = channels[0]->evaluate(timeMod);
		skel->world[3].y = channels[1]->evaluate(timeMod);
		skel->world[3].z = channels[2]->evaluate(timeMod);

		int index = 3;
		for (Joint* j : joints) {
			j->pose.x = channels[index++]->evaluate(timeMod);
			j->pose.y = channels[index++]->evaluate(timeMod);
			j->pose.z = channels[index++]->evaluate(timeMod);

		}
	}
};
