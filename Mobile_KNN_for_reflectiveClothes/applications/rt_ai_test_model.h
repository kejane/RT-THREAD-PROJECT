#ifndef __RT_AI_TEST_MODEL_H
#define __RT_AI_TEST_MODEL_H

/* model info ... */

// model name
#define RT_AI_TEST_MODEL_NAME			"test"

#define RT_AI_TEST_WORK_BUFFER_BYTES	(1612800)

#define AI_TEST_DATA_WEIGHTS_SIZE		(1493120) //unused

#define RT_AI_TEST_BUFFER_ALIGNMENT		(4)

#define RT_AI_TEST_IN_NUM				(1)

#define RT_AI_TEST_IN_1_SIZE			(1 * 3 * 240 * 320)
#define RT_AI_TEST_IN_1_SIZE_BYTES		((1 * 3 * 240 * 320) * 4)
#define RT_AI_TEST_IN_SIZE_BYTES		{	\
	((1 * 3 * 240 * 320) * 4) ,	\
}

#define RT_AI_TEST_IN_TOTAL_SIZE_BYTES	((1 * 3 * 240 * 320) * 4)


#define RT_AI_TEST_OUT_NUM				(1)

#define RT_AI_TEST_OUT_1_SIZE			(1 * 3)
#define RT_AI_TEST_OUT_1_SIZE_BYTES		((1 * 3) * 4)
#define RT_AI_TEST_OUT_SIZE_BYTES		{	\
	((1 * 3) * 4) ,	\
}

#define RT_AI_TEST_OUT_TOTAL_SIZE_BYTES	((1 * 3) * 4)



#define RT_AI_TEST_TOTAL_BUFFER_SIZE		//unused

#endif	//end
