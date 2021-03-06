/*
 * Copyright (c) 2018, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the Intel Corporation nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: Bartosz Kokoszko <bartoszx.kokoszko@linux.intel.com>
 */

#include <sof/audio/component.h>
#include <errno.h>

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <sof/sof.h>

enum test_type {
	SUCCEED = 0,
	FAIL,
	CORRECT_OUTPUT_STATE,

};

struct test_case {
	enum test_type type;
	uint16_t in_state;
	int cmd;
	uint16_t out_state;
	const char *name;
};

#define TEST_CASE(type, in_state, cmd, out_state) \
	{(type), (in_state), (cmd), (out_state), \
	("test_audio_component_comp_set_state__" \
	 #type "__" #in_state "__" #cmd "__" #out_state)}


/*
 * NULL_STATE enum is used in every case, when new state of component is
 * insignificant due to action of testing function
 */
enum {
	NULL_STATE = -1
};

struct test_case test_cases[] = {

	/*succeed set state*/
	TEST_CASE(SUCCEED,		COMP_STATE_PREPARE,
		COMP_TRIGGER_START,	NULL_STATE),
	TEST_CASE(SUCCEED,		COMP_STATE_PAUSED,
		COMP_TRIGGER_RELEASE,	NULL_STATE),
	TEST_CASE(SUCCEED,		COMP_STATE_ACTIVE,
		COMP_TRIGGER_STOP,	NULL_STATE),
	TEST_CASE(SUCCEED,		COMP_STATE_PAUSED,
		COMP_TRIGGER_STOP,	NULL_STATE),
	TEST_CASE(SUCCEED,		COMP_STATE_ACTIVE,
		COMP_TRIGGER_XRUN,	NULL_STATE),
	TEST_CASE(SUCCEED,		COMP_STATE_PAUSED,
		COMP_TRIGGER_XRUN,	NULL_STATE),
	TEST_CASE(SUCCEED,		COMP_STATE_ACTIVE,
		COMP_TRIGGER_PAUSE,	NULL_STATE),
	TEST_CASE(SUCCEED,		COMP_STATE_INIT,
		COMP_TRIGGER_RESET,	NULL_STATE),
	TEST_CASE(SUCCEED,		COMP_STATE_READY,
		COMP_TRIGGER_RESET,	NULL_STATE),
	TEST_CASE(SUCCEED,		COMP_STATE_SUSPEND,
		COMP_TRIGGER_RESET,	NULL_STATE),
	TEST_CASE(SUCCEED,		COMP_STATE_PREPARE,
		COMP_TRIGGER_RESET,	NULL_STATE),
	TEST_CASE(SUCCEED,		COMP_STATE_PAUSED,
		COMP_TRIGGER_RESET,	NULL_STATE),
	TEST_CASE(SUCCEED,		COMP_STATE_ACTIVE,
		COMP_TRIGGER_RESET,	NULL_STATE),
	TEST_CASE(SUCCEED,		COMP_STATE_PREPARE,
		COMP_TRIGGER_PREPARE,	NULL_STATE),
	TEST_CASE(SUCCEED,		COMP_STATE_READY,
		COMP_TRIGGER_PREPARE,	NULL_STATE),

	/*fail set state*/
	TEST_CASE(FAIL,		COMP_STATE_INIT,	COMP_TRIGGER_START,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_READY,	COMP_TRIGGER_START,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_SUSPEND,	COMP_TRIGGER_START,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_PAUSED,	COMP_TRIGGER_START,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_ACTIVE,	COMP_TRIGGER_START,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_INIT,	COMP_TRIGGER_RELEASE,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_READY,	COMP_TRIGGER_RELEASE,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_SUSPEND,	COMP_TRIGGER_RELEASE,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_PREPARE,	COMP_TRIGGER_RELEASE,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_INIT,	COMP_TRIGGER_STOP,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_READY,	COMP_TRIGGER_STOP,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_SUSPEND,	COMP_TRIGGER_STOP,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_PREPARE,	COMP_TRIGGER_STOP,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_INIT,	COMP_TRIGGER_XRUN,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_READY,	COMP_TRIGGER_XRUN,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_SUSPEND,	COMP_TRIGGER_XRUN,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_PREPARE,	COMP_TRIGGER_XRUN,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_INIT,	COMP_TRIGGER_PAUSE,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_READY,	COMP_TRIGGER_PAUSE,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_SUSPEND,	COMP_TRIGGER_PAUSE,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_PREPARE,	COMP_TRIGGER_PAUSE,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_PAUSED,	COMP_TRIGGER_PAUSE,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_INIT,	COMP_TRIGGER_PREPARE,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_SUSPEND,	COMP_TRIGGER_PREPARE,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_PAUSED,	COMP_TRIGGER_PREPARE,
		NULL_STATE),
	TEST_CASE(FAIL,		COMP_STATE_ACTIVE,	COMP_TRIGGER_PREPARE,
		NULL_STATE),
	
	/*correct output state*/
	TEST_CASE(CORRECT_OUTPUT_STATE,		COMP_STATE_PREPARE,
		COMP_TRIGGER_START,	COMP_STATE_ACTIVE),
	TEST_CASE(CORRECT_OUTPUT_STATE,		COMP_STATE_PAUSED,
		COMP_TRIGGER_RELEASE,	COMP_STATE_ACTIVE),
	TEST_CASE(CORRECT_OUTPUT_STATE,		COMP_STATE_ACTIVE,
		COMP_TRIGGER_STOP,	COMP_STATE_PREPARE),
	TEST_CASE(CORRECT_OUTPUT_STATE,		COMP_STATE_PAUSED,
		COMP_TRIGGER_STOP,	COMP_STATE_PREPARE),
	TEST_CASE(CORRECT_OUTPUT_STATE,		COMP_STATE_ACTIVE,
		COMP_TRIGGER_XRUN,	COMP_STATE_PREPARE),
	TEST_CASE(CORRECT_OUTPUT_STATE,		COMP_STATE_PAUSED,
		COMP_TRIGGER_XRUN,	COMP_STATE_PREPARE),
	TEST_CASE(CORRECT_OUTPUT_STATE,		COMP_STATE_ACTIVE,
		COMP_TRIGGER_PAUSE,	COMP_STATE_PAUSED),
	TEST_CASE(CORRECT_OUTPUT_STATE,		COMP_STATE_INIT,
		COMP_TRIGGER_RESET,	COMP_STATE_READY),
	TEST_CASE(CORRECT_OUTPUT_STATE,		COMP_STATE_READY,
		COMP_TRIGGER_RESET,	COMP_STATE_READY),
	TEST_CASE(CORRECT_OUTPUT_STATE,		COMP_STATE_SUSPEND,
		COMP_TRIGGER_RESET,	COMP_STATE_READY),
	TEST_CASE(CORRECT_OUTPUT_STATE,		COMP_STATE_PREPARE,
		COMP_TRIGGER_RESET,	COMP_STATE_READY),
	TEST_CASE(CORRECT_OUTPUT_STATE,		COMP_STATE_PAUSED,
		COMP_TRIGGER_RESET,	COMP_STATE_READY),
	TEST_CASE(CORRECT_OUTPUT_STATE,		COMP_STATE_ACTIVE,
		COMP_TRIGGER_RESET,	COMP_STATE_READY),
	TEST_CASE(CORRECT_OUTPUT_STATE,		COMP_STATE_PREPARE,
		COMP_TRIGGER_PREPARE,	COMP_STATE_PREPARE),
	TEST_CASE(CORRECT_OUTPUT_STATE,		COMP_STATE_READY,
		COMP_TRIGGER_PREPARE,	COMP_STATE_PREPARE),
};

static void test_audio_component_comp_set_state_succeed(struct test_case *tc)
{
	struct comp_dev test_dev;

	test_dev.state = tc->in_state;

	assert_int_equal(comp_set_state(&test_dev, tc->cmd), 0);
}

static void test_audio_component_comp_set_state_correct_output_state(struct test_case *tc)
{
	struct comp_dev test_dev;

	test_dev.state = tc->in_state;
	comp_set_state(&test_dev, tc->cmd);

	assert_int_equal(test_dev.state, tc->out_state);
}

static void test_audio_component_comp_set_state_fail(struct test_case *tc)
{
	struct comp_dev test_drv;

	test_drv.state = tc->in_state;
	assert_int_equal(comp_set_state(&test_drv, tc->cmd), -EINVAL);
}

static void test_audio_component_comp_set_state(void **state)
{
	struct test_case *tc = *((struct test_case **) state);

	switch (tc->type) {
	case SUCCEED:
		test_audio_component_comp_set_state_succeed(tc);
		break;
	case FAIL:
		test_audio_component_comp_set_state_fail(tc);
		break;
	case CORRECT_OUTPUT_STATE:
		test_audio_component_comp_set_state_correct_output_state(tc);
		break;
	}
}

int main(void)
{
	struct CMUnitTest tests[ARRAY_SIZE(test_cases)];

	int i;

	for (i = 0; i < ARRAY_SIZE(test_cases); i++) {
		struct CMUnitTest *t = &tests[i];

		t->name = test_cases[i].name;
		t->test_func = test_audio_component_comp_set_state;
		t->initial_state = &test_cases[i];
		t->setup_func = NULL;
		t->teardown_func = NULL;

	}
	cmocka_set_message_output(CM_OUTPUT_TAP);

	return cmocka_run_group_tests(tests, NULL, NULL);
}
