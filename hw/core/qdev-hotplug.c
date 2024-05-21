/*
 * QDev Hotplug handlers
 *
 * Copyright (c) Red Hat
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#include "qemu/osdep.h"
#include "hw/qdev-core.h"
#include "hw/boards.h"

HotplugHandler *qdev_get_machine_hotplug_handler(DeviceState *dev)
{
    MachineState *machine;
    MachineClass *mc;
    Object *m_obj = qdev_get_machine();

    if (object_dynamic_cast(m_obj, TYPE_MACHINE)) {
        machine = MACHINE(m_obj);
        mc = MACHINE_GET_CLASS(machine);
        if (mc->get_hotplug_handler) {
            return mc->get_hotplug_handler(machine, dev);
        }
    }

    return NULL;
}

bool qdev_hotplug_allowed(DeviceState *dev, Error **errp)
{
    MachineState *machine;
    MachineClass *mc;
    Object *m_obj = qdev_get_machine();

    if (object_dynamic_cast(m_obj, TYPE_MACHINE)) {
        machine = MACHINE(m_obj);
        mc = MACHINE_GET_CLASS(machine);
        if (mc->hotplug_allowed) {
            return mc->hotplug_allowed(machine, dev, errp);
        }
    }

    return true;
}

HotplugHandler *qdev_get_bus_hotplug_handler(DeviceState *dev)
{
    if (dev->parent_bus) {
        return dev->parent_bus->hotplug_handler;
    }
    return NULL;
}

HotplugHandler *qdev_get_hotplug_handler(DeviceState *dev)
{
    HotplugHandler *hotplug_ctrl = qdev_get_machine_hotplug_handler(dev);

    if (hotplug_ctrl == NULL && dev->parent_bus) {
        hotplug_ctrl = qdev_get_bus_hotplug_handler(dev);
    }
    return hotplug_ctrl;
}

/* can be used as ->unplug() callback for the simple cases */
void qdev_simple_device_unplug_cb(HotplugHandler *hotplug_dev,
                                  DeviceState *dev, Error **errp)
{
    qdev_unrealize(dev);
}
