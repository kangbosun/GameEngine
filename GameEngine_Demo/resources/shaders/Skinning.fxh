
#ifndef _SKINNING_FXH_
#define _SKINNING_FXH_

#define MAX_BONE 100

float4x4 gBones[MAX_BONE];

interface ISkinning
{
	float4x4 GetMatrix(uint4 index, float4 weights);
};


class CSkinningDisabled : ISkinning
{
	float4x4 GetMatrix(uint4 index, float4 weights) 
	{ 
		return float4x4( 1, 0, 0, 0,
					     0, 1, 0, 0,
				         0, 0, 1, 0,
				         0, 0, 0, 1 );
	}
};

class CSkinningEnabled : ISkinning
{
	float4x4 GetMatrix(uint4 index, float4 weights)
	{
		float4x4 ret = {0, 0, 0, 0,
		                0, 0, 0, 0,
						0, 0, 0, 0,
						0, 0, 0, 0};
		[unroll]
		for(int i = 0; i < 4; ++i) {
			ret += gBones[index[i]] * weights[i];
		}
		return ret;
	}
};


CSkinningDisabled skinningDisable;
CSkinningEnabled skinningEnable;
ISkinning skinning = skinningDisable;
#endif 