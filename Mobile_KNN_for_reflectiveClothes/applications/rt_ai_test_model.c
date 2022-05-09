#include <rt_ai.h>
#include <backend_k210_kpu.h>
#include <rt_ai_test_model.h>
#include <kpu.h>

extern unsigned char test_kmodel[];

/* based on k210 */
#define RT_AI_TEST_INFO    {       \
    RT_AI_TEST_IN_NUM,             \
    RT_AI_TEST_OUT_NUM,            \
    RT_AI_TEST_IN_SIZE_BYTES,      \
    RT_AI_TEST_OUT_SIZE_BYTES,     \
    RT_AI_TEST_WORK_BUFFER_BYTES,  \
    ALLOC_INPUT_BUFFER_FLAG                 \
}

#define RT_AI_TEST_HANDLE  {         \
    .info   =     RT_AI_TEST_INFO    \
}

#define RT_K210_AI_TEST   {   \
    .parent         = RT_AI_TEST_HANDLE,   \
    .model          = test_kmodel, \
    .dmac           = DMAC_CHANNEL5,        \
}

static struct k210_kpu rt_k210_ai_test = RT_K210_AI_TEST;

static int rt_k210_ai_test_init(){
    rt_ai_register(RT_AI_T(&rt_k210_ai_test),RT_AI_TEST_MODEL_NAME,0,backend_k210_kpu,&rt_k210_ai_test);
    return 0;
}

INIT_APP_EXPORT(rt_k210_ai_test_init);
