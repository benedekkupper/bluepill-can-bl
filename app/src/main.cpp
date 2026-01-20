#include "app_version.h"
#include <can_bl/boot.hpp>
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/reboot.h>

static int cmd_boot_to_update(const shell* sh, [[maybe_unused]] size_t argc,
                              [[maybe_unused]] char** argv)
{
    can_bl::set_update_request();

    sys_reboot(SYS_REBOOT_COLD);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_image,
                               SHELL_CMD(update, NULL, "Boot to update mode", cmd_boot_to_update),
                               SHELL_SUBCMD_SET_END /* Array terminated. */
);
SHELL_CMD_REGISTER(image, &sub_image, "Image commands", NULL);

int main()
{
    printk("Application version %s\n", APP_VERSION_EXTENDED_STRING);
    return 0;
}
