/*
 *  Copyright (c) 2016, Nest Labs, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * @brief
 *   This file includes the platform-specific initializers.
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/time.h>

#include <openthread.h>
#include <tasklet.h>
#include <platform/alarm.h>
#include <platform/uart.h>
#include <posix-platform.h>
#include <selfpipe.h>

uint32_t NODE_ID = 1;
uint32_t WELLKNOWN_NODE_ID = 34;

static fd_set read_fds;
static fd_set write_fds;
static struct timeval timeout;
static int max_fd = -1;

void posixPlatformInit(void)
{
    posixPlatformAlarmInit();
    otPlatUartEnable();
    PlatformRadioInit();
    posixPlatformRandomInit();
}

void otTaskletsSignalPending(otInstance *aInstance){
	selfpipe_push();
}

void posixPlatformSleep(otInstance *aInstance){
    int rval;

    if (!otTaskletsArePending(aInstance))
    {
        rval = select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout);
        selfpipe_pop();
        assert(rval >= 0 && errno != ETIME);
    }
}

void posixPlatformProcessDrivers(otInstance *aInstance)
{
    platformUartProcess();
    PlatformRadioProcess();
    posixPlatformAlarmProcess(aInstance);

    FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);

	platformUartUpdateFdSet(&read_fds, &write_fds, &max_fd);
	selfpipe_UpdateFdSet(&read_fds, &write_fds, &max_fd);
	posixPlatformAlarmUpdateTimeout(&timeout);
}

