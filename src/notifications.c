/*
 * This file is part of pa-applet.
 *
 * © 2012 Fernando Tarlá Cardoso Lemos
 *
 * Refer to the LICENSE file for licensing information.
 *
 */

#include <libnotify/notify.h>

#include "audio_status.h"
#include <math.h>

#define PROGRAM_NAME "pa-applet"

gboolean have_notifications = FALSE;
NotifyNotification *notification = NULL;

char volumeString[16];// length is a bit lower but better safe than sorry

void notifications_init(void)
{
    if (notify_init(PROGRAM_NAME)) {
        // Create and configure the notification
        have_notifications = TRUE;
#if NOTIFY_CHECK_VERSION(0, 7, 0)
        notification = notify_notification_new(PROGRAM_NAME, NULL, NULL);
#else
        notification = notify_notification_new(PROGRAM_NAME, NULL, NULL, NULL);
#endif
        if (notification) {
            notify_notification_set_timeout(notification, NOTIFY_EXPIRES_DEFAULT);
            notify_notification_set_hint_string(notification, "synchronous", "volume");
        }
        else {
            g_printerr("Failed to create a notification\n");
        }
    }
    else {
        g_printerr("Failed to initialize notifications\n");
    }
}

void notifications_destroy(void)
{
    if (have_notifications) {
        notify_uninit();
        if (notification)
            g_object_unref(G_OBJECT(notification));
    }
}

void notifications_flash(void)
{
    // Nothing to do if we don't support notifications
    if (!have_notifications || !notification)
        return;

    // Find the icon name
    const char *icon_name;
    audio_status *as = shared_audio_status();
    if (as->muted) {
        icon_name = "audio-volume-muted";
    }else{
    	icon_name=NULL;
    }

    // Update the notification volume
    notify_notification_set_hint(notification, "value", g_variant_new_double(as->volume));

    // Add transient flag
    notify_notification_set_hint(notification, "transient", g_variant_new_int32(1));

    sprintf(volumeString,"volume %d%%",(int)(round(as->volume)));
    // Update the notification icon
    notify_notification_update(notification, volumeString, icon_name, icon_name);

    // Show the notification
    notify_notification_show(notification, NULL);
}
