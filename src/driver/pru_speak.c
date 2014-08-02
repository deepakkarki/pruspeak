#include <linux/module.h>
#include <linux/err.h>
#include <linux/dma-mapping.h>
#include <linux/remoteproc.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/genalloc.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/pm_runtime.h>
#include <linux/pinctrl/consumer.h>
#include <linux/io.h>
#include <plat/mailbox.h>
#include <linux/virtio_ids.h>
#include <linux/elf.h>
#include <linux/byteorder/generic.h>
#include <linux/virtio.h>
#include <linux/virtio_ring.h>
#include <asm/atomic.h>

#include <linux/pwm.h>

//#include "remoteproc_internal.h"

#ifndef REMOTEPROC_INTERNAL_H
#define REMOTEPROC_INTERNAL_H

#include <linux/irqreturn.h>
#include <linux/firmware.h>
#include "pru_speak.h"



